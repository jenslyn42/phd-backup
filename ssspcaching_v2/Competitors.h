#ifndef COMPETITORS_H
#define COMPETITORS_H

#include "Setting.h"
#include "RoadGraph.h"
#include "Cache.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////


class LRU: public Test{
public:
	LRU(){ };
	LRU(testsetting ts);
	~LRU();

	std::vector<CacheItem> cache;

	void readQuery(intPair query);
    void readStaticQuery(intPair query);
	void readQueries(int numQueries, string inFn);
	void readQueryList(intPairVector queryList);

private:
	bool cacheFull;

	void checkAndUpdateCache(intPair query);
	void checkStaticCache(intPair query);
	void insertItem(uint querySize, intVector nodesInQueryResult, int sNode, int tNode);

	intPairVector trainingSTPointPairs;
	intPairVector testSTPointPairs;

    void readTestData(string fn);
	void readTrainingData(string fn);
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////

class hqf : public Test
{
public:
    hqf(testsetting ts);
    hqf();
    ~hqf();


    aCache cache;

	void readQuery(intPair query);
	void readQueryList(intPairVector queryList);

	void readQueries(int numQueries, string inFn);


private:
    bool cacheFull;
	int calcScoreCounter;
	boost::unordered_map<int, int> calcScoreMap;

	intPairVector queries;

	double startTime,endTime;

	intPairVector trainingSTPointPairs;
	intPairVector testSTPointPairs;

	void readTrainingData(string fn);
    void readTestData(string fn);
    void fillCache(int numQueries, string inFn);
    void checkCache(intPair query);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////


class hqflru : public Test
{
public:
    hqflru(testsetting ts);
    hqflru();
    ~hqflru();


    aCache cache;
    vector<CacheItem> runtimeCache;

	void readQuery(intPair query);
	void readQueryList(intPairVector queryList);

	void readQueries(int numQueries, string inFn);


private:
	bool cacheFull;
	int calcScoreCounter;
	boost::unordered_map<int, int> calcScoreMap;

	vector<pair<int,int> > queries;

	double startTime,endTime;


    intPairVector trainingSTPointPairs;
	intPairVector testSTPointPairs;

	void readTrainingData(string fn);
    void readTestData(string fn);
    void fillCache(int numQueries, string inFn);
    void checkCache(intPair query);
    void checkAndUpdateCache(intPair query);
    void insertItem(uint querySize, intVector nodesInQueryResult, int sNode, int tNode);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HQF_H
