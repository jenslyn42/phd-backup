/********************************************************************************
 *   Copyright (C) 2011 by Jeppe Rishede 					*
 *   jenslyn42@gmail.com							*
 *										*
 *   All rights reserved.							*
 *										*
 *   Redistribution and use in source and binary forms, with or without 	*
 *   modification, are permitted provided that the following conditions are met:*
 *   Redistributions of source code must retain the above copyright notice,	*
 *   this list of conditions and the following disclaimer. 			*
 *   Redistributions in binary form must reproduce the above copyright notice,	*
 *   this list of conditions and the following disclaimer in the documentation	*
 *   and/or other materials provided with the distribution. 			*
 *   Neither the name of the <ORGANIZATION> nor the names of its contributors 	*
 *   may be used to endorse or promote products derived from this software 	*
 *   without specific prior written permission					*
 *                                                                         	*
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   	*
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     	*
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 	*
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER	*
 *   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 	*
 *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   	*
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    	*
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 	*
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  	*
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    	*
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          	*
 ********************************************************************************/

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <list>
#include <ctime>
#include <vector>
#include <utility>
#include <bitset>

#include "osc.h"
#include "lru.h"
#include "FIFO.h"
#include "RoadGraph.h"
#include "testObj.h"
#include "scache.h"
#include "oracle.h"

#include "boost/math/common_factor.hpp"
#include "boost/unordered_map.hpp"
#include <boost/foreach.hpp>

using namespace std;

typedef std::pair<int,int> intPair;

int main(int argc, char *argv[])
{
///Test settings.
testsetting ts;
string testname = "NEW2correctness1000.test";
// string testfile ="ppi.dat";
string testfile ="beijing_map.cedge";
int inputFileType = 3; //1:graph_large, 2: ppi.dat, 3:*.cedge
int numqueries = 100000;


//needed for static test
ts.preComputedQueriesFileName = "queriesGL1E7";
//the number of paths to read from preComputedQueriesFileName file
ts.setNumQueriesForCache(10000);
//ts.queryFileName = "path2.train";
ts.queryFileName = "paths.train";

ts.setStaticQueryType(3);


//needed for probstaticCache
ts.setSplits(0);

///Generate queries
vector<pair<int,int> > queries;
pair<int,int> p;

for(int i=0; i<numqueries; i++)
{
	srand((int)rand());/*
	p.first = (rand()%15499) +1; //ppi.dat
	p.second = (rand()%15499) +1;*/
// 	p.first = (rand()%85286) +1; //arg.cedge, arg.enode
// 	p.second = (rand()%85286) +1;
// 	p.first = (rand()%190913) +1; //caida.cedge, caida.cnode
// 	p.second = (rand()%190913) +1;
	p.first = (rand()%6104) +1; //graph_large.txt
	p.second = (rand()%6104) +1;
// 	p.first = (rand()%63730) +1; //facebook.cedge, facebook.cnode
// 	p.second = (rand()%63730) +1;
	queries.push_back(p);
}


int numQ=numqueries, cacheSize=32000, queryRangeStart=0, queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize(), cacheType = GRAPH_CACHE;
bool gaussian, useOptimalSubstructure, useNodeScore, useHitScore;
double sigma = 2.0;
gaussian=true, useOptimalSubstructure=true, useNodeScore=true, useHitScore = true;

ts.setData(testname, testfile, inputFileType, numQ, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
ts.setTestScenario(PROXY_SCENARIO);

cout << "******************************************" << endl;
cout << "int \t\t" << std::numeric_limits<int>::max() << endl;
cout << "unsigned int\t" << std::numeric_limits<unsigned int>::max() << endl;
cout << "long\t\t" << std::numeric_limits<long>::max() << endl;
cout << "double\t\t" << std::numeric_limits<double>::max() << endl;
cout << sizeof(long) << " " << sizeof(int) << " " <<sizeof(double) <<endl;
cout << "******************************************" << endl;


ts.setTestScenario(PROXY_SCENARIO);
cacheType = LIST_CACHE;



//void oracleTest(testsetting ts);
//
//void hqfTest(testsetting ts);
//
//void lruTest(testsetting ts);
//
//void spcTest(testsetting ts);
//
//void spcStarTest(testsetting ts);





///Oracle test
//{
//testObj *ora;
//testfile ="Aalborg.cedge";
//ts.queryFileName = "AAL_qlog.test";
//testname = "cacheSize_oracle_AAL.test";
//queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
//for(int i = 10000; i <= 320000000; i*=2)
//{
//    ts.setData(testname, testfile, inputFileType, numqueries, i, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//    ora = new testObj(ts,9, queries);
//    ora->runStaticTest();
//    ora-> testObj::~testObj();
//}
//
//testfile ="beijing_map.cedge";
//ts.queryFileName = "beijing_all.test";
//testname = "cacheSize_oracle_BEIJING.test";
//queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
//for(int i = 10000; i <= 320000000; i*=2)
//{
//    ts.setData(testname, testfile, inputFileType, numqueries, i, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//    ora = new testObj(ts,9, queries);
//    ora->runStaticTest();
//    ora-> testObj::~testObj();
//}
//}


///exp test OSC with testObj
//{
// testObj *expTest = new testObj(ts,1, queries);
// expTest->runTest();
// expTest-> testObj::~testObj();
//}

///exp test HQF with testObj
//{
testObj *expTest6;
//
testfile ="Aalborg.cedge";
ts.queryFileName = "AAL_qlog.test";
testname = "cacheSize_HQF_AAL.test";
queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
for(int i = 10000; i <= 320000000; i*=2)
{
    ts.setData(testname, testfile, inputFileType, numqueries, i, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
	ts.setSplits(14);
	expTest6 = new testObj(ts,6, queries);
	expTest6->runStaticTest();
	expTest6-> testObj::~testObj();
}

testfile ="beijing_map.cedge";
ts.queryFileName = "beijing_all.test";
testname = "cacheSize_HQF_BEIJING.test";
queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
for(int i = 10000; i <= 320000000; i*=2)
{
    ts.setData(testname, testfile, inputFileType, numqueries, i, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
	ts.setSplits(14);
	expTest6 = new testObj(ts,6, queries);
	expTest6->runStaticTest();
	expTest6-> testObj::~testObj();
}
//}

///exp test randCache with testObj
//{
/////exp test randCache with testObj
//testObj *expTest7;
//
//ts.setTestScenario(PROXY_SCENARIO);
//cacheType = LIST_CACHE;

//testfile ="Aalborg.cedge";
//ts.queryFileName = "AAL_qlog.test";
//testname = "cacheSize_randcache_AAL.test";
//queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
//for(int i = 10000; i <= 3200000; i*=2)
//{
//    ts.setData(testname, testfile, inputFileType, numqueries, i, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//	ts.setSplits(14);
//	expTest7 = new testObj(ts,7, queries);
//	expTest7->runStaticTest();
//	expTest7-> testObj::~testObj();
//}
//
//testfile ="beijing_map.cedge";
//ts.queryFileName = "beijing_all.test";
//testname = "cacheSize_randcache_BEIJING.test";
//queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
//for(int i = 10000; i <= 3200000; i*=2)
//{
//    ts.setData(testname, testfile, inputFileType, numqueries, i, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//	ts.setSplits(14);
//	expTest7 = new testObj(ts,7, queries);
//	expTest7->runStaticTest();
//	expTest7-> testObj::~testObj();
//}
//}

///exp test LRU with testObj
//{
//testObj *expTest2;
//
//    testfile ="Aalborg.cedge";
//    ts.queryFileName = "AAL_qlog.test";
//    testname = "cacheSize_LRU_AAL.test";
//    queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
//    for(int i = 10000; i <= 3200000; i*=2)
//    {
//        ts.setData(testname, testfile, inputFileType, numqueries, i, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//        expTest2 = new testObj(ts,2, queries);
//        expTest2->runStaticTest();
//        expTest2-> testObj::~testObj();
//    }
//
//    testfile ="beijing_map.cedge";
//    ts.queryFileName = "beijing_all.test";
//    testname = "cacheSize_LRU_BEIJING.test";
//    queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
//    for(int i = 10000; i <= 3200000; i*=2)
//    {
//        ts.setData(testname, testfile, inputFileType, numqueries, i, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//        expTest2 = new testObj(ts,2, queries);
//        expTest2->runStaticTest();
//        expTest2-> testObj::~testObj();
//    }
//}

///cache test SPC with testObj
//{
//testObj *expTest3;

//testfile ="Aalborg.cedge";
//ts.queryFileName = "AAL_qlog.test";
//testname = "CacheSize_SPC_AAL.test";
//queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
////for(int i = 10000; i <= 3200000; i*=2)
//for(int i = 5120000; i <= 320000000; i*=2)
//{
//    ts.setData(testname, testfile, inputFileType, numqueries, i, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//	ts.setSplits(14);
//	expTest3 = new testObj(ts,5, queries);
//	expTest3->runStaticTest();
//	expTest3-> testObj::~testObj();
//}
//
//testfile ="beijing_map.cedge";
//ts.queryFileName = "beijing_all.test";
//queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
//testname = "CacheSize_SPC_BEIJING.test";
////for(int i = 10000; i <= 3200000; i*=2)
//for(int i = 5120000; i <= 320000000; i*=2)
//{
//    ts.setData(testname, testfile, inputFileType, numqueries, i, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//	ts.setSplits(14);
//	expTest3 = new testObj(ts,5, queries);
//	expTest3->runStaticTest();
//	expTest3-> testObj::~testObj();
//}
//}

///LEVEL test SPC with testObj
//{
//testObj *expTest3;
//
//testfile ="Aalborg.cedge";
//ts.queryFileName = "AAL_qlog.test";
//queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
//testname = "Level_SPC_AAL.test";
//for(int i = 4; i <= 18; i = i+2)
//{
//    ts.setData(testname, testfile, inputFileType, numqueries, 160000, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//	ts.setSplits(i);
//	expTest3 = new testObj(ts,5, queries);
//	expTest3->runStaticTest();
//	expTest3-> testObj::~testObj();
//}
//
//testfile ="beijing_map.cedge";
//queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
//ts.queryFileName = "beijing_all.test";
//testname = "Level_SPC_BEIJING.test";
//for(int i = 4; i <= 18; i= i+2)
//{
//    ts.setData(testname, testfile, inputFileType, numqueries, 160000, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//	ts.setSplits(i);
//	expTest3 = new testObj(ts,5, queries);
//	expTest3->runStaticTest();
//	expTest3-> testObj::~testObj();
//}
//}

///cache test SPC-STAR with testObj
//{
//testObj *expTest3;
//
//ts.setTestScenario(PROXY_SCENARIO);
//cacheType = COMPRESSED_G_CACHE;
////
//testfile ="Aalborg.cedge";
//ts.queryFileName = "AAL_qlog.test";
//testname = "CacheSize_SPC-STAR_AAL.test";
//queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
//for(int i = 10000; i <= 3200000; i*=2)
//{
//    ts.setData(testname, testfile, inputFileType, numqueries, i, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//	ts.setSplits(14);
//	expTest3 = new testObj(ts,5, queries);
//	expTest3->runStaticTest();
//	expTest3-> testObj::~testObj();
//}
//
//testfile ="beijing_map.cedge";
//ts.queryFileName = "beijing_all.test";
//queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
//testname = "CacheSize_SPC-STAR_BEIJING.test";
//for(int i = 10000; i <= 3200000; i*=2)
//{
//    ts.setData(testname, testfile, inputFileType, numqueries, i, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//	ts.setSplits(14);
//	expTest3 = new testObj(ts,5, queries);
//	expTest3->runStaticTest();
//	expTest3-> testObj::~testObj();
//}
//}

///cache test HQFLRU with testObj
//{
//testObj *expTest10;
//
//testfile ="Aalborg.cedge";
//ts.queryFileName = "AAL_qlog.test";
//testname = "CacheSize_HQFLRU_AAL.test";
//queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
//for(int i = 10000; i <= 32000000; i*=2)
//{
//    ts.setData(testname, testfile, inputFileType, numqueries, i, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//	ts.setSplits(14);
//	expTest10 = new testObj(ts,8, queries);
//	expTest10->runStaticTest();
//	expTest10-> testObj::~testObj();
//}
//
//testfile ="beijing_map.cedge";
//ts.queryFileName = "beijing_all.test";
//queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize();
//testname = "CacheSize_HQFLRU_BEIJING.test";
//for(int i = 10000; i <= 32000000; i*=2)
//{
//    ts.setData(testname, testfile, inputFileType, numqueries, i, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//	ts.setSplits(14);
//	expTest10 = new testObj(ts,8, queries);
//	expTest10->runStaticTest();
//	expTest10-> testObj::~testObj();
//}
//}


return EXIT_SUCCESS;
};
