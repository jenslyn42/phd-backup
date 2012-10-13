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
#define NODE_BITS (32)
#define BIT (1)

//#define NUM_BITS(csize) (csize*8)


typedef std::pair<int,string> LookupPair;
typedef vector<LookupPair> LookupList;

// algorithmic choice
enum ALGO_CHOICE { ALGO_NONE, ALGO_SPC, ALGO_SPCplus, ALGO_SPCstar, ALGO_LRU, ALGO_HQF, ALGO_SCACHE, ALGO_HQFLRU, ALGO_ORACLE };

// cache storage choice
enum STORAGE_CHOICE { STORE_LIST, STORE_GRAPH, STORE_COMPRESS };

// architecture choice
enum ARCH_CHOICE { ARCH_SERVER, ARCH_PROXY };

// range query algorithm
enum RALG_CHOICE { RALG_FAIR, RALG_NAIVE };


// Important: 	for each "ENUM" type, there is a corresponding "LookupList" in "Setting.cpp"
//				they follow a naming convention.
// 				make sure that "InitEnumMappings" is updated according to ENUM above
extern LookupList ALGO_ENUM, STORAGE_ENUM, ARCH_ENUM, RALG_ENUM;

void InitEnumMappings(); // initialize the above "LookupList"
int MatchEnumCode(LookupList& list,string str);		// lookup an enum value by a string
string MatchEnumString(LookupList& list,int code); 	// lookup a string by an enum value

// queryfile type, used to determine whether to read the test or training file.
enum QLOG_CHOICE { QLOG_TRAIN, QLOG_TEST };

//predeclare in order to define intRegionMap
struct Region;

typedef std::pair<int,int> intPair;
typedef std::vector<intPair>  intPairVector;
typedef std::vector<int>  intVector;

typedef std::pair<double, double> Point;
typedef std::vector<Point> PointVector;
typedef std::pair<Point,Point> PointPairs;
typedef std::pair<intPair,intPair> intPairPairs;


typedef boost::unordered_set<intPair> intPairSet;
typedef boost::unordered_map<int, intPair > intPairMap;
typedef boost::unordered_map<intPair, int> intPairIntMap;
typedef boost::unordered_map<intPair, Point> intPairPointMap;

typedef boost::unordered_map<int, int> intMap;
typedef boost::unordered_map<int, double> intDoubleMap;
typedef boost::unordered_map<int, Point> intPointMap;
typedef boost::unordered_map<int, intVector > intVectorMap;

typedef boost::unordered_map<int, intMap > intintMapMap;
typedef boost::unordered_map<int, std::pair<int, intVector > > intIntVectorMap;
typedef boost::unordered_map<int, std::pair<intPair, intVector > > intIntPairVectorMap;
typedef boost::unordered_map<intPair, intVector> intPairIntVectorMap;
typedef boost::unordered_map<int, Region> intRegionMap;

typedef	boost::unordered_map<int, boost::dynamic_bitset<> > intDBitset;
typedef	boost::unordered_map<Point, int> PointIntMap;
typedef	boost::unordered_map<PointPairs, int> PointPairsIntMap;

typedef boost::unordered_map<string,string> ConfigType;



struct Region {
    int id;
    double xmin,xmax,ymin,ymax;
	PointVector points;

    Region(){ }

    Region(int rid, double _xmin, double _xmax, double _ymin, double _ymax) {
        id = rid;
        xmin = _xmin;
		xmax = _xmax;
		ymin = _ymin;
		ymax = _ymax;
		points.clear();
    }
};


template <class T>
inline std::string to_string(const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}


class TestSetting {
    double fillCacheTime, buildStatisticsTime;

	void printConfigError(string key,bool required);
	void trimSpace(char* str);
	ConfigType cr;

public:

	// these attributes can be accessed directly
	std::string testName;
	std::string testFilePrefix;

	int inputFileType;		// used for RoadGraph
	int scacheQueryType;	// used for SCACHE only
	int splits, itemsInCache, nonEmptyRegionPairs;
    unsigned int numpoi; // used for rangesearch
    int range; // used for rangesearch. For any range there has to exist a pregenerated file done with AbstractCache::generateRangeQueries(int range)

    bool useRange; //used for range queries
    bool useSPtree; //used with range queries
    bool skipSPcalc; //mainly to be used with range queries
	bool useDijkstra;


	ALGO_CHOICE testAlgo;
	STORAGE_CHOICE testStorage;
	ARCH_CHOICE testScenario;
	RALG_CHOICE testRangetype;
	unsigned long cacheSize;

	TestSetting();
	~TestSetting();

	void addConfigFromFile(const char* filename);
	void addConfigFromCmdLine(int argc,char** argv);
	void listConfig();

	float getConfigFloat(string key,bool required=true,float _default=0);
	int getConfigInt(string key,bool required=true,int _default=0);
	long getConfigLong(string key,bool required=true,long _default=0);
	string getConfigString(string key,bool required=true,string _default="");
	bool getConfigBool(string key,bool required=true,bool _default=false);


	int getEnumCode(LookupList& list,string key);

    int getNonEmptyRegionPairs(){return nonEmptyRegionPairs;}
    void setNonEmptyRegionPairs(int rp){nonEmptyRegionPairs = rp;}

    double getItemsInCache(){return itemsInCache;}
    void setItemsInCache(double iic){itemsInCache = iic;}

    double getBuildStatisticsTime(){return buildStatisticsTime;}
    void setBuildStatisticsTime(double st){buildStatisticsTime = st;}

    double getFillCacheTime(){return fillCacheTime;}
    void setFillCacheTime(double ct){fillCacheTime = ct;}

	std::string getTestName() const {return testName;}

	int getSplits() const {return splits;}

	void printSetting();

    double getElapsedTime(double& refTime) {
		return (double(clock()-refTime))/CLOCKS_PER_SEC;
	}
};



#endif
