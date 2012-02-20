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

#include "Setting.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


testsetting::testsetting(string testname, string testFile, int testType, unsigned long cacheSize, int cacheType)
{
	this->setData(testname, testFile, testType, cacheSize, cacheType);
}


testsetting::~testsetting()
{
	testName = "";
	testFile = "";
	testType = 0;
	cacheSize = 0;

	randReplacement = 0;

	splits = -1;
	itemsInCache = 0;
	cacheType = 0;
	nonEmptyRegionPairs = -1;
}

void testsetting::setData(string testname, string testfile, int testType, unsigned long cacheSize, int cacheType)
{
	testName = testname;
	testFile = testfile;
	this->testType = testType;
	setCacheSize(cacheSize, cacheType);

	this->cacheType = cacheType;

	randReplacement = 0;
}

void testsetting::setCacheSize ( unsigned long cs, int cType ) {
    if(cType == GRAPH_CACHE || cType == COMPRESSED_G_CACHE)
        this->cacheSize = cs*8;
    else
        this->cacheSize = cs/2;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////

