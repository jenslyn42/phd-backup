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
#include "lru.h"
#define debug false

#define STATIC_CACHE true

typedef std::pair<int , int> intPair;

LRU::LRU(testsetting ts)
{
	this->ts = ts;
	cacheSize = ts.getCacheSize();
	numTotalQueries = 0;
	numCacheHits = 0;
	cacheUsed = 0;
	numDijkstraCalls = 0;
    readMapData();

	useNodeScore = ts.isUseNodeScore();
	useHitScore = ts.isUseHitScore();
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
	cout<< "2.0 done" << endl;
	readTestData(ts.queryFileName);
	cout<< "2.1 done" << endl;
	readTrainingData(ts.queryFileName);

	cout << "test query pairs:" << testSTPointPairs.size() << endl;
	cout << "training query pairs: " << trainingSTPointPairs.size() << endl;

	if(STATIC_CACHE){
        BOOST_FOREACH(intPair q, trainingSTPointPairs) { readQuery(q); }
    }
    ts.itemsInCache = -1;
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
}

void LRU::checkAndUpdateCache(std::pair< int, int > query)
{
	bool cacheHit = false;
    if(debug) {cout << "cache size: " << cache.size() << " s,t: (" << query.first << "," << query.second << ")" << endl;}
	if(ts.isUseOptimalSubstructure()){
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
	}else{
		BOOST_FOREACH(CacheItem ci, cache )
		{
			if(query.first == ci.s && query.second == ci.t)
			{
				numCacheHits++;
				ci.updateKey(numTotalQueries);
				sort( cache.begin(), cache.end() );
				cacheHit = true;
				break;
			}
		}
	}

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
			CacheItem e (numTotalQueries, spResult, query.first, query.second);
			cache.push_back(e);
		}
		if(debug) cout << "LRU::checkAndUpdateCache 3" << endl;
	}
}

void LRU::checkStaticCache(std::pair< int, int > query)
{
	bool cacheHit = false;
    if(debug) {cout << "cache size: " << cache.size() << " s,t: (" << query.first << "," << query.second << ")" << endl;}
	if(ts.isUseOptimalSubstructure()){
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
	}else{
		BOOST_FOREACH(CacheItem ci, cache )
		{
			if(query.first == ci.s && query.second == ci.t)
			{
				numCacheHits++;
				cacheHit = true;
				break;
			}
		}
	}

    if(!cacheHit)
	{
		RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(query.first, query.second);
		numDijkstraCalls++;
	}
}

void LRU::insertItem(int querySize, std::vector< int > nodesInQueryResult, int sNode, int tNode)
{
	bool notEnoughSpace = true;
	if(debug) cout << "one, LRU::insertItem(" <<querySize <<"," <<nodesInQueryResult.size() <<","<<sNode<<","<<tNode<<")" << endl;
	//insert query into cache, will repeatedly remove items until there is enough space for the new item.
	do{
		if((cacheSize - cacheUsed) > querySize)
		{
			if(debug) cout << "two1, LRU::insertItem cacheSize,cacheUsed " << cacheSize <<"," << cacheUsed <<endl;
			CacheItem cItem (numTotalQueries, nodesInQueryResult, sNode, tNode);
			cache.push_back(cItem);
			cacheUsed = cacheUsed + cItem.size;
			notEnoughSpace = false;
			if(debug) cout << "two2, LRU::insertItem cacheSize,cacheUsed " << cacheSize <<"," << cacheUsed <<endl;
		}
		else if(querySize < cacheSize)
		{
			if(!cacheFull){queryNumCacheFull = numTotalQueries; cacheFull = true;}
			if(debug) cout << "three1, LRU::insertItem" << cache.size() <<"," << cache[0].size <<endl;
			if(debug) cout << "querySize,cacheSize,cacheUsed:" << querySize <<"," << cacheSize <<"," << cacheUsed << endl;
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

	ifstream trainingData (fn.c_str(), ios::in); //*.train file
	if(debug) cout << "one, LRU::readTrainingData! " << endl;
	//find all pairs of nodeids in the training set to have SP done for them. map nodeids to coordinates.
	if(trainingData.is_open())
	{
		if(debug) cout << "two, LRU::readTrainingData! " << endl;
//		int i=0;
		while(getline(trainingData, str))
		{
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

			trainingSTPointPairs.push_back(std::make_pair(atof(tokens[1].c_str()),atof(tokens[2].c_str())));
//
//			if(i<10) {
//                i++;
//                cout << tokens[0] << " " << tokens[1] << " " << tokens[2] << endl;
//                cout << atof(tokens[0].c_str()) << " " << atof(tokens[1].c_str()) << " " << atof(tokens[2].c_str()) << endl;
//			}
		}
		if(debug) cout << endl;
	}
	trainingData.close();
	cout << "two, LRU::readTrainingData end!" << endl;
}

void LRU::readMapData()
{
	cout << "one, LRU::readMapData start!" << endl;
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
		for(int i = 0; i < mapSize; i++)
		{
			getline(in_data, str);
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
			tmpPair = std::make_pair(atof(tokens[1].c_str()),atof(tokens[2].c_str()));
			coordinate2Nodeid[tmpPair] = atoi(tokens[0].c_str());
		}
	}
	in_data.close();
//    if(debug) {
//        cout << "items in coordinate2Nodeid: \n";
//        BOOST_FOREACH(coordinateIntMap::value_type coor, coordinate2Nodeid) { cout << "((" << coor.first.first <<"," << coor.first.second << ")," << coor.second << ")"; }
//        cout << endl;
//    }
	cout << "two, LRU::readMapData end!" << endl;
}
