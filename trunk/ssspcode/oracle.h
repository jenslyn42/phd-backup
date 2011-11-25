#ifndef ORACLE_H
#define ORACLE_H

//#include "CacheItem.h"
#include "testsetting.h"
#include "RoadGraph.h"
#include "utility.h"
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

class oracle
{
public:
    oracle(testsetting ts);
    oracle();
    ~oracle();

    testsetting ts;
    void run();

private:

    std::vector<std::pair<int, int> > trainingSTPointPairs;
    std::vector<std::pair<int, int> > testSTPointPairs;

    regionlist points; //holds all coordinates from the map

    double startTime,endTime;

    boost::unordered_map<coordinate, int> coordinate2Nodeid;
    boost::unordered_map<int, coordinate> nodeid2coordinate;

    boost::unordered_map<int, std::pair<intPair, std::vector<int> > > pathId2spPathTraining; //PathID, < coordinate, shortestPath >
    boost::unordered_map<int, std::pair<intPair, std::vector<int> > > pathId2spPathTest;
    boost::unordered_map<int, vector<int> > nodeId2pathIdTraining;
    boost::unordered_map<int, vector<int> > nodeId2pathIdTest;

    boost::unordered_map<int, std::pair<int, std::vector<int> > > pathId2workloadAnswered; //holds how many paths each training path can answer from the test data as well as the ids of each path answered

	void readMapData();
	void readTrainingData(string fn);
    void readTestData(string fn);
    void buildPathId2spPath();
    void buildNodeId2pathId();

    void futureWorkloadAnswers();
    void print();
};

#endif // ORACLE_H
