#ifndef ORACLE_H
#define ORACLE_H

#include "Setting.h"
#include "RoadGraph.h"
#include "Cache.h"

class oracle: public Test{

public:
    oracle(testsetting ts);
    oracle();
    ~oracle();

    void readQuery(intPair query);
    void readQueryList(intPairVector queryList);
    void readQueries(int numQueries, std::string inFn);

private:
    intPairVector trainingSTPointPairs;
    intPairVector testSTPointPairs;
   
    double startTime,endTime;
    aCache cache;

    boost::unordered_map<int, std::pair<intPair, std::vector<int> > > pathId2spPathTraining; //PathID, < coordinate, shortestPath >
    boost::unordered_map<int, std::pair<intPair, std::vector<int> > > pathId2spPathTest;
    intVectorMap nodeId2pathIdTraining;
    intVectorMap nodeId2pathIdTest;

    //holds how many paths each training path can answer from the test data as well as the ids of each path answered
    boost::unordered_map<int, std::pair<int, std::vector<int> > > pathId2workloadAnswered; //PathID, <(test)paths answered, pid of each answered path>

	void readTrainingData(string fn);
    void readTestData(string fn);
    void buildPathId2spPath();
    void buildNodeId2pathId();

    void futureWorkloadAnswers();
    void fillCache();
    maxWorkloadHeap rankWorkloadAnswered(boost::unordered_map<int, std::pair<int, std::vector<int> > > pathId2workloadAnswered);
    void print();
    void checkCache(intPair query);
};

#endif // ORACLE_H
