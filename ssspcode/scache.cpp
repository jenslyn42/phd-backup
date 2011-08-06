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
#include "scache.h"
#define debug false

scache::scache(testsetting ts)
{
	this->ts = ts;
	cacheSize = ts.getCacheSize();
	numTotalQueries = 0;
	numCacheHits = 0;
	cacheSize = ts.getCacheSize();
	cacheUsed = 0;
	useNodeScore = ts.isUseHitScore();
	useHitScore = ts.isUseNodeScore();
	numDijkstraCalls = 0;
	queryNumCacheFull = -1;
	cacheFull = false;
	calcScoreCounter=0;
}

scache::~scache()
{
}

void scache::readQuery(std::pair< int, int > query)
{
	checkCache(query);
	numTotalQueries++;
}

// void scache::readQueryList(std::vector< std::pair < int , int > > queryList)
// {
// 	int i = 0;
//  	BOOST_FOREACH(intPair q, queryList ) {
// 	i++;
// 	if(i%10000 == 0) cout << "Query " << i << " done" << endl;
// 	readQuery(q);
// 	}
// }

void scache::readQueryList(std::vector< std::pair < int , int > > queryList)
{
	BOOST_FOREACH(intPair q, testSTPointPairs ) { readQuery(q); }
}

void scache::checkCache(std::pair< int, int > query)
{
	bool cacheHit = false;
	vector<int> spResult;

	if(ts.isUseOptimalSubstructure()){//If test is done with optimal substructure
		if(debug) cout << "one1, scache::checkCache! :cacheSize:" << (int) cache.size() <<"::"<< endl;
		BOOST_FOREACH(CacheItem ci, cache )
		{
			if(find(ci.item.begin(),ci.item.end(), query.first) != ci.item.end() && find(ci.item.begin(),ci.item.end(), query.second) != ci.item.end())
			{
				numCacheHits++;
				cacheHit = true;
				break;
			}
		}
	}else{ //if test is done without optimal substructure enabled.
		BOOST_FOREACH(CacheItem ci, cache )
		{
			if(debug) cout << "three, scache::checkCache! " << endl;
			if(query.first == ci.s && query.second == ci.t)
			{
				numCacheHits++;
				cacheHit = true;
				break;
			}
		}
	}

	if(debug) cout << "four, scache::checkCache! cacheHit: " << cacheHit << endl;
	if(!cacheHit)
	{
		if(debug) cout << "five, scache::checkCache!" << endl;
		spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(query.first, query.second);

		numDijkstraCalls++;
	}
	if(debug) cout << "six, scache::checkCache!" << endl;
}

/**	Generate random queries.
*	numQueries:	number of queries to be generated
*	maxVal:		highest query value to be generated, usually size of graph (nodes)
*	outFn:		file to write the queries to.
**/
void scache::generateRandQueries(int numQueries, int maxVal, string outFn)
{
	cout << "scache::generateRandQueries! START" << endl;
	pair<int,int> p;
	vector<int> spResult;
	ofstream cacheFile;
	cacheFile.open(outFn.c_str(), ios::out);

	//write queries to file
	//format:
	//[number of queries/lines in file] *at first line*
	//[line number][space][length of vertex list][space][space seperated vertex list] *remaining lines*

	cacheFile << numQueries << endl;

	for(int i = 0; i < numQueries; i++)
	{
		if(i%5000 == 0) cout << "i:" << i << endl;
		srand((int)rand());
		p.first = (rand()%(maxVal-1)) +1;
		p.second = (rand()%(maxVal-1)) +1;
		spResult.clear();
		spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(p.first, p.second);

// 		cout << i << " " << spResult.size() << " " << p.first <<":" <<p.second <<" ";
// 		BOOST_FOREACH(int vertexID, spResult ){ cout << vertexID << " ";}
// 		cout << endl;
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
void scache::generateRandLongQueries(int numQueries, int maxVal, int k, string outFn)
{
	cout << "scache::generateLongQueries! START" << endl;
	pair<int,int> p, candQuery;
	vector<int> spResult;
	vector<pair<int,int> > tmpQueryCand;
	ofstream cacheFile;

	cacheFile.open(outFn.c_str(), ios::out);

	//write queries to file
	//format:
	//[number of queries/lines in file] *at first line*
	//[line number][space][length of vertex list][space][space seperated vertex list] *remaining lines*

	cacheFile << numQueries << endl;

	for(int i = 0; i < numQueries; i++)
	{
		if(i%5000 == 0) cout << "i:" << i << endl;
		srand((int)rand());
		tmpQueryCand.clear();
		for(int f = 0; f < k; f++)
		{
			p.first = (rand()%(maxVal-1)) +1;
			p.second = (rand()%(maxVal-1)) +1;
			tmpQueryCand.push_back(p);
		}
		candQuery = findLargestQuery(tmpQueryCand, k);
		spResult.clear();
		spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(candQuery.first, candQuery.second);

//	cout << i << " " << spResult.size() << " " << p.first <<":" <<p.second <<" ";
// 	e problem, så kan vi evt bestille flere hjem sammen.

	BOOST_FOREACH(int vertexID, spResult ){ cout << vertexID << " ";}
// 		cout << endl;
		///write out to file, the cache result as one line
		cacheFile << i << " " << spResult.size() << " ";
		BOOST_FOREACH(int vertexID, spResult ){ cacheFile << vertexID << " ";}
		cacheFile << endl;
	}

	cout << "scache::generateLongQueries! END" << endl;

	cacheFile.close();
}

//support for generateLongQueries()
intPair scache::findLargestQuery(vector<pair<int, int> > cCandidates, int k)
{
	boost::unordered_map<int, pair<double,double> > node_coodinates; // node ids and their coordinates
	int nodeIDs [k*2];
	string line;
	std::vector<string> tokens;
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
		for(int t = 0; t<k*2; t++){
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

void scache::readQueries(int numQueries, string inFn)
{
	switch(ts.getStaticQueryType())
	{
		case 1:
			readQ(numQueries, inFn);
			break;
		case 2:
	 		readLargestQueries(numQueries, inFn);
			break;
		case 3:
			readScoredQueries(numQueries, inFn);
			break;
		case 4:
			readScoredQueriesFromTrainFile(numQueries, inFn);
			break;
	}

}

void scache::readQ(int numQueries, string inFn)
{
	int i=0, spSize=0;
	pair<int,int> p;
	string line;
	std::vector<string> tokens;
	std::vector<int> sp;
	ifstream querysFile (inFn.c_str(), ios::in);
	bool cacheNotFull = true;

	if( querysFile.is_open() )
	{
	cout << "scache::readQueries! Starting to load queries into cache" << endl;
		getline(querysFile,line); //read first line: number of lines/queries in file.

		while( querysFile.good() && cacheNotFull)
		{
			getline(querysFile, line);
			boost::algorithm::split(tokens, line, boost::algorithm::is_space());

			spSize = atoi(tokens[1].c_str());

			if(cacheUsed + spSize < cacheSize){
				sp.clear();
				for(int t = 0; t < spSize; t++) sp.push_back(atoi(tokens[t+2].c_str()));

				CacheItem e (i++, sp, atoi(tokens[2].c_str()), atoi(tokens[spSize+1].c_str()));
				cache.push_back(e);
				cacheUsed += e.size;
			}else{
				cacheNotFull = false;
			}
		}
	}
	queryNumCacheFull = i;
	cout << "scache::readQueries! Queries loaded into cache" << endl;

	statistics("glCacheAnalysisRandStatic100000");
	pairStatisticsStoT("glCachePairStoTAnalysisRandStatic10000");
	pairStatisticsAll("glCachePairAllAnalysisRandStatic10000");
}

void scache::readLargestQueries(int numQueries, string inFn)
{
	int i=0, spSize=0, curLine=0;
	Heap mh; //a map to keep track of the position and size of the elements already in the cache, for easy removal.
	HeapEntry tmp;
	pair<int,int> p;
	boost::unordered_map<int, CacheItem> tmpCache;
	string line;
	std::vector<string> tokens;
	std::vector<int> sp;
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
//cout << "i:" << i <<" line: " << line << " cacheUsed " << cacheUsed << endl;
			boost::algorithm::split(tokens, line, boost::algorithm::is_space());
// if(i >104950) cout << i <<" " << tokens[0] << " " << tokens[1] << endl;
			curLine = atoi(tokens[0].c_str());
			spSize = atoi(tokens[1].c_str());

			if(cacheUsed + spSize < cacheSize){
				sp.clear();
				for(int t = 0; t < spSize; t++) sp.push_back(atoi(tokens[t+2].c_str()));

				CacheItem e (i++, sp, atoi(tokens[2].c_str()), atoi(tokens[spSize+1].c_str()));
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
					CacheItem e (i++, sp, atoi(tokens[2].c_str()), atoi(tokens[spSize+1].c_str()));
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
	queryNumCacheFull = tmpCache.size();

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

void scache::readScoredQueries(int numQueries, string inFn)
{
	int i=0, spSize=0, filelines=0, curLine=0, cacheUsed=0;
	maxHeap mh; //a map to keep track of the score of each potential cache item
	HeapEntry tmp, tmp2;
	CacheItem tmpItem;
	boost::unordered_map<int, CacheItem> allSPpaths; //used to load all SP results into memory
	string line;
	std::vector<string> tokens;
	std::vector<int> sp;
	boost::unordered_map<int, int> verticesSeen;
	ifstream querysFile (inFn.c_str(), ios::in);

	readMapData(); //fill up structures such as coordinate2Nodeid
	readTestFile(ts.queryFileName); //read *.test file into memory so there will be some queries to do on the cache.

	cout << "ts.queryFileName: " << ts.queryFileName << endl;
	if( querysFile.is_open() )
	{
	cout << "scache::readScoredQueries! Starting to load queries into memory" << endl;
		getline(querysFile,line); //read first line: number of lines/queries in file.

		filelines = atoi(line.c_str());
		cout << filelines << endl;

		//Read all queries into memory and add each cacheItems score to mh.
		while( curLine < filelines-1 && curLine < numQueries)
		{
			getline(querysFile, line);
			boost::algorithm::split(tokens, line, boost::algorithm::is_space());
			curLine = atoi(tokens[0].c_str());
			spSize = atoi(tokens[1].c_str());

			//make new cache item
			sp.clear();
			for(int t = 0; t < spSize; t++) sp.push_back(atoi(tokens[t+2].c_str()));
			CacheItem e (i, sp, atoi(tokens[2].c_str()), atoi(tokens[spSize+1].c_str()));

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

	//add highest scoring cacheItem to cache, and update candidat scores.
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
	queryNumCacheFull = cacheUsed;
	ts.itemsInCache = cache.size();
	writeoutCacheCoordinates(ts.getTestName(), cache, nodeid2coordinate, ts.getSplits());
	cout << "scache::calcScoreCounter: " << calcScoreCounter << endl;
}

void scache::readScoredQueriesFromTrainFile(int numQueries, string fn)
{
	readMapData();

 	maxHeap mh; //a map to keep track of the score of each potential cache item
	HeapEntry tmp, tmp2;
	CacheItem tmpItem;
	boost::unordered_map<int, CacheItem> allSPpaths; //used to load all SP results into memory
	boost::unordered_map<int, int> verticesSeen;

	vector<int> spResult;
	std::pair<double, double> firstPair, secondPair;
	int firstPnt, secondPnt, i=0, spSize=0, cacheUsed=0;
	string str;
	std::vector<string> tokens;

	ifstream trainingData (fn.c_str(), ios::in); //*.train file
	if(debug) cout << "one, scache::readScoredQueriesFromTrainFile! " << endl;
	//find all pairs of nodeids in the training set to have SP done for them. map nodeids to coordinates.
	if(trainingData.is_open())
	{
		if(debug) cout << "two, scache::readScoredQueriesFromTrainFile! " << endl;
		while(getline(trainingData, str))
		{
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

			firstPair = std::make_pair(atof(tokens[1].c_str()),atof(tokens[2].c_str()));
			secondPair = std::make_pair(atof(tokens[3].c_str()),atof(tokens[4].c_str()));

			firstPnt = coordinate2Nodeid[firstPair];
			secondPnt = coordinate2Nodeid[secondPair];

			trainingSTPointPairs.push_back(std::make_pair(firstPnt,secondPnt));
		}
	}
	trainingData.close();

	BOOST_FOREACH(intPair p, trainingSTPointPairs)
	{
		spResult.clear();
		spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(p.first, p.second);
		spSize = spResult.size();

		//make new cache item
		CacheItem e (i, spResult, spResult.front(), spResult.back());

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
	}while(cacheSize >= cacheUsed && !mh.empty());

	queryNumCacheFull = cacheUsed;

	writeoutCacheCoordinates(ts.getTestName(), cache, nodeid2coordinate, ts.getSplits());
}

void scache::readMapData()
{
	int mapSize = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->getMapsize();
	string mapFile = ts.getTestFile();
	std::pair<double, double> tmpPair;
	string str;
	std::vector<string> tokens;

	mapFile.replace ((mapFile.size()-4), 4, "node"); //change file extention from .cedge to .cnode
	ifstream in_data (mapFile.c_str(), ios::in); //*.cnode file

	//read in the mapping between coordinates and node ids from *.cnode file
	if(in_data.is_open())
	{
		for(int i = 0; i < mapSize+1; i++)
			{
				getline(in_data, str);
				boost::algorithm::split(tokens, str, boost::algorithm::is_space());
				tmpPair = std::make_pair(atof(tokens[1].c_str()),atof(tokens[2].c_str()));
				coordinate2Nodeid[tmpPair] = atoi(tokens[0].c_str());
				nodeid2coordinate[atoi(tokens[0].c_str())] = tmpPair;
			}
	}
	in_data.close();
}

///calculate score to use in readScoredQueries()
int scache::calcScore(vector<int> sp, boost::unordered_map<int, int> vSeen)
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
void scache::statistics(string outFn)
{
	cout << "statistics(" << outFn << ") start" << endl;
	vector<int> iItem, kItem;
	int kMin,kMax,iMin,iMax, count=0;

	for(uint i=0; i< cache.size(); i++) {
		iItem = cache.at(i).item;
		iMin = cache.at(i).item.at(0);
		iMax = cache.at(i).item.at((cache.at(i).size)-1);
		for(uint k=0; k< cache.size(); k++) {
			kItem = cache.at(i).item;
			kMin = cache.at(k).item.at(0);
			kMax = cache.at(k).item.at((cache.at(k).size)-1);
			if(i != k && (std::find(iItem.begin(), iItem.end(), kMin) != iItem.end() &&
				std::find(iItem.begin(), iItem.end(), kMax) != iItem.end())) {
 					count++;
					//cout << "match pair: " << i <<":" <<k <<endl;
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

void scache::pairStatisticsStoT(string outFn)
{
	cout << "pairStatisticsStoT(" << outFn << ") start" << endl;
	intPairIntMap map;
	std::pair<int,int> tmpPair;

	BOOST_FOREACH(CacheItem ci, cache){
		if(ci.s < ci.t)	tmpPair =std::make_pair<int,int>(ci.s,ci.t);
		else tmpPair = std::make_pair<int,int>(ci.t,ci.s);

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

void scache::pairStatisticsAll(string outFn)
{
	cout << "pairStatisticsAll(" << outFn << ") start" << endl;
	typedef boost::unordered_map<std::pair<int,int >, int> pairIntMapping;
	pairIntMapping map;
	std::pair<int,int> tmpPair;

	BOOST_FOREACH(CacheItem ci, cache){
		BOOST_FOREACH(int node1, ci.item){
			BOOST_FOREACH(int node2, ci.item){
				if(node1 != node2){
					if(node1 < node2)
						tmpPair =std::make_pair<int,int>(node1,node2);
					else
						tmpPair = std::make_pair<int,int>(node2,node1);
				}
				map[tmpPair] = map[tmpPair] +1;
			}
		}
	}

	ofstream resultfile;
	resultfile.open(outFn.c_str(), ios::out | ios::ate | ios::app);

	BOOST_FOREACH(pairIntMapping::value_type i, map){
		resultfile << i.first.first <<"\t" << i.first.second << "\t" << i.second << endl;
	}
	resultfile.close();

	cout << "pairStatisticsAll(" << outFn << ") end" << endl;
}

void scache::writeoutCacheCoordinates(string testbasename, std::vector<CacheItem> cm, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits)
{
	cout << "one, scache::writeoutCacheCoordinates start!" << endl;
	vector<int> sp;
	int i=0;
	coordinate c;
	ofstream of;
	string fn = testbasename;
	string app = "STATIC";
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
				of << c.first << " " << c.second << "\n";
			}else
				cout << "scache::writeoutCacheCoordinates ERROR:  unknown node id." << endl;
		}
		of << endl;
		i++;
	}

	of.close();
}

int scache::writeoutTestCoordinates(string testbasename, std::vector<std::pair<int, int> > stPointPairs, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits)
{
	cout << "one, scache::writeoutTestCoordinates start!" << endl;
	vector<int> sp;
	coordinate c;
	string fn = testbasename;
	string app = "TEST.cache";
	fn.replace ((fn.size()-5), app.size(), app); //change file extention from .test TEST.cache
	ifstream ifile(fn.c_str());
	if (ifile) {ifile.close(); return 0;} //file alread exist

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
				cout << "scache::writeoutTestCoordinates ERROR:  unknown node id." << endl;
		}
		of << endl;
	}

	of.close();
	return 1;
}

void scache::readTestFile(string fn)
{
	std::pair<double, double> firstPair, secondPair;
	int firstPnt, secondPnt;
	string str;
	std::vector<string> tokens;

	fn.replace ((fn.size()-5), 5, "test"); //change file extention from .train to .test
	cout << fn << endl;
	ifstream testData (fn.c_str(), ios::in); //*.train file
	if(debug) cout << "one, scache::readTrainingFile! " << endl;

	if(testData.is_open())
	{
		if(debug) cout << "two, scache::readTrainingFile! " << endl;
		while(getline(testData, str))
		{
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

			firstPair = std::make_pair(atof(tokens[1].c_str()),atof(tokens[2].c_str()));
			secondPair = std::make_pair(atof(tokens[3].c_str()),atof(tokens[4].c_str()));

			firstPnt = coordinate2Nodeid[firstPair];
			secondPnt = coordinate2Nodeid[secondPair];

			testSTPointPairs.push_back(std::make_pair(firstPnt,secondPnt));
		}
	}
	testData.close();
}
