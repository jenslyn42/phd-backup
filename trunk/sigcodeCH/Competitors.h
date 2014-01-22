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
  std::set<intPair, priorityCompfunc> ordering; // pid, 'score'
  intSetMap invList; //inveterted list. map int -> unordered_set. or simply: nid -> {pathID}
  

  void buildCache();
  void runQueryList();
  void fillCache();

private:
  long nodesInCache; // keep track of total number of nodes in all cache items
  
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

////////////////////////////////////////////////

#include "Competitors.cpp"

////////////////////////////////////////////////

#endif // COMPETITORS_H
