/****************************************************************************************
 *   Copyright (C) 2011 by Jeppe Rishede 						*
 *   jenslyn42@gmail.com								*
 *											*
 *   All rights reserved.								*
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
#include <queue>
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept> //standard exceptions that both the library and programs can use to report common errors.

#include <algorithm>
#include <assert.h>

#include <ctime>
#include <cmath>
#include <cstdlib>
#include <set>

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
enum ALGO_CHOICE { ALGO_NONE, ALGO_SPC, ALGO_SPCplus, ALGO_SPCstar, ALGO_LRU, ALGO_LRUplus, ALGO_HQF, ALGO_SCACHE, ALGO_HQFLRU, ALGO_ORACLE };

// cache storage choice
enum STORAGE_CHOICE { STORE_LIST, STORE_GRAPH, STORE_COMPRESS };

// architecture choice
enum ARCH_CHOICE { ARCH_SERVER, ARCH_PROXY };

// SP calculation choice
enum SPTYPE_CHOICE {SPTYPE_FULL, SPTYPE_CONCISE, SPTYPE_OPTIMAL };

// Concise calculation choice
enum CONCISETYPE_CHOICE {CONCISETYPE_A, CONCISETYPE_B };

// Optimal calculation choice
enum OPTIMALTYPE_CHOICE {OPTIMALTYPE_ORG, OPTIMALTYPE_KSKIP, OPTIMALTYPE_RAND, OPTIMALTYPE_ORDERFILL };

// Important: 	for each "ENUM" type, there is a corresponding "LookupList" in "Setting.cpp"
//		they follow a naming convention.
// 		make sure that "InitEnumMappings" is updated according to ENUM above
extern LookupList ALGO_ENUM, STORAGE_ENUM, ARCH_ENUM, SPTYPE_ENUM, CONCISETYPE_ENUM, OPTIMALTYPE_ENUM;

void InitEnumMappings(); // initialize the above "LookupList"
int MatchEnumCode(LookupList& list,string str);		// lookup an enum value by a string
string MatchEnumString(LookupList& list,int code); 	// lookup a string by an enum value


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
typedef boost::unordered_map<int, pair<clock_t, int> > intTimeIntPairMap;

typedef boost::unordered_map<int, int> intMap;
typedef boost::unordered_map<int, double> intDoubleMap;
typedef boost::unordered_map<int, Point> intPointMap;
typedef boost::unordered_map<int, intVector > intVectorMap;
typedef boost::unordered_map<int, boost::unordered_set<int> > intSetMap;

typedef boost::unordered_map<int, std::pair<int, intVector > > intIntVectorMap;
typedef boost::unordered_map<int, std::pair<intPair, intVector > > intIntPairVectorMap;

typedef	boost::unordered_map<int, boost::dynamic_bitset<> > intDBitset;
typedef	boost::unordered_map<Point, int> PointIntMap;

typedef boost::unordered_map<string,string> ConfigType;


///////// CH /////////////////
typedef datastr::graph::UpdateableGraph UpdateableGraph;
typedef processing::ConstructCH<UpdateableGraph> ProcessingConstruct;
typedef processing::DijkstraCH<UpdateableGraph, NormalPQueue, 2, false> Dijkstra;
/////////////////////////////

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


class TestSetting {
  double fillCacheTime, buildStatisticsTime, nodesInCache;
  
  void printConfigError(string key,bool required);
  void trimSpace(char* str);
  ConfigType cr;
  
public:
  // these attributes can be accessed directly
  std::string testName;
  std::string testFile;
  std::string queryFileName;

  int inputFileType;    // used for RoadGraph
  int scacheQueryType;  // used for SCACHE only
  int splits, itemsInCache, nonEmptyRegionPairs, optiNum;
  
  bool useConcisepath;
  bool measureConcisepathdegrees;
  bool executeTrainingWorkload;
  bool divideScoreByLength;
  
  unsigned long optiLength, longLength;
  int numOpti, numLong;
  
  ALGO_CHOICE testAlgo;
  STORAGE_CHOICE testStorage;
  ARCH_CHOICE testScenario;
  SPTYPE_CHOICE testSPtype;
  CONCISETYPE_CHOICE testConcisetype;
  OPTIMALTYPE_CHOICE testOptimaltype;
  unsigned long cacheSize, unusedCacheBits;
  

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
  
  double getAvgItemLength(){return (double)nodesInCache / (double) itemsInCache;}
  
  void setNodesInCache(int numNodes){nodesInCache = numNodes;}
  
  long getUnusedCacheBits(){return unusedCacheBits;}
  void setUnusedCacheBits(int numBits){unusedCacheBits = numBits;}
  
  double getBuildStatisticsTime(){return buildStatisticsTime;}
  void setBuildStatisticsTime(double st){buildStatisticsTime = st;}

  double getFillCacheTime(){return fillCacheTime;}
  void setFillCacheTime(double ct){fillCacheTime = ct;}

  std::string getTestName() const {return testName;}

  std::string getTestFile() const{return testFile;}
  
  int getSplits() const {return splits;}
  
  void printSetting();
  
  double getElapsedTime(double& refTime){
    return (double(clock() -refTime))/CLOCKS_PER_SEC;
  }
};

////////////////////////////////////////////////

#include "Setting.cpp"

////////////////////////////////////////////////

#endif
