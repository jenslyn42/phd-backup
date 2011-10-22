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
#ifndef LRU_H
#define LRU_H

#include "CacheItem.h"
#include "testsetting.h"
#include "RoadGraph.h"
#include "Test.h"

#include <algorithm>
#include <vector>
#include <utility>

#include <boost/foreach.hpp>

/**
	@author Jeppe Rishede <jenslyn42@gmail.com>
*/

class LRU: public Test{
public:
	LRU(){ };
	LRU(testsetting ts);
	~LRU();

	std::vector<CacheItem> cache;

	void readQuery(std::pair<int,int> query);
    void readStaticQuery(std::pair<int,int> query);
	void readQueries(int numQueries, string inFn);
	void readQueryList(std::vector<std::pair<int,int> > queryList);
	int getCacheHits(){return numCacheHits;}
	int getTotalQueries(){return numTotalQueries;}
	int getTotalDijkstraCalls(){return numDijkstraCalls;}
	int getQueryNumCacheFull(){return queryNumCacheFull;}

private:
	int numTotalQueries;
	int numCacheHits;
	int numDijkstraCalls;
	int queryNumCacheFull;

	int cacheSize;
	int cacheUsed;

	bool useNodeScore;
	bool useHitScore;
	bool cacheFull;

	testsetting ts;

	void checkAndUpdateCache(std::pair<int,int> query);
	void checkStaticCache(std::pair< int, int > query);
	void insertItem(int querySize, std::vector<int> nodesInQueryResult, int sNode, int tNode);

	boost::unordered_map<coordinate, int> coordinate2Nodeid;
	std::vector<std::pair<int, int> > trainingSTPointPairs;
	std::vector<std::pair<int, int> > testSTPointPairs;

    void readTestData(string fn);
	void readTrainingData(string fn);
	void readMapData();
};

#endif