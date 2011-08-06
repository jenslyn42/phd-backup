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

#include "osc.h"
#include "lru.h"
#include "FIFO.h"
#include "RoadGraph.h"
#include "testObj.h"
#include "scache.h"

#include "boost/math/common_factor.hpp"
#include "boost/unordered_map.hpp"
#include <boost/foreach.hpp>

using namespace std;

typedef std::pair<int,int> intPair;

///Calculates the time in seconds
// double diffclock(clock_t clock1, clock_t clock2)
// {
// 	double diff= clock2-clock1;
// 	double diffsec= (diff*10)/CLOCKS_PER_SEC;
// 	return diffsec;
// }

int main(int argc, char *argv[])
{

///Test settings.
testsetting ts;
string testname = "NEW2correctness1000.test";
// string testfile ="ppi.dat";
string testfile ="OL.cedge";
int inputFileType = 3; //1:graph_large, 2: ppi.dat, 3:*.cedge
int numqueries = 100000;
ts.setNumQueriesForCache(10000);

//needed for static test
ts.preComputedQueriesFileName = "queriesGL1E7";
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

// cout << "error check "<< queries.at(1045).first << "," << queries.at(1045).second << endl;
// cout << "error check "<< queries.back().first << "," << queries.back().second << endl;


int numQ=numqueries, cacheSize=32000, queryRangeStart=0, queryRangeEnd= RoadGraph::mapObject(testfile, inputFileType)->getMapsize(), cacheType = 1;
bool gaussian, useOptimalSubstructure, useNodeScore, useHitScore;
double sigma = 2.0;
gaussian=true, useOptimalSubstructure=true, useNodeScore=true, useHitScore = true;

ts.setData(testname, testfile, inputFileType, numQ, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);



cout << "int \t\t" << std::numeric_limits<int>::max() << endl;
cout << "unsigned int\t" << std::numeric_limits<unsigned int>::max() << endl;
cout << "long\t\t" << std::numeric_limits<long>::max() << endl;
cout << "double\t\t" << std::numeric_limits<double>::max() << endl;


///exp test OSC with testObj
// testObj *expTest = new testObj(ts,1, queries);
// expTest->runTest();
// expTest-> testObj::~testObj();

///exp test LRU with testObj
// testObj *expTest2 = new testObj(ts,2, queries);
// expTest2->runTest();
// expTest2-> testObj::~testObj();


///exp test FIFO with testObj
testObj *expTest3;

//for(int i=0;i<11;i++)
//{
//	if(i%2 == 0){
// 	ts.setSplits(i);
//	ts.setData(testname, testfile, inputFileType, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//	expTest3 = new testObj(ts,5, queries);
//	expTest3->runStaticTest();
//	expTest3-> testObj::~testObj();
//}
//}

// ts.setData(testname, testfile, inputFileType, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
// expTest3 = new testObj(ts,4, queries);
// expTest3->runStaticTest();
// expTest3-> testObj::~testObj();

//ts.queryFileName = "path2.train";
//testname = "NEW2correctness21000.test";
//for(int i=0;i<11;i++)
//{
//	if(i%2 == 0){
// 	ts.setSplits(i);
//	ts.setData(testname, testfile, inputFileType, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
//	expTest3 = new testObj(ts,5, queries);
//	expTest3->runStaticTest();
//	expTest3-> testObj::~testObj();
//}
//}

// ts.setData(testname, testfile, inputFileType, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
// expTest3 = new testObj(ts,4, queries);
// expTest3->runStaticTest();
// expTest3-> testObj::~testObj();


cacheType = 1;
ts.queryFileName = "paths.train";
cacheSize=320000;
testname = "newcache.test";

for(int i=0;i<11;i++)
{
	if(i%3 == 0){
	ts.setSplits(i);
	ts.setData(testname, testfile, inputFileType, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
	expTest3 = new testObj(ts,5, queries);
	expTest3->runStaticTest();
	expTest3-> testObj::~testObj();
}
}

ts.queryFileName = "path2.train";
testname = "newcache2.test";

for(int i=0;i<11;i++)
{
	if(i%3 == 0){
 	ts.setSplits(i);
	ts.setData(testname, testfile, inputFileType, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
	expTest3 = new testObj(ts,5, queries);
	expTest3->runStaticTest();
	expTest3-> testObj::~testObj();
}
}

cacheType = 2;
ts.queryFileName = "paths.train";
testname = "oldcache.test";

for(int i=0;i<11;i++)
{
	if(i%3 == 0){
	ts.setSplits(i);
	ts.setData(testname, testfile, inputFileType, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
	expTest3 = new testObj(ts,5, queries);
	expTest3->runStaticTest();
	expTest3-> testObj::~testObj();
}
}

ts.queryFileName = "path2.train";
testname = "oldcache2.test";

for(int i=0;i<11;i++)
{
	if(i%3 == 0){
 	ts.setSplits(i);
	ts.setData(testname, testfile, inputFileType, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
	expTest3 = new testObj(ts,5, queries);
	expTest3->runStaticTest();
	expTest3-> testObj::~testObj();
}
}

// ts.setData(testname, testfile, inputFileType, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
// expTest3 = new testObj(ts,4, queries);
// expTest3->runStaticTest();
// expTest3-> testObj::~testObj();




//cacheSize
// cacheSize = 10000;
// ts.setData(testname, testfile, inputFileType, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore);
//
// expTest3 = new testObj(ts,5, queries);
// expTest3->runStaticTest();
// expTest3-> testObj::~testObj();
//
//
// cacheSize = 100000;
// ts.setData(testname, testfile, inputFileType, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore);
//
// expTest3 = new testObj(ts,4, queries);
// expTest3->runStaticTest();
// expTest3-> testObj::~testObj();
//
//
// cacheSize = 1000000;
// ts.setData(testname, testfile, inputFileType, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore);
//
// expTest3 = new testObj(ts,4, queries);
// expTest3->runStaticTest();
// expTest3-> testObj::~testObj();
//
//
// cacheSize = 10000000;
// ts.setData(testname, testfile, inputFileType, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore);
//
// expTest3 = new testObj(ts,4, queries);
// expTest3->runStaticTest();
// expTest3-> testObj::~testObj();


return EXIT_SUCCESS;
};
