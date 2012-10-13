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


// March 09
//const bool useRandomPaths=true;
const bool useRandomPaths=false;


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


//Gets its info from a .train file.
void Probcache::runQueryList() {
	intVector spResult;

cout << "****************************************" << endl;
cout << "****************************************" << endl;
cout << "****************************************" << endl;
cout << "****************************************" << endl;

	RoadGraph::mapObject(ts)->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.

    if(ts.useRange){
//B*******************************************
        int counter = 0;
//E*******************************************
        intPairVector& rQueryPairs = rangeObj.testSRQueryPairs;
        BOOST_FOREACH(intPair rq, rQueryPairs){
            intPairVector stQueryPairs = rangeObj.evalQuery(rq.first, rq.second);

//B*******************************************
            if(cache.nodeScores.size() != 0) {
                cout << counter++ << " QueryPair: (" << rq.first << "," << rq.second << ")" << endl;
                maxPairHeap mph;
                pair<int,int> mphPair;
                BOOST_FOREACH(intPair ns, cache.nodeScores){mph.push(ns);}
                while(mph.size() != 0){
                    mphPair = mph.top();
                    mph.pop();
                    cout << "(" << mphPair.first << "," << mphPair.second << ") ";
                }
                cout << endl;
            }
            cache.nodeScores.clear();
//E*******************************************
            BOOST_FOREACH(intPair query, stQueryPairs){
                numTotalQueries++;	// track usage info

                if (debug) cout << "one, rProbcache::checkCache! :cacheSize:" << (int) cache.size() <<"::"<< endl;

                if (cache.checkCache(query)) { // cacheHit
                    numCacheHits++;
                } else {
                    if(debug) cout << "three, rProbcache::checkCache!" << endl;
                    spResult = RoadGraph::computeSP(ts, query.first, query.second);
                    numDijkstraCalls++;
                }
            }
            //Each query should actually be validated after calculating SP, but as results are not used then this is easier
            rangeObj.rangeQuery(rq.first, rq.second, stQueryPairs); //to make runtime realistic, result not currently used
        }
	} else {
	BOOST_FOREACH(intPair testq, testSTPointPairs) {

		numTotalQueries++;	// track usage info

		if (debug) cout << "one, Probcache::checkCache! :cacheSize:" << (int) cache.size() <<"::"<< endl;

		if (cache.checkCache(testq)) { // cacheHit
			numCacheHits++;
		} else {
			if(debug) cout << "three, Probcache::checkCache!" << endl;
			spResult = RoadGraph::computeSP(ts, testq.first, testq.second);
			numDijkstraCalls++;
		}
	}
		if(debug) cout << "four, Probcache::checkCache!" << endl;
	}

//B****************************************************
cout << "THIS IS THE LEEEFT ONE 8" << endl;

    maxPairHeap mph;
    pair<int,int> mphPair;
    BOOST_FOREACH(intPair ns, cache.nodeScores){
        mph.push(ns);
        cout << "(" << ns.first << "," << ns.second << ") ";
    }
cout << endl << "BLOOOMO" << endl;
    while(mph.size() != 0){
        mphPair = mph.top();
        mph.pop();
        cout << "(" << mphPair.first << "," << mphPair.second << ") ";
    }
    cout << endl;
//E****************************************************
}

void Probcache::buildCache()
{
	cout<< "2.0 done" << endl;
    double refTime = clock();
    if(ts.useRange){
        rangeObj.init(ts);
        cout<< "2.1R done, " << ts.getElapsedTime(refTime) << endl;
    }else{
        readQueryLogData(QLOG_TEST);
        cout<< "2.1 done, " << ts.getElapsedTime(refTime) << endl;
        readQueryLogData(QLOG_TRAIN);
    }
	cout<< "2.2 done, " << ts.getElapsedTime(refTime) << endl;
	extractStatistics();
	double _build_stat_time=ts.getElapsedTime(refTime);
	ts.setBuildStatisticsTime(_build_stat_time);
	calcScoreCounter=0;
	cout<< "2.3 done, " << _build_stat_time << endl;

	fillCache();
	double _fill_cache_time=ts.getElapsedTime(refTime);
	ts.setFillCacheTime(_fill_cache_time);
	cout<< "2.4 done, " << _fill_cache_time << endl;
}

void Probcache::extractStatistics() {
    cout << "2.20 Probcache::populateProbStructures! calculating training stats" << endl;
	int temp, r1,r2,v1,v2;
	int totalTrainingPairsSeen=0;
	intPair p;
    intPairVector* ptrQueryPairs=NULL;
    intPairVector allSTqueryPairs;

//    boost::unordered_map<intPair, int > trainingPointsPerRegionPair; //statistics counting S or T point seen per region pair

    cout << "2.21 Probcache::populateProbStructures! calculating training stats" << endl;
    //find the set of all ST pairs from the training set.
    if(ts.useRange){
        intPairVector& rQueryPairs = rangeObj.trainingSRQueryPairs;
        cout << "2.22R Probcache::extractStatistics one rQueryPairs size: " << rQueryPairs.size() << endl;
        BOOST_FOREACH(intPair rq, rQueryPairs){
            intPairVector stQueryPairs = rangeObj.evalQuery(rq.first, rq.second);
            allSTqueryPairs.insert( allSTqueryPairs.end(), stQueryPairs.begin(), stQueryPairs.end() );

            rangeObj.rangeQuery(rq.first, rq.second, stQueryPairs); //to make runtime realistic, result not currently used
        }
		ptrQueryPairs=&allSTqueryPairs;
	} else {
		ptrQueryPairs=&trainingSTPointPairs;
	}

	intPairVector& stQueryPairs=(*ptrQueryPairs);

    cout << "2.23 Probcache::extractStatistics: primary statistics: ";
    //calculate primary statistics for trainingQueriesPerRegionPair
	BOOST_FOREACH(intPair cFirst, stQueryPairs) {
		v1 = cFirst.first;
		v2 = cFirst.second;

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
    cout << trainingQueriesPerRegionPair.size() << endl;
    cout << "2.24 Secondary statistics: ";
    //calculate second round statistics for trainingPointsPerRegionPair, for regionpairs only containing either S or T
    //After this emptyRegionsPerRegionPair will only contain region pairs that does not have any training points in either region
    boost::unordered_map<int, int > SandTlist; //just a list of all points
    BOOST_FOREACH(intPair cSecond, stQueryPairs) {
		v1 = cSecond.first;
		v2 = cSecond.second;
        if (SandTlist.find(v1) == SandTlist.end())
            SandTlist[v1] = 1;
        if (SandTlist.find(v2) == SandTlist.end())
			SandTlist[v2] = 1;
    }


    int rPoint1, rPoint2;
    intPair tmpRpair;
    BOOST_FOREACH(intPair cSecond, SandTlist)
    {
        rPoint1 = mapNodeid2RegionId(cSecond.first);
        if(regionid2nodeidVector.find(rPoint1) != regionid2nodeidVector.end()){
            BOOST_FOREACH(intRegionMap::value_type rMap, mapRegions)
            {
                rPoint2 = rMap.first;
                if(regionid2nodeidVector.find(rPoint2) != regionid2nodeidVector.end() && rPoint1 != rPoint2){
                    if(rPoint1>rPoint2)
                        swap(rPoint1,rPoint2);
                    tmpRpair = std::make_pair<int,int>(rPoint1, rPoint2);
                    if(trainingQueriesPerRegionPair.find(tmpRpair) == trainingQueriesPerRegionPair.end()){
                        if(trainingPointsPerRegionPair.find(tmpRpair) == trainingPointsPerRegionPair.end())
                            trainingPointsPerRegionPair[tmpRpair] = 1;
                        else
                            trainingPointsPerRegionPair[tmpRpair] =+ 1;
                    }
                }
            }
        }
    }
    cout << trainingPointsPerRegionPair.size() << "/" << SandTlist.size() << endl;

	cout << "2.25 Probcache::addStatisticsInfo: " << totalTrainingPairsSeen << " pairs, ... Done" << endl;
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
    cout << "xMin,xMax,yMin,yMax (regVector/points size): " << mapXmin << ","<< mapXmax << ","<< mapYmin << ","<< mapYmax << " ("<< regionsVector.size() << "/" << points.size() << ")"<< endl;
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

	// a simple cost model for Dijkstra and A*
	const double SPCOST_EXPONENT = (ts.useDijkstra)?(2.0):(1.5);

    /// ADVANCED OPTIMIZED VERSION OF "calcScore" function, for SPC only
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

			double temp_count=0;
			for (int i=0;i<length_iter1;i++) {
				int nid1 = spResult[start_iter1+i];
				for (int j=0;j<length_iter2;j++)
					if (start_iter1+i<start_iter2+j) 	// avoid duplicate check
				{
					//printf("\t\t^ %d %d \t %d %d\n",start_iter1+i,start_iter2+j,nid1,nid2);

					bool isNeeded=isEmpty_vSeen;
					if (!isNeeded) {
						int nid2 = spResult[start_iter2+j];
						intPair nodepair = (nid1<nid2)? make_pair(nid1,nid2):make_pair(nid2,nid1);
						if (vSeen.find(nodepair)==vSeen.end())  // if not in cache
							isNeeded=true;
					}

					if (isNeeded) {	// March 7
						if (ts.testScenario == ARCH_PROXY)
							temp_count++;
						else
							temp_count += pow( (start_iter2+j)-(start_iter1+i), SPCOST_EXPONENT );
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
	int cid=0, data=0;
    boost::unordered_map<intPair, int >* tmpPerRegion=NULL;

    double refTime = clock();
	cout << "One. Start fillCacheFromQueriesFileByStatistics" << endl;

	buildRegionId2NodeidVector();
    buildRegionpair2NodepairVector();

	cout << "two. Start fillCacheFromQueriesFileByStatistics" << endl;
    //finding all the region pairs for which the statistics has a non-zero entry for

	// fill up bucketlist with one entry from each region pair.
    boost::unordered_map<intPair,CacheItem> bucketList;	// bucket list

    cout << " fillCacheFromQueriesFileByStatistics @@2 TIME: " << ts.getElapsedTime(refTime) << endl;

    //Only refer to primary statistics.
	ts.setNonEmptyRegionPairs(trainingQueriesPerRegionPair.size());

    cout << "three. Start fillCacheFromQueriesFileByStatistics " << trainingQueriesPerRegionPair.size() << "/" << trainingPointsPerRegionPair.size() << endl;

    while(cache.getCacheUsed() <= ts.cacheSize){
        if(data>1) break;
        switch(data)
        {
            case 0:
                tmpPerRegion = &trainingQueriesPerRegionPair;
                data++;
                cout << "pick 0" << endl;
                break;
            case 1:
                tmpPerRegion = &trainingPointsPerRegionPair;
                data++;
                cout << "pick 1" << endl;
                break;
//            case 2:
//                tmpPerRegion = &emptyRegionsPerRegionPair;
//                break;
        }


        boost::unordered_map<intPair, int >& placeHolderPerRegionPair = (*tmpPerRegion);
        mhCache.empty(); //For safety, should not be neccecarry
        cout << "four: " << placeHolderPerRegionPair.size() << endl;
        int loopsize = placeHolderPerRegionPair.size();
        int progress = loopsize;
        //rank queries based on statistics
        BOOST_FOREACH(intPairIntMap::value_type rpint, placeHolderPerRegionPair) {
            intPair rp = rpint.first;
            cout << progress-- << "/";

            // pickSTpair is randomized; we try it several times in case it picks a pair with empty spResult
            bool isPathFound = false;
            while(!isPathFound) {	// a threshold
                intPair stPair = pickSTpair(rp);	// a shortest path query
                spResult = RoadGraph::computeSP(ts, stPair.first, stPair.second);
                if (spResult.size()>0)
                    if(calcScore(spResult, vSeen)>0) //added to make sure all ST pairs choose useful pairs (no 'waste' of pairs)
                        isPathFound = true;
            }
            cout << loopsize <<" ";
            //make new cache item
            bucketList[rp] = CacheItem(cid, spResult);
            cid++;

            HeapEntry tmp;
            tmp.pID = rp;
            tmp.dist = calcScore(spResult, vSeen);
            mhCache.push(tmp);   //if (tmp.dist>0)

            if(debug)
                cout << "4. score: " << tmp.dist << " tmp.pID: (" << tmp.pID.first << "," << tmp.pID.second << ") tmp.dist: "
                    << tmp.dist << " spLength: " << spResult.size() << " mhCache size: " << mhCache.size() << endl;
        }

    cout << "\n @@3 TIME: " << ts.getElapsedTime(refTime) << endl;

        // fill cache
        int num_cache_paths=0,num_zero_paths=0;
        while(!mhCache.empty()) {
            HeapEntry tmp = mhCache.top();
            mhCache.pop();
            cout << "(" << tmp.pID.first <<"/" << tmp.pID.second << ")-" << num_cache_paths << " ";
            // print score, cid, key
            if (bucketList.find(tmp.pID)==bucketList.end()) {
                cout << "BLARG!! error occurred" << endl;
                exit(0);
            }

            // "tmp.pID" must be found in "bucketList" for the following lines
            CacheItem& tmpItem = bucketList.at(tmp.pID);
            if (cache.hasEnoughSpace(tmpItem)) {
                double curscore = calcScore(tmpItem.item, vSeen);

                if ( (mhCache.size()==0) || (curscore>=mhCache.top().dist) ) {
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

                    int try_limit=100; //limit for how many times we want to try before giving up on finding a pair with a positive
                                       //score from the region pair. WARNING: increase will result in significant impact on running time
                    for(int c=0;c<try_limit;c++){
                        //find a new SP for the current region pair
                        intPair stPair = pickSTpair(tmp.pID);
                        spResult = RoadGraph::computeSP(ts, stPair.first, stPair.second);
                        curscore = calcScore(spResult, vSeen); // update score
                        if(curscore>0){
                            bucketList[tmp.pID] = CacheItem(cid, spResult);
                            cid++;
                            break;
                        }
                    }
                }

                // update the new entry in mhCache
                if (curscore>0) { 	// [Feb 21]
                    tmp.dist = curscore;
                    mhCache.push(tmp);
                }
            }
        }
        printf("\n *** num_cache_paths: %d, num_zero_paths: %d\n",num_cache_paths,num_zero_paths);
        cout << " @@4." << data << " TIME: " << ts.getElapsedTime(refTime) << endl;
    }

	ts.setItemsInCache(cache.numberOfItemsInCache());
    plotCachePoints(cache.cache);

	// only uncomment these lines when we need to plot
	//plotShortestPaths(QLOG_TRAIN);
	//plotShortestPaths(QLOG_TEST);


	cout << "Probcache::calcScoreCounter: " << calcScoreCounter << endl;

cout << " @@5 TIME: " << ts.getElapsedTime(refTime) << endl;

//    cache.writeOutBitmaps();
//    cache.printNodesTokensPaths();

}

//	std::vector<std::pair<int, int> > trainingSTPointPairs;
intPair Probcache::pickSTpair(intPair regionPair) {
	intPair ansPair;
	intPointMap& poiCoordmap = rangeObj.poiCoordMap;

	// [Feb 21] next task: implement "pick path WITHOUT REPLACEMENT!"
	vector<intPair>& nodePairVector = regionPair2nodePairVector[regionPair];
	if (debug)
		cout << "nodepairVector size: " << nodePairVector.size() << endl;

    if (nodePairVector.size()>0) { // pick path WITHOUT REPLACEMENT
		///random choice
		int slot=(int)rand()%nodePairVector.size();
		ansPair=nodePairVector[slot];

        if(ts.useRange){
            if (poiCoordmap.find(ansPair.second) == poiCoordmap.end()){
                int size = nodePairVector.size();
                for(slot=0; slot<size; slot++){
                    ansPair=nodePairVector[slot];
                    if(poiCoordmap.find(ansPair.second) != poiCoordmap.end()){
                        break;
                    }
                }
                if (poiCoordmap.find(ansPair.second) == poiCoordmap.end()){
                    //cout << "(1) FAILURE: Region pair does not contain valid node ids for query " << ansPair.second <<"," << ansPair.first << endl;
                    int i=0;
                    int tmpPOI=-1;
                    int randPOIindex = (int)rand()%poiCoordmap.size();

                    while(true)
                    {
                        BOOST_FOREACH(intPointMap::value_type poi, poiCoordmap)
                        {
                            i++;
                            if(i == randPOIindex){
                                tmpPOI = poi.first;
                                break;
                            }
                        }
                        if(poiCoordmap.find(tmpPOI) != poiCoordmap.end()) break;
                    }
                    ansPair.second = tmpPOI;
                }
            }
        }

		nodePairVector[slot]=nodePairVector.back();	// WITHOUT REPLACEMENT
		nodePairVector.pop_back();


    } else { // random choice
        intVector& nodeVectorRegion1 = regionid2nodeidVector[regionPair.first];
        intVector& nodeVectorRegion2 = regionid2nodeidVector[regionPair.second];

        ///random choice
        int nid1 = nodeVectorRegion1[(int)rand()%nodeVectorRegion1.size()];
        int nid2 = nodeVectorRegion2[(int)rand()%nodeVectorRegion2.size()];

		// March 09
		if (useRandomPaths==false) { // force path removal later
			nid1 = nodeVectorRegion1[0];
			nid2 = nodeVectorRegion2[0];
		}

        if(ts.useRange){
            if (poiCoordmap.find(nid2) != poiCoordmap.end() &&
                poiCoordmap.find(nid2) == poiCoordmap.end()){ swap(nid1, nid2);}

            if (poiCoordmap.find(nid2) == poiCoordmap.end()){

                BOOST_FOREACH(int possibleNid, nodeVectorRegion2){
                    if(poiCoordmap.find(possibleNid) != poiCoordmap.end()){
                        nid2 = possibleNid;
                        break;
                    }
                }
                if (poiCoordmap.find(ansPair.second) == poiCoordmap.end()){
                    //cout << "(2) FAILURE: Region pair does not contain valid node ids for query " << ansPair.second <<"," << ansPair.first << endl;
                    int randPOIindex = (int)rand()%poiCoordmap.size();
                    int i=0;
                    while(true)
                    {
                        BOOST_FOREACH(intPointMap::value_type poi, poiCoordmap)
                        {
                            i++;
                            if(i == randPOIindex){
                                nid2 = poi.first;
                                break;
                            }
                        }
                        if(poiCoordmap.find(nid2) != poiCoordmap.end()) break;
                    }
                }
            }
        }
        ansPair=make_pair<int,int>(nid1,nid2);
    }

    if(poiCoordmap.find(ansPair.second) == poiCoordmap.end()){
        cout << "\n---------------------------------\n PROBLEM: wrong POI point: (" << ansPair.first << ")"
        << ansPair.second << "\n---------------------------------" << endl;
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

void swap(int& a,int& b) {
	int t = a;
	a = b;
	b = t;
}

void Probcache::buildRegionpair2NodepairVector() {
    int distinct_count=0;
    intPairVector* ptrQueryPairs=NULL;
    intPairVector allSTqueryPairs;
    int rid1;
    int rid2;

    if(ts.useRange){
        intPairVector& rQueryPairs = rangeObj.trainingSRQueryPairs;
        BOOST_FOREACH(intPair rq, rQueryPairs){
            intPairVector stQueryPairs = rangeObj.evalQuery(rq.first, rq.second);
            allSTqueryPairs.insert( allSTqueryPairs.end(), stQueryPairs.begin(), stQueryPairs.end() );

            if(!ts.skipSPcalc)
                rangeObj.rangeQuery(rq.first, rq.second, stQueryPairs); //to make runtime realistic, result not currently used
        }
		ptrQueryPairs=&allSTqueryPairs;
	} else {
		ptrQueryPairs=&trainingSTPointPairs;
	}


	BOOST_FOREACH(intPair queryPairs, *ptrQueryPairs) {
        rid1 = mapNodeid2RegionId(queryPairs.first);
        rid2 = mapNodeid2RegionId(queryPairs.second);

        if (!ts.useRange && rid1 > rid2)
			swap(rid1,rid2);

        intPair ip = make_pair(rid1,rid2);
        if (regionPair2nodePairVector.find(ip) == regionPair2nodePairVector.end())
            regionPair2nodePairVector[ip] = vector<intPair>();

		// March 11
		vector<intPair>& nodepairVec=regionPair2nodePairVector[ip];
		if (queryPairs.first>queryPairs.second)
			swap(queryPairs.first,queryPairs.second);
		if (find(nodepairVec.begin(),nodepairVec.end(),queryPairs)==nodepairVec.end()) {
			nodepairVec.push_back(queryPairs);
			distinct_count++;
		}
	}
	printf("** distinct training queries: %d\n",distinct_count);
}

