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
#include "Probcache.h"
#define debug false
#define FILL_CACHE_FROM_TRAINING_DATA false


probstaticCache::probstaticCache()
{

}

probstaticCache::probstaticCache(testsetting ts)
{
	this->ts = ts;

	cache.init(ts);

	cacheSize = cache.size();
	numTotalQueries = 0;
	numCacheHits = 0;
	numDijkstraCalls = 0;
	cacheFull = false;

	readMapData();

	// *** Ken updated
	// function of "buildCoordinate2RegionIdMap" included below
	makePartitions(ts.getSplits());
	calcScoreCounter=0;
}

probstaticCache::~probstaticCache()
{
// 	for(int i=0;i<mapSize;i++)
// 		delete [] trainingQueriesPerRegionPair[i];
// 	delete [] trainingQueriesPerRegionPair;
}

void probstaticCache::readQuery(intPair query) {
	numTotalQueries++;	// track usage info

	bool cacheHit = false;
	vector<int> spResult;

	if(debug) cout << "one, probstaticcache::checkCache! :cacheSize:" << (int) cache.size() <<"::"<< endl;
	if(cache.checkCache(query))
	{
		numCacheHits++;
		cacheHit = true;
	}


//if (query.first!=2512 || query.second!=99841) return;

	if(!cacheHit) {
		if(debug) cout << "three, probstaticcache::checkCache!" << endl;
		spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(query.first, query.second);
		numDijkstraCalls++;
	}
	if(debug) cout << "four, probstaticcache::checkCache!" << endl;
}

//does not use argument, but does instead get its info from a .train file.
void probstaticCache::readQueryList(std::vector<intPair> queryList)
{
	RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.
	BOOST_FOREACH(intPair q, testSTPointPairs ) { readQuery(q); }
}

void probstaticCache::readQueries(int numQueries, string inFn)
{
	cout<< "2.0 done" << endl;
double refTime = clock();
	readTestData(ts.queryFileName);
	cout<< "2.1 done, " << getElapsedTime(refTime) << endl;

	readTrainingData(ts.queryFileName);
	// sumUpNodes();   	// this statement too expensive
	cout<< "2.2 done, " << getElapsedTime(refTime) << endl;

	extractStatistics();
	double _build_stat_time=getElapsedTime(refTime);
	ts.setBuildStatisticsTime(_build_stat_time);
	calcScoreCounter=0;
	cout<< "2.3 done, " << _build_stat_time << endl;

	fillCache(numQueries, inFn);
	double _fill_cache_time=getElapsedTime(refTime);
	ts.setFillCacheTime(_fill_cache_time);
	cout<< "2.4 done, " << _fill_cache_time << endl;
}


// the following function is expensive, but it doesn't help the algorithm
/*void probstaticCache::sumUpNodes()
{
    int totalNodes = 0;
    vector<int> spR;
    BOOST_FOREACH(intPair tn, trainingSTPointPairs)
    {
        spR =RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(tn.first, tn.second);
        totalNodes = totalNodes + spR.size();
    }
    cout << "total nodes in SPs from training set: " <<totalNodes << " in " <<trainingSTPointPairs.size() << " paths" << endl;
}*/


void probstaticCache::extractStatistics() {
	int temp, r1,r2,v1,v2;
	int totalTrainingPairsSeen=0;
	intPair p;

	cout << "probstaticcache::populateProbStructures! calculating training stats" << endl;
	BOOST_FOREACH(intPair c, trainingSTPointPairs) {
		v1 = c.first;
		v2 = c.second;

		if (v1 > v2) {
			temp = v1; v1 = v2 ; v2 = temp;
		}

		if (v1 != v2) {
			r1 = mapNodeid2RegionId(v1);
			r2 = mapNodeid2RegionId(v2);
			if (r1 > r2) {
				temp = r1; r1 = r2 ; r2 = temp;
			}
			p = make_pair(r1,r2);
			if (trainingQueriesPerRegionPair.find(p) == trainingQueriesPerRegionPair.end())
				trainingQueriesPerRegionPair[p] = 1;
			else
				trainingQueriesPerRegionPair[p] =+ 1;
			totalTrainingPairsSeen++;
		}
	}
	cout << totalTrainingPairsSeen << " pairs, ... Done" << endl;
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
	int firstPnt, secondPnt, temp;
	string str;
	std::vector<string> tokens;

	fn.replace ((fn.size()-5), 5, ".test"); //change file extention from .train to .test
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

            if(firstPnt > secondPnt){temp = firstPnt; firstPnt = secondPnt; secondPnt = temp;}

			testSTPointPairs.push_back(std::make_pair(firstPnt,secondPnt));
			if(debug) cout << "tree, probstaticcache::readTestData end of fileline loop.!" << endl;
		}
	}
	testData.close();
	cout << "two, probstaticcache::readTestData end! size: " << testSTPointPairs.size() << endl;
}

//file on the form:
//record_id, S_id, T_id, S_x, S_y, T_x, T_y.
void probstaticCache::readTrainingData(string fn)
{
	cout << "probstaticcache::readTrainingData start!" << endl;
	std::pair<double, double> firstPair, secondPair;
	int firstPnt, secondPnt, temp;
	string str;
	std::vector<string> tokens;

	fn.replace ((fn.size()-5), 5, ".train"); //change file extention from .test to .train
	ifstream trainingData (fn.c_str(), ios::in); //*.train file
    cout << "training fn: " << fn << endl;
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

            if(firstPnt > secondPnt){temp = firstPnt; firstPnt = secondPnt; secondPnt = temp;}

			trainingSTPointPairs.push_back(std::make_pair(firstPnt,secondPnt));
		}
	}
	trainingData.close();
	cout << "probstaticcache::readTrainingData end! size:" << trainingSTPointPairs.size() << endl;
}

bool xxCompfunc(coordinate i,coordinate j) {return (i.first<j.first);} //sort based on x values
bool yyCompfunc(coordinate i,coordinate j) {return (i.second<j.second);} //sort based on y values


bool probstaticCache::makePartitions(int splits) {
	int axis = 0;
    cout << "probstaticcache::makePartitions start" << endl;
	double mapXmin,mapXmax,mapYmin,mapYmax;

	//partitionsMade = pow(2,splits);

	vector<region> regionsVector;

	sort(points.begin(), points.end(), xxCompfunc);
	mapXmin = points.front().first;
	mapXmax = points.back().first;
	sort(points.begin(), points.end(), yyCompfunc);
	mapYmin = points.front().second;
	mapYmax = points.back().second;

	region reg(0,mapXmin,mapXmax,mapYmin,mapYmax);
	reg.points=points;

	regionsVector.push_back(reg);
    cout << "two. probstaticcache::makePartitions splitting start: " << splits << " splits." << endl;
	for(int i=0; i<splits; i++) {
		split(regionsVector, axis);
		axis++;
	}
    cout << "three, probstaticcache::makePartitions set id of each region" << endl;
	//set the id of each region.

	int total_points=0;
	// update the region_id of points here
	for (uint region_id=0; region_id<regionsVector.size(); region_id++) {
		region& r = regionsVector[region_id];
		r.id = region_id;
		mapRegions[region_id] = r;

		// *** KEN updated:  February 11
		// for each coordinate in "r.points" (for this partition only)
		BOOST_FOREACH(coordinate p, r.points) {
			coordinate2regionidMap[p] = region_id;
		}

		total_points+=r.points.size();
		//printf("region %d: %d\n",region_id,r.points.size());
	}
    cout << "total points: " << total_points << ", " << points.size() << endl;

	cout << "mapRegions size: " << mapRegions.size() << endl;
	ts.setSplits(log2(mapRegions.size()));


	// *** Feb 14 added
	// compute "nodeid2regionid", to be used for calcScore function
	nodeid2regionid.clear();
	for (int nid=0;nid<mapSize;nid++) {
		int rid = mapNodeid2RegionId(nid);
		nodeid2regionid.push_back( rid );
		//printf("%d: %d\n",nid,rid);
	}


	if (!mapRegions.empty())
		return true;
	else
		return false;
}

void probstaticCache::split(std::vector<region>& regions, int axis) {
	std::vector< region > tmpRegions;

	BOOST_FOREACH (region reg, regions) {
        int size = reg.points.size();
		if (size == 0)
			continue;

		// override split axis
		/*double xdiff=reg.xmax-reg.xmin;
		double ydiff=reg.ymax-reg.ymin;
		if (xdiff>ydiff)
			axis=0;
		else
			axis=1;*/
		//printf(" axis: %d  \t %f %f\n",axis,xdiff,ydiff);


		if (axis%2==0)
			sort(reg.points.begin(), reg.points.end(), xxCompfunc);
		else
			sort(reg.points.begin(), reg.points.end(), yyCompfunc);

		region leftReg(axis,reg.xmin,reg.xmax,reg.ymin,reg.ymax);
		region rightReg(axis,reg.xmin,reg.xmax,reg.ymin,reg.ymax);

		int middle = size/2;
		leftReg.points.assign( reg.points.begin(), reg.points.begin()+middle );
		rightReg.points.assign( reg.points.begin()+middle, reg.points.end() );


		//printf("%d %d | %d %d\n",leftReg.points.size(),rightReg.points.size(),reg.points.size(),middle);
		if(axis%2==0) {
			leftReg.xmax = leftReg.points.back().first;		// left part split
			rightReg.xmin = rightReg.points.front().first; 	// right part split
		} else {
			leftReg.ymax = leftReg.points.back().second;		// left part split
			rightReg.ymin = rightReg.points.front().second;	// right part split
		}

		tmpRegions.push_back(leftReg);
		tmpRegions.push_back(rightReg);
	}

	regions.clear();
	regions=tmpRegions;

	tmpRegions.clear();	// clear temporary storage
}



// [Feb 13] Note: the following function is called heavily
// optimize this function tomorrow: use an "array" for direct "node-to-region" access!

int probstaticCache::mapNodeid2RegionId(int nid)
{
    if(nodeid2coordinate.find(nid) != nodeid2coordinate.end())
        return mapCoordinate2RegionId(nodeid2coordinate.at(nid));
    else{
        cout << "node " << nid << " does not exits in nodeid2coordinate"  << endl;
        return -1;
    }
}

int probstaticCache::mapCoordinate2RegionId(coordinate coord)
{
    if(coordinate2regionidMap.find(coord) != coordinate2regionidMap.end())
        return coordinate2regionidMap.at(coord);
    else{
        cout << "coordinate (" << coord.first << "," << coord.second << ") does not fit in a region" << endl;
        return -1;
    }
}


double probstaticCache::calcScore(intVector& spResult, intPairSet& vSeen) {
	double score = 0.0;
	bool isEmpty_vSeen=(vSeen.size()==0);

	calcScoreCounter++;

    // ADVANCED OPTIMIZED VERSION OF "calcScore" function, for SPC only

	// analyze the continuous region in the path
	int last_region=-2;
	vector<intPair> regionset;
	for (uint i=0;i<spResult.size();i++) {
		int nid = spResult[i];
		int rid = nodeid2regionid[nid];
		if (rid!=last_region) {
			last_region=rid;
			regionset.push_back(make_pair(rid,1));
		} else {
			regionset.back().second++;
		}
	}

	// do not repeat the same path
	int start_iter1=0;
	for (uint a=0;a<regionset.size();a++) {
		int r1=regionset[a].first;
		int length_iter1=regionset[a].second;

		// incorrect to use "a" directly because of "wrong offset
		int start_iter2=0;
		for (uint b=0;b<regionset.size();b++) {	// include "a" (different nodes in the same region)
			int r2=regionset[b].first;
			int length_iter2=regionset[b].second;
			if (b<a) {	// correct skipping
				start_iter2+=length_iter2;
				continue;
			}

			int temp_count=0;
			for (int i=0;i<length_iter1;i++) {
				int nid1 = spResult[start_iter1+i];
				for (int j=0;j<length_iter2;j++)
					if (start_iter1+i<start_iter2+j) 	// avoid duplicate check
				{
					//printf("\t\t^ %d %d \t %d %d\n",start_iter1+i,start_iter2+j,nid1,nid2);
					if (isEmpty_vSeen)
						temp_count++;
					else {
						int nid2 = spResult[start_iter2+j];
						intPair nodepair = (nid1<nid2)? make_pair(nid1,nid2):make_pair(nid2,nid1);
						if (vSeen.find(nodepair)==vSeen.end())  // if not in cache
							temp_count++;
					}
				}
			}

			if (temp_count>0) {
				intPair regionpair = (r1 < r2)? make_pair(r1,r2):make_pair(r2,r1);
				double temp_score=0;
				if (trainingQueriesPerRegionPair.find(regionpair) != trainingQueriesPerRegionPair.end())
					temp_score = trainingQueriesPerRegionPair.at(regionpair);
				score = score + temp_count*temp_score;
			}

			start_iter2+=length_iter2;
		}

		start_iter1+=length_iter1;
	}

	//printf("# %d %d\n",spResult.size(),regionset.size());
	//for (int a=0;a<regionset.size();a++)
	//	printf("\t%d %d\n",regionset[a].first,regionset[a].second);



	/*
	// SIMPLE VERSION THAT ALWAYS WORK
	BOOST_FOREACH(int nid1, spResult)
	{
		r1 = nodeid2regionid[nid1];
		BOOST_FOREACH(int nid2, spResult)
		{
			if (nid1 < nid2 && vSeen.find(make_pair(nid1,nid2)) == vSeen.end())
			{
				r2 = nodeid2regionid[nid2];
				intPair temp_pair = (r1 < r2)? make_pair(r1,r2):make_pair(r2,r1);
				double temp_score=0;
				if (trainingQueriesPerRegionPair.find(temp_pair) != trainingQueriesPerRegionPair.end())
					temp_score = trainingQueriesPerRegionPair.at(temp_pair);
				score = score + temp_score;	// commonCalc(r1,r2);
			}
		}
	}*/


	// final update
	if (spResult.size()>0) {
		if (ts.getTestScenario() == SERVER_SCENARIO)
			score = score * pow(spResult.size(),2);
		else
			score = score / spResult.size();
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
	intPairSet vSeen; //map vertices seen
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

			CacheItem e (i, sp);
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
		score = calcScore(sp, vSeen);
		tmp.id = icm.first;
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

 		if(allSPpaths.find(tmp.id) != allSPpaths.end()){
			if(cache.hasEnoughSpace(allSPpaths.at(tmp.id))){

				tmpItem = allSPpaths.at(tmp.id);
				score = calcScore(tmpItem.item, vSeen);

				if ( (mhCache.size()==0) || (score >= mhCache.top().dist) ) {
					if (cache.insertItemWithScore(tmpItem, score)) {
						BOOST_FOREACH(int v1, tmpItem.item) {	// update "vSeen"
							BOOST_FOREACH(int v2, tmpItem.item) {
								if (v1 < v2)
									vSeen.insert(make_pair(v1,v2));
							}
						}
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
	ts.setItemsInCache(cache.numberOfItemsInCache());

	writeoutCacheCoordinates(ts.getTestName(), cache.getCacheContentVector());
	writeoutTrainingCoordinates(ts.getTestName(), trainingSTPointPairs);
	writeoutTestCoordinates(ts.getTestName(), testSTPointPairs);
	cout << "scache::calcScoreCounter: " << calcScoreCounter << endl;
}

void probstaticCache::fillCacheFromQueriesFileByStatistics(int numQueries, string inFn) {
    intVector spResult;
    intPairSet vSeen;
    maxHeap mhCache;
	int cid=0;

double refTime = clock();
	cout << "One. Start fillCacheFromQueriesFileByStatistics" << endl;
	buildRegionId2NodeidVector();
    if (FILL_CACHE_FROM_TRAINING_DATA)
		buildRegionpair2NodepairVector();
    cout << "two1. Start fillCacheFromQueriesFileByStatistics" << endl;
    //finding all the region pairs for which the statistics has a non-zero entry for


	// fill up bucketlist with one entry from each region pair.
	RoadGraph* graph = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType());
    boost::unordered_map<intPair,CacheItem> bucketList;	// bucket list
	vector<intPair> regionPairsSeen; // only a local variable
	BOOST_FOREACH(intPairIntMap::value_type rpint, trainingQueriesPerRegionPair)
        regionPairsSeen.push_back(rpint.first);


cout << " @@2 TIME: " << getElapsedTime(refTime) << endl;

	ts.setNonEmptyRegionPairs(regionPairsSeen.size());

    cout << "three. Start fillCacheFromQueriesFileByStatistics " << regionPairsSeen.size() << endl;

	//rank queries based on statistics
	BOOST_FOREACH(intPair rp, regionPairsSeen) {
        bool isPathFound = true;
        int num_trials = 0;
        if (debug)
			cout << "3.1.  rp: (" << rp.first << "," << rp.second << ") " << endl;

        do {
			intPair stPair = pickSTpair(rp);	//to hold s and t for a shortest path query
            if (debug)
				cout << "3.2 stPair: (" << stPair.first << "," << stPair.second << ") ("
					<< rp.first << "," << rp.second << ") num_trials: " << num_trials << endl;

			spResult = graph->dijkstraSSSP(stPair.first, stPair.second);

            num_trials++;
            if (num_trials>20) // a threshold
				isPathFound = false; //As pickSTpair is randomized, we try it several times in case it picks a pair with empty spResult
        } while (spResult.size() == 0 && isPathFound == true);

		if (isPathFound) {
            if(debug)
				cout << "3.3 spResult.size: " << spResult.size() << endl;
            //make new cache item
            CacheItem e (cid, spResult);
            cid++;
            bucketList[rp] = e;

			HeapEntry tmp;
			tmp.pID = rp;
			tmp.dist = calcScore(spResult, vSeen);
			//printf(" \t$$ %d %f\n", e.id, tmp.dist );

			mhCache.push(tmp);
			if(debug)
				cout << "4. score: " << tmp.dist << " tmp.pID: (" << tmp.pID.first << "," << tmp.pID.second << ") tmp.dist: "
					<< tmp.dist << " spLength: " << spResult.size() << " mhCache size: " << mhCache.size() << endl;
        }
    }

cout << " @@3 TIME: " << getElapsedTime(refTime) << endl;

	// fill cache
	int num_cache_paths=0,num_zero_paths=0;
	while(!mhCache.empty()) {
		HeapEntry tmp = mhCache.top();
		mhCache.pop();

		// print score, cid, key
 		if (bucketList.find(tmp.pID)==bucketList.end()) {
			cout << "BLARG!! error occurred" << endl;
			exit(0);
		}

		// "tmp.pID" must be found in "bucketList" for the following lines
		if (cache.hasEnoughSpace(bucketList.at(tmp.pID))) {
			CacheItem tmpItem = bucketList.at(tmp.pID);
			double curscore = calcScore(tmpItem.item, vSeen);

//printf("%f | %d %f %f\n",mhCache.top().dist, tmpItem.id, tmp.dist, score );

			if ( (mhCache.size()==0) || (curscore>=mhCache.top().dist) ) {
				if (cache.insertItemWithScore(tmpItem, curscore)) {
					num_cache_paths++;
					if (curscore==0)
						num_zero_paths++;

					BOOST_FOREACH(int v1, tmpItem.item) {
						BOOST_FOREACH(int v2, tmpItem.item) {
							if (v1 < v2)
								vSeen.insert(make_pair(v1,v2));
						}
					}
				}

				//find a new SP for the current region pair
				intPair stPair = pickSTpair(tmp.pID);
				spResult = graph->dijkstraSSSP(stPair.first, stPair.second);

				CacheItem e (cid, spResult);
				cid++;
				bucketList[tmp.pID] = e;
				curscore = calcScore(spResult, vSeen);
			}

			// update the new entry in mhCache
			tmp.dist = curscore;
			mhCache.push(tmp);
		}

	}

printf(" *** num_cache_paths: %d, num_zero_paths: %d\n",num_cache_paths,num_zero_paths);
cout << " @@4 TIME: " << getElapsedTime(refTime) << endl;

	ts.setItemsInCache(cache.numberOfItemsInCache());

    writeoutCacheCoordinates(ts.getTestName(), cache.getCacheContentVector());
	writeoutTrainingCoordinates(ts.getTestName(), trainingSTPointPairs);
	writeoutTestCoordinates(ts.getTestName(), testSTPointPairs);
	cout << "probstaticcache::calcScoreCounter: " << calcScoreCounter << endl;

cout << " @@5 TIME: " << getElapsedTime(refTime) << endl;

//    cache.writeOutBitmaps();
//    cache.printNodesTokensPaths();

}


//	std::vector<std::pair<int, int> > trainingSTPointPairs;
intPair probstaticCache::pickSTpair(intPair regionPair) {
	intPair ansPair;

    if (FILL_CACHE_FROM_TRAINING_DATA) {
        vector<intPair>& nodePairVector = regionPair2nodePairVector.at(regionPair);
        if (debug)
			cout << "nodepairVector size: " << nodePairVector.size() << endl;

        ///random choice
//        ansPair=nodePairVector.at((int)rand()%nodePairVector.size());

        ///fixed values to check for correctness
        ansPair=nodePairVector.at((ts.randReplacement++)%nodePairVector.size());
    } else {
        vector<int>& nodeidVectorRegion1 = regionid2nodeidVector.at(regionPair.first);
        vector<int>& nodeidVectorRegion2 = regionid2nodeidVector.at(regionPair.second);

        ///random choice
//        int nid1 = nodeidVectorRegion1.at((int)rand()%nodeidVectorRegion1.size());
//        int nid2 = nodeidVectorRegion2.at((int)rand()%nodeidVectorRegion2.size());

        ///fixed values to check for correctness
        int nid1 = nodeidVectorRegion1.at((ts.randReplacement++)%nodeidVectorRegion1.size());
        int nid2 = nodeidVectorRegion2.at((ts.randReplacement++)%nodeidVectorRegion2.size());


        if (nid1 < nid2)
            ansPair=make_pair<int,int>(nid1,nid2);
        else
            ansPair=make_pair<int,int>(nid2,nid1);
    }

	return ansPair;
}

void probstaticCache::buildRegionId2NodeidVector() {
    int rid;
    vector<int> nodeidVector;
    BOOST_FOREACH(intCoordinateMap::value_type ic, nodeid2coordinate) {
        rid = mapCoordinate2RegionId(ic.second);
        if(regionid2nodeidVector.find(rid) == regionid2nodeidVector.end())
            regionid2nodeidVector[rid] = nodeidVector;

        regionid2nodeidVector.at(rid).push_back(ic.first);
    }
}

void probstaticCache::buildRegionpair2NodepairVector()
{
    int rid1,rid2,temp;
    vector<intPair> nodepairVector;
    intPair ip;
    BOOST_FOREACH(intPair c, trainingSTPointPairs)
	{
        rid1 = mapNodeid2RegionId(c.first);
        rid2 = mapNodeid2RegionId(c.second);
        if(rid1 > rid2){temp = rid1; rid1 = rid2; rid2 = temp;}
        ip = std::make_pair<int,int>(rid1,rid2);

        if(regionPair2nodePairVector.find(ip) == regionPair2nodePairVector.end())
            regionPair2nodePairVector[ip] = nodepairVector;

        regionPair2nodePairVector.at(ip).push_back(c);
	}
}

