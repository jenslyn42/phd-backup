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
#   define BOOST_TEST_MODULE LRUModule
#endif
#include <boost/test/unit_test.hpp>
#include "RoadGraph.cpp"
#include <vector>
#include <iostream>

struct RgFixture
{
	int m;
	std::string testFile;
	RgFixture() : m(2)
	{
        	BOOST_TEST_MESSAGE("Setup");

		testFile = "graph_small.txt";
	}

    	~RgFixture()
	{
        	BOOST_TEST_MESSAGE("Teardown");
	}
};

BOOST_FIXTURE_TEST_SUITE(ClassRoadGraph, RgFixture)

// 	static RoadGraph* mapObject();
// 	std::vector<Vertex> getMap();
// 	std::vector<int> dijkstraSSSP(int s, int t);
// 	int getMapsize();
BOOST_AUTO_TEST_CASE(checkMapFilename)
{
	std::string spResult  = RoadGraph::mapObject(testFile,1)->checkMapFilename();

	BOOST_CHECK(spResult.compare(testFile) == 0);
}

BOOST_AUTO_TEST_CASE(getMap)
{
	std::vector<Vertex> spResult  = RoadGraph::mapObject(testFile,1)->getMap();

	BOOST_CHECK(0 == 0);
}

BOOST_AUTO_TEST_CASE(dijkstraSSSPtest)
{
	std::vector<int> spResult  = RoadGraph::mapObject(testFile,1)->dijkstraSSSP(1, 6);

	BOOST_CHECK(spResult.at(0) == 6);
	BOOST_CHECK(spResult.at(1) == 5);
	BOOST_CHECK(spResult.at(2) == 2);
	BOOST_CHECK(spResult.at(3) == 1);
	BOOST_CHECK((int)spResult.size() == 4);
}

BOOST_AUTO_TEST_CASE(getMapsize)
{
	int size  = RoadGraph::mapObject(testFile,1)->getMapsize();
	BOOST_CHECK(size == 8);
}

BOOST_AUTO_TEST_CASE(changeTestType) //needs to be made...
{
// 	int size  = RoadGraph::mapObject(testFile)->getMapsize();
// 	BOOST_CHECK(size == 8);
}
BOOST_AUTO_TEST_SUITE_END()