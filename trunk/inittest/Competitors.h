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
	vector<int> checkAndUpdateCache(intPair query, bool constWeight);
	bool isSame(intVector& one, intVector& second );
	pair<intPair,Point> similarity(intVector& one, intVector& second );
	double lcp(intVector& one, intVector& second );
	void aggregate(IntDoubleIntPairIntPairPointMap& simMap);
    void lcpAgg(IntDoubleIntPairIntPairPointMap& simMap);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HQF_H
