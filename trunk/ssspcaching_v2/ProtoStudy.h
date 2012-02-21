#ifndef PROTOSTUDY_H
#define PROTOSTUDY_H

#include "Setting.h"
#include "RoadGraph.h"
#include "Cache.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////

class Oracle: public AbstractCache {

public:
    Oracle(TestSetting ts);
    ~Oracle();
	
    void runQueryList();
    void buildCache();

private:
    CacheStorage cache;

	intIntPairVectorMap pathId2spPathTraining; //PathID, < coordinate, shortestPath >
    intIntPairVectorMap pathId2spPathTest;
    intVectorMap nodeId2pathIdTraining;
    intVectorMap nodeId2pathIdTest;

    //holds how many paths each training path can answer from the test data as well as the ids of each path answered
    intIntVectorMap pathId2workloadAnswered; //PathID, <(test)paths answered, pid of each answered path>

    void buildPathId2spPath();
    void buildNodeId2pathId();

    void futureWorkloadAnswers();
    void fillCache();
    maxWorkloadHeap rankWorkloadAnswered(boost::unordered_map<int, std::pair<int, std::vector<int> > > pathId2workloadAnswered);
    void print();
    void checkCache(intPair query);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////


// *** [Feb 20] obsolete class (calculation of "cacheUsed" not yet updated as the number of bits)
class Scache: public AbstractCache {
public:
	Scache(TestSetting ts);
	~Scache();

	std::vector<CacheItem> cache;
	
	
	void buildCache();
	void runQueryList();

	void generateRandQueries(uint numQueries, uint maxVal, string outFn);
	void generateRandLongQueries(uint numQueries, uint maxVal, uint k, string outFn);
	
	
	void readQ(string inFn);
	void readLargestQueries(string inFn);
	void readScoredQueries(string inFn);
	void readProbQueries(string inFn);
	void readScoredQueriesFromTrainFile(string inFn);

private:
	uint calcScoreCounter;
	boost::unordered_map<int, int> calcScoreMap;
	intPairVector queries;
	
	void checkCache(std::pair<int,int> query);
	intPair findLargestQuery(intPairVector cCandidates, uint k);
	int calcScore(intVector sp, boost::unordered_map<int, int> vSeen);
	void statistics(string outFn);
	void pairStatisticsStoT(string outFn);
	void pairStatisticsAll(string outFn);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // PROTOSTUDY_H
