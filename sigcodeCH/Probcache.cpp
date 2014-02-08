/****************************************************************************************
 *   Copyright (C) 2011 by Jeppe Rishede 						*
 *   jenslyn42@gmail.com								*
 *											*
 ***************************************************************************************/

#define debugProbc false
#define debugRand false
#define debugkskip false


Probcache::Probcache(TestSetting ts) {
  this->ts = ts;
  
  cache.init(ts);
  stats("mem.use", "Probcache::Probcache() after cache.init()");
  cacheSize = cache.size();
  numTotalQueries = 0;
  numCacheHits = 0;
  numDijkstraCalls = 0;

  readMapData();
  stats("mem.use", "Probcache::Probcache() after read data");
  
  makePartitions(ts.getSplits());
  stats("mem.use", "Probcache::Probcache() after make partitions");
  calcScoreCounter=0;
}

Probcache::~Probcache() { 
  trainingSTPointPairs.clear();
  testSTPointPairs.clear();
  points.clear();
  Point2Nodeid.clear();
  nodeid2Point.clear();
  queries.clear();
  trainingQueriesPerRegionPair.clear();
  nodeid2regionid.clear();
  Point2regionidMap.clear();
  regionid2nodeidVector.clear();
  regionPair2nodePairVector.clear();
  mapRegions.clear();
}


void Probcache::runQueryList() {
  cout << "Probcache::runQueryList()" << endl;
  intVector spResult;
  unsigned long existingNodesvisited; // *1* used for keeping track of number of nodes visited by a SP call.
  RoadGraph::mapObject(ts)->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.
  BOOST_FOREACH(intPair q, testSTPointPairs) { 

    numTotalQueries++;// track usage info
    if (debugProbc) cout << "one, Probcache::checkCache! :cacheSize:" << (int) cache.size() <<"::"<< endl;

    if (cache.checkCache(q)) { // cacheHit
      numCacheHits++;
    } else {
      if(debugProbc) cout << "three, Probcache::checkCache!" << endl;
      existingNodesvisited = RoadGraph::mapObject(ts)->numNodeVisits; // *1* nodes visited before call
      
      if(ts.executeTrainingWorkload) spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(q.first, q.second);
      numDijkstraCalls++;
      //////////////////////////////////
//       if(numTotalQueries > 9) exit(1);
     /* intVector spResultLong, spResultShort;
      RoadGraph::mapObject(ts)->setConcisePathUse(false);
      spResultLong = RoadGraph::mapObject(ts)->dijkstraSSSP(q.first, q.second);   

      RoadGraph::mapObject(ts)->setConcisePathUse(true);
      spResultShort = RoadGraph::mapObject(ts)->dijkstraSSSP(q.first, q.second);
	
      cout << "full/concise - ratio: " << spResultLong.size() << " / " << spResultShort.size() << " - " << (double)spResultLong.size() / (double)spResultShort.size() << endl;
      if(numDijkstraCalls > 200) break;
      */ ////////////////////////////////
    }

    if(debugProbc) cout << "four, Probcache::checkCache!" << endl;
  }
}

void Probcache::buildCache() {
  cout<< "2.0 done" << endl;
  stats("mem.use", "2.0 done");
  double refTime = clock();
  readQueryLogData(QLOG_TEST);
  cout<< "2.1 done, " << getElapsedTime(refTime) << endl;
  stats("mem.use", "2.1 done");
  readQueryLogData(QLOG_TRAIN);
  cout<< "2.2 done, " << getElapsedTime(refTime) << endl;
  stats("mem.use", "2.2 done");
  extractStatistics();
  double _build_stat_time=getElapsedTime(refTime);
  ts.setBuildStatisticsTime(_build_stat_time);
  calcScoreCounter=0;
  cout<< "2.3 done, " << _build_stat_time << endl;
  stats("mem.use", "2.3 done");
  fillCache();
  double _fill_cache_time=getElapsedTime(refTime);
  ts.setFillCacheTime(_fill_cache_time);
  cout<< "2.4 done, " << _fill_cache_time << endl;
  stats("mem.use", "2.4 done");
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
        trainingQueriesPerRegionPair[p] += 1;
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

  int total_points=0;
  // update the region_id of points here
  for (uint region_id=0; region_id<regionsVector.size(); region_id++) {
    Region& r = regionsVector[region_id];
    r.id = region_id;
    mapRegions[region_id] = r;

    BOOST_FOREACH(Point p, r.points) {
      Point2regionidMap[p] = region_id;
    }

    total_points+=r.points.size();
  }
  cout << "total points: " << total_points << ", " << points.size() << endl;
  cout << "mapRegions size: " << mapRegions.size() << endl;

  // compute "nodeid2regionid", to be used for calcScore function
  nodeid2regionid.clear();
  for (int nid=0;nid<mapSize;nid++) {
    int rid = mapNodeid2RegionId(nid);
    nodeid2regionid.push_back( rid );
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

    if (axis%2==0)
      sort(reg.points.begin(), reg.points.end(), xxCompfunc);
    else
      sort(reg.points.begin(), reg.points.end(), yyCompfunc);

    Region leftReg(axis,reg.xmin,reg.xmax,reg.ymin,reg.ymax);
    Region rightReg(axis,reg.xmin,reg.xmax,reg.ymin,reg.ymax);

    int middle = size/2;
    leftReg.points.assign( reg.points.begin(), reg.points.begin()+middle );
    rightReg.points.assign( reg.points.begin()+middle, reg.points.end() );

    if(axis%2==0) {
      leftReg.xmax = leftReg.points.back().first;  // left part split
      rightReg.xmin = rightReg.points.front().first;   // right part split
    }else {
      leftReg.ymax = leftReg.points.back().second;    // left part split
      rightReg.ymin = rightReg.points.front().second;  // right part split
    }

    tmpRegions.push_back(leftReg);
    tmpRegions.push_back(rightReg);
  }

  regions.clear();
  regions=tmpRegions;

  tmpRegions.clear();  // clear temporary storage
}

// Note: the following function is called heavily
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
/*

intVectorMap regToCandidates; //for each region participating in a pair with non-zero benefit, store the nodes mapping to this region.
intPairSet BenefitRegPairs; //store each region pair that has a non-zero benefit score.*/
double Probcache::calcScore(intVector& spResult, intPairSet& vSeen) {
  intVector placeholder;
  intVectorMap placeholder2;
  intPairSet placeholder3;
  calcScore(spResult, vSeen, placeholder, placeholder2, placeholder3, false );
}


double Probcache::calcScore(intVector& spResult, intPairSet& vSeen, intVector& spConcise, intVectorMap& regToCandidates, intPairSet& benefitRegPairs, bool useStatArgs) {
  //cout << "Probcache::calcScore(spResult.size():" << spResult.size() << ", vSeen.size():" <<vSeen.size() << ")" << endl;
  double score = 0.0;
  bool isEmpty_vSeen=(vSeen.size()==0);

  calcScoreCounter++;
  
  boost::unordered_set<int> conciseRegions; //all the regions nodes in spConcise maps to
  if(useStatArgs){
    BOOST_FOREACH(int nid, spConcise) 
      conciseRegions.insert(nodeid2regionid[nid]);
  }
  // analyze the continuous region in the path
  int last_region=-2;
  vector<intPair> regionset;
  for (uint i=0;i<spResult.size();i++) {
    int nid = spResult[i];
    int rid = nodeid2regionid[nid];
    if (rid!=last_region) {
      last_region=rid;
      regionset.push_back(make_pair(rid,1));
    }else
      regionset.back().second++;
    
    //add nid to Rid->{Nid} map, if nid is not part of spConcise or some node in spConcise already maps to rid
    if(useStatArgs && find(spConcise.begin(), spConcise.end(), nid) == spConcise.end() && conciseRegions.find(rid) == conciseRegions.end()){
      if(regToCandidates.find(rid) == regToCandidates.end())
	regToCandidates[rid] = vector<int>();
      regToCandidates[rid].push_back(nid);
    }     
  }
    
  // do not repeat the same path
  int start_iter1=0;
  for (uint a=0;a<regionset.size();a++) {
    int r1=regionset[a].first;
    int length_iter1=regionset[a].second;

    int start_iter2=0;
    for (uint b=0;b<regionset.size();b++) {// include "a" (different nodes in the same region)
      int r2=regionset[b].first;
      int length_iter2=regionset[b].second;
      if (b<a) {// correct skipping
	start_iter2+=length_iter2;
	continue;
      }

      //Incorrect to use "a" directly because of wrong offset
      int temp_count=0; //incremented when a node-pair, not already in the cache, is found
      for (int i=0;i<length_iter1;i++) {
	int nid1 = spResult[start_iter1+i];
	for (int j=0;j<length_iter2;j++){
	  if (start_iter1+i<start_iter2+j){ // avoid duplicate check
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
      }
      if (temp_count>0) {
	intPair regionpair = (r1 < r2)? make_pair(r1,r2):make_pair(r2,r1);
	double temp_score=0;
	if (trainingQueriesPerRegionPair.find(regionpair) != trainingQueriesPerRegionPair.end())
	  temp_score = trainingQueriesPerRegionPair.at(regionpair);
	score = score + temp_count*temp_score;
	
	//Identify those pairs of region ids with positive benefit, where neither region can contribute any benefit to the path alone.
	if(useStatArgs && temp_score>0 && conciseRegions.find(r1) != conciseRegions.end() && conciseRegions.find(r2) != conciseRegions.end()){
	  benefitRegPairs.insert(regionpair);
	}
      }
    
      start_iter2+=length_iter2;
    }
    start_iter1+=length_iter1;
  }

  // final update
  if (spResult.size()>0 && ts.devideScoreByLength) {
    if (ts.testScenario == ARCH_SERVER)
      score = score * pow(spResult.size(),2);
    else
      score = score / spResult.size();
  }
  return score;
}
	
void Probcache::fillCache(){
	fillCacheFromQueriesFileByStatistics();
}

void Probcache::fillCacheFromQueriesFileByStatistics() {
  intVector spResult;
  intPairSet vSeen;
  maxHeap mhCache;
  int cid=0;
  bool isPathFound = false;
	
  double refTime = clock();
  cout << "One. Start fillCacheFromQueriesFileByStatistics" << endl;
	
  buildRegionId2NodeidVector();
  buildRegionpair2NodepairVector();

  // fill up bucketlist with one entry from each region pair.
  RoadGraph* graph = RoadGraph::mapObject(ts);
  boost::unordered_map<intPair,CacheItem> bucketList;	// bucket list
  intPair stPair;
	
  cout << " @@2 TIME: " << getElapsedTime(refTime) << endl;

  ts.setNonEmptyRegionPairs(trainingQueriesPerRegionPair.size());

  cout << "three. Start fillCacheFromQueriesFileByStatistics " << trainingQueriesPerRegionPair.size() << endl;

  //rank queries based on statistics
  BOOST_FOREACH(intPairIntMap::value_type rpint, trainingQueriesPerRegionPair) {
    intPair rp = rpint.first;
    if (debugProbc)
      cout << "3.1. rp: (" << rp.first << "," << rp.second << ") " << endl;

    // pickSTpair is randomized; we try it several times in case it picks a pair with empty spResult
    isPathFound = false;
    for (int num_trials=0; num_trials<20; num_trials++ ) {	// a threshold
      stPair = pickSTpair(rp);	// a shortest path query
      if (debugProbc)
	cout << "3.2 stPair: (" << stPair.first << "," << stPair.second << ") (" << rp.first << "," << rp.second << ") num_trials: " << num_trials << endl;

      spResult = graph->dijkstraSSSP(stPair.first, stPair.second);
      if (debugProbc) cout << "3.2.1 rp: (" << rp.first << "," << rp.second << ") " << endl;
      if (spResult.size()>0) {
	isPathFound = true;
	break; 
      }
    }

    if (isPathFound) {
      if(debugProbc)
	cout << "3.3 spResult.size: " << spResult.size() << endl;

      //optimal path
      if(ts.testSPtype == SPTYPE_OPTIMAL) spResult = optiPath(stPair, vSeen, false, ts.optiNum);
      
      //make new cache item
      bucketList[rp] = CacheItem(cid, spResult);
      cid++;

      HeapEntry tmp;
      tmp.pID = rp;
      tmp.dist = calcScore(spResult, vSeen);

      if (tmp.dist>0) // new
	mhCache.push(tmp);

      if(debugProbc)
	cout << "4. score: " << tmp.dist << " tmp.pID: (" << tmp.pID.first << "," << tmp.pID.second << ") tmp.dist: " << tmp.dist << " spLength: " << spResult.size() << " mhCache size: " << mhCache.size() << endl;
    }
  }

  cout << " @@3 TIME: " << getElapsedTime(refTime) << endl;
  stats("mem.use", "Probcache::fillcache() @@3 done");
  // fill cache
  int num_cache_paths=0,num_zero_paths=0;
  while(!mhCache.empty()) {
    //cout << "mhCache size:" << mhCache.size() << endl;
    HeapEntry tmp = mhCache.top();
    mhCache.pop();

    if (bucketList.find(tmp.pID)==bucketList.end()) {
      cout << "BLARG!! error occurred" << endl;
      exit(0);
    }

    // "tmp.pID" must be found in "bucketList" for the following lines
    CacheItem& tmpItem = bucketList.at(tmp.pID);
   // cout << "hasEnoughSpace / item size: ";
   // (cache.hasEnoughSpace(tmpItem))? cout << "true / " : cout << "false / ";
   // cout << tmpItem.item.size();
    
    if (cache.hasEnoughSpace(tmpItem)) {
      double curscore = calcScore(tmpItem.item, vSeen);

      if ( (mhCache.size()==0) || (curscore>=mhCache.top().dist) ) {
	if (curscore>0) {
	  if (cache.insertItemWithScore(tmpItem, curscore)) {
	    num_cache_paths++;
 
	    //cout << " ADDED! Numpaths: " << num_cache_paths;
	    BOOST_FOREACH(int v1, tmpItem.item) {
	      BOOST_FOREACH(int v2, tmpItem.item) {
		if (v1 < v2)
		  vSeen.insert(make_pair(v1,v2));
	      }
	    }
	  }
	}
	//find a new SP for the current region pair
	// pickSTpair is randomized; we try it several times in case it picks a pair with empty spResult
	for (int num_trials=0; num_trials<20; num_trials++ ) {	// num_trials: a threshold
	  stPair = pickSTpair(tmp.pID);

	  if(ts.testSPtype == SPTYPE_OPTIMAL) spResult = optiPath(stPair, vSeen, false, ts.optiNum);
	  else 
	    spResult = graph->dijkstraSSSP(stPair.first, stPair.second);
	  curscore = calcScore(spResult, vSeen); // update score  

	  if (curscore > 0)
	    break;	    
	}	
	bucketList[tmp.pID] = CacheItem(cid, spResult);
	cid++;
      }
      // update the new entry in mhCache
      if (curscore>0) {
	tmp.dist = curscore;
	mhCache.push(tmp);
      }
    }
  }

  printf("\n*** num_cache_paths: %d, numCacheBits left: %d\n",num_cache_paths, cache.getCachespaceleftBits());
  cout << " @@4 TIME: " << getElapsedTime(refTime) << endl;

  ts.setNodesInCache(cache.numberOfNodesInCache());
  ts.setItemsInCache(cache.numberOfItemsInCache());
  ts.setUnusedCacheBits(cache.getCachespaceleftBits());

  // only uncomment these lines when we need to plot
  //plotCachePoints(cache.cache);
  //plotShortestPaths(QLOG_TRAIN);
  //plotShortestPaths(QLOG_TEST);

  cout << "Probcache::calcScoreCounter: " << calcScoreCounter << endl;

  cout << " @@5 TIME: " << getElapsedTime(refTime) << endl;

  //cache.writeOutBitmaps();
  //cache.printNodesTokensPaths();
}

intPair Probcache::pickSTpair(intPair regionPair) {
  intPair ansPair;

  vector<intPair>& nodePairVector = regionPair2nodePairVector[regionPair];
  if (debugProbc)
    cout << "Probcache::pickSTpair 1 nodepairVector size: " << nodePairVector.size() << endl;
	
  if (nodePairVector.size()>0) { // pick path WITHOUT REPLACEMENT
    ///random choice
    int slot=(int)rand()%nodePairVector.size();
    ansPair=nodePairVector[slot];
    if (debugProbc) cout << "Probcache::pickSTpair 2 nodepairVector size: " << nodePairVector.size() << ", " << regionPair2nodePairVector[regionPair].size() << endl;
    nodePairVector[slot]=nodePairVector.back();// WITHOUT REPLACEMENT
    if (debugProbc) cout << "Probcache::pickSTpair 2.5 nodepairVector size: " << nodePairVector.size() << ", " << regionPair2nodePairVector[regionPair].size() << endl;
    nodePairVector.pop_back();
    if (debugProbc) cout << "Probcache::pickSTpair 2.6 nodepairVector size: " << nodePairVector.size() << ", " << regionPair2nodePairVector[regionPair].size() << endl;
  }else {// random choice
    intVector& nodeVectorRegion1 = regionid2nodeidVector[regionPair.first];
    intVector& nodeVectorRegion2 = regionid2nodeidVector[regionPair.second];
    if (debugProbc)    cout << "Probcache::pickSTpair 3 nodepairVector size: " << nodePairVector.size() << endl;
    ///random choice
    int nid1 = nodeVectorRegion1[(int)rand()%nodeVectorRegion1.size()];
    int nid2 = nodeVectorRegion2[(int)rand()%nodeVectorRegion2.size()];
    if (debugProbc)    cout << "Probcache::pickSTpair 3.5 nodepairVector size: " << nodePairVector.size() << endl;
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
    if (regionPair2nodePairVector.find(ip) == regionPair2nodePairVector.end()){
      regionPair2nodePairVector[ip] = vector<intPair>();
    }
    regionPair2nodePairVector[ip].push_back(c);
  }
}

intVector Probcache::optiPath(intPair stPair, intPairSet& vSeen, bool random, int num){
  if(ts.testOptimaltype == OPTIMALTYPE_ORG)
    return optimalPath(stPair, vSeen, random);
  if(ts.testOptimaltype == OPTIMALTYPE_ORDERFILL)
    return optimalOrderedFill(stPair, vSeen, random);
  else if(ts.testOptimaltype == OPTIMALTYPE_KSKIP)
    return kskip(stPair, num);
  else if(ts.testOptimaltype == OPTIMALTYPE_RAND)
    return Probcache::random(stPair, num);
}
  
intVector Probcache::optimalPath(intPair stPair, intPairSet& vSeen, bool random){
  if(debugProbc) cout << "Probcache::optimalPath((" << stPair.first <<","<<stPair.second << "), " << random << ")" << endl;
  intVector spResultShort, spResultLong, spResultIntermediate, spDiff, returnResult;
  double longScore=0.0, conciseScore=0.0, intermediateScore=0.0;
  int choice;
  std::vector<int>::iterator originalIt, conciseIt, choicePosIt;
  if(debugProbc) cout << "Probcache::optimalPath Q_01:(" << endl;
  //Order is important! call setConcisePathUse false last!
  RoadGraph::mapObject(ts)->setConcisePathUse(true);
  spResultShort = RoadGraph::mapObject(ts)->dijkstraSSSP(stPair.first, stPair.second);
  conciseScore = calcScore(spResultShort, vSeen);  
  RoadGraph::mapObject(ts)->setConcisePathUse(false);
  spResultLong = RoadGraph::mapObject(ts)->dijkstraSSSP(stPair.first, stPair.second);
  longScore = calcScore(spResultLong, vSeen);    

  intVector tempLong, tempConsise;
  tempLong = spResultLong;
  tempConsise = spResultShort;
  std::sort (tempLong.begin(),tempLong.end());
  std::sort (tempConsise.begin(),tempConsise.end());
  
  spResultIntermediate = spResultShort;
	
  //find the set difference between concise and full path. This set is the candidate set for insertion when calculating optimalPath
  std::set_symmetric_difference(tempConsise.begin(), tempConsise.end(), tempLong.begin(), tempLong.end(), std::back_inserter(spDiff));
  
  if(debugProbc) cout << "Probcache::optimalPath Q_02:(" << stPair.first << "," << stPair.second << ") " << conciseScore << "/" << longScore << " " << spResultIntermediate.size() << "/" << spResultLong.size() << " (" << tempConsise.size() << "," << tempLong.size() << "," << spDiff.size() << ")" << endl;

  //pick which node to insert randomly, used to show that random pick is sub optimal to score based version. 
  if(random){
    double currentScore=conciseScore, bestScore=conciseScore;
    returnResult = spResultShort;
    while(!spDiff.empty()){
      choice = spDiff[(int)rand()%spDiff.size()];
      choicePosIt = find(spDiff.begin(), spDiff.end(), choice);
      originalIt = find(spResultLong.begin(), spResultLong.end(), choice);
      while(find(spResultIntermediate.begin(), spResultIntermediate.end(), *originalIt) == spResultIntermediate.end()){
	originalIt--;
      }
      conciseIt = find(spResultIntermediate.begin(), spResultIntermediate.end(), *originalIt);
      spResultIntermediate.insert(conciseIt+1, choice);
      intermediateScore = calcScore(spResultIntermediate, vSeen);
         
      if(intermediateScore > bestScore) {
	bestScore=intermediateScore;
	returnResult=spResultIntermediate;
      }
      currentScore=intermediateScore;
      spDiff.erase(choicePosIt);     
    }
    return returnResult;
  }else{
    int curBestOption;
    double tmpBestScore, bestScore=conciseScore, currentScore = conciseScore, intermediateScore, currentBasescore = conciseScore;
    //cout << "Q2_2: " << spDiff.size() << ", " << spDiff.empty() << "," << bestScore << endl;
    while(!spDiff.empty()){
      //cout << "Q3_22:" << endl;
      tmpBestScore=-1.0;
      //cout << "Q3_2:" << endl;
      BOOST_FOREACH(int option, spDiff){
	intermediateScore = calcAdditionalScore(spResultIntermediate, option, vSeen);

	if(intermediateScore > tmpBestScore){
	  tmpBestScore = intermediateScore;
	  curBestOption = option;
	}
      }
//       cout << "Probcache::optiPath Q4_2:" << endl;
      originalIt = find(spResultLong.begin(), spResultLong.end(), curBestOption);

      while(find(spResultIntermediate.begin(), spResultIntermediate.end(), *originalIt) == spResultIntermediate.end()){
	originalIt--;
      }
      conciseIt = find(spResultIntermediate.begin(), spResultIntermediate.end(), *originalIt);
      
      if(ts.devideScoreByLength) 
	currentBasescore = ((currentBasescore*(double)(spResultIntermediate.size())) + tmpBestScore)/(double)(spResultIntermediate.size()+1);
      else
	currentBasescore = (currentBasescore*(double)(spResultIntermediate.size())) + tmpBestScore;
//       cout << "Probcache::optiPath Q5_2:" << endl;
      if(currentBasescore > currentScore) {     
	spResultIntermediate.insert(conciseIt+1, curBestOption);

	choicePosIt = find(spDiff.begin(), spDiff.end(), curBestOption);
	spDiff.erase(choicePosIt);
      }else{
	////////////////////
	if(debugProbc) cout << "long/concise/optimal: " << spResultLong.size() << " / " << spResultShort.size() << " / " << spResultIntermediate.size() << endl;
	ts.optiLength += spResultIntermediate.size();
	ts.numOpti++;
	ts.longLength+= spResultLong.size();
	ts.numLong++;
      cout << "Probcache::optimalPath Q_041:(" << ts.optiLength << ", " <<ts.numOpti << ") (" << ts.longLength << ", " << ts.numLong << ")" << endl;
	///////////////////
	return spResultIntermediate;
      }      
      currentScore=currentBasescore;
    }
	////////////////////
	if(debugProbc) cout << "long/concise/optimal: " << spResultLong.size() << " / " << spResultShort.size() << " / " << spResultIntermediate.size() << endl;
	ts.optiLength += spResultIntermediate.size();
	ts.numOpti++;
	ts.longLength+= spResultLong.size();
	ts.numLong++;
      cout << "Probcache::optimalPath Q_042:(" << ts.optiLength << ", " <<ts.numOpti << ") (" << ts.longLength << ", " << ts.numLong << ")" << endl;
	///////////////////
    return spResultIntermediate;
  }
}


intVector Probcache::optimalOrderedFill(intPair stPair, intPairSet& vSeen, bool random){
  if(debugProbc) cout << "Probcache::optimalOrderedFill((" << stPair.first <<","<<stPair.second << "), " << random << ")" << endl;
  intVector spResultShort, spResultLong, spResultIntermediate, spDiff;
  double longScore=0.0, conciseScore=0.0, intermediateScore=0.0;
  intVectorMap regToCandidates;
  intPairSet benefitRegPairs;
  boost::unordered_set<int> extraNidToAdd;
  int choice;
  std::vector<int>::iterator originalIt, conciseIt, choicePosIt;
  if(debugProbc) cout << "Probcache::optimalOrderedFill Q_01:(" << endl;
  //Order is important! call setConcisePathUse false last!
  RoadGraph::mapObject(ts)->setConcisePathUse(true);
  spResultShort = RoadGraph::mapObject(ts)->dijkstraSSSP(stPair.first, stPair.second);
  conciseScore = calcScore(spResultShort, vSeen);  
  RoadGraph::mapObject(ts)->setConcisePathUse(false);
  spResultLong = RoadGraph::mapObject(ts)->dijkstraSSSP(stPair.first, stPair.second);
  longScore = calcScore(spResultLong, vSeen, spResultShort, regToCandidates, benefitRegPairs);    
  vector<bool> nodesWithBenefit(spResultLong.size(),false);
  
  BOOST_FOREACH(intPair regPair, benefitRegPairs){
    BOOST_FOREACH(int candidate, regToCandidates[regPair.first]){
      extraNidToAdd.insert(candidate);
      cout << "V1: " << candidate << "," << extraNidToAdd.size() << " - ";
    }
    BOOST_FOREACH(int candidate, regToCandidates[regPair.second]){
      extraNidToAdd.insert(candidate);    
      cout << "V2: " << candidate << "," << extraNidToAdd.size() << " - " << endl;
    }
  }
  
  spResultIntermediate = spResultShort;

  cout << "Probcache::optimalOrderedFill Q_02: " << benefitRegPairs.size() << ", " << regToCandidates.size() << ", " << extraNidToAdd.size() << endl;
  //For each node in the full path with a none-zero score or en entry in the concise path, 
  //set the corrosponding entry in nodesWithBenefit to true
  for(int cur=0;cur<spResultLong.size(); cur++){
    spResultIntermediate.push_back(spResultLong[cur]);
    intermediateScore = calcScore(spResultIntermediate, vSeen);  
    spResultIntermediate.pop_back();
    if(conciseScore < intermediateScore || extraNidToAdd.find(spResultLong[cur]) != extraNidToAdd.end())
      nodesWithBenefit[cur] = true;
    else{
      for(int i=0; i<spResultShort.size(); i++){
	if(spResultShort[i] == spResultLong[cur]) {
	  nodesWithBenefit[cur] = true;
	  break;
	}
      }
    }
  }

  for(int j=0; j<nodesWithBenefit.size(); j++){
    if(nodesWithBenefit[j]){
      spResultIntermediate.push_back(spResultLong[j]);
    }
  }
  
//   for(int j=0; j<nodesWithBenefit.size(); j++){
//      cout << nodesWithBenefit[j] << ",";
//   }
// double Probcache::calcScore(intVector& spResult, intPairSet& vSeen, intVector& spConcise, intVectorMap& regToCandidates, intPairSet& benefitRegPairs, bool useStatArgs)   
   
   
  ////////////////////
  if(debugProbc) cout << "long/concise/optimal: " << spResultLong.size() << " / " << spResultShort.size() << " / " << spResultIntermediate.size() << endl;
  ts.optiLength += spResultIntermediate.size();
  ts.numOpti++;
  ts.longLength+= spResultLong.size();
  ts.numLong++;
  //cout << "Probcache::optimalOrderedFill Q_042:(" << ts.optiLength << ", " <<ts.numOpti << ") (" << ts.longLength << ", " << ts.numLong << ")" << endl;
  ///////////////////
   return spResultIntermediate;
}


intVector Probcache::kskip(intPair stPair, int pct){
  if(debugProbc) cout << "Probcache::kskip((" << stPair.first <<","<< stPair.second << "), " << pct << ")" << endl;
  intVector spResultShort, spResultLong, spResultIntermediate, spDiff;
  std::vector<int>::iterator originalIt, conciseIt;
  if(debugProbc) cout << "Probcache::kskip Q_01:(" << endl;
  //Order is important! call setConcisePathUse false last!
  RoadGraph::mapObject(ts)->setConcisePathUse(true);
  spResultShort = RoadGraph::mapObject(ts)->dijkstraSSSP(stPair.first, stPair.second); 
  RoadGraph::mapObject(ts)->setConcisePathUse(false);
  spResultLong = RoadGraph::mapObject(ts)->dijkstraSSSP(stPair.first, stPair.second);  

  if(debugProbc) cout << "Probcache::kskip Q_02:(" << endl;
  int nodesInOptimal = (double)spResultLong.size()*(double)((double)pct/(double)100.0); //calc number of nodes in optimal k-skip path
  int diffSize = spResultLong.size() - spResultShort.size();
  int kskip;
  //if the optimal k-skip length is shorter than CONCISE, just return concise
  if(nodesInOptimal-spResultShort.size() <= 0)
    return spResultShort;
  else
     kskip = diffSize / (nodesInOptimal-spResultShort.size()); //calc k skip
  
  if(kskip < 1) return spResultShort;
  
  if(debugProbc) cout << "Probcache::kskip Q_03:( " << nodesInOptimal << ", " << diffSize << ", " << kskip << ", " << pct << ", " << spResultLong.size() << "/" << spResultShort.size() << endl;
  intVector tempLong, tempConsise;
  tempLong = spResultLong;
  tempConsise = spResultShort;
  std::sort (tempLong.begin(),tempLong.end());
  std::sort (tempConsise.begin(),tempConsise.end());
  
  spResultIntermediate = spResultShort;
 
  //find the set difference between concise and full path. This set is the candidate set for insertion when calculating optimalPath
  std::set_symmetric_difference(tempConsise.begin(), tempConsise.end(), tempLong.begin(), tempLong.end(), std::back_inserter(spDiff));

  for(int i=0; i < spDiff.size(); i +=kskip){
    originalIt = find(spResultLong.begin(), spResultLong.end(), spDiff[i]);
    
    while(find(spResultIntermediate.begin(), spResultIntermediate.end(), *originalIt) == spResultIntermediate.end()){
      originalIt--;
    }
    conciseIt = find(spResultIntermediate.begin(), spResultIntermediate.end(), *originalIt);
   
    spResultIntermediate.insert(conciseIt+1, spDiff[i]);
  }
  if(debugProbc) cout << "Probcache::kskip Q_05:( " << spResultIntermediate.size() << endl;
  return spResultIntermediate;
}


intVector Probcache::random(intPair stPair, int pct) {
  if(debugRand) cout << "Probcache::random((" << stPair.first <<","<<stPair.second << "), " << pct << ")" << endl;
  intVector spResultShort, spResultLong, spResultIntermediate, spDiff;
  int choice;
  std::vector<int>::iterator originalIt, conciseIt, choicePosIt;
  if(debugRand) cout << "Probcache::random Q_01:(" << endl;
  //Order is important! call setConcisePathUse false last!
  RoadGraph::mapObject(ts)->setConcisePathUse(true);
  spResultShort = RoadGraph::mapObject(ts)->dijkstraSSSP(stPair.first, stPair.second); 
  RoadGraph::mapObject(ts)->setConcisePathUse(false);
  spResultLong = RoadGraph::mapObject(ts)->dijkstraSSSP(stPair.first, stPair.second);  

  if(debugRand) cout << "Probcache::random Q_02:(" << endl;
  if(pct == 100) return spResultLong;
  int nodesInOptimal = (double)spResultLong.size()*(double)((double)pct/(double)100.0); //calc number of nodes in optimal random path
  
  //if the optimal random length is shorter than CONCISE, just return concise
  if(spResultShort.size() >= nodesInOptimal) return spResultShort;
  
  if(debugRand) cout << "Probcache::random Q_03:( " << nodesInOptimal << ", " << pct << ", " << spResultLong.size() << "/" << spResultShort.size() << endl;
  intVector tempLong, tempConsise;
  tempLong = spResultLong;
  tempConsise = spResultShort;
  std::sort (tempLong.begin(),tempLong.end());
  std::sort (tempConsise.begin(),tempConsise.end());
  
  spResultIntermediate = spResultShort;
 
  //find the set difference between concise and full path. This set is the candidate set for insertion when calculating optimalPath
  std::set_symmetric_difference(tempConsise.begin(), tempConsise.end(), tempLong.begin(), tempLong.end(), std::back_inserter(spDiff));

  while(spResultIntermediate.size() <= nodesInOptimal && !spDiff.empty()){
    choice = spDiff[(int)rand()%spDiff.size()];    
    choicePosIt = find(spDiff.begin(), spDiff.end(), choice);
    originalIt = find(spResultLong.begin(), spResultLong.end(), choice);
    while(find(spResultIntermediate.begin(), spResultIntermediate.end(), *originalIt) == spResultIntermediate.end()){
      originalIt--;
    }
    conciseIt = find(spResultIntermediate.begin(), spResultIntermediate.end(), *originalIt);
    spResultIntermediate.insert(conciseIt+1, choice);
    spDiff.erase(choicePosIt);
  }
  if(debugRand) cout << "Probcache::random Q_05:( " << spResultIntermediate.size() << endl;
  return spResultIntermediate;
}


double Probcache::calcAdditionalScore(intVector& path, int nid, intPairSet& vSeen) {
  double temp_score, score = 0.0;
  int rid2, rid1 = nodeid2regionid[nid];
  intPair regionpair, nodepair;

  BOOST_FOREACH(int pnid, path){
    nodepair = (nid < pnid)? make_pair(nid,pnid):make_pair(pnid,nid);
    if (vSeen.find(nodepair)==vSeen.end()){  // if not in cache
      rid2 = nodeid2regionid[pnid];
      regionpair = (rid1 < rid2)? make_pair(rid1,rid2):make_pair(rid2,rid1);
      temp_score=0;
      if (trainingQueriesPerRegionPair.find(regionpair) != trainingQueriesPerRegionPair.end())
	temp_score = trainingQueriesPerRegionPair.at(regionpair);
      score = score + temp_score;
    }
  }
}
