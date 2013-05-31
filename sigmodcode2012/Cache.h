/********************************************************************************
 *   Copyright (C) 2011 by Jeppe Rishede 					*
 *   jenslyn42@gmail.com							*
 *										*
 *   All rights reserved.							*
 *										*
 *   Redistribution and use in source and binary forms, with or without 	*
 *   modification, are permitted provided that the following conditions are met:*
 *   Redistributions of source code must retain the above copyright notice,	*
 *   this list of conditions and the following disclaimer. 			*
 *   Redistributions in binary form must reproduce the above copyright notice,	*
 *   this list of conditions and the following disclaimer in the documentation	*
 *   and/or other materials provided with the distribution. 			*
 *   Neither the name of the <ORGANIZATION> nor the names of its contributors 	*
 *   may be used to endorse or promote products derived from this software 	*
 *   without specific prior written permission					*
 *                                                                         	*
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   	*
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     	*
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 	*
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER	*
 *   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 	*
 *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   	*
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    	*
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 	*
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  	*
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    	*
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          	*
 ********************************************************************************/
#ifndef CACHEITEM_H
#define CACHEITEM_H


#include "Setting.h"
#include "RoadGraph.h"

class CacheItem {
	int accessKey;
	double score;

public:
	int id, s, t, size;
	intVector item;

	CacheItem();
	CacheItem(int key, intVector& item);
	CacheItem(int key, intVector& item, int queryStartNode, int queryTargetNode);

	void updateKey(int newKey) {accessKey = newKey;}
	int key() const {return accessKey;}
	void setScore(double score){this->score = score;}
	double getScore(){return score;}
	
 	inline bool operator< (CacheItem const& cItem)const {return (accessKey < cItem.key());}
};

typedef boost::unordered_map<int, CacheItem> intCacheitemMap;
typedef boost::unordered_map<intPair, CacheItem> intPairCacheitemMap;

enum QLOG_CHOICE { QLOG_TRAIN, QLOG_TEST };

class AbstractCache {

// these attributes are accessible by subclasses
protected:
	PointVector points; //holds all Points from the map
	PointIntMap Point2Nodeid;
	intPointMap nodeid2Point;
	intPairVector trainingSTPointPairs,testSTPointPairs;
	
	int numTotalQueries;
	int numCacheHits;
	int numDijkstraCalls;
	int mapSize;

	// important: note that the cache size is expressed as the number of "bits"
	unsigned long cacheSize;
	unsigned long cacheUsed;

public:
	AbstractCache(){ };
	~AbstractCache(){ };

	TestSetting ts;

	// virtual functions to be defined in subclasses
	virtual void buildCache() {};
	virtual void runQueryList() {};

	// shared methods
	int getCacheHits(){return numCacheHits;}
	int getTotalQueries(){return numTotalQueries;}
	int getTotalDijkstraCalls(){return numDijkstraCalls;}

	void readMapData();
	void plotCachePoints(vector<CacheItem>& cm);
	bool plotShortestPaths(QLOG_CHOICE qlog);
	void readQueryLogData(QLOG_CHOICE qlog);
	
	double getElapsedTime(double& refTime) {
		double oldTime=refTime;
		refTime=clock();
		return (double(clock()-oldTime))/CLOCKS_PER_SEC;
	}

};

struct CompressPidType {
	int token;
	int prev_token,prev_size;
	intPairVector vecpair;	// CONSIDERED in the cache size
	intVector vecnodes;	// NOT CONSIDERED in the cache size (just for temporary use)
};

class CompressedPidTokens {
public:
	CompressedPidTokens();
	~CompressedPidTokens();

	void insertPath(intVector& vecpath);
	int GetTotalNumItems() {return _total_pid_items;}
	void printNodesTokensPaths();
	int getNumNodes() {return NodeTokens.size();}
	int GetNumTokens() { return _num_token;}

private:
	boost::unordered_map<int,int> NodeTokens;						// the token of each node
	boost::unordered_map<int,CompressPidType*> TokenCompressPids;	// the compressed path of each token

	int _num_token;
	int pathID;
	int _total_pid_items;

	int GenerateToken() { return (_num_token++);}
	void insertVecPair( intPairVector& vecpair, int path_id );
	void printVecPair( intPairVector& vecpair );
	void insertCompressedPid( int path_id, intVector& tmpModify, CompressPidType* prevcomppath );
};


class CacheStorage {
public:

	// add an init function in the constructor to initialize the invertedLists
	CacheStorage();
	CacheStorage(TestSetting ts);

	void init(TestSetting ts);

	~CacheStorage(){ };

	bool insertItem(CacheItem ci);
	bool insertItemWithScore(CacheItem ci, double score);
	bool checkCache(intPair query);
	bool hasEnoughSpace(CacheItem ci);
	bool hasEnoughSpace(intVector& sp);
	unsigned long getCacheUsed(){return cacheUsed;}
	unsigned long size(){return cacheSize;}
	int numberOfNodesInCache(){return numberOfNodes;}
	int numberOfItemsInCache(){return cache.size();}
	void printNodesTokensPaths();
	int getCachespaceleftBits(){return cacheSize - cacheUsed;}
	
	vector<CacheItem> cache;
	
private:
	intVector* invertedLists;	// to support fast query processing
	int mapSize;
	
	STORAGE_CHOICE testStorage;

	double totalEntriesInCompressedBitsets; //only used with COMPRESSED_G_CACHE
	unsigned long cacheSize;
	unsigned long cacheUsed;
	uint numberOfNodes;
	boost::unordered_map<int, boost::dynamic_bitset<> > nodeIdsInCache;
	
	CompressedPidTokens pidSets;

	void updateCacheUsed(CacheItem ci);

};

#endif
