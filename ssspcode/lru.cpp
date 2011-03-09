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
#include "lru.h"
#define debug false

typedef std::pair<int , int> intPair;

LRU::LRU(testsetting ts)
{
	this->ts = ts;
	cacheSize = ts.getCacheSize();
	numTotalQueries = 0;
	numCacheHits = 0;
	cacheUsed = 0;
	numDijkstraCalls = 0;

	useNodeScore = ts.isUseNodeScore();
	useHitScore = ts.isUseHitScore();
}

LRU::~ LRU()
{
}

void LRU::readQuery(std::pair< int, int > query)
{
	checkAndUpdateCache(query);
	numTotalQueries++;
}

void LRU::readQueryList(std::vector< std::pair<int,int> > queryList)
{
	BOOST_FOREACH(intPair q, queryList )
	{
		readQuery(q);
	}
}

void LRU::checkAndUpdateCache(std::pair< int, int > query)
{
	bool cacheHit = false;

	if(ts.isUseOptimalSubstructure()){
		BOOST_FOREACH(CacheItem ci, cache )
		{
			if(find(ci.item.begin(),ci.item.end(), query.first) != ci.item.end()) 
				if(debug) cout << "LRU::checkAndUpdateCache FIRST TRUE" << endl;
			if(find(ci.item.begin(),ci.item.end(), query.second) != ci.item.end())
				if(debug) cout << "LRU::checkAndUpdateCache SECOND TRUE" << endl;

			if(find(ci.item.begin(),ci.item.end(), query.first) != ci.item.end() && find(ci.item.begin(),ci.item.end(), query.second) != ci.item.end())
			{
				if(debug) cout << "LRU::checkAndUpdateCache BOTH TRUE" << endl;
				numCacheHits++;
				ci.updateKey(numTotalQueries);
				sort(cache.begin(), cache.end());
				cacheHit = true;
				break;
			}
		}
		if(debug) cout << "LRU::checkAndUpdateCache " << endl;
	}else{
		BOOST_FOREACH(CacheItem ci, cache )
		{
			if(query.first == ci.s && query.second == ci.t)
			{
				numCacheHits++;
				ci.updateKey(numTotalQueries);
				sort( cache.begin(), cache.end() );
				cacheHit = true;
				break;
			}
		}
	}

	if(!cacheHit)
	{
		vector<int> spResult  = RoadGraph::mapObject(ts.getTestFile())->dijkstraSSSP(query.first, query.second);
		numDijkstraCalls++;
		int querySize = spResult.size();
		if(cache.size() != 0){
			if(debug) cout << "LRU::checkAndUpdateCache 1, querySize: "<<querySize << endl;
			insertItem(querySize, spResult, query.first, query.second);
		}else{
			if(debug) cout << "LRU::checkAndUpdateCache 2, querySize: "<<querySize << endl;
			CacheItem e (numTotalQueries, spResult, query.first, query.second);
			cache.push_back(e);
		}
		if(debug) cout << "LRU::checkAndUpdateCache 3" << endl;
	}
}

void LRU::insertItem(int querySize, std::vector< int > nodesInQueryResult, int sNode, int tNode)
{
	bool notEnoughSpace = true;
	if(debug) cout << "one, LRU::insertItem(" <<querySize <<"," <<nodesInQueryResult.size() <<","<<sNode<<","<<tNode<<")" << endl;
	//insert query into cache, will repeatedly remove items until there is enough space for the new item.
	do{
		if((cacheSize - cacheUsed) > querySize)
		{
			if(debug) cout << "two1, LRU::insertItem cacheSize,cacheUsed " << cacheSize <<"," << cacheUsed <<endl;
			CacheItem cItem (numTotalQueries, nodesInQueryResult, sNode, tNode);
			cache.push_back(cItem);
			cacheUsed = cacheUsed + cItem.size;
			notEnoughSpace = false;
			if(debug) cout << "two2, LRU::insertItem cacheSize,cacheUsed " << cacheSize <<"," << cacheUsed <<endl;
		}
		else if(querySize < cacheSize)
		{
			if(debug) cout << "three1, LRU::insertItem" << cache.size() <<"," << cache[0].size <<endl;
			if(debug) cout << "querySize,cacheSize,cacheUsed:" << querySize <<"," << cacheSize <<"," << cacheUsed << endl;
			int itemSize = cache[0].size;
			cache.erase(cache.begin());
			cacheUsed = cacheUsed - itemSize;
			if(debug) cout << "three2, LRU::insertItem" <<endl;
		}
		else
			break;
	}while(notEnoughSpace);
	if(debug) cout << "four, LRU::insertItem" <<endl;
}


