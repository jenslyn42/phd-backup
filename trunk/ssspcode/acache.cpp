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

#include "acache.h"

aCache::aCache(testsetting ts)
{
	if(ts.cacheType == GRAPH_CACHE || ts.cacheType == LIST_CACHE)
	{
		cacheType = ts.cacheType;
		cacheSize = ts.getCacheSize();
		cacheUsed = 0;
		numberOfNodes = 0;
		numItems = 0;
	}
	else
		cout << "invalid cache type set: " << ts.cacheType << endl;
}

bool aCache::insertItem(CacheItem ci)
{
	if(!hasEnoughSpace(ci)) return false;

	numItems++;

	cache.push_back(ci);
	updateCacheUsed(ci);
	return true;
}

bool aCache::insertItemWithScore(CacheItem ci, double score)
{
	ci.setScore(score);
	insertItem(ci);
}

bool aCache::checkCache(int s, int t)
{
	vector<int> cItem;

	BOOST_FOREACH(CacheItem ci, cache )
	{
		cItem = ci.item;
		if(find(cItem.begin(),cItem.end(), s) != cItem.end() && find(cItem.begin(),cItem.end(), t) != cItem.end())
			return true;
	}
	return false;
}

bool aCache::checkCache(std::pair< int, int > query)
{
	return checkCache(query.first, query.second);
}

bool aCache::checkCache(CacheItem ci)
{
	return checkCache(ci.s, ci.t);
}

//assumes cache item ci has NOT been added to vector<CacheItem> cache
bool aCache::hasEnoughSpace(CacheItem ci)
{
	if(cacheType == GRAPH_CACHE)
	{
		int newNodes = 0; //nodes in ci which is not already in graph

		BOOST_FOREACH(int v, ci.item)
			if(nodeIdsInCache.find(v) == nodeIdsInCache.end()){	newNodes++;	}

		if( (cacheUsed+nodeIdsInCache.size()*BIT) + (newNodes*NODE_BITS+(cache.size()+1)*newNodes*BIT) < cacheSize) return true;
	}
	else if(cacheType == LIST_CACHE)
	{
		if(cacheUsed + ci.size*NODE_BITS < cacheSize) return true;
	}else
		std::cout << "aCache::hasEnoughSpace! Invalid cache type set: " << cacheType << endl;

	return false;
}

//assumes cache item ci has already been added to vector<CacheItem> cache
void aCache::updateCacheUsed(CacheItem ci)
{
	if(cacheType == GRAPH_CACHE)
	{
		int nodesToBeAdded = 0;
		boost::dynamic_bitset<> bitset(0);

		BOOST_FOREACH(int v, ci.item)
		{
			if(nodeIdsInCache.find(v) == nodeIdsInCache.end())
			{
				boost::dynamic_bitset<> bitset(cache.size()-1); //set all bits to zero in the bitmap for the first cache.size()-1 bits
				nodesToBeAdded++;
				nodeIdsInCache[v] = bitset;
			}
		}

		BOOST_FOREACH(intDBitset::value_type nb, nodeIdsInCache)
		{
			if(find(ci.item.begin(), ci.item.end(), nb.first) != ci.item.end())
				nodeIdsInCache.at(nb.first).push_back(1);
			else
				nodeIdsInCache.at(nb.first).push_back(0);
		}

		cacheUsed =  nodeIdsInCache.size() * (NODE_BITS + BIT*cache.size()) ;
		numberOfNodes = nodeIdsInCache.size();
	}
	else if(cacheType == LIST_CACHE)
	{
		cacheUsed = cacheUsed + ci.size*NODE_BITS;
		numberOfNodes = numberOfNodes + ci.size;
	}else
		std::cout << "aCache::hasEnoughSpace! Invalid cache type set: " << cacheType << endl;
}

void aCache::writeOutBitmaps()
{
	int nodeid=0;
	BOOST_FOREACH(intDBitset::value_type nb, nodeIdsInCache)
	{
		nodeid = nb.first;
		cout << nb.first << " " << nb.second << endl;
	}
	cout << nodeIdsInCache.size() << endl;
	cout << nodeIdsInCache.at(nodeid).size() << endl;
	cout << nodeIdsInCache.size()*nodeIdsInCache.at(nodeid).size() << endl;
}

string aCache::getScoreAndContent()
{

}
