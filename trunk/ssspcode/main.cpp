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

#include "boost/math/common_factor.hpp"
#include "boost/unordered_map.hpp"
#include <boost/foreach.hpp>

using namespace std;

typedef std::pair<int,int> intPair;

///Calculates the time in seconds
double diffclock(clock_t clock1, clock_t clock2)
{
	double diff= clock2-clock1;
	double diffsec= (diff*10)/CLOCKS_PER_SEC;
	return diffsec;
}

int main(int argc, char *argv[])
{

///Generate queries
vector<pair<int,int> > queries;
int s,t;
pair<int,int> p;

for(int i=0; i<10000; i++)
{

	srand((int)rand());
	p.first = (rand()%6104) +1;
	p.second = (rand()%6104) +1;
	queries.push_back(p);
}

// cout << "error check "<< queries.at(1045).first << "," << queries.at(1045).second << endl;
// cout << "error check "<< queries.back().first << "," << queries.back().second << endl;

///Test settings. many options from here are not used directly yet
testsetting ts;
string testname = "test";
string testfile ="graph_large.txt";
OSC ff;	

int numqueries=7000, cacheSize=10000000, queryRangeStart=0, queryRangeEnd=RoadGraph::mapObject(testfile)->getMapsize();
bool gaussian, useOptimalSubstructure, useNodeScore, useHitScore;
double sigma = 2.0;
gaussian=true, useOptimalSubstructure=true, useNodeScore=true, useHitScore = true;

ts.setData(testname, testfile, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore);

///To hold the start/end time of OSC,LRU,FIFO
clock_t start, end;

///exp test OSC with testObj
// testObj *expTest = new testObj(ts,1, queries);
// expTest->runTest();
// expTest-> testObj::~testObj();


///exp test LRU with testObj
// testObj *expTest2 = new testObj(ts,2, queries);
// expTest2->runTest();
// expTest2-> testObj::~testObj();


///exp test FIFO with testObj
testObj *expTest3 = new testObj(ts,3, queries);
expTest3->runTest();
expTest3-> testObj::~testObj();


///test  OSC

// OSC *methodone = new OSC(ts);
// start = clock();
// BOOST_FOREACH( intPair qpair, queries )
// {
// //	cout << "OSC ("<< qpair.first <<","<<qpair.second <<")" << endl;
// 	methodone->readQuery(qpair);
// }
// end = clock();
// cout << "OSC time: " << double(diffclock(start,end)) << " sec. dc: " << methodone->getTotalDijkstraCalls() <<"," << methodone->getCacheHits()<< endl;

///Test LRU
// start = clock();
// LRU *methodtwo = new LRU(ts);
// BOOST_FOREACH( intPair qpair, queries )
// {
// 	methodtwo->readQuery(qpair);
// }
// end = clock();
// cout << "LRU time: " << double(diffclock(start,end)) << " sec. dc: " << methodtwo->getTotalDijkstraCalls() <<"," << methodtwo->getCacheHits()<< endl;

///Test FIFO
// start = clock();
// FIFO *methodthree = new  FIFO(ts);
// BOOST_FOREACH( intPair qpair, queries )
// {
// 	methodthree->readQuery(qpair);
// }
// end = clock();
// cout << "FIFO time: " << double(diffclock(start,end)) << " sec. dc: " << methodthree->getTotalDijkstraCalls() <<"," << methodthree->getCacheHits()<< endl;


///Write out test information from the SP algorithm
unorderedIntMap nodecalls =  RoadGraph::mapObject(ts.getTestFile())->totalNodeCalls;
int totalcalls = 0;
BOOST_FOREACH (unorderedIntMap::value_type node, nodecalls)
{
	//cout << node.first <<":" <<node.second << endl;
 	totalcalls += node.second;
// 	if(node.second < 50) cout << "low: " << node.second << endl;
// 	if(node.second > 410) cout << "high: " << node.second << endl;
}
cout << "Vertices visited: " << totalcalls << endl;

return EXIT_SUCCESS;
};