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
#ifndef SETTING_H
#define SETTING_H

/**
	@author Jeppe Rishede <jenslyn42@gmail.com>
*/

#include <utility>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <typeinfo>
#include <iostream>
#include <fstream>

#include <algorithm>
#include <assert.h>

#include <ctime>
#include <cmath>
#include <cstdlib>




#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/dynamic_bitset.hpp>
using namespace std;


//Used in acache and when defining tests
#define NODE_BITS 32
#define BIT 1
#define GRAPH_CACHE 1
#define LIST_CACHE 2
#define COMPRESSED_G_CACHE 3
#define SERVER_SCENARIO 10
#define PROXY_SCENARIO 11

enum CACHE_METHOD { METHOD_NONE, METHOD_SPC, METHOD_LRU, METHOD_HQF, METHOD_SCACHE, METHOD_HQFLRU, METHOD_ORACLE };



typedef std::pair<int,int> intPair;
typedef std::vector<intPair>  intPairVector;
typedef std::vector<int>  intVector;

typedef std::pair<double, double> coordinate;
typedef std::vector<coordinate > regionlist;
typedef std::pair<coordinate, coordinate> coordinatePairs;
typedef std::pair<intPair, intPair > intPairPairs;


typedef boost::unordered_set<intPair> intPairSet;

typedef boost::unordered_map<int, intPair > intPairMap;
typedef boost::unordered_map<intPair, int> intPairIntMap;

typedef boost::unordered_map<int, int> intMap;
typedef boost::unordered_map<int, double> intDoubleMap;

typedef boost::unordered_map<int, coordinate> intCoordinateMap;
typedef boost::unordered_map<int, intVector > intVectorMap;

typedef boost::unordered_map<int, std::pair<int, intVector > > intIntVectorMap;
typedef boost::unordered_map<int, std::pair<intPair, intVector > > intIntPairVectorMap;


typedef	boost::unordered_map<int, boost::dynamic_bitset<> > intDBitset;
typedef	boost::unordered_map<coordinate, int> coordinateIntMap;


struct region {
    int id;
    double xmin,xmax,ymin,ymax;
	regionlist points;

    region(){ }

    region(int rid, double _xmin, double _xmax, double _ymin, double _ymax) {
        id = rid;
        xmin = _xmin;
		xmax = _xmax;
		ymin = _ymin;
		ymax = _ymax;
		points.clear();
    }
};

typedef boost::unordered_map<int, region> regionMap;





class testsetting {


private:
	std::string testName;
	std::string testFile;
	int testType;
	unsigned long cacheSize;

	//Indicates which method should be used to fill the cache.
	int staticQueryType;

	int splits, itemsInCache, nonEmptyRegionPairs, testScenario;

    double fillCacheTime, buildStatisticsTime;

public:

	testsetting(){ };

	testsetting(string testname, string testfile, int testType, unsigned long cacheSize, int cacheType);

	void setData(string testname, string testfile, int testType, unsigned long cacheSize, int cacheType);

	~testsetting();

	std::string preComputedQueriesFileName;
	std::string queryFileName;

	int cacheType;

	//instead of Rand()
	unsigned long randReplacement;


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

	std::string getTestName() const {return testName;}

 	void setCacheSize (unsigned long cs, int cType );
	unsigned long getCacheSize() const {return cacheSize;}

	std::string getTestFile() const{return testFile;}

	void setTestType (int theValue){testType = theValue;}
	int getTestType() const{return testType;}

	void setStaticQueryType (int val){staticQueryType = val;}
	int getStaticQueryType() const{return staticQueryType;}

	void setSplits(int theValue){splits = theValue;}
	int getSplits() const {return splits;}

	void writeOut()
	{
	    cout << "\n--------------------------" << endl;
        cout << "testName " << testName << endl;
        cout << "testFile " << testFile << endl;
        cout << "testType" << testType << endl;
        cout << "cacheSize " <<  cacheSize << endl;
        cout << "staticQueryType " <<  staticQueryType << endl;
        cout << "splits, itemsInCache, nonEmptyRegionPairs " <<  splits << " " << itemsInCache << " " << nonEmptyRegionPairs << endl;
        cout << "fillCacheTime, buildStatisticsTime" <<  fillCacheTime << " " << buildStatisticsTime << endl;
        cout << "--------------------------\n" << endl;
	}
};



#endif
