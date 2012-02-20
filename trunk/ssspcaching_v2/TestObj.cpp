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
#include "TestObj.h"
#define debug true


testObj::testObj(testsetting settings, CACHE_METHOD testType) {
	ts = settings;

	switch( testType ){
		case METHOD_LRU:
			if (debug) cout << "testObj:: constructor: LRU test choosen" <<endl;
			test = new LRU(settings);
			break;
		case METHOD_SCACHE:
			if (debug) cout << "testObj:: constructor: STATIC test choosen" <<endl;
			test = new scache(settings);
			break;
		case METHOD_SPC:
			if (debug) cout << "testObj:: constructor: probstaticCache test choosen" <<endl;
			test = new probstaticCache(settings);
			break;
        case METHOD_HQF:
			if (debug) cout << "testObj:: constructor: hqf test choosen" <<endl;
			test = new hqf(settings);
			break;
        case METHOD_HQFLRU:
			if (debug) cout << "testObj:: constructor: hqflru test choosen" <<endl;
			test = new hqflru(settings);
			break;
        case METHOD_ORACLE:
			if (debug) cout << "testObj:: constructor: Oracle test choosen" <<endl;
			test = new oracle(settings);
			break;
        default:
			break;
	}
}


testObj::~testObj() {
	delete test;
}


void testObj::runTest() {
	if (debug) cout << "testObj::runTest: test started" <<endl;

	start = clock();
	int i = 0;
	BOOST_FOREACH(intPair qpair, queries )
	{
		if(debug) cout << "query NUM: " << ++i << endl;
		test->readQuery(qpair);
	}
	end = clock();
 	//if (debug) cout << "testObj::runTest: test ended" <<endl;
	testResults(start,end);

	RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->resetRoadGraph();
}

void testObj::runStaticTest() {
	if (debug) cout << "testObj::runStaticTest: static test started" <<endl;

	// Feb 15 CHANGE TOMORROW, fix first parameter to "0" for the moment

	test->readQueries(0, ts.preComputedQueriesFileName);
	if (debug) cout << "testObj::runStaticTest: queries read" <<endl;

 	start = clock();
 	test->readQueryList(queries);
 	end = clock();

  	if (debug) cout << "testObj::runStaticTest: static test ended" <<endl;
 	testResults(start,end);

 	RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->resetRoadGraph();
}

void testObj::testResults(clock_t s, clock_t e) {
	///Console output

	unsigned long numNodeVisits = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->numNodeVisits;
	int ssspCalls = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->ssspCalls;

	cout << "Query time:\t" << (double(e-s))/CLOCKS_PER_SEC << " sec." << endl;
	cout << "Cachehits:\t" << test->getCacheHits() << "(" << test->getTotalDijkstraCalls() << ")" << endl;
	cout << "sssp Calls:\t" << ssspCalls << endl;
	cout << "Vertices visited:\t" << numNodeVisits << endl;

	cout << "Class:\t" << typeid(*test).name() <<  endl;
	cout << "Cachesize:\t"<<ts.getCacheSize() << endl;
	cout << "Cache items:\t" << test->ts.getItemsInCache() << endl;

	cout << "Splits:\t" << test->ts.getSplits() << endl;
	cout << "Query file:\t" << ts.queryFileName << endl;

	cout << "Fill cache time:\t" << test->ts.getFillCacheTime() << " Sec." << endl;
	cout << "Calc Statistics:\t" << test->ts.getBuildStatisticsTime() << " Sec." <<endl;
	cout << "Non Empty Regions:\t" << test->ts.getNonEmptyRegionPairs() << endl;
	cout << "Scenario\t" << test->ts.getTestScenario() << "\n" << endl;

    bool fileExist = false;
    ifstream fin((ts.getTestName()).c_str());
    if (fin)
		fileExist = true;// check to see if file exists
    fin.close();

    ///file output
	ofstream resultfile;
	resultfile.open((ts.getTestName()).c_str(), ios::out | ios::ate | ios::app);
    if(!fileExist){
        resultfile << "Time\tCachehits\tTotalDijkstraCalls\tTestMethod\t#QueryCacheFull\tItemsInCache\t";
        resultfile << "VerticesVisited\tCacheSize\tSplits\tQueryfile\t";
        resultfile << "SPcalls\tCalcStatisticsTime\tFillCacheTime\tSampleSize\tNonEmptyRegions\tScenario" << endl;
    }
    resultfile << (double(e-s))/CLOCKS_PER_SEC << "\t" << test->getCacheHits() << "\t" << test->getTotalDijkstraCalls() << "\t" << typeid(*test).name()  << "\t" << test->ts.getItemsInCache() << "\t";
    resultfile << "\t" << numNodeVisits << "\t" << ts.getCacheSize() << "\t" << test->ts.getSplits() << "\t" << ts.queryFileName << "\t";
    resultfile << ssspCalls << "\t" << test->ts.getBuildStatisticsTime() << "\t" << test->ts.getFillCacheTime() << "\t";
    resultfile << "\t" << test->ts.getNonEmptyRegionPairs() << "\t" << test->ts.getTestScenario() << endl;

    resultfile.close();
}
