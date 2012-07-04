#include "Competitors.h"
#define debug false

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
//
//    generateRangeQueries(10);
//    cout << "10 done" << endl;
//    generateRangeQueries(50);
//    cout << "50 done" << endl;
//    generateRangeQueries(100);
//    cout << "100 done" << endl;
//    generateRangeQueries(150);
//    cout << "150 done" << endl;
//    generateRangeQueries(200);
//    cout << "200 done" << endl;
//    generateRangeQueries(400);
//    cout << "400 done" << endl;
//    generateRangeQueries(500);
//    cout << "500 done" << endl;
//    generateRangeQueries(800);
//    cout << "800 done" << endl;
//    generateRangeQueries(1000);
//    cout << "1000 done" << endl;
//    generateRangeQueries(1500);
//    cout << "1500 done" << endl;
//    generateRangeQueries(2000);
//    cout << "2000 done" << endl;
//
//    generatePOI();
//cout << "ABOUT ABOUT ABORT ABORT ABORT" << endl;
//exit(1);
}

LRU::~ LRU()
{
}

void LRU::buildCache()
{
	cout<< "2.0 done cachesize:" << cacheSize << endl;
	if(ts.useRange)
        rangeObj.init(ts);
    else
        readQueryLogData(QLOG_TEST);

	cout << "test query pairs:" << testSTPointPairs.size() << endl;
}

void LRU::runQueryList()
{
	RoadGraph::mapObject(ts)->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.

	if(ts.useRange){
        intPairVector& rQueryPairs = rangeObj.testSRQueryPairs;
        BOOST_FOREACH(intPair rq, rQueryPairs){
            intPairVector stQueryPairs = rangeObj.evalQuery(rq.first, rq.second);
            BOOST_FOREACH(intPair q, stQueryPairs){
                checkAndUpdateCache(q);
                numTotalQueries++;
            }
            rangeObj.rangeQuery(rq.first, rq.second, stQueryPairs);
        }
    }else{
        BOOST_FOREACH(intPair q, testSTPointPairs ) {
            checkAndUpdateCache(q);
            numTotalQueries++;
        }
    }

    this->ts.setBuildStatisticsTime(0);
    ts.setNonEmptyRegionPairs(0);
    this->ts.setFillCacheTime(0);
    this->ts.setItemsInCache(cache.size());
}

void LRU::checkAndUpdateCache(intPair query)
{
	bool cacheHit = false;
    if(debug)
	{
		cout << "cache size: " << cache.size() << " s,t: (" << query.first << "," << query.second << ")" << endl;
	}

	BOOST_FOREACH(CacheItem ci, cache ) {
		if (find(ci.item.begin(),ci.item.end(), query.first) != ci.item.end() &&
		    find(ci.item.begin(),ci.item.end(), query.second) != ci.item.end())
		{
	//		if(debug) {cout << "SP: ("; BOOST_FOREACH(int node, ci.item ) { cout << node << ","; } cout << ")"; }
			if(debug)
				cout << "LRU::checkAndUpdateCache BOTH TRUE" << endl;
			numCacheHits++;
			ci.updateKey(numTotalQueries);
			sort(cache.begin(), cache.end());
			cacheHit = true;
			break;
		}
	}
	if(debug) cout << "LRU::checkAndUpdateCache " << endl;

	if(!cacheHit)
	{
		vector<int> spResult = RoadGraph::computeSP(ts, query.first, query.second);
		numDijkstraCalls++;
		int querySize = spResult.size();

		//unsigned long lastNodeVisits=RoadGraph::mapObject(ts)->lastNodeVisits;
		//cout << "LRU:queryID: " << numTotalQueries << ": " << querySize << " " << lastNodeVisits << endl;

		if(cache.size() != 0){
			if(debug) cout << "LRU::checkAndUpdateCache 1, querySize: "<< querySize << endl;
			insertItem(spResult);
		}else{
			if(debug) cout << "LRU::checkAndUpdateCache 2, querySize: "<< querySize << endl;
			CacheItem e (numTotalQueries, spResult);
			cache.push_back(e);
		}
		if(debug) cout << "LRU::checkAndUpdateCache 3" << endl;
	}
}


void LRU::insertItem(intVector& sp) {

	uint spSize = sp.size();
	bool notEnoughSpace = true;
	if(debug) cout << "one, LRU::insertItem(" << spSize <<"," <<sp.size() << ")" << endl;
	//insert query into cache, will repeatedly remove items until there is enough space for the new item.
	do{

		if((cacheSize - cacheUsed) > spSize*NODE_BITS) {
			if(debug)
				cout << "two1, LRU::insertItem cacheSize,cacheUsed " << cacheSize <<"," << cacheUsed <<endl;
			CacheItem cItem (numTotalQueries, sp);
			cache.push_back(cItem);
			cacheUsed = cacheUsed + cItem.size*NODE_BITS;
			notEnoughSpace = false;
			if (debug)
				cout << "two2, LRU::insertItem cacheSize,cacheUsed " << cacheSize <<"," << cacheUsed <<endl;

		} else if ( spSize*NODE_BITS < cacheSize) {

			if (debug)
				cout << "three1, LRU::insertItem" << cache.size() <<"," << cache[0].size <<endl;
			int itemSize = cache[0].size;	// Ken: Is this the oldest item?
			cache.erase(cache.begin());
			cacheUsed = cacheUsed - itemSize*NODE_BITS;
			if (debug)
				cout << "three2, LRU::insertItem" <<endl;

		} else
			break;
	} while(notEnoughSpace);

	if(debug)
		cout << "four, LRU::insertItem" <<endl;
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

HQF::~HQF()
{
    //dtor
}

void HQF::runQueryList() {
	RoadGraph::mapObject(ts)->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.

	if(ts.useRange){
        intPairVector& rQueryPairs = rangeObj.testSRQueryPairs;
        BOOST_FOREACH(intPair rq, rQueryPairs){
            intPairVector stQueryPairs = rangeObj.evalQuery(rq.first, rq.second);
            BOOST_FOREACH(intPair q, stQueryPairs){
                checkCache(q);
                numTotalQueries++;
            }
            rangeObj.rangeQuery(rq.first, rq.second, stQueryPairs);
        }
    }else{
        BOOST_FOREACH(intPair q, testSTPointPairs ) {
            checkCache(q);
            numTotalQueries++;
        }
    }

	ts.setNonEmptyRegionPairs(0);
	ts.setBuildStatisticsTime(0);
}

void HQF::buildCache()
{
    cout<< "2.0 done" << endl;
	if(ts.useRange){
        rangeObj.init(ts);
    }else{
        readQueryLogData(QLOG_TEST);
        cout<< "2.1 done" << endl;
        readQueryLogData(QLOG_TRAIN);
        cout<< "2.2 done" << endl;
    }

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
	intPairVector* ptrQueryPairs=NULL;
    intPairVector allSTqueryPairs;

    if(ts.useRange){
        intPairVector& rQueryPairs = rangeObj.trainingSRQueryPairs;
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

    BOOST_FOREACH(intPair p, stQueryPairs)
	{
        if(coordinateStats.find(p) == coordinateStats.end())
            coordinateStats[p] = 1;
        else
            coordinateStats[p] = coordinateStats[p] + 1;
	}

	//int countStat=0;
    BOOST_FOREACH(intPair rp, stQueryPairs)
    {
        spResult = RoadGraph::computeSP(ts, rp.first, rp.second);

		//make new cache item
        CacheItem e (iId, spResult);
        iId++;
        bucketList[rp] = e;
        tmp.pID = rp;
		tmp.dist = coordinateStats[rp];

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

void HQF::checkCache(intPair query){
	bool cacheHit = false;
	vector<int> spResult;

	if(debug) cout << "one, hqf::checkCache! :cacheSize:" << (int) cache.size() <<"::"<< endl;
	if(cache.checkCache(query))
	{
		numCacheHits++;
		cacheHit = true;
	}

	if(debug) cout << "two, hqf::checkCache! cacheHit: " << cacheHit << endl;
	if(!cacheHit)
	{
		if(debug) cout << "three, hqf::checkCache!" << endl;
		spResult = RoadGraph::computeSP(ts, query.first, query.second);
		numDijkstraCalls++;

		//unsigned long lastNodeVisits=RoadGraph::mapObject(ts)->lastNodeVisits;
		//cout << "HQF:queryID: " << numTotalQueries << ": " << spResult.size() << " " << lastNodeVisits << endl;
	}
	if(debug) cout << "four, hqf::checkCache!" << endl;
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
		spResult = RoadGraph::computeSP(ts, rp.first, rp.second);

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
    if(debug) {cout << "cache size: " << runtimeCache.size() << " s,t: (" << query.first << "," << query.second << ")" << endl;}
    BOOST_FOREACH(CacheItem ci, runtimeCache )
    {
        if(find(ci.item.begin(),ci.item.end(), query.first) != ci.item.end() && find(ci.item.begin(),ci.item.end(), query.second) != ci.item.end())
        {
//		    if(debug) {cout << "SP: ("; BOOST_FOREACH(int node, ci.item ) { cout << node << ","; } cout << ")"; }
            if(debug) cout << "hqflru::checkAndUpdateCache BOTH TRUE" << endl;
            numCacheHits++;
            ci.updateKey(numTotalQueries);
            sort(runtimeCache.begin(), runtimeCache.end());
            cacheHit = true;
            break;
        }
    }
    if(debug) cout << "hqflru::checkAndUpdateCache " << endl;

	if(!cacheHit)
	{
		vector<int> spResult = RoadGraph::computeSP(ts, query.first, query.second);
		numDijkstraCalls++;
		int querySize = spResult.size();
		if(runtimeCache.size() != 0){
			if(debug) cout << "hqflru::checkAndUpdateCache 1, querySize: "<< querySize << endl;
			insertItem(querySize, spResult, query.first, query.second);
		}else{
			if(debug) cout << "hqflru::checkAndUpdateCache 2, querySize: "<< querySize << endl;
			CacheItem e (numTotalQueries, spResult);
			runtimeCache.push_back(e);
			cacheUsed = e.size*NODE_BITS;
		}
		if(debug) cout << "hqflru::checkAndUpdateCache 3" << endl;
	}
}

void HybridHQFLRU::insertItem(uint querySize, std::vector< int > nodesInQueryResult, int sNode, int tNode)
{
	bool notEnoughSpace = true;
	if(debug) cout << "one, hqflru::insertItem(" <<querySize <<"," <<nodesInQueryResult.size() <<","<<sNode<<","<<tNode<<")" << endl;
	//insert query into cache, will repeatedly remove items until there is enough space for the new item.
	do{
		if((cacheSize - cacheUsed) > querySize)
		{
			if(debug) cout << "two1, hqflru::insertItem cacheSize,cacheUsed " << cacheSize <<"," << cacheUsed <<endl;
			CacheItem cItem (numTotalQueries, nodesInQueryResult);
			runtimeCache.push_back(cItem);
			cacheUsed = cacheUsed + cItem.size*NODE_BITS;
			notEnoughSpace = false;
			if(debug) cout << "two2, hqflru::insertItem cacheSize,cacheUsed " << cacheSize <<"," << cacheUsed <<endl;
		}
		else if(querySize < cacheSize && runtimeCache.size() != 0)
		{
			if(debug) cout << "three1, hqflru::insertItem" << runtimeCache.size() <<"," << runtimeCache[0].size << "," << cacheUsed << "," << querySize << "," << cacheSize << endl;
			int itemSize = runtimeCache[0].size;
			runtimeCache.erase(runtimeCache.begin());
			cout << cacheUsed << "," << itemSize << "," << itemSize*NODE_BITS << endl;
			cacheUsed = cacheUsed - itemSize*NODE_BITS;
            cout << cacheUsed << "," << itemSize << "," << itemSize*NODE_BITS << endl;
			if(debug) cout << "three2, hqflru::insertItem" <<endl;
		}
		else
			break;
	}while(notEnoughSpace);
	if(debug) cout << "four, hqflru::insertItem" <<endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

