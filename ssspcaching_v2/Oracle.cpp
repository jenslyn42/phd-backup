#include "Oracle.h"

#define debug false

oracle::oracle()
{

}

oracle::oracle(testsetting ts)
{
    this->ts = ts;
    cache.init(ts);
    readMapData();
    readTrainingData(ts.queryFileName);
    readTestData(ts.queryFileName);
    buildPathId2spPath();
    buildNodeId2pathId();
    numTotalQueries = 0;
    numCacheHits = 0;
    numDijkstraCalls = 0;
}

oracle::~oracle()
{
    //dtor
}

void oracle::readQueries(int numQueries, std::string inFn)
{
    futureWorkloadAnswers();
    fillCache();
    print();
}

void oracle::readQuery(std::pair< int, int > query)
{
	checkCache(query);
	numTotalQueries++;
}

void oracle::readQueryList(std::vector<std::pair<int,int> > queryList)
{
    RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.
	BOOST_FOREACH(intPair q, testSTPointPairs ) { readQuery(q); }
}


//file on the form:
//record_id, S_id, T_id, S_x, S_y, T_x, T_y.
void oracle::readTestData(string fn)
{
	cout << "one, oracle::readTestData start!" << endl;
	std::pair<double, double> firstPair, secondPair;
	int firstPnt, secondPnt, temp;
	string str;
	std::vector<string> tokens;

//*********************************************************************************************
//****************************CORRECT**********************************************************
	ifstream testData (fn.c_str(), ios::in); //*.test file
    cout << "filename test: " << fn << endl;

//**********************************************************************************************
//****************************MODIFIED********************************************************'*
//	ifstream testData ("AAL_qlog_fakeEmpty.train", ios::in); //*.train file
//	cout << "training fn: " << fn << endl;
//**********************************************************************************************

	//find all pairs of nodeids in the test set to have SP done for them. map nodeids to coordinates.
	if(testData.is_open())
	{
        if(debug) cout << "two, oracle::readTestData file [" << fn << "] opened!" << endl;
		while(getline(testData, str))
		{
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

			firstPair = std::make_pair(atof(tokens[3].c_str()),atof(tokens[4].c_str()));
			secondPair = std::make_pair(atof(tokens[5].c_str()),atof(tokens[6].c_str()));

			firstPnt = coordinate2Nodeid[firstPair];
			secondPnt = coordinate2Nodeid[secondPair];

            if(firstPnt > secondPnt){temp = firstPnt; firstPnt = secondPnt; secondPnt = temp;}

			testSTPointPairs.push_back(std::make_pair(firstPnt,secondPnt));
			if(debug) cout << "tree, oracle::readTestData end of fileline loop.!" << endl;
		}
	}
	testData.close();
	cout << "two, oracle::readTestData end! size: " << testSTPointPairs.size() << endl;
}

//file on the form:
//record_id, S_id, T_id, S_x, S_y, T_x, T_y.
void oracle::readTrainingData(string fn)
{
	cout << "oracle::readTrainingData start!" << endl;
	std::pair<double, double> firstPair, secondPair;
	int firstPnt, secondPnt, temp;
	string str;
	std::vector<string> tokens;

	fn.replace ((fn.size()-5), 5, ".train"); //change file extention from .test to .train
	ifstream trainingData (fn.c_str(), ios::in); //*.train file
	cout << "training fn: " << fn << endl;
	if(debug) cout << "one, oracle::readTrainingData! " << endl;
	//find all pairs of nodeids in the training set to have SP done for them. map nodeids to coordinates.
	if(trainingData.is_open())
	{
		if(debug) cout << "two, oracle::readTrainingData! " << endl;
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
	cout << "oracle::readTrainingData end! size:" << trainingSTPointPairs.size() << endl;
}

void oracle::buildPathId2spPath()
{
    int i=0;
    vector<int> spResult;
    cout << "oracle::buildPathId2spPath start" << endl;
    BOOST_FOREACH(intPair rp, trainingSTPointPairs)
    {
        spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(rp.first, rp.second);
        pathId2spPathTraining[i] = make_pair<intPair, std::vector<int> >(rp, spResult);
        i++;
    }
    cout << "Training... Done" << endl;

    i=0;
    BOOST_FOREACH(intPair rp2, testSTPointPairs)
    {
        spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(rp2.first, rp2.second);
        pathId2spPathTest[i] = make_pair<intPair, std::vector<int> >(rp2, spResult);
        i++;
    }
    cout << "Test... Done" << endl;
}

void oracle::buildNodeId2pathId()
{
    cout << "oracle::buildNodeId2pathId start" << endl;
    vector<int> pathVector;
    BOOST_FOREACH(intIntPairVectorMap::value_type iv, pathId2spPathTraining)
    {
        pathVector = iv.second.second;
        BOOST_FOREACH(int node, pathVector)
        {
            if(nodeId2pathIdTraining.find(node) == nodeId2pathIdTraining.end())
                nodeId2pathIdTraining[node] = vector<int>(1, iv.first);
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
                nodeId2pathIdTest[node] = vector<int>(1, iv.first);
			else
				nodeId2pathIdTest.at(node).push_back(iv.first);
        }
    }
    cout << "nodeId2pathIdTest Done" << endl;
}

void oracle::futureWorkloadAnswers()
{
    cout << "oracle::futureWorkloadAnswers start" << endl;
    vector<int> v1, v2, tmp; //vectors of which paths each node belongs to
    boost::unordered_map<int, vector<int> >::iterator it1, it2;
    coordinate coor;

    BOOST_FOREACH(intIntPairVectorMap::value_type rp, pathId2spPathTest)
    {
        coor = rp.second.first;
        if((it1 = nodeId2pathIdTraining.find(coor.first)) != nodeId2pathIdTraining.end() &&
           (it2 = nodeId2pathIdTraining.find(coor.second)) != nodeId2pathIdTraining.end())
        {
            v1 = it1->second;
            v2 = it2->second;
            if(v1 > v2) {tmp = v1; v1 = v2 ; v2 = tmp;}

            BOOST_FOREACH(int pid1, v1) //pid is path id
            {
                BOOST_FOREACH(int pid2, v2)
                {
                    if(pid1 == pid2)
                    {
                        if(pathId2workloadAnswered.find(pid1) == pathId2workloadAnswered.end()){
                            pathId2workloadAnswered[pid1] = make_pair<int, vector<int> >(0, vector<int>(0, 0));
//                            if(pathId2workloadAnswered.at(pid1).second.empty() == true){cout << "empty for pid: " << pid1 << endl;}
                        }
                        pathId2workloadAnswered.at(pid1).first = pathId2workloadAnswered.at(pid1).first + 1;
                        pathId2workloadAnswered[pid1].second.push_back(rp.first);
                    }
                }
            }
        }
    }
    cout << "... Done" << endl;
}

void oracle::fillCache()
{
    boost::unordered_map<int, std::pair<int, std::vector<int> > > workloadAns = pathId2workloadAnswered;
    maxWorkloadHeap mwhCache;
    HeapWorkloadEntry awl;
    vector<int> tmpIdlist2;
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
            vector<int>& tmpIdlist = wla.second.second;
            BOOST_FOREACH(int pid1, tmpIdlist)
            {
                keepPid = true;
                BOOST_FOREACH(int pid2, awl.answeredPaths.second)
                {
                    if(pid1 == pid2)
                        keepPid = false;
                        break;
                }
                if(keepPid == true) tmpIdlist2.push_back(pid1);
            }
            workloadAns.at(wla.first).second = tmpIdlist2;
            tmpIdlist2.clear();
        }
        cout << "oracle::fillCache 4" << endl;
    }
}

maxWorkloadHeap oracle::rankWorkloadAnswered(boost::unordered_map<int, std::pair<int, std::vector<int> > > workloadAnswered)
{
//    cout << "oracle::rankWorkloadAnswered 1" << endl;
    maxWorkloadHeap mwhCache;
//    cout << "oracle::rankWorkloadAnswered 2" << endl;
    HeapWorkloadEntry temp;
//    cout << "oracle::rankWorkloadAnswered 3" << endl;
    BOOST_FOREACH(intIntVectorMap::value_type wa, workloadAnswered)
    {
        if(pathId2spPathTest.at(wa.first).second.size() != 0){
            temp.pathId = wa.first;
//            cout << "oracle::rankWorkloadAnswered 3.1 " << wa.first << endl;
            temp.pathLength = (int)pathId2spPathTest.at(wa.first).second.size(); //lenght of path
//            cout << "oracle::rankWorkloadAnswered 3.2 " << temp.pathLength << endl;
            temp.answeredPaths = wa.second;
//            cout << "oracle::rankWorkloadAnswered 3.3 " << wa.second.first << "/" << wa.second.second.size() << endl;
            mwhCache.push(temp);
        }
    }
//    cout << "oracle::rankWorkloadAnswered 4" << endl;
    return mwhCache;
}

void oracle::checkCache(std::pair< int, int > query)
{
	bool cacheHit = false;
	vector<int> spResult;

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
		spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(query.first, query.second);
		numDijkstraCalls++;
	}
	if(debug) cout << "four, probstaticcache::checkCache!" << endl;
}

void oracle::print()
{
    cout << "nIds answering: " << pathId2workloadAnswered.size() << endl;
}
