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



Probcache::Probcache(TestSetting ts)
{
	this->ts = ts;

	cache.init(ts);

	cacheSize = cache.size();
	numTotalQueries = 0;
	numCacheHits = 0;
	numDijkstraCalls = 0;

	readMapData();
	
	

	// *** Ken updated
	// function of "buildPoint2RegionIdMap" included below
	makePartitions(ts.getSplits());
	calcScoreCounter=0;
}

Probcache::~Probcache() {
	//dtor
	// 	for(int i=0;i<mapSize;i++)
	// 		delete [] trainingQueriesPerRegionPair[i];
	// 	delete [] trainingQueriesPerRegionPair;
}



//does not use argument, but does instead get its info from a .train file.
void Probcache::runQueryList() {
	intVector spResult;
	
	RoadGraph::mapObject(ts)->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.
	
	BOOST_FOREACH(intPair q, testSTPointPairs) { 
		
		numTotalQueries++;	// track usage info

		if (debug) 
			cout << "one, Probcache::checkCache! :cacheSize:" << (int) cache.size() <<"::"<< endl;
			
		if (cache.checkCache(q)) { // cacheHit
			numCacheHits++;
		} else {
			if(debug) cout << "three, Probcache::checkCache!" << endl;
			spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(q.first, q.second);
			numDijkstraCalls++;
		}
		
		if(debug) 
			cout << "four, Probcache::checkCache!" << endl;
	}
}

void Probcache::buildCache()
{
	cout<< "2.0 done" << endl;
double refTime = clock();
	readQueryLogData(QLOG_TEST);
	cout<< "2.1 done, " << getElapsedTime(refTime) << endl;

	readQueryLogData(QLOG_TRAIN);
	// sumUpNodes();   	// this statement too expensive
	cout<< "2.2 done, " << getElapsedTime(refTime) << endl;

	extractStatistics();
	double _build_stat_time=getElapsedTime(refTime);
	ts.setBuildStatisticsTime(_build_stat_time);
	calcScoreCounter=0;
	cout<< "2.3 done, " << _build_stat_time << endl;

	fillCache();
	double _fill_cache_time=getElapsedTime(refTime);
	ts.setFillCacheTime(_fill_cache_time);
	cout<< "2.4 done, " << _fill_cache_time << endl;
}


void Probcache::extractStatistics() {
	int temp, r1,r2,v1,v2;
	int totalTrainingPairsSeen=0;
	intPair p;

	cout << "Probcache::populateProbStructures! calculating training stats" << endl;
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



bool xxCompfunc(Point i,Point j) {return (i.first<j.first);} //sort based on x values
bool yyCompfunc(Point i,Point j) {return (i.second<j.second);} //sort based on y values


bool Probcache::makePartitions(int splits) {
	int axis = 0;
    cout << "Probcache::makePartitions start" << endl;
	double mapXmin,mapXmax,mapYmin,mapYmax;

	//partitionsMade = pow(2,splits);

	vector<Region> regionsVector;

	sort(points.begin(), points.end(), xxCompfunc);
	mapXmin = points.front().first;
	mapXmax = points.back().first;
	sort(points.begin(), points.end(), yyCompfunc);
	mapYmin = points.front().second;
	mapYmax = points.back().second;

	Region reg(0,mapXmin,mapXmax,mapYmin,mapYmax);
	reg.points=points;

	regionsVector.push_back(reg);
    cout << "two. Probcache::makePartitions splitting start: " << splits << " splits." << endl;
	for(int i=0; i<splits; i++) {
		split(regionsVector, axis);
		axis++;
	}
    cout << "three, Probcache::makePartitions set id of each region" << endl;
	//set the id of each region.

	int total_points=0;
	// update the region_id of points here
	for (uint region_id=0; region_id<regionsVector.size(); region_id++) {
		Region& r = regionsVector[region_id];
		r.id = region_id;
		mapRegions[region_id] = r;

		// *** KEN updated:  February 11
		// for each Point in "r.points" (for this partition only)
		BOOST_FOREACH(Point p, r.points) {
			Point2regionidMap[p] = region_id;
		}

		total_points+=r.points.size();
		//printf("region %d: %d\n",region_id,r.points.size());
	}
    cout << "total points: " << total_points << ", " << points.size() << endl;

	cout << "mapRegions size: " << mapRegions.size() << endl;

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

void Probcache::split(std::vector<Region>& regions, int axis) {
	std::vector<Region > tmpRegions;

	BOOST_FOREACH (Region reg, regions) {
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

		Region leftReg(axis,reg.xmin,reg.xmax,reg.ymin,reg.ymax);
		Region rightReg(axis,reg.xmin,reg.xmax,reg.ymin,reg.ymax);

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

int Probcache::mapNodeid2RegionId(int nid) {
    if(nodeid2Point.find(nid) != nodeid2Point.end())
        return mapPoint2RegionId(nodeid2Point[nid]);
    else {
        cout << "node " << nid << " is not in nodeid2Point"  << endl;
        return -1;
    }
}

int Probcache::mapPoint2RegionId(Point coord) {
    if(Point2regionidMap.find(coord) != Point2regionidMap.end())
        return Point2regionidMap[coord];
    else {
        cout << "Point (" << coord.first << "," << coord.second << ") does not fit in a region" << endl;
        return -1;
    }
}


double Probcache::calcScore(intVector& spResult, intPairSet& vSeen) {
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
		if (ts.testScenario == ARCH_SERVER)
			score = score * pow(spResult.size(),2);
		else
			score = score / spResult.size();
	}

	return score;
}


void Probcache::fillCache()
{
//	fillCacheFromQueriesFile(numQueries, inFn);
	fillCacheFromQueriesFileByStatistics();
}

/*void Probcache::fillCacheFromQueriesFile()
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
		cout << "Probcache::fillCacheFromQueriesFile! Starting to load queries into memory";
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

	cout << "Probcache::fillCacheFromQueriesFile! Initial scoring started";
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

	writeoutCachePoints(ts.getTestName(), cache.getCacheContentVector());
	cout << "scache::calcScoreCounter: " << calcScoreCounter << endl;
}*/

void Probcache::fillCacheFromQueriesFileByStatistics() {
    intVector spResult;
    intPairSet vSeen;
    maxHeap mhCache;
	int cid=0;

double refTime = clock();
	cout << "One. Start fillCacheFromQueriesFileByStatistics" << endl;
	
	buildRegionId2NodeidVector();
    buildRegionpair2NodepairVector();
    
	cout << "two1. Start fillCacheFromQueriesFileByStatistics" << endl;
    //finding all the region pairs for which the statistics has a non-zero entry for


	// fill up bucketlist with one entry from each region pair.
	RoadGraph* graph = RoadGraph::mapObject(ts);
    boost::unordered_map<intPair,CacheItem> bucketList;	// bucket list
	


cout << " @@2 TIME: " << getElapsedTime(refTime) << endl;

	ts.setNonEmptyRegionPairs(trainingQueriesPerRegionPair.size());

    cout << "three. Start fillCacheFromQueriesFileByStatistics " << trainingQueriesPerRegionPair.size() << endl;

	//rank queries based on statistics
	BOOST_FOREACH(intPairIntMap::value_type rpint, trainingQueriesPerRegionPair) {
        intPair rp = rpint.first;
	
        if (debug)
			cout << "3.1.  rp: (" << rp.first << "," << rp.second << ") " << endl;

		// pickSTpair is randomized; we try it several times in case it picks a pair with empty spResult
        bool isPathFound = false;
        for (int num_trials=0; num_trials<20; num_trials++ ) {	// a threshold
			intPair stPair = pickSTpair(rp);	// a shortest path query
            if (debug)
				cout << "3.2 stPair: (" << stPair.first << "," << stPair.second << ") ("
					<< rp.first << "," << rp.second << ") num_trials: " << num_trials << endl;

			spResult = graph->dijkstraSSSP(stPair.first, stPair.second);
            if (spResult.size()>0) {
				isPathFound = true;
				break; 
			}
        }

		if (isPathFound) {
            if(debug)
				cout << "3.3 spResult.size: " << spResult.size() << endl;
            //make new cache item
            
            bucketList[rp] = CacheItem(cid, spResult);
			cid++;

			HeapEntry tmp;
			tmp.pID = rp;
			tmp.dist = calcScore(spResult, vSeen);
			//printf(" \t$$ %d %f\n", e.id, tmp.dist );

			// [Feb 21]
			if (tmp.dist>0) // new
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
		CacheItem& tmpItem = bucketList.at(tmp.pID);
		if (cache.hasEnoughSpace(tmpItem)) {
			double curscore = calcScore(tmpItem.item, vSeen);

//printf("%f | %d %f %f\n",mhCache.top().dist, tmpItem.id, tmp.dist, score );

			if ( (mhCache.size()==0) || (curscore>=mhCache.top().dist) ) {

				// [Feb 21]
				if (curscore>0) {
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
				}

				//find a new SP for the current region pair
				intPair stPair = pickSTpair(tmp.pID);
				spResult = graph->dijkstraSSSP(stPair.first, stPair.second);
				
				bucketList[tmp.pID] = CacheItem(cid, spResult);
				cid++;
				curscore = calcScore(spResult, vSeen); // update score
			}

			// update the new entry in mhCache
			if (curscore>0) { 	// [Feb 21]
				tmp.dist = curscore;
				mhCache.push(tmp);
			}
		}

	}

printf(" *** num_cache_paths: %d, num_zero_paths: %d\n",num_cache_paths,num_zero_paths);
cout << " @@4 TIME: " << getElapsedTime(refTime) << endl;

	ts.setItemsInCache(cache.numberOfItemsInCache());
    plotCachePoints(cache.cache);
	
	// only uncomment these lines when we need to plot
	//plotShortestPaths(QLOG_TRAIN);
	//plotShortestPaths(QLOG_TEST);
	
	
	cout << "Probcache::calcScoreCounter: " << calcScoreCounter << endl;

cout << " @@5 TIME: " << getElapsedTime(refTime) << endl;

//    cache.writeOutBitmaps();
//    cache.printNodesTokensPaths();

}


//	std::vector<std::pair<int, int> > trainingSTPointPairs;
intPair Probcache::pickSTpair(intPair regionPair) {
	intPair ansPair;

	// [Feb 21] next task: implement "pick path WITHOUT REPLACEMENT!"
	vector<intPair>& nodePairVector = regionPair2nodePairVector[regionPair];
	if (debug)
		cout << "nodepairVector size: " << nodePairVector.size() << endl;
	
    if (nodePairVector.size()>0) { // pick path WITHOUT REPLACEMENT
		///random choice
		int slot=(int)rand()%nodePairVector.size();
		ansPair=nodePairVector[slot];
		
		nodePairVector[slot]=nodePairVector.back();	// WITHOUT REPLACEMENT
		nodePairVector.pop_back();
    } else { // random choice
	
        intVector& nodeVectorRegion1 = regionid2nodeidVector[regionPair.first];
        intVector& nodeVectorRegion2 = regionid2nodeidVector[regionPair.second];

        ///random choice
        int nid1 = nodeVectorRegion1[(int)rand()%nodeVectorRegion1.size()];
        int nid2 = nodeVectorRegion2[(int)rand()%nodeVectorRegion2.size()];

        if (nid1 < nid2)
            ansPair=make_pair<int,int>(nid1,nid2);
        else
            ansPair=make_pair<int,int>(nid2,nid1);
    }

	return ansPair;
}

void Probcache::buildRegionId2NodeidVector() {
    BOOST_FOREACH(intPointMap::value_type ic, nodeid2Point) {
        int rid = mapPoint2RegionId(ic.second);
        if (regionid2nodeidVector.find(rid) == regionid2nodeidVector.end())
            regionid2nodeidVector[rid] = vector<int>();
        regionid2nodeidVector[rid].push_back(ic.first);
    }
}

void Probcache::buildRegionpair2NodepairVector() {
    int rid1,rid2,temp;
    BOOST_FOREACH(intPair c, trainingSTPointPairs) {
        rid1 = mapNodeid2RegionId(c.first);
        rid2 = mapNodeid2RegionId(c.second);
        if (rid1 > rid2) {
			temp = rid1; rid1 = rid2; rid2 = temp;
		}
		
        intPair ip = make_pair(rid1,rid2);
        if (regionPair2nodePairVector.find(ip) == regionPair2nodePairVector.end())
            regionPair2nodePairVector[ip] = vector<intPair>();
        regionPair2nodePairVector[ip].push_back(c);
	}
}

