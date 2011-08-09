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
#include "testObj.h"
#define debug true

testObj::testObj(testsetting settings, int testType)
{
	ts = settings;

	switch( testType ){
		case 1:
			if (debug) cout << "testObj:: constructor: OSC test choosen" <<endl;
 			test = new OSC(settings);
			break;
		case 2:
			if (debug) cout << "testObj:: constructor: LRU test choosen" <<endl;
			test = new LRU(settings);
			break;
		case 3:
			if (debug) cout << "testObj:: constructor: FIFO test choosen" <<endl;
			test = new FIFO(settings);
			break;
		case 4:
			if (debug) cout << "testObj:: constructor: STATIC test choosen" <<endl;
			test = new scache(settings);
			break;
		case 5:
			if (debug) cout << "testObj:: constructor: probstaticCache test choosen" <<endl;
			test = new probstaticCache(settings);
			break;

			default:
			break;
	}

	generateQueries(settings.getNumQueries());
}

testObj::testObj(testsetting settings, int testType, vector< pair<int,int> > _queries)
{
	ts = settings;

	switch( testType ){
		case 1:
			if (debug) cout << "testObj:: constructor: OSC test choosen" <<endl;
 			test = new OSC(settings);
			break;
		case 2:
			if (debug) cout << "testObj:: constructor: LRU test choosen" <<endl;
			test = new LRU(settings);
			break;
		case 3:
			if (debug) cout << "testObj:: constructor: FIFO test choosen" <<endl;
			test = new FIFO(settings);
			break;
		case 4:
			if (debug) cout << "testObj:: constructor: STATIC test choosen" <<endl;
			test = new scache(settings);
			break;
		case 5:
			if (debug) cout << "testObj:: constructor: probstaticCache test choosen" <<endl;
			test = new probstaticCache(settings);
			break;
	}
	queries = _queries;
}

testObj::~ testObj()
{
	delete test;
}


void testObj::generateQueries(int numQueries)
{
	for(int i=0; i<numQueries; i++)
	{
		p.first = rand()%6105 +1;
		p.second = rand()%6105 +1;
		queries.push_back(p);
	}
}

void testObj::runTest()
{
	if (debug) cout << "testObj::runTest: test started" <<endl;
	int i = 0;
	start = clock();
	BOOST_FOREACH( intPair qpair, queries )
	{
		if( debug) cout << "query NUM: " << ++i << endl;
		test->readQuery(qpair);
	}
	end = clock();
 	//if (debug) cout << "testObj::runTest: test ended" <<endl;
	testResults(start,end);

	RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->resetRoadGraph();
}

void testObj::runStaticTest()
{
	if (debug) cout << "testObj::runStaticTest: static test started" <<endl;
	test->readQueries(ts.getNumQueriesForCache(), ts.preComputedQueriesFileName);
	if (debug) cout << "testObj::runStaticTest: queries read" <<endl;
 	start = clock();
 	test->readQueryList(queries);
 	end = clock();
  	if (debug) cout << "testObj::runStaticTest: static test ended" <<endl;
 	testResults(start,end);

 	RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->resetRoadGraph();
}

void testObj::testResults(clock_t s, clock_t e)
{
	///Console output
	intMap nodecalls =  RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->totalNodeCalls;
	long totalcalls = 0;
	BOOST_FOREACH (intMap::value_type node, nodecalls){totalcalls += (long)node.second;}

	cout << "Time:\t" << (double(e-s))/CLOCKS_PER_SEC << " sec." << endl;
	cout << "Cachehits:\t" << test->getTotalDijkstraCalls() <<"," << test->getCacheHits() << endl;
	cout << typeid(*test).name() <<  endl;
	cout << "QueryNum Cache full:\t" << test->getQueryNumCacheFull() << "(" << test->ts.itemsInCache << ")" << endl;

	cout << "Vertices visited:\t" << totalcalls << endl;
	cout << "Cachesize:\t"<<ts.getCacheSize() << endl;
	cout << "Splits:\t" << ts.getSplits() << endl;
	cout << "Query file:\t" << ts.queryFileName << endl;

	cout << "sssp Calls:\t" <<RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->ssspCalls << endl;
	cout << "Calc Statistics:\t" << test->ts.buildStatisticsTime << " sec." <<endl;
	cout << "Fill cache:\t" << test->ts.fillCacheTime << " sec." << endl;
	cout << "Sample size:\t" << ts.getNumQueriesForCache() << "\n" << endl;

	///file output
	ofstream resultfile;
	resultfile.open((ts.getTestName()).c_str(), ios::out | ios::ate | ios::app);

	resultfile << "Time:\t" << (double(e-s))/CLOCKS_PER_SEC << " sec." << endl;
	resultfile << "Cachehits:\t" << test->getTotalDijkstraCalls() <<"," << test->getCacheHits() << endl;
	resultfile << typeid(*test).name() <<  endl;
	resultfile << "QueryNum Cache full:\t" << test->getQueryNumCacheFull() << "(" << test->ts.itemsInCache << ")" << endl;

	resultfile << "Vertices visited:\t" << totalcalls << endl;
	resultfile << "Cachesize:\t"<<ts.getCacheSize() << endl;
	resultfile << "Splits:\t" << ts.getSplits() << endl;
	resultfile << "Query file:\t" << ts.queryFileName << endl;

	resultfile << "sssp Calls:\t" << RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->ssspCalls << endl;
	resultfile << "Calc Statistics:\t" << test->ts.buildStatisticsTime << " sec." <<endl;
	resultfile << "Fill cache:\t" << test->ts.fillCacheTime << " sec." << endl;
	resultfile << "Sample size:\t" << ts.getNumQueriesForCache() << "\n" << endl;
	resultfile.close();
}
