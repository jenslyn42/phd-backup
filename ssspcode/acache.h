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

#ifndef ACACHE_H
#define ACACHE_H

/**
	@author Jeppe Rishede <jenslyn42@gmail.com>
*/

#include "CacheItem.h"
#include "testsetting.h"
#include "utility.h"

#include <boost/foreach.hpp>
#include "boost/unordered_map.hpp"

#include <string>
#include <vector>
#include <iostream>

#define NODE_BITS 32
#define BIT 1
#define GRAPH_CACHE 1
#define LIST_CACHE 2

class aCache{
public:
	aCache(){ };
	aCache(testsetting ts);

	~aCache(){ };

	bool insertItem(CacheItem ci);
	bool checkCache(int s, int t);
	bool checkCache(std::pair<int,int> query);
	bool checkCache(CacheItem ci);
	bool hasEnoughSpace(CacheItem ci);
	int getCacheUsed(){return cacheUsed;}
	int getSize(){return cacheSize;}
	int getNumberOfNodesInCache(){return numberOfNodes;}
	int getNumberOfItemsInCache(){return numItems;}
	vector<CacheItem> getCacheContentVector(){return cache;}

private:
	int cacheType;
	uint cacheSize;
	uint cacheUsed;
	uint numberOfNodes;
	uint numItems;
	vector<int> nodeIdsInCache;
	vector<CacheItem> cache;

	void updateCacheUsed(CacheItem ci);

};

#endif
