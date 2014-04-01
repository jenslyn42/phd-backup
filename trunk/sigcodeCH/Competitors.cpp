#define debugCompet false
#define debugProbc false
///////////////////////////////////////////////////////////////////////////////////////////////////////////


LRU::LRU(TestSetting ts)
{
  this->ts = ts;
  cacheSize = ts.cacheSize;
  numTotalQueries = 0;
  numCacheHits = 0;
  cacheUsed = 0;
  numDijkstraCalls = 0;
  readMapData();
}

LRU::~ LRU()
{
  trainingSTPointPairs.clear();
  testSTPointPairs.clear();
  points.clear();
  Point2Nodeid.clear();
  nodeid2Point.clear();

  cache.clear();
}


void LRU::buildCache()
{
  cout<< "2.0 done cachesize:" << cacheSize << endl;
  readQueryLogData(QLOG_TEST);

  cout << "test query pairs:" << testSTPointPairs.size() << endl;
}

void LRU::runQueryList()
{
  RoadGraph::mapObject(ts)->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.

  BOOST_FOREACH(intPair q, testSTPointPairs ) {
    checkAndUpdateCache(q);
    numTotalQueries++;
  }

  this->ts.setBuildStatisticsTime(0);
  ts.setNonEmptyRegionPairs(0);
  this->ts.setFillCacheTime(0);
  this->ts.setItemsInCache(cache.size());
}

void LRU::checkAndUpdateCache(intPair query)
{
  bool cacheHit = false;
/*
  if(debugCompet) {
    cout << "cache size: " << cache.size() << " s,t: (" << query.first << "," << query.second << ")" << endl;
  }
*/

  for (vector<CacheItem>::iterator itr = cache.begin(); itr != cache.end(); ++itr) {
    if (find((*itr).item.begin(),(*itr).item.end(), query.first) != (*itr).item.end() && 
        find((*itr).item.begin(),(*itr).item.end(), query.second) != (*itr).item.end())
    {
      if(debugCompet) 
        cout << "LRU::checkAndUpdateCache CACHE HIT CACHE HIT CACHE HIT" << endl;
      numCacheHits++;
      (*itr).updateKey(numTotalQueries);
      sort(cache.begin(), cache.end());
      cacheHit = true;
      break;
    }
  }
   if(debugCompet){
    std::priority_queue<int> cacheQueue;
    for (vector<CacheItem>::iterator itr = cache.begin(); itr != cache.end(); ++itr) {
      cacheQueue.push((*itr).key());
    }
    cout << "*H* ";
    while(!cacheQueue.empty()){
      cout << cacheQueue.top() << " ";
      cacheQueue.pop();
    }
    cout << endl;
   }

  if(!cacheHit)
  {
    vector<int> spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(query.first, query.second);
    numDijkstraCalls++;
    int querySize = spResult.size();

    if(cache.size() != 0){
      if(debugCompet) cout << "LRU::checkAndUpdateCache 1, querySize: "<< querySize << endl;
      insertItem(spResult);
    }else{
      if(debugCompet) cout << "LRU::checkAndUpdateCache 2, querySize: "<< querySize << endl;
      CacheItem e (numTotalQueries, spResult);
      cache.push_back(e);
    }
//     if(debugCompet) cout << "LRU::checkAndUpdateCache 3" << endl;
  }
}


void LRU::insertItem(intVector& sp) {
  int spSize = sp.size();
  bool notEnoughSpace = true;
  if(debugCompet){
    cout << "one, LRU::insertItem(" << spSize << ") - " << numTotalQueries << ", " << numCacheHits << endl;
    std::priority_queue<int> cacheQueue;    
    for (vector<CacheItem>::iterator itr = cache.begin(); itr != cache.end(); ++itr) {
      cacheQueue.push((*itr).key());
    }
    cout << "*B* ";
    while(!cacheQueue.empty()){
      cout << cacheQueue.top() << " ";
      cacheQueue.pop();
    }
    cout << endl;
  }
  //insert query into cache, will repeatedly remove items until there is enough space for the new item.
  do{
    if((cacheSize - cacheUsed) >= spSize*NODE_BITS) {
      if(debugCompet) 
        cout << "two1, LRU::insertItem INSERT (cacheSize,cacheUsed) " << cacheSize <<"," << cacheUsed;
      CacheItem cItem (numTotalQueries, sp);
      cache.push_back(cItem);
      cacheUsed = cacheUsed + cItem.size*NODE_BITS;
      notEnoughSpace = false;
      if (debugCompet) 
	cout << " TWO:(" << cacheSize <<"," << cacheUsed << ")"<<endl;

    } else if ( spSize*NODE_BITS < cacheSize) {
      if (debugCompet) 
        cout << "three1, LRU::insertItem REMOVE (node,size): " << cache[0].id << ", " << cache[0].size <<endl;
      int itemSize = cache[0].size;  // Ken: Is this the oldest item?
      cache.erase(cache.begin());
      cacheUsed = cacheUsed - itemSize*NODE_BITS;
//       if (debugCompet) 
//         cout << "three2, LRU::insertItem" <<endl;

    } else
      break;
  } while(notEnoughSpace);

  if(debugCompet) {
    std::priority_queue<int> cacheQueue;    
    for (vector<CacheItem>::iterator itr = cache.begin(); itr != cache.end(); ++itr) {
      cacheQueue.push((*itr).key());
    }
    cout << "*C* ";
    while(!cacheQueue.empty()){
      cout << cacheQueue.top() << " ";
      cacheQueue.pop();
    }
    cout << endl;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

LRUPLUS::LRUPLUS(TestSetting ts)
{
  this->ts = ts;
  cacheSize = ts.cacheSize;
  numTotalQueries = 0;
  orderVal = 0;
  numCacheHits = 0;
  cacheUsed = 0;
  numDijkstraCalls = 0;
  readMapData();
  
  numEvicted =0;
  numEvictedZeroBitmap =0;
  nodesInCache=0;
  ts.setNodesInCache(nodesInCache);

  if(this->ts.useLRUbitmap && (this->ts.testSPtype != SPTYPE_FULL || ts.testOptimaltype != OPTIMALTYPE_ORG))
    this->ts.useLRUbitmap = false;
}

LRUPLUS::~ LRUPLUS() {
  cout << "LRUPLUS::~ LRUPLUS() S" << endl;
  trainingSTPointPairs.clear();
  cache.clear();
  testSTPointPairs.clear();
  points.clear();
  Point2Nodeid.clear();
  nodeid2Point.clear();
  cout << "LRUPLUS::~ LRUPLUS() E" << endl;
}


void LRUPLUS::buildCache()
{
  cout<< "2.0 done cachesize:" << cacheSize << endl;
  readQueryLogData(QLOG_TEST);
  cout<< "2.1 done" << endl;
  readQueryLogData(QLOG_TRAIN);
  cout<< "2.2 done" << endl;
  double refTime = clock();
  cout<< "2.3 done" << endl;
  fillCache();
  ts.setFillCacheTime(getElapsedTime(refTime));
//   cout << "2.3 done, fillCache: " << ts.getFillCacheTime() << endl;

  cout << "test query pairs:" << testSTPointPairs.size() << endl;
}

void LRUPLUS::fillCache(){
  BOOST_FOREACH(intPair q, trainingSTPointPairs ) {
    checkAndUpdateCache(q);
    numTotalQueries++;
  }
  numCacheHits=0;
  this->resetDijkstraCalls();
}

void LRUPLUS::runQueryList()
{
  RoadGraph::mapObject(ts)->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.

  BOOST_FOREACH(intPair q, testSTPointPairs ) {
    checkAndUpdateCache(q);
    numTotalQueries++;
  }

  this->ts.setBuildStatisticsTime(0);
  ts.setNonEmptyRegionPairs(0);
  this->ts.setItemsInCache(cache.size());
  ts.setNodesInCache(nodesInCache);
  ts.setUnusedCacheBits((cacheSize - cacheUsed)*NODE_BITS);
  
  cout << "nodesInCache: " << nodesInCache << " / " << ts.getNodesInCache() << endl;
  cout << "itemsInCache: " << cache.size() << " / " << ts.getItemsInCache() << endl;
  cout << "Avg. itemLength: " << nodesInCache / cache.size() << " / " << ts.getAvgItemLength() << endl;
  
  //// stats write out code ////
  int reducedInCache=0, fullIncache=0;
  string statfilename = "lrustats_";
  statfilename.insert(0,ts.testFile);
  statfilename.append(boost::lexical_cast<std::string>(ts.cacheSize));
  statfilename.append(".stats");
  ofstream statfile;
  statfile.open((statfilename).c_str(), ios::out | ios::trunc);
  BOOST_FOREACH(intPairMap::value_type stat, lrustats){
    if(cache.find(stat.first) != cache.end()){
      if(stat.second.second != -1) reducedInCache++;
      else fullIncache++;
    }
    statfile << stat.first << "\t" << stat.second.first << "\t" << stat.second.second << endl;
  }
  statfile << "\n*******\n" << fullIncache << "\t" << reducedInCache << "\n*******" << endl;
  statfile.close();
  //////////////////////////////
  /// cache hit stats ///
  //////////////////////////////
  string statHITfilename = "lruHITstats_";
  statHITfilename.insert(0,ts.testFile);
  statHITfilename.append(boost::lexical_cast<std::string>(ts.cacheSize));
  statHITfilename.append(".stats");
  ofstream statHitfile;
  statHitfile.open((statHITfilename).c_str(), ios::out | ios::trunc);
  BOOST_FOREACH(intPairIntMap::value_type stat, hitstats){
    statHitfile << "(" << stat.first.first << ", " << stat.first.second << ")\t" << stat.second << endl;
  }
  /// Eviction stats ///
  statHitfile << "\n*******\n" << numEvicted << "\t" << numEvictedZeroBitmap << "\n*******" << endl;
  statHitfile.close();
  //////////////////////
}

void LRUPLUS::checkAndUpdateCache(intPair query)
{
  bool cacheHit = false;
  int tmpScore;

  if(debugCompet)
    cout << "cache size: " << cache.size() << " s,t: (" << query.first << "," << query.second << ")" << endl;


  boost::unordered_set<int>& query1 = invList[query.first];
  boost::unordered_set<int>& query2 = invList[query.second];
  int cachehit;

  int counter=0;
  for (boost::unordered_set<int>::iterator itr = query1.begin(); itr != query1.end(); ++itr) {
    if((query2.find(*itr)) != query2.end()){
      numCacheHits++;
      orderVal++;
      cacheHit = true;
      CacheItem& tmpItem = cache[*itr];
      ordering.erase(std::make_pair<int,int>(*itr, tmpItem.key() ) );
      ordering.insert(std::make_pair<int,int>(*itr, orderVal));
      tmpItem.updateKey(orderVal);
      if(ts.useLRUbitmap){
	vector<int>& path = tmpItem.item;
	for(int i=0; i<path.size(); i++){
	  if(path[i] == query.first || path[i] == query.second){
	    usefullParts[tmpItem.id][i] = 1;
	    counter=i;
	    break;
	  }
	}
	for(;counter<path.size(); counter++){
	  if(path[counter] == query.first || path[counter] == query.second){
	    usefullParts[tmpItem.id][counter] = 1;
	    break;
	  }
	}
      }
      (tmpItem.s > tmpItem.t) ? (hitstats[make_pair<int,int>(tmpItem.t,tmpItem.s)] ++) : (hitstats[make_pair<int,int>(tmpItem.s,tmpItem.t)] ++);

      if(debugCompet) 
	cout << "LRUPLUS::checkAndUpdateCache CACHE HIT CACHE HIT CACHE HIT" << endl;
      break;
    }
  }

//  if(debugCompet){
//    std::priority_queue<int> cacheQueue;
//    BOOST_FOREACH(intCacheitemMap::value_type ca, cache){cacheQueue.push(ca.second.key());}
//    cout << "*H* ";
//    while(!cacheQueue.empty()){
//      cout << cacheQueue.top() << " ";
//      cacheQueue.pop();
//    }
//    cout << endl;
//  }

  if(!cacheHit) {
    vector<int> spResult;
    pair<intVector, intVector> spaths;
    if(ts.testOptimaltype == OPTIMALTYPE_KSKIP)
      spResult = kskip(query, ts.optiNum);
    else {
      if(ts.useLRUbitmap){
	RoadGraph::mapObject(ts)->setConcisePathUse(true);
	spaths = RoadGraph::mapObject(ts)->conciseDijkstraSSSP(query.first, query.second);
	spResult = spaths.first;
      }else
	spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(query.first, query.second);
    }

    numDijkstraCalls++;
    int querySize = spResult.size();

    if(debugCompet) cout << "LRU::checkAndUpdateCache 1, querySize: "<< querySize << endl;
    if(ts.useLRUbitmap)
      insertItem(spResult, spaths.second);
    else
      insertItem(spResult);
  }
}


int LRUPLUS::insertItem(intVector& sp) {
  intVector placeholder;
  return insertItem(sp, placeholder);  
}

int LRUPLUS::insertItem(intVector& sp, intVector& conciseSp) {
  int spSize = sp.size();
  bool notEnoughSpace = true;

/*  if(debugCompet){
    cout << "two, LRUPLUS::insertItem(" << spSize << ") - " << numTotalQueries << ", " << numCacheHits << endl;
    std::priority_queue<int> cacheQueue;
    BOOST_FOREACH(intCacheitemMap::value_type ca, cache){cacheQueue.push(ca.second.key());}
    cout << "*B* ";
    while(!cacheQueue.empty()){
      cout << cacheQueue.top() << " ";
      cacheQueue.pop();
    }
    cout << endl;
  }*/
  //insert query into cache, will repeatedly remove items until there is enough space for the new item.
  do{
    if((cacheSize - cacheUsed) >= spSize*NODE_BITS) {
      if(debugCompet)
        cout << "two1, LRUPLUS::insertItem INSERT (cacheSize,cacheUsed) " << cacheSize <<"," << cacheUsed << endl;
      orderVal++;
      CacheItem cItem (orderVal, sp);
      cache[cItem.id] = cItem;
      nodesInCache += cItem.size;
      ordering.insert(std::make_pair<int,int>(cItem.id, cItem.key()));

      //update inverted list
      for (vector<int>::iterator itr = sp.begin(); itr != sp.end(); ++itr) {
	if(invList.find(*itr) == invList.end())
	  invList[*itr] = boost::unordered_set<int>();
	invList[*itr].insert(cItem.id);
      }
      cacheUsed = cacheUsed + cItem.size*NODE_BITS;
      if(spSize != cItem.size) cout << "LRUPLUS::insertItem ERROR ERROR ERROR :: spSize != cItem.size" << endl;

      //each path keeps a bitmap for tracking which nodes are used in CONCISE
      //such information will be used to decide which concise path to use (before the actual eviction)
      //Extract bitmap
      uint curConsPos=0, curFullPos=0;
      if(ts.useLRUbitmap){
	concisePartsp[cItem.id] = boost::dynamic_bitset<>(cItem.size);
	usefullParts[cItem.id] = boost::dynamic_bitset<>(cItem.size);
	for (vector<int>::iterator itr = sp.begin(); itr != sp.end(); ++itr, curFullPos++) {
	  if(sp[0] != conciseSp[0]) //if sp and conciseSp are starting from oppisite ends, reverse conciseSp.
	    std::reverse(conciseSp.begin(), conciseSp.end());
	  if(*itr == conciseSp[curConsPos]) {
	    concisePartsp[cItem.id].flip(curFullPos);
	    curConsPos++;
	  }
	}
	
      }
      if (debugCompet)
        cout << " TWO:(" << cacheSize <<"," << cacheUsed << ")"<<endl;

      removalStatus[cItem.id] = 1;
      notEnoughSpace = false;
      lrustats[cItem.id] = make_pair<int,int>(cItem.size,-1);
      return cItem.id;

    }else if ( spSize*NODE_BITS < cacheSize) {
//       3 cases:
//       1. full item -> reduce to CONCISE + node pairs  && ts.testSPtype == SPTYPE_CONCISEwhere path has contributed to a cache hit
//       2. reduced item -> reduce to CONCISE path
//       3. CONCISE path -> remove path

      if (debugCompet)
        cout << "three1, LRU::insertItem REMOVE (node,size): " << (*(ordering.begin())).first << "(" << (*(ordering.begin())).second << "), " << cache[(*(ordering.begin())).first].size << endl;
      intPair rID = *(ordering.begin()); // path to remove
      int rPid = rID.first;
      vector<int>& rItem = cache[rPid].item;
      intVector tempItem;
      boost::dynamic_bitset<> tempConsiseParts;

      if(!ts.useLRUbitmap || ts.testSPtype == SPTYPE_CONCISE)
	removalStatus[rPid] = 3; //remove path from cache, do not reduce path size

      switch(removalStatus[rPid]){
	case 1: //reduce the path
	  //cout << "Case 1:" << endl;
	  for (int i=0; i< rItem.size(); i++) {
	    if(concisePartsp[rPid].test(i)){
	      tempItem.push_back(rItem[i]);
	      tempConsiseParts.push_back(1);
	    }else if(usefullParts[rPid].test(i)) {
	      tempItem.push_back(rItem[i]);
	      tempConsiseParts.push_back(0);
	    }else{
	      tempConsiseParts.push_back(0);
	      invList[rItem[i]].erase(rPid); //remove node -> path from inverted list

	    }
	  }
	  
	  //update items LRU position, as if it had just been inserted or caused a cache hit
	  orderVal++;
	  ordering.erase(std::make_pair<int,int>(rPid, cache[rPid].key() ) );
	  ordering.insert(std::make_pair<int,int>(rPid, orderVal));
	  cache[rPid].updateKey(orderVal);

	  nodesInCache -= (rItem.size() - tempItem.size());
	  cacheUsed -= (rItem.size() - tempItem.size())*NODE_BITS;
	  cache[rPid].item = tempItem;
	  cache[rPid].size = tempItem.size();
	  concisePartsp[rPid] = tempConsiseParts;
	  usefullParts.erase(rPid);
	  usefullParts[rPid] = boost::dynamic_bitset<>(cache[rPid].size);
	  removalStatus[rPid] = 3; //set to 3 to skip case 2, set to 2 to use case 2.
	  lrustats[rPid].second = tempItem.size();
	  break;
	case 2: //limit path to CONCISE
	  //cout << "Case 2:" << endl;
	  for (int i=0; i< rItem.size(); i++) {
	    if(concisePartsp[rPid].test(i)){
	      tempItem.push_back(rItem[i]);
	    }else
	      invList[rItem[i]].erase(rPid); //remove node -> path from inverted list
	  }

	  //update items LRU position, as if it had just been inserted or caused a cache hit
// 	  orderVal++;
// 	  ordering.erase(std::make_pair<int,int>(rPid, cache[rPid].key() ) );
// 	  ordering.insert(std::make_pair<int,int>(rPid, orderVal));
// 	  cache[rPid].updateKey(orderVal);

	  nodesInCache -= (rItem.size() - tempItem.size());
	  cacheUsed -= (rItem.size() - tempItem.size())*NODE_BITS;
	  cache[rPid].item = tempItem;
	  cache[rPid].size = tempItem.size();
	  concisePartsp.erase(rPid);
	  cacheUsed -= tempItem.size()*NODE_BITS;
	  removalStatus[rPid] = 3;
	  break;
	case 3: //remove path
	  numEvicted++;
	  if(ts.useLRUbitmap && !usefullParts[rPid].any())
	    numEvictedZeroBitmap++;
	  //update inverted list
	  for(vector<int>::iterator itr = rItem.begin(); itr != rItem.end(); ++itr){
//cout << "case 3.01 " << rPid << " " << *itr << " " << invList[*itr].size() << endl;
	    if(invList[*itr].find(rPid) != invList[*itr].end()) {  invList[*itr].erase(rPid);}
	  }
	  int itemSize = cache[rPid].size;  // oldest item
	  cache.erase(rPid);
	  ordering.erase(ordering.begin());
	  nodesInCache -= itemSize;

	  cacheUsed = cacheUsed - itemSize*NODE_BITS;
	  break;
      }
    } else
      break;
  } while(notEnoughSpace);

//  if(debugCompet){
//    std::priority_queue<int> cacheQueue;
//    BOOST_FOREACH(intCacheitemMap::value_type ca, cache){cacheQueue.push(ca.second.key());}
//    cout << "*C* ";
//    while(!cacheQueue.empty()){
//      cout << cacheQueue.top() << " ";
//      cacheQueue.pop();
//    }
//    cout << endl;
//    cout << "ERROR - PATH TOO LONG FOR CACHE";
//    return -1;
//  }
}



intVector LRUPLUS::kskip(intPair stPair, int pct){
  if(debugProbc) cout << "LRUPLUS::kskip((" << stPair.first <<","<< stPair.second << "), " << pct << ")" << endl;
  intVector spResultIntermediate, spDiff;
  pair<intVector, intVector> spaths;
  std::vector<int>::iterator originalIt, conciseIt;
  if(debugProbc) cout << "LRUPLUS::kskip Q_01:(" << endl;
  //Order is important! call setConcisePathUse false last!
  RoadGraph::mapObject(ts)->setConcisePathUse(true);
  spaths = RoadGraph::mapObject(ts)->conciseDijkstraSSSP(stPair.first, stPair.second); 
  intVector& spResultShort = spaths.second;
  intVector& spResultLong = spaths.first;

  if(debugProbc) cout << "LRUPLUS::kskip Q_02:(" << endl;
  int nodesInOptimal = (double)spResultLong.size()*(double)((double)pct/(double)100.0); //calc number of nodes in optimal k-skip path
  int diffSize = spResultLong.size() - spResultShort.size();
  int kskip;
  //if the optimal k-skip length is shorter than CONCISE, just return concise
  if(nodesInOptimal-spResultShort.size() <= 0)
    return spResultShort;
  else
     kskip = diffSize / (nodesInOptimal-spResultShort.size()); //calc k skip
  
  if(kskip < 1) return spResultShort;
  
  if(debugProbc) cout << "LRUPLUS::kskip Q_03:( " << nodesInOptimal << ", " << diffSize << ", " << kskip << ", " << pct << ", " << spResultLong.size() << "/" << spResultShort.size() << endl;
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
  if(debugProbc) cout << "LRUPLUS::kskip Q_05:( " << spResultIntermediate.size() << endl;
  return spResultIntermediate;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////

HQF::HQF(TestSetting ts)
{
  this->ts = ts;
  cache.init(ts);
  cacheSize = ts.cacheSize;
  numTotalQueries = 0;
  numCacheHits = 0;
  numDijkstraCalls = 0;
  readMapData();
  calcScoreCounter=0;
}

HQF::~HQF() {
  trainingSTPointPairs.clear();
  testSTPointPairs.clear();
  points.clear();
  Point2Nodeid.clear();
  nodeid2Point.clear();

  calcScoreMap.clear();
  queries.clear();
}


void HQF::runQueryList() {
  RoadGraph::mapObject(ts)->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.
  BOOST_FOREACH(intPair q, testSTPointPairs ) {
    checkCache(q);
    numTotalQueries++;
  }

  ts.setNonEmptyRegionPairs(0);
  ts.setBuildStatisticsTime(0);
}

void HQF::buildCache()
{
  cout<< "2.0 done" << endl;
  readQueryLogData(QLOG_TEST);
  cout<< "2.1 done" << endl;
  readQueryLogData(QLOG_TRAIN);
  cout<< "2.2 done" << endl;
  double refTime = clock();
  fillCache();
  ts.setFillCacheTime(getElapsedTime(refTime));
  cout << "2.3 done" << endl;
}



void HQF::fillCache()
{
  boost::unordered_map<intPair, int> coordinateStats;
  boost::unordered_map<intPair,CacheItem> bucketList;
  vector<int> spResult;
  maxHeap mhCache;
  HeapEntry tmp;
  int iId=0;

  BOOST_FOREACH(intPair p, trainingSTPointPairs)
  {
    if(coordinateStats.find(p) == coordinateStats.end())
      coordinateStats[p] = 1;
    else
      coordinateStats[p] = coordinateStats[p] + 1;
  }

  BOOST_FOREACH(intPair rp, trainingSTPointPairs)
  {
    spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(rp.first, rp.second);

    //make new cache item
    CacheItem e (iId, spResult);
    iId++;
    bucketList[rp] = e;
    tmp.pID = rp;
    tmp.dist = coordinateStats.at(rp);
    mhCache.push(tmp);
  }

  //Fill cache
  while(!mhCache.empty()) {
    tmp= mhCache.top();
    mhCache.pop();

    if(bucketList.find(tmp.pID) != bucketList.end()) {
      if(cache.hasEnoughSpace(bucketList.at(tmp.pID)))
	if(cache.insertItem(bucketList.at(tmp.pID))){}
    }else{
      cout << "BLARG!! error occurred" << endl;
    }
  }
  ts.setItemsInCache(cache.numberOfItemsInCache());
  plotCachePoints(cache.cache);
}

void HQF::checkCache(intPair query)
{
  bool cacheHit = false;
  vector<int> spResult;

  if(debugCompet) cout << "one, hqf::checkCache! :cacheSize:" << (int) cache.size() <<"::"<< endl;
  if(cache.checkCache(query)) {
    numCacheHits++;
    cacheHit = true;
  }

  if(debugCompet) cout << "two, hqf::checkCache! cacheHit: " << cacheHit << endl;
  if(!cacheHit)
  {
    if(debugCompet) cout << "three, hqf::checkCache!" << endl;
    spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(query.first, query.second);
    numDijkstraCalls++;
  }
  if(debugCompet) cout << "four, hqf::checkCache!" << endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////


HybridHQFLRU::HybridHQFLRU(TestSetting ts)
{
  this->ts = ts;
  cache.init(ts);
  cacheSize = cache.size();
  numTotalQueries = 0;
  numCacheHits = 0;
  numDijkstraCalls = 0;
  readMapData();
  calcScoreCounter=0;
}

HybridHQFLRU::~HybridHQFLRU()
{
    //dtor
}


void HybridHQFLRU::runQueryList() {
  runtimeCache = cache.cache;
  sort(runtimeCache.begin(), runtimeCache.end());
  cacheUsed = cache.getCacheUsed();
  cout<< "3.0 Start" << endl;
  RoadGraph::mapObject(ts)->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.
  cout<< "3.1 Done" << endl;
  BOOST_FOREACH(intPair q, testSTPointPairs ) { 
//     cout << "q: (" << q.first <<"," << q.second << ")" << endl;
    checkAndUpdateCache(q);
    numTotalQueries++;
  }
  cout << "3.2 Done" << endl;

  ts.setNonEmptyRegionPairs(0);
  ts.setBuildStatisticsTime(0);
}

void HybridHQFLRU::buildCache()
{
  cout<< "2.0 done" << endl;
  readQueryLogData(QLOG_TEST);
  cout<< "2.1 done" << endl;
  readQueryLogData(QLOG_TRAIN);
  cout<< "2.2 done" << endl;
  double refTime = clock();
  fillCache();
  ts.setFillCacheTime(getElapsedTime(refTime));
  cout << "2.3 done" << endl;
}


void HybridHQFLRU::fillCache()
{
  boost::unordered_map<intPair, int> coordinateStats;
  boost::unordered_map<intPair,CacheItem> bucketList;
  vector<int> spResult;
  maxHeap mhCache;
  HeapEntry tmp;
  int iId=0;

  BOOST_FOREACH(intPair p, trainingSTPointPairs)
  {
    if (coordinateStats.find(p) == coordinateStats.end())
      coordinateStats[p] = 1;
    else
      coordinateStats[p] = coordinateStats[p] + 1;
  }

  BOOST_FOREACH(intPair rp, trainingSTPointPairs)
  {
    spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(rp.first, rp.second);

    //make new cache item
    CacheItem e (iId, spResult);
    iId++;
    bucketList[rp] = e;
    tmp.pID = rp;
    tmp.dist = coordinateStats.at(rp);
    mhCache.push(tmp);

    
    //Fill cache
    while(!mhCache.empty())
    {
      tmp= mhCache.top();
      mhCache.pop();

      if(bucketList.find(tmp.pID) != bucketList.end())
      {
	if(cache.hasEnoughSpace(bucketList.at(tmp.pID)))
		  if(cache.insertItem(bucketList.at(tmp.pID))){}
      }else{
	cout << "BLARG!! error occurred" << endl;
      }
    }
    ts.setItemsInCache(cache.numberOfItemsInCache());
    plotCachePoints(cache.cache);
  }
}


void HybridHQFLRU::checkAndUpdateCache(intPair query)
{
  bool cacheHit = false;
  if(debugCompet) {cout << "cache size: " << runtimeCache.size() << " s,t: (" << query.first << "," << query.second << ")" << endl;}
  BOOST_FOREACH(CacheItem ci, runtimeCache )
  {
    if(find(ci.item.begin(),ci.item.end(), query.first) != ci.item.end() && find(ci.item.begin(),ci.item.end(), query.second) != ci.item.end())
    {
      numCacheHits++;
      ci.updateKey(numTotalQueries);
      sort(runtimeCache.begin(), runtimeCache.end());
      cacheHit = true;
      break;
    }
  }
  if(debugCompet) cout << "hqflru::checkAndUpdateCache " << endl;

  if(!cacheHit)
  {
    vector<int> spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(query.first, query.second);
    numDijkstraCalls++;
    int querySize = spResult.size();
    if(runtimeCache.size() != 0){
      if(debugCompet) cout << "hqflru::checkAndUpdateCache 1, querySize: "<< querySize << endl;
      insertItem(querySize, spResult, query.first, query.second);
    }else{
      if(debugCompet) cout << "hqflru::checkAndUpdateCache 2, querySize: "<< querySize << endl;
      CacheItem e (numTotalQueries, spResult);
      runtimeCache.push_back(e);
      cacheUsed = e.size*NODE_BITS;
    }
    if(debugCompet) cout << "hqflru::checkAndUpdateCache 3" << endl;
  }
}

void HybridHQFLRU::insertItem(uint querySize, std::vector< int > nodesInQueryResult, int sNode, int tNode)
{
  bool notEnoughSpace = true;
  if(debugCompet) cout << "one, hqflru::insertItem(" <<querySize <<"," <<nodesInQueryResult.size() <<","<<sNode<<","<<tNode<<")" << endl;
  //insert query into cache, will repeatedly remove items until there is enough space for the new item.
  do{
    if((cacheSize - cacheUsed) > querySize)
    {
      if(debugCompet) cout << "two1, hqflru::insertItem cacheSize,cacheUsed " << cacheSize <<"," << cacheUsed <<endl;
      CacheItem cItem (numTotalQueries, nodesInQueryResult);
      runtimeCache.push_back(cItem);
      cacheUsed = cacheUsed + cItem.size*NODE_BITS;
      notEnoughSpace = false;
      if(debugCompet) cout << "two2, hqflru::insertItem cacheSize,cacheUsed " << cacheSize <<"," << cacheUsed <<endl;
    }
    else if(querySize < cacheSize && runtimeCache.size() != 0)
    {
      if(debugCompet) cout << "three1, hqflru::insertItem" << runtimeCache.size() <<"," << runtimeCache[0].size << "," << cacheUsed << "," << querySize << "," << cacheSize << endl;
      int itemSize = runtimeCache[0].size;
      runtimeCache.erase(runtimeCache.begin());
      cout << cacheUsed << "," << itemSize << "," << itemSize*NODE_BITS << endl;
      cacheUsed = cacheUsed - itemSize*NODE_BITS;
            cout << cacheUsed << "," << itemSize << "," << itemSize*NODE_BITS << endl;
      if(debugCompet) cout << "three2, hqflru::insertItem" <<endl;
    }
    else
      break;
  }while(notEnoughSpace);
  if(debugCompet) cout << "four, hqflru::insertItem" <<endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

