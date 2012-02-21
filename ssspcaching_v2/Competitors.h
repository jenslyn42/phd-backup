#ifndef COMPETITORS_H
#define COMPETITORS_H

#include "Setting.h"
#include "RoadGraph.h"
#include "Cache.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////


class LRU: public AbstractCache
{
public:
	LRU(TestSetting ts);
	~LRU();

	std::vector<CacheItem> cache;

	void buildCache();
	void runQueryList();

private:
	void checkAndUpdateCache(intPair query);
	void insertItem(intVector& sp);
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////

class HQF: public AbstractCache
{
public:
    HQF(TestSetting ts);
    ~HQF();


    CacheStorage cache;

	
	void buildCache();
	void runQueryList();


private:
	int calcScoreCounter;
	boost::unordered_map<int, int> calcScoreMap;
	intPairVector queries;
	
    void fillCache();
    void checkCache(intPair query);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////


class HybridHQFLRU: public AbstractCache
{
public:
    HybridHQFLRU(TestSetting ts);
    ~HybridHQFLRU();


    CacheStorage cache;
    vector<CacheItem> runtimeCache;
	
	void buildCache();
	void runQueryList();
	

private:
	int calcScoreCounter;
	boost::unordered_map<int, int> calcScoreMap;
	intPairVector queries;

    void fillCache();
    void checkAndUpdateCache(intPair query);
    void insertItem(uint querySize, intVector nodesInQueryResult, int sNode, int tNode);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HQF_H
