#ifndef COMPETITORS_H
#define COMPETITORS_H

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

class LRUPLUS: public AbstractCache
{
public:
  LRUPLUS(TestSetting ts);
  ~LRUPLUS();

  struct priorityCompfunc {
    bool operator() (pair<int,int> i, pair<int,int> j) const{
        return i.second < j.second;
    }
  };
  
  boost::unordered_map<int, CacheItem> cache;
  std::set<intPair, priorityCompfunc> ordering; // pid, 'timestamp'
  intSetMap invList; //inveterted list. map int -> unordered_set. or simply: nid -> {pathID}
  
  void buildCache();
  void runQueryList();
  void fillCache();
  intVector kskip(intPair stPair, int pct);

private:
  uint orderVal; //for LRU ordering. Lower=older item, higher=newer item
  uint numEvicted; //stats: total number of items evicted
  uint numEvictedZeroBitmap; //stats: total number of items with zero bits set in 'usefullParts'
  long nodesInCache; // keep track of total number of nodes in all cache items
  boost::unordered_map<int, boost::dynamic_bitset<> > concisePartsp; // pid -> bitmap of which nodes are used by concise
  boost::unordered_map<int, boost::dynamic_bitset<> > usefullParts; // pid -> bitmap of which nodes are used when path contributes to a cache hit
  intMap removalStatus; // pid -> removal status: 1: full path, 2: reduced path, 3: concise path.
  boost::unordered_map<int, std::pair<int, intPairPairs> > lrustats; // itemID -> (length before shrink, ((length after shrink, lenght of base concise path)(sid,tid)) )
  boost::unordered_map<intPair, int> hitstats; // itemID -> (length before shrink, length after shrink)
  std::deque<int> window; //represents a sliding window with queries that has contributed to a cache hit <pid>

  void checkAndUpdateCache(intPair query);
  int insertItem(intVector& sp);
  int insertItem(intVector& sp, intVector&  conciseSp);
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

////////////////////////////////////////////////

#include "Competitors.cpp"

////////////////////////////////////////////////

#endif // COMPETITORS_H
