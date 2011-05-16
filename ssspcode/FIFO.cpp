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
#include "FIFO.h"
#define debug false

typedef std::pair<int,int> intPair;

FIFO::FIFO(testsetting ts)
{
	this->ts = ts;
	cacheSize = ts.getCacheSize();
	numTotalQueries = 0;
	numCacheHits = 0;
	cacheSize = ts.getCacheSize();
	cacheUsed = 0;
	useNodeScore = ts.isUseHitScore();
	useHitScore = ts.isUseNodeScore();
	numDijkstraCalls = 0;
	queryNumCacheFull = -1;
	cacheFull = false;
}

FIFO::~FIFO()
{
}

void FIFO::readQuery(std::pair< int, int > query)
{
	checkAndUpdateCache(query);
	numTotalQueries++;
}

void FIFO::readQueryList(std::vector< std::pair < int , int > > queryList)
{
	BOOST_FOREACH(intPair q, queryList )
	{
		readQuery(q);
	}
}

void FIFO::checkAndUpdateCache(std::pair< int, int > query)
{
	bool cacheHit = false;
	int ii = 0;
	if(debug) cout << "one, fifo::checkAndUpdateCache! " << ii << endl;
	if((int) cache.size() != 0)
	{	
		if(ts.isUseOptimalSubstructure()){
			if(debug) cout << "one1, fifo::checkAndUpdateCache! :cacheSize:" << (int) cache.size() <<"::"<< endl;
			BOOST_FOREACH(CacheItem ci, cache )
			{
				if(find(ci.item.begin(),ci.item.end(), query.first) != ci.item.end() && find(ci.item.begin(),ci.item.end(), query.second) != ci.item.end())
				{
					numCacheHits++;
					cacheHit = true;
					break;
				}
			}
		}else{
			BOOST_FOREACH(CacheItem ci, cache )
			{
				if(debug) cout << "three, fifo::checkAndUpdateCache! " << ++ii  << endl;
				if(query.first == ci.s && query.second == ci.t)
				{
					numCacheHits++;
					cacheHit = true;
					break;
				}
			}
		}
		if(debug) cout << "four, fifo::checkAndUpdateCache!" << endl;
	}
	if(debug) cout << "five, fifo::checkAndUpdateCache! cacheHit: " << cacheHit << endl;
	if(!cacheHit)
	{
		if(debug) cout << "six, fifo::checkAndUpdateCache!" << endl;
		vector<int> spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(query.first, query.second);
		numDijkstraCalls++;
		if(debug) cout << "seven, fifo::checkAndUpdateCache!" << endl;
		int querySize = spResult.size();
		if(cache.size() != 0){
			insertItem(querySize, spResult, query.first, query.second);
		}else{
			CacheItem e (numTotalQueries, spResult, query.first, query.second);
			cache.push_back(e);
		}
	}
	if(debug) cout << "eight, fifo::checkAndUpdateCache!" << endl;
}

void FIFO::insertItem(int querySize, std::vector< int > nodesInQueryResult, int sNode, int tNode)
{
	bool notEnoughSpace = true;

	//insert query into cache, will repeatedly remove items until there is enough space for the new item.
	do{
		if((cacheSize - cacheUsed) > querySize)
		{
			CacheItem cItem (numTotalQueries, nodesInQueryResult, sNode, tNode);
			cache.push_back(cItem);
			cacheUsed = cacheUsed + cItem.size;
			notEnoughSpace = false;
		}
		else if(querySize < cacheSize)
		{
			if(!cacheFull){queryNumCacheFull = numTotalQueries; cacheFull = true;}
			cacheUsed = cacheUsed - cache.back().size;
			cache.pop_back();
		}
		else
			break;
	}while(notEnoughSpace);
}