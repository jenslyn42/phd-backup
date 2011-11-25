#ifndef RANDCACHE_H
#define RANDCACHE_H

#include "CacheItem.h"
#include "testsetting.h"
#include "Test.h"
#include "RoadGraph.h"
#include "utility.h"
#include "acache.h"

#include <boost/foreach.hpp>
#include "boost/unordered_map.hpp"
#include <boost/lexical_cast.hpp>

#include <string>
#include <vector>
#include <fstream>
#include "stdio.h"
#include <math.h>
#include <iostream>
#include <ctime>
#include <cstdlib>

class randcache : public Test
{
public:
    randcache(testsetting ts);
    randcache();
    ~randcache();


    aCache cache;

	void readQuery(std::pair<int,int> query);
	void readQueryList(std::vector< std::pair<int,int> > queryList);
	int getCacheHits(){return numCacheHits;}
	int getTotalQueries(){return numTotalQueries;}
	int getTotalDijkstraCalls(){return numDijkstraCalls;}
	int getQueryNumCacheFull(){return queryNumCacheFull;}

	void readQueries(int numQueries, string inFn);


private:
    int numTotalQueries;
	int numCacheHits;
	int numDijkstraCalls;
	int queryNumCacheFull;

	int cacheSize;
	int cacheUsed;

	bool useNodeScore;
	bool useHitScore;
	bool cacheFull;
	int mapSize;
	int calcScoreCounter;
	boost::unordered_map<int, int> calcScoreMap;
    regionlist points; //holds all coordinates from the map

	vector<pair<int,int> > queries;

	double startTime,endTime;

	boost::unordered_map<coordinate, int> coordinate2Nodeid;
	boost::unordered_map<int, coordinate> nodeid2coordinate;
	boost::unordered_map<intPair, intPair> cacheStats; // (node1,node2) -> (seen count, sp length)

    std::vector<std::pair<int, int> > trainingSTPointPairs;
	std::vector<std::pair<int, int> > testSTPointPairs;

	void readMapData();
	void readTrainingData(string fn);
    void readTestData(string fn);
    void fillCache(int numQueries, string inFn);
    void checkCache(std::pair<int,int> query);

    void writeoutCacheCoordinates(string testbasename, vector<CacheItem> cm, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits);
	int writeoutTestCoordinates(string testbasename, std::vector<intPair > stPointPairs, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits);
	int writeoutTrainingCoordinates(string testbasename, std::vector<intPair > stPointPairs, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits);


};

#endif // RANDCACHE_H
