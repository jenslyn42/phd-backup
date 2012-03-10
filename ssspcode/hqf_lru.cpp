#include "hqf_lru.h"
#define debug true

hqflru::hqflru()
{

}

hqflru::hqflru(testsetting ts)
{
    this->ts = ts;
	cache = aCache(ts);
	cacheSize = cache.size();
	numTotalQueries = 0;
	numCacheHits = 0;
	numDijkstraCalls = 0;
	queryNumCacheFull = -1;
	cacheFull = false;
	readMapData();
	calcScoreCounter=0;
	mapSize = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->getMapsize();
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

void hqflru::readMapData()
{
	cout << "one, hqflru::readMapData start!" << endl;
	int mapSize = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->getMapsize();
	cout << "mapsize: " << mapSize << endl;
	string mapFile = ts.getTestFile();
	std::pair<double, double> tmpPair;
	string str;
	std::vector<string> tokens;

	mapFile.replace ((mapFile.size()-4), 4, "node"); //change file extention from .cedge to .cnode
	cout << "mapfile: " << mapFile << endl;
	ifstream in_data (mapFile.c_str(), ios::in); //*.cnode file

	//read in the mapping between coordinates and node ids from *.cnode file
	if(in_data.is_open())
	{
		for(int i = 0; i < mapSize; i++)
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

	cout << "two, hqflru::readMapData end!" << endl;
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
        CacheItem e (iId, spResult, spResult.front(), spResult.back());
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
	queryNumCacheFull = cache.numberOfNodesInCache();
	ts.setItemsInCache(cache.numberOfItemsInCache());

    writeoutCacheCoordinates(ts.getTestName(), cache.getCacheContentVector(), nodeid2coordinate, ts.getSplits());
	writeoutTrainingCoordinates(ts.getTestName(), trainingSTPointPairs, nodeid2coordinate, ts.getSplits());
	writeoutTestCoordinates(ts.getTestName(), testSTPointPairs, nodeid2coordinate, ts.getSplits());

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

void hqflru::writeoutCacheCoordinates(string testbasename, vector<CacheItem> cm, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits)
{
	cout << "hqflru::writeoutCacheCoordinates start!";
	vector<int> sp;
	int i=0;
	coordinate c;
	ofstream of;
	string fn = testbasename;
	string app = "D" + boost::lexical_cast<std::string>(numSplits);
	string app2 = "C" + boost::lexical_cast<std::string>(cacheSize);
	app.append(app2);
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
				cout << "\nhqflru::writeoutCacheCoordinates ERROR:  unknown node id." << endl;
		}
		of << endl;
		i++;
	}
	of.close();

    fn = testbasename;
    app = "D" + boost::lexical_cast<std::string>(numSplits);
	app2 = "C" + boost::lexical_cast<std::string>(cacheSize);
	app.append(app2);
 	app.append(".statCache");
	fn.replace ((fn.size()-5), app.size(), app); //change file extention from .test to fnD#splits.cache
	of.open(fn.c_str(), ios::out | ios::ate | ios::app);

    BOOST_FOREACH(intPairPairs stats, cacheStats)// stats: (node1,node2) -> (seen count, sp length)
        of << stats.first.first << " " << stats.first.second << " " << stats.second.first << " " << stats.second.second << endl;

	of.close();
	cout << " ... Done!" << endl;
}

int hqflru::writeoutTestCoordinates(string testbasename, std::vector<std::pair<int, int> > stPointPairs, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits)
{
	cout << "hqflru::writeoutTestCoordinates start!";
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
				cout << "\nhqflru::writeoutTestCoordinates ERROR:  unknown node id." << endl;
		}
		of << endl;
	}
	cout << " ... Done!" << endl;
	of.close();
	return 1;
}

int hqflru::writeoutTrainingCoordinates(string testbasename, std::vector<std::pair<int, int> > stPointPairs, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits)
{
	cout << "hqflru::writeoutTrainingCoordinates start!";
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
				cout << "\nhqflru::writeoutTrainingCoordinates ERROR:  unknown node id." << endl;
		}
		of << endl;
	}
    cout << " ... Done!" << endl;
	of.close();
	return 1;
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
			CacheItem e (numTotalQueries, spResult, query.first, query.second);
			runtimeCache.push_back(e);
			cacheUsed = e.size*NODE_BITS;
		}
		if(debug) cout << "hqflru::checkAndUpdateCache 3" << endl;
	}
}

void hqflru::insertItem(int querySize, std::vector< int > nodesInQueryResult, int sNode, int tNode)
{
	bool notEnoughSpace = true;
	if(debug) cout << "one, hqflru::insertItem(" <<querySize <<"," <<nodesInQueryResult.size() <<","<<sNode<<","<<tNode<<")" << endl;
	//insert query into cache, will repeatedly remove items until there is enough space for the new item.
	do{
		if((cacheSize - cacheUsed) > querySize)
		{
			if(debug) cout << "two1, hqflru::insertItem cacheSize,cacheUsed " << cacheSize <<"," << cacheUsed <<endl;
			CacheItem cItem (numTotalQueries, nodesInQueryResult, sNode, tNode);
			runtimeCache.push_back(cItem);
			cacheUsed = cacheUsed + cItem.size*NODE_BITS;
			notEnoughSpace = false;
			if(debug) cout << "two2, hqflru::insertItem cacheSize,cacheUsed " << cacheSize <<"," << cacheUsed <<endl;
		}
		else if(querySize < cacheSize && runtimeCache.size() != 0)
		{
			if(!cacheFull){queryNumCacheFull = numTotalQueries; cacheFull = true;}
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