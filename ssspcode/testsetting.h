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
#include <ctime>

#include "utility.h"

#include <typeinfo>
#include <iostream>



using namespace std;


class testsetting{
private:
	std::string testName;
	std::string testFile;
	int testType;
	int numQueries;
	int numQueriesForCache; //indicates how many queries should be used to fill/train cache (used with static methods)
	int cacheSize;
	int queryRangeStart;
	int queryRangeEnd;

	//Indicates which method should be used to fill the cache.
	int staticQueryType;

	pair<bool,double> gaus;

	//is NOT implemented
	bool useOptimalSubstructure;
	bool useNodeScore;
	bool useHitScore;

	int splits, itemsInCache, nonEmptyRegionPairs, testScenario;

    double fillCacheTime, buildStatisticsTime;

public:

	testsetting(){ };

	testsetting(string testname, string testfile, int testType, int numqueries, int cacheSize, int queryRangeStart, int queryRangeEnd, bool gaussian, double sigma, bool useOptimalSubstructure, bool useNodeScore, bool useHitScore, int cacheType);

	void setData(string testname, string testfile, int testType, int numqueries, int cacheSize, int queryRangeStart, int queryRangeEnd, bool gaussian, double sigma, bool useOptimalSubstructure, bool useNodeScore, bool useHitScore, int cacheType);

	~testsetting();

	std::string preComputedQueriesFileName;
	std::string queryFileName;

	int cacheType;


    int getNonEmptyRegionPairs(){return nonEmptyRegionPairs;}
    void setNonEmptyRegionPairs(int rp){nonEmptyRegionPairs = rp;}

    int getTestScenario(){return testScenario;}
    void setTestScenario(int ts){testScenario = ts;}

    double getItemsInCache(){return itemsInCache;}
    void setItemsInCache(double iic){itemsInCache = iic;}

    double getBuildStatisticsTime(){return buildStatisticsTime;}
    void setBuildStatisticsTime(double st){buildStatisticsTime = st;}

    double getFillCacheTime(){return fillCacheTime;}
    void setFillCacheTime(double ct){fillCacheTime = ct;}

// 	void setTestName (const std::string& theValue ) {testName = theValue;}
	std::string getTestName() const {return testName;}

// 	void setNumQueries (int theValue ) {numQueries = theValue;}
	int getNumQueries() {return numQueries;}

 	void setCacheSize ( int cs, int cType );
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

	void setTestType (int theValue){testType = theValue;}
	int getTestType() const{return testType;}

	//Used in readQueries(int numQueries, string inFn)
	void setStaticQueryType (int val){staticQueryType = val;}
	int getStaticQueryType() const{return staticQueryType;}

	void setSplits(int theValue){splits = theValue;}
	int getSplits() const {return splits;}

	void setNumQueriesForCache (int theValue) {numQueriesForCache = theValue;}
	int getNumQueriesForCache() const {return numQueriesForCache;}

	void writeOut()
	{
	    cout << "\n--------------------------" << endl;
        cout << "testName " << testName << endl;
        cout << "testFile " << testFile << endl;
        cout << "testType" << testType << endl;
        cout << "numQueries " <<  numQueries << endl;
        cout << "numQueriesForCache " <<  numQueriesForCache << endl;
        cout << "cacheSize " <<  cacheSize << endl;
        cout << "queryRangeStart " <<  queryRangeStart << endl;
        cout << "queryRangeEnd " <<  queryRangeEnd << endl;
        cout << "staticQueryType " <<  staticQueryType << endl;
        cout << "useOptimalSubstructure " <<  useOptimalSubstructure << endl;
        cout << "useNodeScore " <<  useNodeScore << endl;
        cout << "useHitScore " <<  useHitScore << endl;
        cout << "splits, itemsInCache, nonEmptyRegionPairs " <<  splits << " " << itemsInCache << " " << nonEmptyRegionPairs << endl;
        cout << "fillCacheTime, buildStatisticsTime" <<  fillCacheTime << " " << buildStatisticsTime << endl;
        cout << "--------------------------\n" << endl;
	}
};

#endif
