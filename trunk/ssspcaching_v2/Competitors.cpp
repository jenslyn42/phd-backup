#include "Competitors.h"
#define debug false

///////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STATIC_CACHE false

typedef std::pair<int , int> intPair;

LRU::LRU(testsetting ts)
{
	this->ts = ts;
	cacheSize = ts.getCacheSize()/32;
	numTotalQueries = 0;
	numCacheHits = 0;
	cacheUsed = 0;
	numDijkstraCalls = 0;
    readMapData();
}

LRU::~ LRU()
{
}

void LRU::readQuery(std::pair< int, int > query)
{
   	checkAndUpdateCache(query);
    numTotalQueries++;
}

void LRU::readStaticQuery(std::pair< int, int > query)
{
    checkStaticCache(query);
    numTotalQueries++;
}

void LRU::readQueries(int numQueries, string inFn)
{
	cout<< "2.0 done cachesize/given cs:" << cacheSize <<"/" << ts.getCacheSize() << endl;
	readTestData(ts.queryFileName);
	cout<< "2.1 done" << endl;
	readTrainingData(ts.queryFileName);

	cout << "test query pairs:" << testSTPointPairs.size() << endl;
	cout << "training query pairs: " << trainingSTPointPairs.size() << endl;

	if(STATIC_CACHE){
        BOOST_FOREACH(intPair q, trainingSTPointPairs) { readQuery(q); }
    }
}

void LRU::readQueryList(std::vector< std::pair<int,int> > queryList)
{
	RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.
    if(STATIC_CACHE){
        numTotalQueries = 0;
        numCacheHits = 0;
        numDijkstraCalls = 0;
        BOOST_FOREACH(intPair q, testSTPointPairs ) { readStaticQuery(q); }
    }else{
        BOOST_FOREACH(intPair q, testSTPointPairs ) { readQuery(q); }
    }

    this->ts.setBuildStatisticsTime(-1.0);
    ts.setNonEmptyRegionPairs(-1);
    this->ts.setFillCacheTime(-1.0);
    this->ts.setItemsInCache(cache.size());
}

void LRU::checkAndUpdateCache(std::pair< int, int > query)
{
	bool cacheHit = false;
    if(debug) {cout << "cache size: " << cache.size() << " s,t: (" << query.first << "," << query.second << ")" << endl;}


		BOOST_FOREACH(CacheItem ci, cache )
		{
			if(find(ci.item.begin(),ci.item.end(), query.first) != ci.item.end() && find(ci.item.begin(),ci.item.end(), query.second) != ci.item.end())
			{
//			    if(debug) {cout << "SP: ("; BOOST_FOREACH(int node, ci.item ) { cout << node << ","; } cout << ")"; }
				if(debug) cout << "LRU::checkAndUpdateCache BOTH TRUE" << endl;
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
		vector<int> spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(query.first, query.second);
		numDijkstraCalls++;
		int querySize = spResult.size();
		if(cache.size() != 0){
			if(debug) cout << "LRU::checkAndUpdateCache 1, querySize: "<< querySize << endl;
			insertItem(querySize, spResult, query.first, query.second);
		}else{
			if(debug) cout << "LRU::checkAndUpdateCache 2, querySize: "<< querySize << endl;
			CacheItem e (numTotalQueries, spResult);
			cache.push_back(e);
		}
		if(debug) cout << "LRU::checkAndUpdateCache 3" << endl;
	}
}

void LRU::checkStaticCache(std::pair< int, int > query)
{
	bool cacheHit = false;
    if(debug) {cout << "cache size: " << cache.size() << " s,t: (" << query.first << "," << query.second << ")" << endl;}


		BOOST_FOREACH(CacheItem ci, cache )
		{
			if(find(ci.item.begin(),ci.item.end(), query.first) != ci.item.end() && find(ci.item.begin(),ci.item.end(), query.second) != ci.item.end())
			{
//			    if(debug) {cout << "SP: ("; BOOST_FOREACH(int node, ci.item ) { cout << node << ","; } cout << ")"; }
				if(debug) cout << "LRU::checkStaticCache BOTH TRUE" << endl;
				numCacheHits++;
				cacheHit = true;
				break;
			}
		}
		if(debug) cout << "LRU::checkStaticCache " << endl;


    if(!cacheHit)
	{
		RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(query.first, query.second);
		numDijkstraCalls++;
	}
}

void LRU::insertItem(uint querySize, std::vector< int > nodesInQueryResult, int sNode, int tNode)
{
	bool notEnoughSpace = true;
	if(debug) cout << "one, LRU::insertItem(" <<querySize <<"," <<nodesInQueryResult.size() <<","<<sNode<<","<<tNode<<")" << endl;
	//insert query into cache, will repeatedly remove items until there is enough space for the new item.
	do{
		if((cacheSize - cacheUsed) > querySize)
		{
			if(debug) cout << "two1, LRU::insertItem cacheSize,cacheUsed " << cacheSize <<"," << cacheUsed <<endl;
			CacheItem cItem (numTotalQueries, nodesInQueryResult);
			cache.push_back(cItem);
			cacheUsed = cacheUsed + cItem.size;
			notEnoughSpace = false;
			if(debug) cout << "two2, LRU::insertItem cacheSize,cacheUsed " << cacheSize <<"," << cacheUsed <<endl;
		}
		else if(querySize < cacheSize)
		{
			if(!cacheFull){ cacheFull = true; }
			if(debug) cout << "three1, LRU::insertItem" << cache.size() <<"," << cache[0].size <<endl;
			int itemSize = cache[0].size;
			cache.erase(cache.begin());
			cacheUsed = cacheUsed - itemSize;
			if(debug) cout << "three2, LRU::insertItem" <<endl;
		}
		else
			break;
	}while(notEnoughSpace);
	if(debug) cout << "four, LRU::insertItem" <<endl;
}


//file on the form:
//record_id, S_x, S_y, T_x, T_y.
void LRU::readTestData(string fn)
{
	cout << "one, LRU::readTestData start!" << endl;
	std::pair<double, double> firstPair, secondPair;
	string str;
	std::vector<string> tokens;

	fn.replace ((fn.size()-5), 5, ".test"); //change file extention from .train to .test
	ifstream testData (fn.c_str(), ios::in); //*.test file
    cout << "readtestData:" << fn << endl;

	//find all pairs of nodeids in the test set to have SP done for them. map nodeids to coordinates.
	if(testData.is_open())
	{
        if(debug) cout << "two, LRU::readTestData file [" << fn << "] opened!" << endl;
		while(getline(testData, str))
		{
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

			testSTPointPairs.push_back(std::make_pair(atof(tokens[1].c_str()),atof(tokens[2].c_str())));
		}
	}
	testData.close();
	cout << "two, LRU::readTestData end!" << endl;
}

//file on the form:
//record_id, S_x, S_y, T_x, T_y.
void LRU::readTrainingData(string fn)
{
	cout << "one, LRU::readTrainingData start!" << endl;
	std::pair<double, double> firstPair, secondPair;
	string str;
	std::vector<string> tokens;

	fn.replace ((fn.size()-5), 5, ".train"); //change file extention from .test to .train
	ifstream trainingData (fn.c_str(), ios::in); //*.train file
	cout<< "training fn: " << fn << endl;
	if(debug) cout << "one, LRU::readTrainingData! " << endl;
	//find all pairs of nodeids in the training set to have SP done for them. map nodeids to coordinates.
	if(trainingData.is_open())
	{
		if(debug) cout << "two, LRU::readTrainingData! " << endl;

		while(getline(trainingData, str))
		{
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

			trainingSTPointPairs.push_back(std::make_pair(atof(tokens[1].c_str()),atof(tokens[2].c_str())));


		}
		if(debug) cout << endl;
	}
	trainingData.close();
	cout << "two, LRU::readTrainingData end!" << endl;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////

hqf::hqf()
{

}

hqf::hqf(testsetting ts)
{
    this->ts = ts;
	cache.init(ts);
	cacheSize = cache.size();
	numTotalQueries = 0;
	numCacheHits = 0;
	numDijkstraCalls = 0;
	cacheFull = false;
	readMapData();
	calcScoreCounter=0;
}

hqf::~hqf()
{
    //dtor
}

void hqf::readQuery(std::pair<int,int> query)
{
	checkCache(query);
	numTotalQueries++;
}

void hqf::readQueryList(std::vector< std::pair<int,int> > queryList)
{
	RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.
	BOOST_FOREACH(intPair q, testSTPointPairs ) { readQuery(q); }

	ts.setNonEmptyRegionPairs(-1);
	ts.setBuildStatisticsTime(-1);
}

void hqf::readQueries(int numQueries, string inFn)
{
    cout<< "2.0 done" << endl;
	readTestData(ts.queryFileName);
	cout<< "2.1 done" << endl;
	readTrainingData(ts.queryFileName);
	cout<< "2.2 done" << endl;
    startTime = clock();
 	fillCache(numQueries, inFn);
	endTime = clock();
	ts.setFillCacheTime((double(endTime-startTime))/CLOCKS_PER_SEC);
	cout << "2.3 done" << endl;
}


//file on the form:
//record_id, S_id, T_id, S_x, S_y, T_x, T_y.
void hqf::readTestData(string fn)
{
	cout << "one, hqf::readTestData start!" << endl;
	std::pair<double, double> firstPair, secondPair;
	int firstPnt, secondPnt, temp;
	string str;
	std::vector<string> tokens;

	fn.replace ((fn.size()-5), 5, ".test"); //change file extention from .train to .test
	ifstream testData (fn.c_str(), ios::in); //*.test file
    cout << "training fnL" << fn << endl;

	//find all pairs of nodeids in the test set to have SP done for them. map nodeids to coordinates.
	if(testData.is_open())
	{
        if(debug) cout << "two, hqf::readTestData file [" << fn << "] opened!" << endl;
		while(getline(testData, str))
		{
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

			firstPair = std::make_pair(atof(tokens[3].c_str()),atof(tokens[4].c_str()));
			secondPair = std::make_pair(atof(tokens[5].c_str()),atof(tokens[6].c_str()));

			firstPnt = coordinate2Nodeid[firstPair];
			secondPnt = coordinate2Nodeid[secondPair];

            if(firstPnt > secondPnt){temp = firstPnt; firstPnt = secondPnt; secondPnt = temp;}

			testSTPointPairs.push_back(std::make_pair(firstPnt,secondPnt));
			if(debug) cout << "tree, hqf::readTestData end of fileline loop.!" << endl;
		}
	}
	testData.close();
	cout << "two, hqf::readTestData end! size: " << testSTPointPairs.size() << endl;
}

//file on the form:
//record_id, S_id, T_id, S_x, S_y, T_x, T_y.
void hqf::readTrainingData(string fn)
{
	cout << "hqf::readTrainingData start!" << endl;
	std::pair<double, double> firstPair, secondPair;
	int firstPnt, secondPnt, temp;
	string str;
	std::vector<string> tokens;

	fn.replace ((fn.size()-5), 5, ".train"); //change file extention from .test to .train
	ifstream trainingData (fn.c_str(), ios::in); //*.train file
	if(debug) cout << "one, hqf::readTrainingData! " << endl;
	//find all pairs of nodeids in the training set to have SP done for them. map nodeids to coordinates.
	if(trainingData.is_open())
	{
		if(debug) cout << "two, hqf::readTrainingData! " << endl;
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
	cout << "hqf::readTrainingData end! size:" << trainingSTPointPairs.size() << endl;
}

void hqf::fillCache(int numQueries, string inFn)
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
        spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(rp.first, rp.second);

       //make new cache item
        CacheItem e (iId, spResult);
        iId++;
        bucketList[rp] = e;
        tmp.pID = rp;
		tmp.dist = coordinateStats.at(rp);
		mhCache.push(tmp);

		cacheStats[rp] = std::make_pair<int,int>(coordinateStats.at(rp), spResult.size());
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

    writeoutCacheCoordinates(ts.getTestName(), cache.getCacheContentVector());
	writeoutTrainingCoordinates(ts.getTestName(), trainingSTPointPairs);
	writeoutTestCoordinates(ts.getTestName(), testSTPointPairs);

}

void hqf::checkCache(std::pair< int, int > query)
{
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
		spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(query.first, query.second);
		numDijkstraCalls++;
	}
	if(debug) cout << "four, hqf::checkCache!" << endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////

hqflru::hqflru()
{

}

hqflru::hqflru(testsetting ts)
{
    this->ts = ts;
	cache.init(ts);
	cacheSize = cache.size();
	numTotalQueries = 0;
	numCacheHits = 0;
	numDijkstraCalls = 0;
	cacheFull = false;
	readMapData();
	calcScoreCounter=0;
}

hqflru::~hqflru()
{
    //dtor
}

void hqflru::readQuery(std::pair<int,int> query)
{
    cout << "query: (" << query.first <<"," << query.second << ")" << endl;
	checkAndUpdateCache(query);
	numTotalQueries++;
}

void hqflru::readQueryList(std::vector< std::pair<int,int> > queryList)
{
    runtimeCache = cache.getCacheContentVector();
    sort(runtimeCache.begin(), runtimeCache.end());
    cacheUsed = cache.getCacheUsed();
    cout<< "3.0 Start" << endl;
	RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.
    cout<< "3.1 Done" << endl;
	BOOST_FOREACH(intPair q, testSTPointPairs ) { readQuery(q); }
    cout << "3.2 Done" << endl;

	ts.setNonEmptyRegionPairs(-1);
	ts.setBuildStatisticsTime(-1);
}

void hqflru::readQueries(int numQueries, string inFn)
{
    cout<< "2.0 done" << endl;
	readTestData(ts.queryFileName);
	cout<< "2.1 done" << endl;
	readTrainingData(ts.queryFileName);
	cout<< "2.2 done" << endl;
    startTime = clock();
 	fillCache(numQueries, inFn);
	endTime = clock();
	ts.setFillCacheTime((double(endTime-startTime))/CLOCKS_PER_SEC);
	cout << "2.3 done" << endl;
}

//file on the form:
//record_id, S_id, T_id, S_x, S_y, T_x, T_y.
void hqflru::readTestData(string fn)
{
	cout << "one, hqflru::readTestData start!" << endl;
	std::pair<double, double> firstPair, secondPair;
	int firstPnt, secondPnt, temp;
	string str;
	std::vector<string> tokens;

	fn.replace ((fn.size()-5), 5, ".test"); //change file extention from .train to .test
	ifstream testData (fn.c_str(), ios::in); //*.test file
    cout << "training fn: " << fn << endl;

	//find all pairs of nodeids in the test set to have SP done for them. map nodeids to coordinates.
	if(testData.is_open())
	{
        if(debug) cout << "two, hqflru::readTestData file [" << fn << "] opened!" << endl;
		while(getline(testData, str))
		{
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

			firstPair = std::make_pair(atof(tokens[3].c_str()),atof(tokens[4].c_str()));
			secondPair = std::make_pair(atof(tokens[5].c_str()),atof(tokens[6].c_str()));

			firstPnt = coordinate2Nodeid[firstPair];
			secondPnt = coordinate2Nodeid[secondPair];

            if(firstPnt > secondPnt){temp = firstPnt; firstPnt = secondPnt; secondPnt = temp;}

			testSTPointPairs.push_back(std::make_pair(firstPnt,secondPnt));
			if(debug) cout << "tree, hqflru::readTestData end of fileline loop.!" << endl;
		}
	}
	testData.close();
	cout << "two, hqflru::readTestData end! size: " << testSTPointPairs.size() << endl;
}

//file on the form:
//record_id, S_id, T_id, S_x, S_y, T_x, T_y.
void hqflru::readTrainingData(string fn)
{
	cout << "hqflru::readTrainingData start!" << endl;
	std::pair<double, double> firstPair, secondPair;
	int firstPnt, secondPnt, temp;
	string str;
	std::vector<string> tokens;

	fn.replace ((fn.size()-5), 5, ".train"); //change file extention from .test to .train
	ifstream trainingData (fn.c_str(), ios::in); //*.train file
	if(debug) cout << "one, hqflru::readTrainingData! " << endl;
	//find all pairs of nodeids in the training set to have SP done for them. map nodeids to coordinates.
	if(trainingData.is_open())
	{
		if(debug) cout << "two, hqflru::readTrainingData! " << endl;
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
	cout << "hqflru::readTrainingData end! size:" << trainingSTPointPairs.size() << endl;
}

void hqflru::fillCache(int numQueries, string inFn)
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
        spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(rp.first, rp.second);

       //make new cache item
        CacheItem e (iId, spResult);
        iId++;
        bucketList[rp] = e;
        tmp.pID = rp;
		tmp.dist = coordinateStats.at(rp);
		mhCache.push(tmp);

		cacheStats[rp] = std::make_pair<int,int>(coordinateStats.at(rp), spResult.size());
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

    writeoutCacheCoordinates(ts.getTestName(), cache.getCacheContentVector());
	writeoutTrainingCoordinates(ts.getTestName(), trainingSTPointPairs);
	writeoutTestCoordinates(ts.getTestName(), testSTPointPairs);

}

void hqflru::checkCache(std::pair< int, int > query)
{
	bool cacheHit = false;
	vector<int> spResult;

	if(debug) cout << "one, hqflru::checkCache! :cacheSize:" << (int) cache.size() <<"::"<< endl;
	if(cache.checkCache(query))
	{
		numCacheHits++;
		cacheHit = true;
	}

	if(debug) cout << "two, hqflru::checkCache! cacheHit: " << cacheHit << endl;
	if(!cacheHit)
	{
		if(debug) cout << "three, hqflru::checkCache!" << endl;
		spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(query.first, query.second);
		numDijkstraCalls++;
	}
	if(debug) cout << "four, hqflru::checkCache!" << endl;
}

void hqflru::checkAndUpdateCache(std::pair< int, int > query)
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
		vector<int> spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(query.first, query.second);
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

void hqflru::insertItem(uint querySize, std::vector< int > nodesInQueryResult, int sNode, int tNode)
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
			if(!cacheFull){ cacheFull = true;}
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


