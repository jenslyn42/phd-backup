/****************************************************************************************
 *   Copyright (C) 2011 by Jeppe Rishede 						*
 *   jenslyn42@gmail.com								*
 *											*
 *   All rights reserved.								*
 *											*
 *   Redistribution and use in source and binary forms, with or without 		*
 *   modification, are permitted provided that the following conditions are met:	*
 *   Redistributions of source code must retain the above copyright notice,		*
 *   this list of conditions and the following disclaimer. 				*
 *   Redistributions in binary form must reproduce the above copyright notice,		*
 *   this list of conditions and the following disclaimer in the documentation		*
 *   and/or other materials provided with the distribution. 				*
 *   Neither the name of the <ORGANIZATION> nor the names of its contributors 		*
 *   may be used to endorse or promote products derived from this software 		*
 *   without specific prior written permission						*
 *                                                                         		*
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   		*
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     		*
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 		*
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER		*
 *   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 		*
 *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   		*
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    		*
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 		*
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  		*
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    		*
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          		*
 ***************************************************************************************/
#ifndef PROBSTATICCACHE_H
#define PROBSTATICCACHE_H



#include "Setting.h"
#include "RoadGraph.h"
#include "Cache.h"



/**
	@author Jeppe Rishede <jenslyn42@gmail.com>
*/
class probstaticCache: public Test{
public:
	probstaticCache(testsetting ts);
	probstaticCache();

	~probstaticCache();

	//std::vector<CacheItem> cache;
	aCache cache;

	void readQuery(intPair query);
	void readQueryList(std::vector<intPair> queryList);
	void readQueries(int numQueries, string inFn);

private:
	unsigned long cacheSize;
	unsigned long cacheUsed;

	bool cacheFull;
	int calcScoreCounter;
	vector<intPair> queries;

	double startTime,endTime;

	boost::unordered_map<intPair, int > trainingQueriesPerRegionPair;


	// [Feb 14] added
	intVector nodeid2regionid;

	boost::unordered_map<coordinate, int> coordinate2regionidMap;
	boost::unordered_map<int, intVector > regionid2nodeidVector;
	boost::unordered_map<intPair, vector<intPair> > regionPair2nodePairVector;

	boost::unordered_map<int, region> mapRegions; //holds all regions after a call to makePartitions()

	std::vector<intPair> trainingSTPointPairs;
	std::vector<intPair> testSTPointPairs;


	double calcScore(intVector& sp, intPairSet& vSeen);
	void extractStatistics();

	void readData(string fn);
    void readTestData(string fn);
	void readTrainingData(string fn);
	bool makePartitions(int splits);
	void split(std::vector<region>& regions, int axis);
	int mapNodeid2RegionId(int nid);
	int mapCoordinate2RegionId(coordinate coord);
	void fillCache(int numQueries, string inFn);
	void fillCacheFromQueriesFile(int numQueries, string inFn);
	void fillCacheFromQueriesFileByStatistics(int numQueries, string inFn);
    intPair pickSTpair(intPair regionPair);
    void buildRegionId2NodeidVector();
    void buildRegionpair2NodepairVector();
	//void sumUpNodes();
};

#endif
