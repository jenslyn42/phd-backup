/****************************************************************************************
 *   Copyright (C) 2011 by Jeppe Rishede 						*
 *   jenslyn42@gmail.com								*
 *											*
 *   All rights reserved.								*
 *											*
 *   Redistribution and use in source and binary forms, with or without 		*
 *   modification, are permitted provided that the following conditions are met:	*
 *   Redistributions of source code must retain the above copyright notice,		*
 *   this list of conditions and the following disclaimer. 				*
 *   Redistributions in binary form must reproduce the above copyright notice,		*
 *   this list of conditions and the following disclaimer in the documentation		*
 *   and/or other materials provided with the distribution. 				*
 *   Neither the name of the <ORGANIZATION> nor the names of its contributors 		*
 *   may be used to endorse or promote products derived from this software 		*
 *   without specific prior written permission						*
 *                                                                         		*
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   		*
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     		*
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 		*
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER		*
 *   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 		*
 *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   		*
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    		*
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 		*
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  		*
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    		*
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          		*
 ***************************************************************************************/
#include "probstaticcache.h"
#define debug false

probstaticCache::probstaticCache()
{

}

probstaticCache::probstaticCache(testsetting ts)
{
	this->ts = ts;
	cache = aCache(ts);
	cacheSize = cache.size();
	numTotalQueries = 0;
	numCacheHits = 0;
	useNodeScore = ts.isUseHitScore();
	useHitScore = ts.isUseNodeScore();
	numDijkstraCalls = 0;
	queryNumCacheFull = -1;
	cacheFull = false;
	readMapData();
	makePartitions(ts.getSplits());
	calcScoreCounter=0;
	mapSize = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->getMapsize();
}

probstaticCache::~probstaticCache()
{
	for(int i=0;i<mapSize;i++)
		delete [] trainingQueriesPerRegionPair[i];
	delete [] trainingQueriesPerRegionPair;
}

void probstaticCache::readQuery(std::pair< int, int > query)
{
	checkCache(query);
	numTotalQueries++;
}

// void probstaticCache::readQueryList(std::vector< std::pair < int , int > > queryList)
// {
// 	int i = 0;
//  	BOOST_FOREACH(intPair q, queryList ) {
// 	i++;
// 	if(i%10000 == 0) cout << "Query " << i << " done" << endl;
// 	readQuery(q);
// 	}
// }

//does not use argument, but does instead get its info from a .train file.
void probstaticCache::readQueryList(std::vector< std::pair < int , int > > queryList)
{
	RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.
	BOOST_FOREACH(intPair q, testSTPointPairs ) { readQuery(q); }
}

void probstaticCache::readQueries(int numQueries, string inFn)
{
	cout<< "2.0 done" << endl;
	readTestData(ts.queryFileName);
	cout<< "2.1 done" << endl;
	readTrainingData(ts.queryFileName);
	cout<< "2.2 done" << endl;
	startTime = clock();
	statistics(ts.queryFileName);
	endTime = clock();
	ts.buildStatisticsTime = (double(endTime-startTime))/CLOCKS_PER_SEC;
	calcScoreCounter=0;
	cout<< "2.3 done" << endl;
	startTime = clock();
 	fillCache(numQueries, inFn);
	endTime = clock();
	ts.fillCacheTime = (double(endTime-startTime))/CLOCKS_PER_SEC;
	cout<< "2.4 done" << endl;
}

void probstaticCache::checkCache(std::pair< int, int > query)
{
	bool cacheHit = false;
	vector<int> spResult;

	if(debug) cout << "one, probstaticcache::checkCache! :cacheSize:" << (int) cache.size() <<"::"<< endl;
	if(cache.checkCache(query))
	{
		numCacheHits++;
		cacheHit = true;
	}

	if(debug) cout << "two, probstaticcache::checkCache! cacheHit: " << cacheHit << endl;
	if(!cacheHit)
	{
		if(debug) cout << "three, probstaticcache::checkCache!" << endl;
		spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(query.first, query.second);
		numDijkstraCalls++;
	}
	if(debug) cout << "four, probstaticcache::checkCache!" << endl;
}

void probstaticCache::statistics(string outFn)
{
	populateProbStructures();
}

void probstaticCache::readMapData()
{
	cout << "one, probstaticcache::readMapData start!" << endl;
	int mapSize = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->getMapsize();
	string mapFile = ts.getTestFile();
	std::pair<double, double> tmpPair;
	string str;
	std::vector<string> tokens;

	mapFile.replace ((mapFile.size()-4), 4, "node"); //change file extention from .cedge to .cnode
	ifstream in_data (mapFile.c_str(), ios::in); //*.cnode file

	//read in the mapping between coordinates and node ids from *.cnode file
	if(in_data.is_open())
	{
		for(int i = 0; i < mapSize+1; i++)
		{
			getline(in_data, str);
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
			tmpPair = std::make_pair(atof(tokens[1].c_str()),atof(tokens[2].c_str()));
			coordinate2Nodeid[tmpPair] = atoi(tokens[0].c_str());
			nodeid2coordinate[atoi(tokens[0].c_str())] = tmpPair;
			points.push_back(tmpPair);
		}
	}
	in_data.close();

	writeoutCacheCoordinates(ts.getTestName(), cache.getCacheContentVector(), nodeid2coordinate, ts.getSplits());

	cout << "two, probstaticcache::readMapData end!" << endl;
}

//file on the form:
//record_id, S_id, T_id, S_x, S_y, T_x, T_y.
void probstaticCache::readData(string fn)
{
	readTrainingData(fn);
	readTestData(fn);
}

//file on the form:
//record_id, S_id, T_id, S_x, S_y, T_x, T_y.
void probstaticCache::readTestData(string fn)
{
	cout << "one, probstaticcache::readTestData start!" << endl;
	std::pair<double, double> firstPair, secondPair;
	int firstPnt, secondPnt;
	string str;
	std::vector<string> tokens;

	fn.replace ((fn.size()-5), 5, "test"); //change file extention from .train to .test
	ifstream testData (fn.c_str(), ios::in); //*.test file


	//find all pairs of nodeids in the test set to have SP done for them. map nodeids to coordinates.
	if(testData.is_open())
	{
        if(debug) cout << "two, probstaticcache::readTestData file [" << fn << "] opened!" << endl;
		while(getline(testData, str))
		{
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

			firstPair = std::make_pair(atof(tokens[3].c_str()),atof(tokens[4].c_str()));
			secondPair = std::make_pair(atof(tokens[5].c_str()),atof(tokens[6].c_str()));

			firstPnt = coordinate2Nodeid[firstPair];
			secondPnt = coordinate2Nodeid[secondPair];

			testSTPointPairs.push_back(std::make_pair(firstPnt,secondPnt));
			if(debug) cout << "tree, probstaticcache::readTestData end of fileline loop.!" << endl;
		}
	}
	testData.close();
	cout << "two, probstaticcache::readTestData end!" << endl;
}

//file on the form:
//record_id, S_id, T_id, S_x, S_y, T_x, T_y.
void probstaticCache::readTrainingData(string fn)
{
	cout << "one, probstaticcache::readTrainingData start!" << endl;
	std::pair<double, double> firstPair, secondPair;
	int firstPnt, secondPnt;
	string str;
	std::vector<string> tokens;

	ifstream trainingData (fn.c_str(), ios::in); //*.train file
	if(debug) cout << "one, probstaticcache::readTrainingData! " << endl;
	//find all pairs of nodeids in the training set to have SP done for them. map nodeids to coordinates.
	if(trainingData.is_open())
	{
		if(debug) cout << "two, probstaticcache::readTrainingData! " << endl;
		while(getline(trainingData, str))
		{
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

			firstPair = std::make_pair(atof(tokens[3].c_str()),atof(tokens[4].c_str()));
			secondPair = std::make_pair(atof(tokens[5].c_str()),atof(tokens[6].c_str()));

			firstPnt = coordinate2Nodeid[firstPair];
			secondPnt = coordinate2Nodeid[secondPair];

			trainingSTPointPairs.push_back(std::make_pair(firstPnt,secondPnt));
		}
	}
	trainingData.close();
	cout << "two, probstaticcache::readTrainingData end!" << endl;
}

bool xxCompfunc(std::pair<double, double> i,std::pair<double, double> j) {return (i.first<j.first);} //sort based on x values
bool yyCompfunc(std::pair<double, double> i,std::pair<double, double> j) {return (i.second<j.second);} //sort based on y values

bool probstaticCache::makePartitions(int splits)
{
	int axis = 0, j = 0;

	double mapXmin,mapXmax,mapYmin,mapYmax;
	partitionsMade = pow(2,splits);
	vector<region> regionsVector, tmpRegions;
	sort(points.begin(), points.end(), xxCompfunc);
	mapXmin = points.front().first;
	mapXmax = points.back().first;
	sort(points.begin(), points.end(), yyCompfunc);
	mapYmin = points.front().second;
	mapYmax = points.back().second;

	region reg(0,points,mapXmin,mapXmax,mapYmin,mapYmax);
	regionsVector.push_back(reg);

	for(int i=0; i<splits; i++)
	{
		tmpRegions = split(regionsVector, axis);
		regionsVector = tmpRegions;
		axis++;
	}

	//set the id of each region.
	BOOST_FOREACH(region r, regionsVector)
	{
		r.id = j;
		mapRegions[j] = r;
		j++;
	}
	cout << "mapRegions size: " << mapRegions.size() << endl;
	if(!mapRegions.empty()) return true;

	return false;
}

std::vector<region> probstaticCache::split(std::vector< region > regions, int axis)
{
	std::vector< region > tmpRegions;
	int size = 0;
	int middle = 0;

	regionlist leftdata, rightdata;

	BOOST_FOREACH (region reg, regions)
	{
		if(axis%2)
			sort(reg.points.begin(), reg.points.end(), xxCompfunc);
		else
			sort(reg.points.begin(), reg.points.end(), yyCompfunc);

		region leftReg(axis,leftdata,reg.xmin,reg.xmax,reg.ymin,reg.ymax);
		region rightReg(axis,rightdata,reg.xmin,reg.xmax,reg.ymin,reg.ymax);

		size = reg.points.size();
		middle = size/2;
		leftdata.resize(middle);
		rightdata.resize(size-middle);
		copy(reg.points.begin(), reg.points.begin()+middle, leftdata.begin());
		copy(reg.points.begin()+middle+1, reg.points.end(), rightdata.begin());


		leftReg.points = leftdata;
		rightReg.points = rightdata;

		if(axis%2){
			leftReg.xmax = (rightdata.front().first)-0.000001;
			rightReg.xmin = rightdata.front().first;
		}else{
			leftReg.ymax = (rightdata.front().second)-0.000001;
			rightReg.ymin = rightdata.front().second;
		}

		tmpRegions.push_back(leftReg);
		tmpRegions.push_back(rightReg);
	}

	return tmpRegions;
}

int probstaticCache::mapNodeid2RegionId(int nid)
{
	return mapCoordinate2RegionId(nodeid2coordinate.at(nid));
}

int probstaticCache::mapCoordinate2RegionId(std::pair< double, double > coord)
{
	region reg;
	double x,y;
	BOOST_FOREACH(regionMap::value_type r, mapRegions)
	{
		reg = r.second;
		x = coord.first;
		y = coord.second;
		if(x <= reg.xmax && x >= reg.xmin && y <= reg.ymax && y >= reg.ymin)
			return r.first;
	}

 	return -1;
}

void probstaticCache::populateProbStructures()
{
	int temp, r1,r2,v1,v2;
	totalTrainingPairsSeen=0;

	cout << "probstaticcache::populateProbStructures! Init 2d array trainingQueriesPerRegionPair";
	//init array trainingQueriesPerRegionPair[mapSize][mapSize] dynamically
	trainingQueriesPerRegionPair = new int*[mapSize];
	for(int i=0; i<mapSize; i++)
		trainingQueriesPerRegionPair[i] = new int[mapSize];
	//init all values
	for(int i=0;i<mapSize;i++)
		for(int j=0;j<mapSize;j++)
			trainingQueriesPerRegionPair[i][j] = 0;
	cout << " ... Done" << endl;

	cout << "probstaticcache::populateProbStructures! calculating training stats";
	BOOST_FOREACH(intPair c, trainingSTPointPairs)
	{
		v1 = c.first;
		v2 = c.second;

		if(v1 > v2) {temp = v1; v1 = v2 ; v2 = temp;}

		if(v1 != v2)
		{
			r1 = mapNodeid2RegionId(v1);
			r2 = mapNodeid2RegionId(v2);
			if(r1 > r2) {temp = r1; r1 = r2 ; r2 = temp;}

			trainingQueriesPerRegionPair[r1][r2] =+1;

			totalTrainingPairsSeen++;
		}
	}
	cout << " ... Done" << endl;
}

double probstaticCache::calcScore(vector<int> spResult, boost::unordered_map<pair<int, int>, int> vSeen)
{
	double score = 0.0;
	calcScoreCounter++;
	int r1, r2;

	BOOST_FOREACH(int nid1, spResult)
	{
		r1 = mapNodeid2RegionId(nid1);
		BOOST_FOREACH(int nid2, spResult)
		{
			if(nid1 < nid2 && vSeen.find(make_pair(nid1,nid2)) == vSeen.end())
			{
				r2 = mapNodeid2RegionId(nid2);
				if(r1 > r2)
				{
					if(trainingQueriesPerRegionPair[r2][r1] != 0)
						score = score + (double)trainingQueriesPerRegionPair[r2][r1];
				}else{
					if(trainingQueriesPerRegionPair[r1][r2] != 0)
						score = score + (double)trainingQueriesPerRegionPair[r1][r2];
				}
			}
		}
	}
	return score;
}

void probstaticCache::fillCache(int numQueries, string inFn)
{
//	fillCacheFromQueriesFile(numQueries, inFn);
	fillCacheFromQueriesFileByStatistics(numQueries, inFn);
}

void probstaticCache::fillCacheFromQueriesFile(int numQueries, string inFn)
{
	vector<int> sp;
	CacheItem tmpItem;
	int i=0, spSize=0, filelines=0, curLine=0;
	double score;
	maxHeap mhCache;
	HeapEntry tmp;
	boost::unordered_map<std::pair<int,int>, int> vSeen, tmpVSeen; //map vertices seen
	boost::unordered_map<int, CacheItem> allSPpaths; //used to load all SP results into memory
	string line;
	std::vector<string> tokens;
	ifstream querysFile (inFn.c_str(), ios::in);

	//load queries from query file
	if( querysFile.is_open() )
	{
		cout << "probstaticcache::fillCacheFromQueriesFile! Starting to load queries into memory";
		getline(querysFile,line); //read first line: number of lines/queries in file.

		filelines = atoi(line.c_str());

		//Read all queries into memory and add each cacheItems score to max-heap mh.
		while( curLine < filelines-1 && curLine < numQueries)
		{
			getline(querysFile, line);
			boost::algorithm::split(tokens, line, boost::algorithm::is_space());
			curLine = atoi(tokens[0].c_str());
			spSize = atoi(tokens[1].c_str());

			//make new cache item
			sp.clear();
			for(int t = 0; t < spSize; t++)
				sp.push_back(atoi(tokens[t+2].c_str()));
			CacheItem e (i, sp, atoi(tokens[2].c_str()), atoi(tokens[spSize+1].c_str()));

			//add cache item to map holding all queries from file
			allSPpaths[i] = e;
			i++;
		}

	}

	cout << " ... Done" << endl;

	cout << "probstaticcache::fillCacheFromQueriesFile! Initial scoring started";
	//rank queries based on statistics
	startTime = clock();
	BOOST_FOREACH(intCacheitemMap::value_type icm, allSPpaths)
	{
		score = 0;
		sp = icm.second.item;

		score = calcScore(sp, tmpVSeen);

		tmp.id = icm.first;
		tmp.dist = score;
		mhCache.push(tmp);
	}
	endTime = clock();
	cout << " ... Done. TIME: " << (double(endTime-startTime))/CLOCKS_PER_SEC << endl;

	int curItem = 0;
	//Fill cache
	while(!mhCache.empty())
	{
		tmp= mhCache.top();
		mhCache.pop();

 		if(allSPpaths.find(tmp.id) != allSPpaths.end()){
			if(cache.hasEnoughSpace(allSPpaths.at(tmp.id))){

				tmpItem = allSPpaths.at(tmp.id);
				calcScoreMap[curItem] = calcScoreMap[curItem] +1; //# calcScore has been called for each cache insertion
				score = calcScore(tmpItem.item, vSeen);

				if(score >= mhCache.top().dist)
				{
					if(cache.insertItemWithScore(tmpItem, score))
					{
						BOOST_FOREACH(int v1, tmpItem.item)
						{
							BOOST_FOREACH(int v2, tmpItem.item)
							{
								if(v1 < v2)
								vSeen[make_pair(v1,v2)] = 1;
							}
						}
						curItem++;
					}
				}else{
					tmp.dist = (double) score;
					mhCache.push(tmp);
				}
			}
		}else{
			cout << "BLARG!! error occurred" << endl;
			cout << "tmpItem.id: " << tmpItem.id << ":" << tmpItem.item.size() << "(" << tmpItem.s << "," << tmpItem.t << ")" << endl;
		}
	}
	queryNumCacheFull = cache.numberOfNodesInCache();
	ts.itemsInCache = cache.numberOfItemsInCache();

	writeoutCacheCoordinates(ts.getTestName(), cache.getCacheContentVector(), nodeid2coordinate, ts.getSplits());
	writeoutTrainingCoordinates(ts.getTestName(), trainingSTPointPairs, nodeid2coordinate, ts.getSplits());
	writeoutTestCoordinates(ts.getTestName(), testSTPointPairs, nodeid2coordinate, ts.getSplits());
	cout << "scache::calcScoreCounter: " << calcScoreCounter << endl;
	//BOOST_FOREACH(intMap::value_type ii, calcScoreMap){cout << ii.first <<"," << ii.second << endl;}
	//cache.writeOutBitmaps();
}

void probstaticCache::fillCacheFromQueriesFileByStatistics(int numQueries, string inFn)
{
    vector<int> nodeidVectorRegion1, nodeidVectorRegion2, spResult, sp;
    int cid=0;
    pair<int,int> stPair; //to hold s and t for a shortest path query
    CacheItem tmpItem;
    double score;
    maxHeap mhCache;
	HeapEntry tmp;
    boost::unordered_map<std::pair<int,int>, int> vSeen;
    buildRegionId2NodeidVector();

    //finding all the region pairs for which the statistics has a non-zero entry for
    for(int i=0;i<mapSize;i++)
		for(int j=i;j<mapSize;j++)
			if(trainingQueriesPerRegionPair[i][j] != 0)
                regionPairsSeen.push_back(make_pair<int,int>(i,j));

    //filing up bucketlist with one entry from each region pair.
    boost::unordered_map<intPair,CacheItem> bucketList;

    BOOST_FOREACH(intPair rp, regionPairsSeen)
    {
        stPair = pickSTpair(rp);

		spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(stPair.first, stPair.second);
		//make new cache item
		CacheItem e (cid, spResult, spResult.front(), spResult.back());
        cid++;
        bucketList[rp] = e;
    }
    ts.nonEmptyRegionPairs = bucketList.size();

	cout << "probstaticcache::fillCacheFromQueriesFileByStatistics! Initial scoring of " << bucketList.size() << " items started";
	//rank queries based on statistics
	startTime = clock();
	BOOST_FOREACH(intPairCacheitemMap::value_type icm, bucketList)
	{
		score = 0;
		sp = icm.second.item;

		score = calcScore(sp, vSeen);

		tmp.pID = icm.first;
		tmp.dist = score;
		mhCache.push(tmp);
	}
	endTime = clock();
	cout << " ... Done. TIME: " << (double(endTime-startTime))/CLOCKS_PER_SEC << endl;

	//Fill cache
	while(!mhCache.empty())
	{
		tmp= mhCache.top();
		mhCache.pop();

 		if(bucketList.find(tmp.pID) != bucketList.end())
 		{
			if(cache.hasEnoughSpace(bucketList.at(tmp.pID)))
			{
				tmpItem = bucketList.at(tmp.pID);
				score = calcScore(tmpItem.item, vSeen);

				if(score >= mhCache.top().dist)
				{
					if(cache.insertItemWithScore(tmpItem, score))
					{
						BOOST_FOREACH(int v1, tmpItem.item)
						{
							BOOST_FOREACH(int v2, tmpItem.item)
							{
								if(v1 < v2)
								vSeen[make_pair(v1,v2)] = 1;
							}
						}
					}
					//find a new SP for the current region pair
					stPair = pickSTpair(tmp.pID);
                    spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(stPair.first, stPair.second);

                    CacheItem e (cid, spResult, spResult.front(), spResult.back());
                    cid++;
                    bucketList[tmp.pID] = e;

                    score = calcScore(spResult, vSeen);
                    tmp.dist = score;
                    mhCache.push(tmp);
				}else{
					tmp.dist = (double) score;
					mhCache.push(tmp);
				}
			}
		}else{
			cout << "BLARG!! error occurred" << endl;
			cout << "tmpItem.id: " << tmpItem.id << ":" << tmpItem.item.size() << "(" << tmpItem.s << "," << tmpItem.t << ")" << endl;
		}
	}
	queryNumCacheFull = cache.numberOfNodesInCache();
	ts.itemsInCache = cache.numberOfItemsInCache();

    writeoutCacheCoordinates(ts.getTestName(), cache.getCacheContentVector(), nodeid2coordinate, ts.getSplits());
	writeoutTrainingCoordinates(ts.getTestName(), trainingSTPointPairs, nodeid2coordinate, ts.getSplits());
	writeoutTestCoordinates(ts.getTestName(), testSTPointPairs, nodeid2coordinate, ts.getSplits());
	cout << "probstaticcache::calcScoreCounter: " << calcScoreCounter << endl;
//    cache.writeOutBitmaps();
//    cache.printNodesTokensPaths();

}

pair<int,int> probstaticCache::pickSTpair(pair<int,int> regionPair)
{
    int rid1, rid2, nid1, nid2;
    rid1 = regionPair.first;
    rid2 = regionPair.second;
    vector<int> nodeidVectorRegion1, nodeidVectorRegion2;

//    srand(clock());
    nodeidVectorRegion1 = regionid2nodeidVector.at(rid1);
    nodeidVectorRegion2 = regionid2nodeidVector.at(rid2);

    nid1 = nodeidVectorRegion1.at((int)rand()%nodeidVectorRegion1.size());
    nid2 = nodeidVectorRegion2.at((int)rand()%nodeidVectorRegion2.size());

    return std::make_pair<int,int>(nid1,nid2);
}

void probstaticCache::buildRegionId2NodeidVector()
{
    int rid;
    vector<int> nodeidVector;
    BOOST_FOREACH(intCoordinateMap::value_type ic, nodeid2coordinate)
    {
        rid = mapCoordinate2RegionId(ic.second);
        if(regionid2nodeidVector.find(rid) == regionid2nodeidVector.end())
            regionid2nodeidVector[rid] = nodeidVector;

        regionid2nodeidVector.at(rid).push_back(ic.first);
    }
}

void probstaticCache::writeoutCacheCoordinates(string testbasename, vector<CacheItem> cm, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits)
{
	cout << "probstaticcache::writeoutCacheCoordinates start!";
	vector<int> sp;
	int i=0;
	coordinate c;
	ofstream of;
	string fn = testbasename;
	string app = "D" + boost::lexical_cast<std::string>(numSplits);
 	app.append(".cache");
	fn.replace ((fn.size()-5), app.size(), app); //change file extention from .test to fnD#splits.cache
	of.open(fn.c_str(), ios::out | ios::ate | ios::app);

	BOOST_FOREACH(CacheItem ci, cm)
	{
		sp = ci.item;

		BOOST_FOREACH(int v, sp)
		{
			if(nodeid2coordinate.find(v) != nodeid2coordinate.end())
			{
				c = nodeid2coordinate.at(v);
				of << ci.getScore() << " " << c.first << " " << c.second << "\n";
			}else
				cout << "\nprobstaticcache::writeoutCacheCoordinates ERROR:  unknown node id." << endl;
		}
		of << endl;
		i++;
	}

	of.close();
	cout << " ... Done!" << endl;
}

int probstaticCache::writeoutTestCoordinates(string testbasename, std::vector<std::pair<int, int> > stPointPairs, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits)
{
	cout << "probstaticcache::writeoutTestCoordinates start!";
	vector<int> sp;
	coordinate c;
	string fn = testbasename;
	string app = "TEST.cache";
	fn.replace ((fn.size()-5), app.size(), app); //change file extention from .test TEST.cache
	ifstream ifile(fn.c_str());
	if (ifile) {ifile.close(); return 0;} //file already exist

	ofstream of(fn.c_str(), ios::out | ios::ate | ios::app);

	BOOST_FOREACH(intPair ip, stPointPairs)
	{
		sp = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(ip.first, ip.second);

		BOOST_FOREACH(int v, sp)
		{
			if(nodeid2coordinate.find(v) != nodeid2coordinate.end())
			{
				c = nodeid2coordinate.at(v);
				of << c.first << " " << c.second << "\n";
			}else
				cout << "\nprobstaticcache::writeoutTestCoordinates ERROR:  unknown node id." << endl;
		}
		of << endl;
	}
	cout << " ... Done!" << endl;
	of.close();
	return 1;
}

int probstaticCache::writeoutTrainingCoordinates(string testbasename, std::vector<std::pair<int, int> > stPointPairs, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits)
{
	cout << "probstaticcache::writeoutTrainingCoordinates start!";
	vector<int> sp;
	coordinate c;
	string fn = testbasename;
	string app = "TRAIN.cache";
	fn.replace ((fn.size()-5), app.size(), app); //change file extention from .test TEST.cache
	ifstream ifile(fn.c_str());
	if (ifile) {ifile.close(); return 0;} //file already exist

	ofstream of(fn.c_str(), ios::out | ios::ate | ios::app);

	BOOST_FOREACH(intPair ip, stPointPairs)
	{
		sp = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(ip.first, ip.second);

		BOOST_FOREACH(int v, sp)
		{
			if(nodeid2coordinate.find(v) != nodeid2coordinate.end())
			{
				c = nodeid2coordinate.at(v);
				of << c.first << " " << c.second << "\n";
			}else
				cout << "\nprobstaticcache::writeoutTrainingCoordinates ERROR:  unknown node id." << endl;
		}
		of << endl;
	}
    cout << " ... Done!" << endl;
	of.close();
	return 1;
}
