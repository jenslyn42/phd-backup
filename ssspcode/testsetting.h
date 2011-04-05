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
#ifndef TESTSETTING_H
#define TESTSETTING_H


/**
	@author Jeppe Rishede <jenslyn42@gmail.com>
*/

#include <string>
#include <vector>
using namespace std;

class testsetting{
private:
	std::string testName;
	std::string testFile;
	int numQueries;
	int cacheSize;
	int queryRangeStart;
	int queryRangeEnd;

	pair<bool,double> gaus;

	//is NOT implemented
	bool useOptimalSubstructure;
	bool useNodeScore;
	bool useHitScore;

public:
	testsetting(){ };

	testsetting(string testname, string testfile, int numqueries, int cacheSize, int queryRangeStart, int queryRangeEnd, bool gaussian, double sigma, bool useOptimalSubstructure, bool useNodeScore, bool useHitScore);

	void setData(string testname, string testfile, int numqueries, int cacheSize, int queryRangeStart, int queryRangeEnd, bool gaussian, double sigma, bool useOptimalSubstructure, bool useNodeScore, bool useHitScore);

	~testsetting();

// 	void setTestName (const std::string& theValue ) {testName = theValue;}
	std::string getTestName() const {return testName;}

// 	void setNumQueries (int theValue ) {numQueries = theValue;}
	int getNumQueries() {return numQueries;}

// 	void setCacheSize ( int theValue ) {cacheSize = theValue;}
	int getCacheSize() const {return cacheSize;}

// 	void setSkewedData ( bool theValue ) {skewedData = theValue;}
	bool isSkewedData() const {return gaus.first;}
	double getSigma() const {return gaus.second;}

// 	void setUseNodeScore (bool val) {useNodeScore = val;}
	bool isUseNodeScore() const {return useNodeScore;}
	bool isUseHitScore() const{return useHitScore;}

	void setUseOptimalSubstructure (bool theValue) {useOptimalSubstructure = theValue;}
	bool isUseOptimalSubstructure() const {return useOptimalSubstructure;}

	int getQueryRangeStart() const{return queryRangeStart;}
	int getQueryRangeEnd() const{return queryRangeEnd;}

	std::string getTestFile() const{return testFile;}
};

#endif