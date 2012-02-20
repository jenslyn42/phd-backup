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

#include "Setting.h"
#include "RoadGraph.h"
#include "Cache.h"


/**
	@author Jeppe Rishede <jenslyn42@gmail.com>
*/
class scache: public Test{
public:
	scache(){ };
	scache(testsetting ts);
	~scache();

	std::vector<CacheItem> cache;

	void readQuery(intPair query);
	void readQueryList(intPairVector queryList);

	void generateRandQueries(uint numQueries, uint maxVal, string outFn);
	void generateRandLongQueries(uint numQueries, uint maxVal, uint k, string outFn);
	void readQueries(uint numQueries, string inFn);
	void readQ(uint numQueries, string inFn);
	void readLargestQueries(uint numQueries, string inFn);
	void readScoredQueries(uint numQueries, string inFn);
	void readProbQueries(uint numQueries,string inFn);
	void readScoredQueriesFromTrainFile(uint numQueries, string inFn);

private:
	//uint cacheSize;		// why uint?
	//uint cacheUsed;

	bool cacheFull;
	uint calcScoreCounter;
	boost::unordered_map<int, int> calcScoreMap;

	intPairVector queries;
	intPairVector trainingSTPointPairs;
	intPairVector testSTPointPairs;
	
	void checkCache(std::pair<int,int> query);
	intPair findLargestQuery(intPairVector cCandidates, uint k);
	int calcScore(intVector sp, boost::unordered_map<int, int> vSeen);
	void statistics(string outFn);
	void pairStatisticsStoT(string outFn);
	void pairStatisticsAll(string outFn);
	void readTestFile(string fn);
};

#endif
