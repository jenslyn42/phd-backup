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
#include "testsetting.h"

#include "string.h"
#include <vector>


testsetting::testsetting(string testname, int numNodes, int numqueries, int cacheSize, int queryRangeStart, int queryRangeEnd, vector< bool > atests, bool gaussian, double sigma, bool skewedData, bool useOptimalSubstructure, bool useNodeScore, bool useHitScore)
{
	this->setData(testname, numNodes, numqueries, cacheSize, queryRangeStart, queryRangeEnd, atests, gaussian, sigma, skewedData, useOptimalSubstructure, useNodeScore, useHitScore);
}


testsetting::~testsetting()
{
	testName = "";
	numRoadGraphNodes = 0;
	numQueries = 0;
	cacheSize = 0;
	queryRangeStart = 0;
	queryRangeEnd = 0;
	acT.~activeTests();
	gauS.~gaussian();
	skewedData = false;
	useOptimalSubstructure = false;
	useNodeScore = false;
	useHitScore = false;
}

void testsetting::setData(string testname, int numNodes, int numqueries, int cacheSize, int queryRangeStart, int queryRangeEnd, vector< bool > atests, bool gaussian, double sigma, bool skewedData, bool useOptimalSubstructure, bool useNodeScore, bool useHitScore)
{
	testName = testname;
	numRoadGraphNodes = numNodes;
	numQueries = numqueries;
	this->cacheSize = cacheSize;
	this->queryRangeStart = queryRangeStart;
	this->queryRangeEnd = queryRangeEnd;
	acT.setData(atests);

	gauS.setData(gaussian, sigma);

	this->skewedData = skewedData;
	this->useOptimalSubstructure = useOptimalSubstructure;
	this->useNodeScore = useNodeScore;
	this->useHitScore = useHitScore;
}


activeTests::activeTests()
{

}

void activeTests::setData(vector< bool > aTests)
{
	osc = aTests[0];
	lru = aTests[1];
	fifo = aTests[2];
}

activeTests::~ activeTests()
{
	osc = false;
	lru = false;
	fifo = false;
}

void gaussian::setData(bool useGau, double sigma)
{
	useGaussian = useGau;
	this->sigma = sigma;
}

gaussian::~ gaussian()
{
	useGaussian = false;
	sigma = 0.0;
}