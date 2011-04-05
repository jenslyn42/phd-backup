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
#   define BOOST_TEST_MODULE FifoModule
#endif
#include <boost/test/unit_test.hpp>
#include "Vertex.cpp"
//#include "testsetting.cpp"
//#include "RoadGraph.cpp"
#include <vector>
#include <iostream>

struct zeFixture
{
	int id, nid, weight;
	Vertex v;

	zeFixture() : id(2), nid(4), weight(5)
	{
        	BOOST_TEST_MESSAGE("Setup");

		v.updateVertexID(id,nid,weight);
	}

    	~zeFixture()
	{
        	BOOST_TEST_MESSAGE("Teardown");
		
	}
};

BOOST_FIXTURE_TEST_SUITE(ClassVertex, zeFixture)


// 	void addNeighbour(int id , int weight);

BOOST_AUTO_TEST_CASE(updateVertex)
{
	BOOST_CHECK_EQUAL(v.getId(), 2);
	BOOST_CHECK_EQUAL(v.getAdjacencylist().size(), 1);
	v.updateVertexID(4,6,2);
	BOOST_CHECK_EQUAL(v.getAdjacencylist().size(), 2);
	BOOST_CHECK_EQUAL(v.getId(), 4);
}


BOOST_AUTO_TEST_CASE(getAdjacencylist)
{
	BOOST_CHECK_EQUAL(v.getAdjacencylist()[4], 5);
	BOOST_CHECK_EQUAL(v.getId(), 2);
}

BOOST_AUTO_TEST_CASE(addNeighbour)
{
	BOOST_CHECK_EQUAL(v.getAdjacencylist()[4], 5);
	BOOST_CHECK_EQUAL(v.getAdjacencylist().size(), 1);

	v.addNeighbour(12,6);
	v.addNeighbour(13,10);
	v.addNeighbour(14,15);
	v.addNeighbour(15,21);

	BOOST_CHECK_EQUAL(v.getAdjacencylist()[14], 15);
	BOOST_CHECK_EQUAL(v.getAdjacencylist()[12], 6);
	BOOST_CHECK_EQUAL(v.getAdjacencylist().size(), 5);
}

BOOST_AUTO_TEST_CASE(toString)
{
//TODO: after implementation, write this test
}

BOOST_AUTO_TEST_SUITE_END()