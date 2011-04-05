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
#define debug false

testObj::testObj(testsetting settings, int testType)
{
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
	}

	generateQueries(settings.getNumQueries());
}

testObj::testObj(testsetting settings, int testType, vector< pair<int,int> > _queries)
{
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
}

void testObj::testResults(clock_t s, clock_t e)
{
cout << "Time: " << (double(e-s))/CLOCKS_PER_SEC << " sec. dc: " << test->getTotalDijkstraCalls() <<"," << test->getCacheHits() << " " << typeid(*test).name() << endl;
}