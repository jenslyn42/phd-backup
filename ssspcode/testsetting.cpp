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
#ifndef TESTSETTING_CPP
#define TESTSETTING_CPP

#include "testsetting.h"

testsetting::testsetting(string testname, string testFile, int testType, int numqueries, int cacheSize, int queryRangeStart, int queryRangeEnd, bool gaussian, double sigma, bool useOptimalSubstructure, bool useNodeScore, bool useHitScore, int cacheType)
{
	this->setData(testname, testFile, testType, numqueries, cacheSize, queryRangeStart, queryRangeEnd, gaussian, sigma, useOptimalSubstructure, useNodeScore, useHitScore, cacheType);
}


testsetting::~testsetting()
{
	testName = "";
	testFile = "";
	testType = 0;
	numQueries = 0;
	numQueriesForCache = 0;
	cacheSize = 0;
	queryRangeStart = 0;
	queryRangeEnd = 0;

	gaus.first=false;
	gaus.second=0.0;

	useOptimalSubstructure = false;
	useNodeScore = false;
	useHitScore = false;
	splits = -1;
	itemsInCache = 0;
	cacheType = 0;
	nonEmptyRegions = -1;
}

void testsetting::setData(string testname, string testfile, int testType, int numqueries, int cacheSize, int queryRangeStart, int queryRangeEnd, bool gaussian, double sigma, bool useOptimalSubstructure, bool useNodeScore, bool useHitScore, int cacheType)
{
	testName = testname;
	testFile = testfile;
	this->testType = testType;
	numQueries = numqueries;
	this->cacheSize = cacheSize;
	this->queryRangeStart = queryRangeStart;
	this->queryRangeEnd = queryRangeEnd;

	gaus.first = gaussian;
	gaus.second = sigma;

	this->useOptimalSubstructure = useOptimalSubstructure;
	this->useNodeScore = useNodeScore;
	this->useHitScore = useHitScore;
	this->cacheType = cacheType;
}

#endif
