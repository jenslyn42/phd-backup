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

#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE OSCModule
#endif
#include <boost/test/unit_test.hpp>
#include "osc.cpp"
// #include "testsetting.cpp"
// #include "RoadGraph.cpp"
// #include "Vertex.cpp"
#include <vector>
#include <iostream>

struct oscFixture
{
	int m;
	testsetting ts;
	OSC ff;

	string testname;
	string testfile;
	int numqueries, cacheSize, queryRangeStart, queryRangeEnd;
	vector< bool > atests;
	bool gaussian, useOptimalSubstructure, useNodeScore, useHitScore;
	double sigma;


	oscFixture() : m(2)
	{
        	BOOST_TEST_MESSAGE("Setup osc");

		testname ="osc test";
		testfile ="graph_small.txt";
		numqueries=3000, cacheSize=60, queryRangeStart=0, queryRangeEnd=6105;
		gaussian=true, useOptimalSubstructure=true, useNodeScore=true, useHitScore = true;
		sigma= 2.0;

    		ts.setData(testname, testfile, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore);
		
		OSC tmp (ts);		
    		ff = tmp; 

	}

    	~oscFixture()
	{
        	BOOST_TEST_MESSAGE("Teardown osc");
	}
};

BOOST_FIXTURE_TEST_SUITE(ClassOSC, oscFixture)

BOOST_AUTO_TEST_CASE(getTotalQueries)
{
	BOOST_CHECK(ff.getTotalQueries() == 0);
}

BOOST_AUTO_TEST_CASE(getCacheHits)
{
	BOOST_CHECK_EQUAL(ff.getCacheHits(), 0);
}

BOOST_AUTO_TEST_CASE(readQuery)
{
	int q = ff.getTotalQueries();
	pair<int,int> p (2,3);
	ff.readQuery(p);
	BOOST_CHECK_EQUAL(ff.getTotalQueries(), q+1);
}

BOOST_AUTO_TEST_CASE(readQueryList)
{
	int q = ff.getTotalQueries();
	vector< std::pair<int,int> > ql;
	ql.push_back(pair<int,int>(2,1));
	ql.push_back(pair<int,int>(2,7));
	ql.push_back(pair<int,int>(1,4));
	ql.push_back(pair<int,int>(6,1));
	ff.readQueryList(ql);
	BOOST_CHECK_EQUAL(ff.getTotalQueries(), q+4);
}

BOOST_AUTO_TEST_SUITE_END()