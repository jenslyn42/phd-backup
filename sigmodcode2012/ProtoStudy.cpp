#include "ProtoStudy.h"

#define debug false

///////////////////////////////////////////////////////////////////////////////////////////////////////////

Oracle::Oracle(TestSetting ts) {
	this->ts = ts;
	cache.init(ts);
	readMapData();
	readQueryLogData(QLOG_TRAIN);
	readQueryLogData(QLOG_TEST);
	buildPathId2spPath();
	buildNodeId2pathId();
	numTotalQueries = 0;
	numCacheHits = 0;
	numDijkstraCalls = 0;
}

Oracle::~Oracle() { }

void Oracle::buildCache() {
	futureWorkloadAnswers();
	fillCache();
	print();
}

void Oracle::runQueryList() {
	RoadGraph::mapObject(ts)->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.
	BOOST_FOREACH(intPair q, testSTPointPairs ) { 
		checkCache(q);
		numTotalQueries++;
	}
}


void Oracle::buildPathId2spPath() {
	int i=0;
	intVector spResult;
	cout << "oracle::buildPathId2spPath start" << endl;
	BOOST_FOREACH(intPair rp, trainingSTPointPairs)
	{
		spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(rp.first, rp.second);
		pathId2spPathTraining[i] = make_pair<intPair, intVector >(rp, spResult);
		i++;
	}
	cout << "Training... Done" << endl;

	i=0;
	BOOST_FOREACH(intPair rp2, testSTPointPairs)
	{
		spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(rp2.first, rp2.second);
		pathId2spPathTest[i] = make_pair<intPair, intVector >(rp2, spResult);
		i++;
	}
	cout << "Test... Done" << endl;
}

void Oracle::buildNodeId2pathId()
{
	cout << "oracle::buildNodeId2pathId start" << endl;
	intVector pathVector;
	BOOST_FOREACH(intIntPairVectorMap::value_type iv, pathId2spPathTraining)
	{
		pathVector = iv.second.second;
		BOOST_FOREACH(int node, pathVector)
		{
			if(nodeId2pathIdTraining.find(node) == nodeId2pathIdTraining.end())
				nodeId2pathIdTraining[node] = intVector(1, iv.first);
			else
				nodeId2pathIdTraining.at(node).push_back(iv.first);
		}
	}
	cout << "nodeId2pathIdTraining Done" << endl;

	BOOST_FOREACH(intIntPairVectorMap::value_type iv, pathId2spPathTest)
	{
		pathVector = iv.second.second;
		BOOST_FOREACH(int node, pathVector)
		{
			if(nodeId2pathIdTest.find(node) == nodeId2pathIdTest.end())
				nodeId2pathIdTest[node] = intVector(1, iv.first);
			else
				nodeId2pathIdTest.at(node).push_back(iv.first);
		}
	}
	cout << "nodeId2pathIdTest Done" << endl;
}

void Oracle::futureWorkloadAnswers()
{
  cout << "oracle::futureWorkloadAnswers start" << endl;
  intVector v1, v2, tmp; //vectors of which paths each node belongs to
  boost::unordered_map<int, intVector >::iterator it1, it2;
  Point coor;

  BOOST_FOREACH(intIntPairVectorMap::value_type rp, pathId2spPathTest) {
    coor = rp.second.first;
    if((it1 = nodeId2pathIdTraining.find(coor.first)) != nodeId2pathIdTraining.end() &&
      (it2 = nodeId2pathIdTraining.find(coor.second)) != nodeId2pathIdTraining.end()){
      v1 = it1->second;
      v2 = it2->second;
      if(v1 > v2) {tmp = v1; v1 = v2 ; v2 = tmp;}

      BOOST_FOREACH(int pid1, v1) {//pid is path id
	BOOST_FOREACH(int pid2, v2) {
	  if(pid1 == pid2){
	    if(pathId2workloadAnswered.find(pid1) == pathId2workloadAnswered.end())
	      pathId2workloadAnswered[pid1] = make_pair<int, intVector >(0, intVector(0, 0));

	    pathId2workloadAnswered.at(pid1).first = pathId2workloadAnswered.at(pid1).first + 1;
	    pathId2workloadAnswered[pid1].second.push_back(rp.first);
	  }
	}
      }
    }
  }
  cout << "... Done" << endl;
}

void Oracle::fillCache()
{
	boost::unordered_map<int, pair<int, intVector > > workloadAns = pathId2workloadAnswered;
	maxWorkloadHeap mwhCache;
	HeapWorkloadEntry awl;
	intVector tmpIdlist2;
	bool keepPid = true;

	while(!workloadAns.empty())
	{
		awl.pathId=-1; awl.pathLength=-1;awl.answeredPaths.first=-1;
		cout << "oracle::fillCache 0.1" << endl;
		mwhCache = rankWorkloadAnswered(workloadAns);
		if(mwhCache.size() == 0) break;
		cout << "oracle::fillCache 0.2" << endl;
		awl = mwhCache.top();
		cout << "oracle::fillCache 0.3 mwhCache/workloadAns size, top:id/length/size " << mwhCache.size() << "/" << workloadAns.size() << ", " << awl.pathId << "/" << awl.pathLength << "/" << awl.answeredPaths.second.size() << endl;
		mwhCache.pop();
		cout << "oracle::fillCache 1" << endl;

		CacheItem e (awl.pathId, pathId2spPathTraining.at(awl.pathId).second);
		if(cache.hasEnoughSpace(e))
			if(cache.insertItem(e)){}
		cout << "oracle::fillCache 2" << endl;
		workloadAns.erase(awl.pathId);
		cout << "oracle::fillCache 3" << endl;
		BOOST_FOREACH(intIntVectorMap::value_type wla, workloadAns)
		{
			cout << "size: " << wla.second.second.size() << endl;
			intVector& tmpIdlist = wla.second.second;
			BOOST_FOREACH(int pid1, tmpIdlist)
			{
				keepPid = true;
				BOOST_FOREACH(int pid2, awl.answeredPaths.second)
				{
					if(pid1 == pid2){
						keepPid = false;
						break;
					}
				}
				if(keepPid == true) tmpIdlist2.push_back(pid1);
			}
			workloadAns.at(wla.first).second = tmpIdlist2;
			tmpIdlist2.clear();
		}
		cout << "oracle::fillCache 4" << endl;
	}
}

maxWorkloadHeap Oracle::rankWorkloadAnswered(boost::unordered_map<int, pair<int, intVector > > workloadAnswered)
{
	maxWorkloadHeap mwhCache;
	HeapWorkloadEntry temp;

	BOOST_FOREACH(intIntVectorMap::value_type wa, workloadAnswered)
	{
		if(pathId2spPathTest.at(wa.first).second.size() != 0){
			temp.pathId = wa.first;
			temp.pathLength = (int)pathId2spPathTest.at(wa.first).second.size(); //lenght of path
			temp.answeredPaths = wa.second;
			mwhCache.push(temp);
		}
	}
	return mwhCache;
}

void Oracle::checkCache(intPair query)
{
	bool cacheHit = false;
	intVector spResult;

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
		spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(query.first, query.second);
		numDijkstraCalls++;
	}
	if(debug) cout << "four, probstaticcache::checkCache!" << endl;
}

void Oracle::print()
{
	cout << "nIds answering: " << pathId2workloadAnswered.size() << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

Scache::Scache(TestSetting ts)
{
	this->ts = ts;
	cacheSize = ts.cacheSize;
	numTotalQueries = 0;
	numCacheHits = 0;
	cacheUsed = 0;
	numDijkstraCalls = 0;
	calcScoreCounter=0;
}

Scache::~Scache()
{
}

void Scache::runQueryList()
{
	BOOST_FOREACH(intPair q, testSTPointPairs ) { 
		checkCache(q);
		numTotalQueries++;
	}
}

void Scache::checkCache(intPair query)
{
	bool cacheHit = false;
	intVector spResult;

	//If test is done with optimal substructure
	if(debug) cout << "one1, scache::checkCache! :cacheSize:" << (int) cache.size() <<"::"<< endl;
	BOOST_FOREACH(CacheItem ci, cache )
	{
		if (find(ci.item.begin(),ci.item.end(), query.first) != ci.item.end() && 
		    find(ci.item.begin(),ci.item.end(), query.second) != ci.item.end())
		{
			numCacheHits++;
			cacheHit = true;
			break;
		}
	}

	if(debug) cout << "four, scache::checkCache! cacheHit: " << cacheHit << endl;
	if(!cacheHit)
	{
		if(debug) cout << "five, scache::checkCache!" << endl;
		spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(query.first, query.second);

		numDijkstraCalls++;
	}
	if(debug) cout << "six, scache::checkCache!" << endl;
}

/**	Generate random queries.
*	numQueries:	number of queries to be generated
*	maxVal:		highest query value to be generated, usually size of graph (nodes)
*	outFn:		file to write the queries to.
**/
void Scache::generateRandQueries(uint numQueries, uint maxVal, string outFn)
{
	cout << "scache::generateRandQueries! START" << endl;
	intPair p;
	intVector spResult;
	ofstream cacheFile;
	cacheFile.open(outFn.c_str(), ios::out);

	//write queries to file
	//format:
	//[number of queries/lines in file] *at first line*
	//[line number][space][length of vertex list][space][space separated vertex list] *remaining lines*

	cacheFile << numQueries << endl;

	for(uint i = 0; i < numQueries; i++)
	{
		if(i%5000 == 0) cout << "i:" << i << endl;
		srand((int)rand());
		p.first = (rand()%(maxVal-1)) +1;
		p.second = (rand()%(maxVal-1)) +1;
		spResult.clear();
		spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(p.first, p.second);

		///write out to file, the cache result as one line
		cacheFile << i << " " << spResult.size() << " ";
		BOOST_FOREACH(int vertexID, spResult ){ cacheFile << vertexID << " ";}
		cacheFile << endl;
	}

	cout << "scache::generateRandQueries! END" << endl;

	cacheFile.close();
}

/** 	Generate sets of k queries, choosing the query with largest euclidian distance and adding it to the cache.
*	numQueries: 	number of queries to be generated
*	maxVal:		highest query value to be generated, usually size of graph (nodes)
*	k:		size of each query set to choose a candidate from.
*	outFn:		file to write the queries to.
**/
void Scache::generateRandLongQueries(uint numQueries, uint maxVal, uint k, string outFn)
{
	cout << "scache::generateLongQueries! START" << endl;
	intPair p, candQuery;
	intVector spResult;
	vector<intPair > tmpQueryCand;
	ofstream cacheFile;

	cacheFile.open(outFn.c_str(), ios::out);

	//write queries to file
	//format:
	//[number of queries/lines in file] *at first line*
	//[line number][space][length of vertex list][space][space seperated vertex list] *remaining lines*

	cacheFile << numQueries << endl;

	for(uint i = 0; i < numQueries; i++)
	{
		if(i%5000 == 0) cout << "i:" << i << endl;
		srand((int)rand());
		tmpQueryCand.clear();
		for(uint f = 0; f < k; f++)
		{
			p.first = (rand()%(maxVal-1)) +1;
			p.second = (rand()%(maxVal-1)) +1;
			tmpQueryCand.push_back(p);
		}
		candQuery = findLargestQuery(tmpQueryCand, k);
		spResult.clear();
		spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(candQuery.first, candQuery.second);


		BOOST_FOREACH(int vertexID, spResult ){ cout << vertexID << " ";}
		///write out to file, the cache result as one line
		cacheFile << i << " " << spResult.size() << " ";
		BOOST_FOREACH(int vertexID, spResult ){ cacheFile << vertexID << " ";}
		cacheFile << endl;
	}

	cout << "scache::generateLongQueries! END" << endl;

	cacheFile.close();
}

///support for generateLongQueries()
intPair Scache::findLargestQuery(vector<intPair> cCandidates, uint k)
{
	boost::unordered_map<int,Point> node_coodinates; // node ids and their coordinates
	int nodeIDs [k*2];
	string line;
	vector<string> tokens;
	ifstream cnodeFile;
	cnodeFile.open(ts.getTestFile().c_str(), ios::in);

	//extract all node ids from the queries for easier comparizon when reading .cnode file
	int i = 0;
	BOOST_FOREACH(intPair cand, cCandidates )
	{
		nodeIDs[i] = cand.first;
		nodeIDs[i+1] = cand.second;
		i = i+2;
	}

	//find coordinates of all nodes and add them to node_coodinates.
	while(cnodeFile.good())
	{
		getline(cnodeFile, line);
		boost::algorithm::split(tokens, line, boost::algorithm::is_space());
		int tmpVal;
		for(uint t = 0; t<k*2; t++){
			tmpVal = atoi(tokens[0].c_str());
			if(tmpVal == nodeIDs[t]){
				node_coodinates[nodeIDs[t]] = pair<double,double>(atoi(tokens[1].c_str()),atoi(tokens[2].c_str()));
			}
		}
	}

	//calculate distance and find query pair with largest euclidian distance
	intPair largestDistPair;
	double dist = 0, largestDist = 0;
	double firstVal, secondVal;
	pair<double,double>  curFirstCoor;
	pair<double,double>  curSecondCoor;
	BOOST_FOREACH(intPair cand, cCandidates )
	{
		curFirstCoor = node_coodinates.at(cand.first);
		curSecondCoor = node_coodinates.at(cand.second);
		firstVal = curFirstCoor.first - curSecondCoor.first;
		secondVal = curFirstCoor.second - curSecondCoor.second;
		dist = sqrt((firstVal*firstVal)+(secondVal*secondVal));
		if(dist >= largestDist)
		{
			largestDist = dist;
			largestDistPair = cand;
		}
	}
	return largestDistPair;
}

void Scache::buildCache() {
	// extract parameter from "ts"
	string inFn=ts.queryFileName;
	
	switch(ts.scacheQueryType)
	{
		case 1:
			readQ(inFn);
			break;
		case 2:
	 		readLargestQueries(inFn);
			break;
		case 3:
			readScoredQueries(inFn);
			break;
		case 4:
			readScoredQueriesFromTrainFile(inFn);
			break;
	}

}

void Scache::readQ(string inFn)
{
	int i=0, spSize=0;
	intPair p;
	string line;
	vector<string> tokens;
	intVector sp;
	ifstream querysFile (inFn.c_str(), ios::in);
	bool cacheNotFull = true;

	if( querysFile.is_open() )
	{
	cout << "scache::readQ! Starting to load queries into cache" << endl;
		getline(querysFile,line); //read first line: number of lines/queries in file.

		while( querysFile.good() && cacheNotFull)
		{
			getline(querysFile, line);
			boost::algorithm::split(tokens, line, boost::algorithm::is_space());

			spSize = atoi(tokens[1].c_str());

			if(cacheUsed + spSize < cacheSize){
				sp.clear();
				for(int t = 0; t < spSize; t++)
					sp.push_back(atoi(tokens[t+2].c_str()));

				CacheItem e (i++, sp);
				cache.push_back(e);
				cacheUsed += e.size;
			}else{
				cacheNotFull = false;
			}
		}
	}
	cout << "scache::readQ! Queries loaded into cache" << endl;

	statistics("glCacheAnalysisRandStatic100000");
	pairStatisticsStoT("glCachePairStoTAnalysisRandStatic10000");
	pairStatisticsAll("glCachePairAllAnalysisRandStatic10000");
}

void Scache::readLargestQueries(string inFn)
{
	int i=0, spSize=0, curLine=0;
	Heap mh; //a map to keep track of the position and size of the elements already in the cache, for easy removal.
	HeapEntry tmp;
	intPair p;
	boost::unordered_map<int, CacheItem> tmpCache;
	string line;
	vector<string> tokens;
	intVector sp;
	ifstream querysFile (inFn.c_str(), ios::in);

	if( querysFile.is_open() )
	{
	cout << "scache::readLargestQueries! Starting to load queries into cache" << endl;
		getline(querysFile,line); //read first line: number of lines/queries in file.

		int filelines = atoi(line.c_str());
		cout << filelines << endl;
		while( curLine < filelines-1 )
		{

			getline(querysFile, line);
			boost::algorithm::split(tokens, line, boost::algorithm::is_space());
			curLine = atoi(tokens[0].c_str());
			spSize = atoi(tokens[1].c_str());

			if(cacheUsed + spSize < cacheSize){
				sp.clear();
				for(int t = 0; t < spSize; t++) sp.push_back(atoi(tokens[t+2].c_str()));

				CacheItem e (i++, sp);
  				tmpCache[i] = e;
				cacheUsed += e.size;

				HeapEntry he;
				he.id= i;
				he.dist= (double) spSize;
				mh.push(he);
			}else if(spSize > (int) mh.top().dist){
				do{
					tmp= mh.top();
					mh.pop();
 					tmpCache.erase(tmp.id);
					cacheUsed -= (int) tmp.dist;
				}while(spSize > (int) mh.top().dist && cacheUsed + spSize > cacheSize);

				//insert new element element if there is space enough in cache.
				if(cacheUsed + spSize < cacheSize){
					sp.clear();
					for(int t = 0; t < spSize; t++) sp.push_back(atoi(tokens[t+2].c_str()));
					CacheItem e (i++, sp);
 					tmpCache[i] = e;
					cacheUsed += e.size;

					HeapEntry he;
					he.id= i;
					he.dist= (double) spSize;
					mh.push(he);
				}
			}
		}
	}

	while(mh.size() > 0)
	{
		tmp= mh.top();
		mh.pop();
		cache.push_back(tmpCache.at((int)tmp.id));
	}
	cout << "scache::readLargestQueries! " << cache.size() << " Queries loaded into cache" << endl;

	// statistics("glCacheAnalysisRandStatic100000");
	pairStatisticsStoT("glCachePairStoTAnalysisRandStaticLongest100000");
	pairStatisticsAll("glCachePairAllAnalysisRandStaticLongest100000");

}

void Scache::readScoredQueries(string inFn)
{
	uint i=0, spSize=0, filelines=0, curLine=0;
	unsigned long cacheUsed=0;
	maxHeap mh; //a map to keep track of the score of each potential cache item
	HeapEntry tmp, tmp2;
	CacheItem tmpItem;
	boost::unordered_map<int, CacheItem> allSPpaths; //used to load all SP results into memory
	string line;
	vector<string> tokens;
	intVector sp;
	boost::unordered_map<int, int> verticesSeen;
	ifstream querysFile (inFn.c_str(), ios::in);

	readMapData(); //fill up structures such as coordinate2Nodeid
	readQueryLogData(QLOG_TEST); //read *.test file into memory, queries for the cache.

	cout << "ts.queryFileName: " << ts.queryFileName << endl;
	if( querysFile.is_open() )
	{
	cout << "scache::readScoredQueries! Starting to load queries into memory" << endl;
		getline(querysFile,line); //read first line: number of lines/queries in file.

		filelines = atoi(line.c_str());
		cout << filelines << endl;

		//Read all queries into memory and add each cacheItems score to mh.
		while( curLine < filelines-1 )
		{
			getline(querysFile, line);
			boost::algorithm::split(tokens, line, boost::algorithm::is_space());
			curLine = atoi(tokens[0].c_str());
			spSize = atoi(tokens[1].c_str());

			//make new cache item
			sp.clear();
			for(uint t = 0; t < spSize; t++) sp.push_back(atoi(tokens[t+2].c_str()));
			CacheItem e (i, sp);

			//add cache item to map holding all queries from file
			allSPpaths[i] = e;
			tmp.id = i;
			tmp.dist = (double) spSize;
			mh.push(tmp);
			i++;
		}

	}
	cout << "scache::readScoredQueries! done loading queries into memory" << endl;
	cout << "scache::readScoredQueries! aSPp size:" << allSPpaths.size() << endl;
	//add the largest item to the cache and add all nodes seen to verticesSeen.
	tmp= mh.top();
	cache.push_back(allSPpaths[tmp.id]);
	allSPpaths.erase(tmp.id);
	mh.pop();
	BOOST_FOREACH(int vertice, (allSPpaths[tmp.id]).item)
	{
		verticesSeen[vertice] = 1;
	}

	//add highest scoring cacheItem to cache, and update candidate scores.
	uint score = 0;
	uint curItem = 0;
	do{
		calcScoreMap[curItem] = calcScoreMap[curItem] +1;
		tmp= mh.top();
		mh.pop();
		tmpItem = allSPpaths[tmp.id];
		score = calcScore(tmpItem.item, verticesSeen);
		spSize = tmpItem.size;

		if(score >= mh.top().dist && cacheUsed + spSize <= cacheSize){
			if(tmpItem.size + cache.size() > cacheSize) break; //break loop if new item is to large
			cache.push_back(tmpItem);
			cacheUsed = cacheUsed + spSize;
			curItem++;

			BOOST_FOREACH(int vertice, tmpItem.item)
			{
				verticesSeen[vertice] = 1;
			}
		}else if(cacheUsed + spSize <= cacheSize){
			tmp.dist = score;
			mh.push(tmp);
		}
	}while(cacheSize > cacheUsed && !mh.empty());
	cout << "scache::readScoredQueries! cache size:" << cache.size() << "/" << cacheUsed<< endl;
	ts.setItemsInCache(cache.size());
	plotCachePoints(cache);
	cout << "scache::calcScoreCounter: " << calcScoreCounter << endl;
}

void Scache::readScoredQueriesFromTrainFile(string fn)
{
	readMapData();

 	maxHeap mh; //a map to keep track of the score of each potential cache item
	HeapEntry tmp, tmp2;
	CacheItem tmpItem;
	boost::unordered_map<int, CacheItem> allSPpaths; //used to load all SP results into memory
	boost::unordered_map<int, int> verticesSeen;

	intVector spResult;
	pair<double, double> firstPair, secondPair;
	uint firstPnt, secondPnt, i=0, spSize=0;
	unsigned long cacheUsed=0;
	string str;
	vector<string> tokens;

	ifstream trainingData (fn.c_str(), ios::in); //*.train file
	if(debug) cout << "one, scache::readScoredQueriesFromTrainFile! " << endl;
	//find all pairs of nodeids in the training set to have SP done for them. map nodeids to coordinates.
	if(trainingData.is_open())
	{
		if(debug) cout << "two, scache::readScoredQueriesFromTrainFile! " << endl;
		while(getline(trainingData, str))
		{
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

			firstPair = make_pair(atof(tokens[1].c_str()),atof(tokens[2].c_str()));
			secondPair = make_pair(atof(tokens[3].c_str()),atof(tokens[4].c_str()));

			firstPnt = Point2Nodeid[firstPair];
			secondPnt = Point2Nodeid[secondPair];

			trainingSTPointPairs.push_back(make_pair(firstPnt,secondPnt));
		}
	}
	trainingData.close();

	BOOST_FOREACH(intPair p, trainingSTPointPairs)
	{
		spResult.clear();
		spResult = RoadGraph::mapObject(ts)->dijkstraSSSP(p.first, p.second);
		spSize = spResult.size();

		//make new cache item
		CacheItem e (i, spResult);

		//add cache item to map holding all queries from file
		allSPpaths[i] = e;
		tmp.id = i;
		tmp.dist = (double) spSize;
		mh.push(tmp);
		i++;
	}

	cout << "scache::readScoredQueriesFromTrainFile! done loading & converting queries in memory" << endl;
	//add the largest item to the cache and add all nodes seen to verticesSeen.
	tmp= mh.top();
	cache.push_back(allSPpaths[tmp.id]);
	allSPpaths.erase(tmp.id);
	mh.pop();
	BOOST_FOREACH(int vertice, (allSPpaths[tmp.id]).item)
	{
		verticesSeen[vertice] = 1;
	}

	//add highest scoring cacheItem to cache, and update candidat scores.
	uint score = 0;
	do{
		tmp= mh.top();
		mh.pop();
		tmpItem = allSPpaths[tmp.id];
		score = calcScore(tmpItem.item, verticesSeen);
		spSize = tmpItem.size;

		if(score > mh.top().dist && cacheUsed + spSize <= cacheSize){
			if(tmpItem.size + cache.size() > cacheSize) break; //break loop if new item is to large
			cache.push_back(tmpItem);
			cacheUsed = cacheUsed + spSize;
			BOOST_FOREACH(int vertice, tmpItem.item)
			{
				verticesSeen[vertice] = 1;
			}
		}else if(cacheUsed + spSize <= cacheSize)
		{
			tmp.dist = score;
			mh.push(tmp);
			cout << "2" << endl;
		}
		if(debug) cout << "scache::readScoredQueriesFromTrainFile! cacheSize: " << cache.size() <<":" << cacheUsed << " (" << mh.size() << ")" << endl;
	} while(cacheSize >= cacheUsed && !mh.empty());

	plotCachePoints(cache);
}


///calculate score to use in readScoredQueries()
int Scache::calcScore(intVector sp, boost::unordered_map<int, int> vSeen)
{
	int score = 0;
	calcScoreCounter++;
	BOOST_FOREACH(int v1, sp)
	{
		BOOST_FOREACH(int v2, sp)
		{
			if(v1 != v2 && vSeen.find(v1) == vSeen.end() && vSeen.find(v2) == vSeen.end())
				score++;
		}
	}
	return score/2;
}

///writes out to file:outFn the count of each node in the cache,
///which is member of an item completely covered by another item.
void Scache::statistics(string outFn)
{
	cout << "statistics(" << outFn << ") start" << endl;
	intVector iItem, kItem;
	int kMin,kMax,iMin,iMax, count=0;

	for(uint i=0; i< cache.size(); i++) {
		iItem = cache.at(i).item;
		iMin = cache.at(i).item.at(0);
		iMax = cache.at(i).item.at((cache.at(i).size)-1);
		for(uint k=0; k< cache.size(); k++) {
			kItem = cache.at(i).item;
			kMin = cache.at(k).item.at(0);
			kMax = cache.at(k).item.at((cache.at(k).size)-1);
			if(i != k && (find(iItem.begin(), iItem.end(), kMin) != iItem.end() &&
				find(iItem.begin(), iItem.end(), kMax) != iItem.end())) {
 					count++;
			}
		}
	}
	cout << "cacheSize " << cache.size() <<", count " << count << endl;


	intMap nodePopularity;

	BOOST_FOREACH(CacheItem ci, cache){
		BOOST_FOREACH(int node, ci.item){
			nodePopularity[node] = nodePopularity[node] + 1;
		}
	}
	///file output
	ofstream resultfile;
	resultfile.open(outFn.c_str(), ios::out | ios::ate | ios::app);

	BOOST_FOREACH(intMap::value_type i, nodePopularity){
		resultfile << i.first <<"\t" << i.second << endl;
	}
	resultfile.close();

	cout << "statistics(" << outFn << ") end" << endl;
}

void Scache::pairStatisticsStoT(string outFn)
{
	cout << "pairStatisticsStoT(" << outFn << ") start" << endl;
	intPairIntMap map;
	intPair tmpPair;

	BOOST_FOREACH(CacheItem ci, cache){
		if(ci.s < ci.t)	tmpPair =make_pair<int,int>(ci.s,ci.t);
		else tmpPair = make_pair<int,int>(ci.t,ci.s);

		map[tmpPair] = map[tmpPair] +1;
	}

	ofstream resultfile;
	resultfile.open(outFn.c_str(), ios::out | ios::ate | ios::app);

	BOOST_FOREACH(intPairIntMap::value_type i, map){
		resultfile << i.first.first <<"\t" << i.first.second << "\t" << i.second << endl;
	}
	resultfile.close();

	cout << "pairStatisticsStoT(" << outFn << ") end" << endl;
}

void Scache::pairStatisticsAll(string outFn)
{
	cout << "pairStatisticsAll(" << outFn << ") start" << endl;
	intPairIntMap map;
	intPair tmpPair;

	BOOST_FOREACH(CacheItem ci, cache){
		BOOST_FOREACH(int node1, ci.item){
			BOOST_FOREACH(int node2, ci.item){
				if(node1 != node2){
					if(node1 < node2)
						tmpPair =make_pair<int,int>(node1,node2);
					else
						tmpPair = make_pair<int,int>(node2,node1);
				}
				map[tmpPair] = map[tmpPair] +1;
			}
		}
	}

	ofstream resultfile;
	resultfile.open(outFn.c_str(), ios::out | ios::ate | ios::app);

	BOOST_FOREACH(intPairIntMap::value_type i, map){
		resultfile << i.first.first <<"\t" << i.first.second << "\t" << i.second << endl;
	}
	resultfile.close();

	cout << "pairStatisticsAll(" << outFn << ") end" << endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////

