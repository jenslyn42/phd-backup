#ifndef ORACLE_H
#define ORACLE_H

#include "CacheItem.h"
#include "testsetting.h"
#include "RoadGraph.h"
#include "utility.h"
#include "acache.h"
#include "Test.h"
//
#include <boost/foreach.hpp>
#include "boost/unordered_map.hpp"
//#include <boost/lexical_cast.hpp>
//
#include <string>
#include <vector>
#include <fstream>
#include "stdio.h"
//#include <math.h>
#include <iostream>
//#include <ctime>
//#include <cstdlib>

class oracle: public Test{

public:
    oracle(testsetting ts);
    oracle();
    ~oracle();

    void readQuery(std::pair<int,int> query);
    void readQueryList(std::vector<std::pair<int,int> > queryList);
    int getCacheHits(){return numCacheHits;}
    int getTotalQueries(){return numTotalQueries;}
    int getTotalDijkstraCalls(){return numDijkstraCalls;}
    int getQueryNumCacheFull(){return -1;}
    void readQueries(int numQueries, std::string inFn);

private:

    std::vector<std::pair<int, int> > trainingSTPointPairs;
    std::vector<std::pair<int, int> > testSTPointPairs;

    regionlist points; //holds all coordinates from the map

    double startTime,endTime;
    aCache cache;

    int numTotalQueries;
    int numCacheHits;
    int numDijkstraCalls;

    boost::unordered_map<coordinate, int> coordinate2Nodeid;
    boost::unordered_map<int, coordinate> nodeid2coordinate;

    boost::unordered_map<int, std::pair<intPair, std::vector<int> > > pathId2spPathTraining; //PathID, < coordinate, shortestPath >
    boost::unordered_map<int, std::pair<intPair, std::vector<int> > > pathId2spPathTest;
    boost::unordered_map<int, vector<int> > nodeId2pathIdTraining;
    boost::unordered_map<int, vector<int> > nodeId2pathIdTest;

    //holds how many paths each training path can answer from the test data as well as the ids of each path answered
    boost::unordered_map<int, std::pair<int, std::vector<int> > > pathId2workloadAnswered; //PathID, <(test)paths answered, pid of each answered path>

	void readMapData();
	void readTrainingData(string fn);
    void readTestData(string fn);
    void buildPathId2spPath();
    void buildNodeId2pathId();

    void futureWorkloadAnswers();
    void fillCache();
    maxWorkloadHeap rankWorkloadAnswered(boost::unordered_map<int, std::pair<int, std::vector<int> > > pathId2workloadAnswered);
    void print();
    void checkCache(std::pair< int, int > query);
};

#endif // ORACLE_H
