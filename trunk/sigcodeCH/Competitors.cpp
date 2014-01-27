#define debugCompet true

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

  BOOST_FOREACH(CacheItem ci, cache ) {
    if (find(ci.item.begin(),ci.item.end(), query.first) != ci.item.end() && 
        find(ci.item.begin(),ci.item.end(), query.second) != ci.item.end())
    {
      if(debugCompet) 
        cout << "LRU::checkAndUpdateCache CACHE HIT CACHE HIT CACHE HIT" << endl;
      numCacheHits++;
      ci.updateKey(numTotalQueries);
      sort(cache.begin(), cache.end());
      cacheHit = true;
      break;
    }
  }
//   if(debugCompet) cout << "LRU::checkAndUpdateCache 0" << endl;

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
  numCacheHits = 0;
  cacheUsed = 0;
  numDijkstraCalls = 0;
  readMapData();
}

LRUPLUS::~ LRUPLUS()
{
  trainingSTPointPairs.clear();
  testSTPointPairs.clear();
  points.clear();
  Point2Nodeid.clear();
  nodeid2Point.clear();

  cache.clear();
}


void LRUPLUS::buildCache()
{
  cout<< "2.0 done cachesize:" << cacheSize << endl;
  readQueryLogData(QLOG_TEST);
//   cout<< "2.1 done" << endl;
//   readQueryLogData(QLOG_TRAIN);
//   cout<< "2.2 done" << endl;
  double refTime = clock();
  fillCache();
  ts.setFillCacheTime(getElapsedTime(refTime));
//   cout << "2.3 done, fillCache: " << ts.getFillCacheTime() << endl;

  cout << "test query pairs:" << testSTPointPairs.size() << endl;
}

void LRUPLUS::fillCache(){
//   BOOST_FOREACH(intPair q, trainSTPointPairs ) {
//     checkAndUpdateCache(q);
//     numTotalQueries++;
//
//     numCacheHits=0;
//   }
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
}

void LRUPLUS::checkAndUpdateCache(intPair query)
{
  bool cacheHit = false;
  int tmpScore;
/*
  if(debugCompet)
    cout << "cache size: " << cache.size() << " s,t: (" << query.first << "," << query.second << ")" << endl;
*/

  boost::unordered_set<int>& query1 = invList[query.first];
  boost::unordered_set<int>& query2 = invList[query.second];
  int cachehit;
  boost::unordered_set<int>::const_iterator got;

  for (boost::unordered_set<int>::iterator itr = query1.begin(); itr != query1.end(); ++itr) {
    if((got = query2.find(*itr)) != query2.end()){
      numCacheHits++;
      cacheHit = true;
      CacheItem& tmpItem = cache[*itr];
      ordering.erase(std::make_pair<int,int>(*itr, tmpItem.key() ) );
      ordering.insert(std::make_pair<int,int>(*itr, numTotalQueries));
      tmpItem.updateKey(numTotalQueries);
      nodesInCache =+ tmpItem.size;
//       if(debugCompet) cout << "LRUPLUS::checkAndUpdateCache CACHE HIT CACHE HIT CACHE HIT" << endl;
//
      if(debugCompet) cout << "LRU::checkAndUpdateCache CACHE HIT CACHE HIT CACHE HIT" << endl;
      break;
    }
  }
//   if(debugCompet) cout << "LRUPLUS::checkAndUpdateCache 0" << endl;

  if(!cacheHit) {
    vector<int> spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(query.first, query.second);
    numDijkstraCalls++;
    int querySize = spResult.size();

    if(cache.size() != 0){
//       if(debugCompet) cout << "LRUPLUS::checkAndUpdateCache 1, querySize: "<< querySize << endl;
      if(debugCompet) cout << "LRU::checkAndUpdateCache 1, querySize: "<< querySize << endl;
      insertItem(spResult);
    }else{
//       if(debugCompet) cout << "LRUPLUS::checkAndUpdateCache 2, querySize: "<< querySize << endl;
      if(debugCompet) cout << "LRU::checkAndUpdateCache 2, querySize: "<< querySize << endl;
      CacheItem cItem (numTotalQueries, spResult);
      cache[cItem.id] = cItem;
      ordering.insert(std::make_pair<int,int>(cItem.id, cItem.key()));
      nodesInCache =+ cItem.size;
      //update inverted lists
      for (vector<int>::iterator itr = spResult.begin(); itr != spResult.end(); ++itr) {
	if(invList.find(*itr) == invList.end())
	  invList[*itr] = boost::unordered_set<int>();

	invList[*itr].insert(cItem.id);
      }
    }
//     if(debugCompet) cout << "LRUPLUS::checkAndUpdateCache 3" << endl;
  }
}


void LRUPLUS::insertItem(intVector& sp) {
  int spSize = sp.size();
  bool notEnoughSpace = true;
//   if(debugCompet){
//     cout << "one, LRUPLUS::insertItem(" << spSize << ")" << endl;
  if(debugCompet){
    cout << "one, LRU::insertItem(" << spSize << ") - " << numTotalQueries << ", " << numCacheHits << endl;
    std::priority_queue<int> cacheQueue;
    BOOST_FOREACH(intCacheitemMap::value_type ca, cache){cacheQueue.push(ca.second.key());}
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
//       if(debugCompet)
//         cout << "two1, LRUPLUS::insertItem INSERT (cacheSize,cacheUsed) " << cacheSize <<"," << cacheUsed;
      if(debugCompet)
        cout << "two1, LRU::insertItem INSERT (cacheSize,cacheUsed) " << cacheSize <<"," << cacheUsed;
      CacheItem cItem (numTotalQueries, sp);
      cache[cItem.id] = cItem;
      nodesInCache =+ cItem.size;
      ordering.insert(std::make_pair<int,int>(cItem.id, cItem.key()));
      //update inverted list
      for (vector<int>::iterator itr = sp.begin(); itr != sp.end(); ++itr) {
	if(invList.find(*itr) == invList.end()){
	  invList[*itr] = boost::unordered_set<int>();
	  invList[*itr].insert(cItem.id);
	}else{
	  invList[*itr].insert(cItem.id);
	}
      }

      cacheUsed = cacheUsed + cItem.size*NODE_BITS;
      if(spSize != cItem.size) cout << "LRUPLUS::insertItem ERROR ERROR ERROR :: spSize != cItem.size" << endl;

      notEnoughSpace = false;
      if (debugCompet)
        cout << " TWO:(" << cacheSize <<"," << cacheUsed << ")"<<endl;

    }else if ( spSize*NODE_BITS < cacheSize) {
//       if (debugCompet)
//         cout << "three1, LRUPLUS::insertItem REMOVE (node,size): " << (*(ordering.begin())).first << ", " << cache[(*(ordering.begin())).first].size <<endl;
      if (debugCompet)
        cout << "three1, LRU::insertItem REMOVE (node,size): " << (*(ordering.begin())).first << ", " << cache[(*(ordering.begin())).first].size <<endl;
      intPair rID = *(ordering.begin()); // path to remove
      int rPid = rID.first;
      vector<int>& rItem = cache[rPid].item;

      //update inverted list
      for(vector<int>::iterator itr = rItem.begin(); itr != rItem.end(); ++itr){
	invList[*itr].erase(rPid);
      }

      int itemSize = cache[rPid].size;  // oldest item
      cache.erase(rPid);
      ordering.erase(ordering.begin());
      nodesInCache =- itemSize;

      cacheUsed = cacheUsed - itemSize*NODE_BITS;
//       if (debugCompet)
//         cout << "three2, LRUPLUS::insertItem" <<endl;

    } else
      break;
  } while(notEnoughSpace);
  
  if(debugCompet){
    std::priority_queue<int> cacheQueue;
    BOOST_FOREACH(intCacheitemMap::value_type ca, cache){cacheQueue.push(ca.second.key());}
    cout << "*C* ";
    while(!cacheQueue.empty()){
      cout << cacheQueue.top() << " ";
      cacheQueue.pop();
    }
    cout << endl;
  }
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
    cout << "q: (" << q.first <<"," << q.second << ")" << endl;
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
  }

  cout << "fillCache loop start" << endl;
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

