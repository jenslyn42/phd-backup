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
#ifndef SCACHE_H
#define SCACHE_H

#include "CacheItem.h"
#include "testsetting.h"
#include "Test.h"
#include "RoadGraph.h"
#include "utility.h"

#include <boost/foreach.hpp>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <math.h>


/**
	@author Jeppe Rishede <jenslyn42@gmail.com>
*/
class scache: public Test{
public:
	scache(){ };
	scache(testsetting ts);
	~scache();

	std::vector<CacheItem> cache;

	void readQuery(std::pair<int,int> query);
	void readQueryList(std::vector< std::pair<int,int> > queryList);
	int getCacheHits(){return numCacheHits;}
	int getTotalQueries(){return numTotalQueries;}
	int getTotalDijkstraCalls(){return numDijkstraCalls;}
	int getQueryNumCacheFull(){return queryNumCacheFull;}

	void generateRandQueries(uint numQueries, uint maxVal, string outFn);
	void generateRandLongQueries(uint numQueries, uint maxVal, uint k, string outFn);
	void readQueries(uint numQueries, string inFn);
	void readQ(uint numQueries, string inFn);
	void readLargestQueries(uint numQueries, string inFn);
	void readScoredQueries(uint numQueries, string inFn);
	void readProbQueries(uint numQueries,string inFn);
	void readScoredQueriesFromTrainFile(uint numQueries, string inFn);

private:
	int numTotalQueries;
	int numCacheHits;
	int numDijkstraCalls;
	int queryNumCacheFull;

	uint cacheSize;
	uint cacheUsed;

	bool useNodeScore;
	bool useHitScore;
	bool cacheFull;
	uint calcScoreCounter;
	boost::unordered_map<int, int> calcScoreMap;

	vector<pair<int,int> > queries;
	std::vector<std::pair<int, int> > trainingSTPointPairs;
	std::vector<std::pair<int, int> > testSTPointPairs;
	boost::unordered_map<std::pair<double, double>, int> coordinate2Nodeid;
	boost::unordered_map<int, std::pair<double, double> > nodeid2coordinate;

	void checkCache(std::pair<int,int> query);
	intPair findLargestQuery(vector<pair<int, int> > cCandidates, uint k);
	int calcScore(vector<int> sp, boost::unordered_map<int, int> vSeen);
	void statistics(string outFn);
	void pairStatisticsStoT(string outFn);
	void pairStatisticsAll(string outFn);
	void readMapData();
	void writeoutCacheCoordinates(string testbasename, std::vector<CacheItem> cm, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits);
	int writeoutTestCoordinates(string testbasename, std::vector<intPair > stPointPairs, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits);
	void readTestFile(string fn);
};

#endif
