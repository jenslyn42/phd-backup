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
#ifndef OSC_CPP
#define OSC_CPP

#include "osc.h"
#define debug false

OSC::OSC(testsetting ts)
{
	cacheSize = ts.getCacheSize();

	nodeHits.reserve(ts.getQueryRangeEnd());
	nodeHits.insert(nodeHits.begin(),ts.getQueryRangeEnd(),0);

	numTotalQueries = 0;
	numCacheHits = 0;
	numDijkstraCalls = 0;
	cacheSize = ts.getCacheSize();
	cacheUsed = 0;
	useNodeScore = ts.isUseNodeScore();
	useHitScore = ts.isUseHitScore();
	testFile = ts.getTestFile();
	testType = ts.getTestType();
}

OSC::~ OSC()
{
}

void OSC::readQuery(std::pair< int, int > query)
{
	if(debug) cout << "one, OSC::readQuery: ("<< query.first <<","<<query.second <<")" <<endl;
	checkAndUpdateCache(query);
	if(debug) cout << "two, OSC::readQuery " <<endl;
	numTotalQueries++;
}

void OSC::readQueryList(std::vector< std::pair<int,int> > queryList)
{
	BOOST_FOREACH(intPair q, queryList )
	{
		readQuery(q);
	}
}

void OSC::checkAndUpdateCache(std::pair< int, int > query)
{
	bool cacheHit = false;
	vector<int> cIt;
	vector<int> itemSubMatch;
	if(debug) cout << "one, OSC::checkAndUpdateCache("<< query.first <<","<<query.second <<")" <<endl;

	BOOST_FOREACH(CacheItem ci, cache)
	{
		cIt = ci.item;
		if(find(cIt.begin(),cIt.end(), query.first) != cIt.end() && find(cIt.begin(),cIt.end(), query.second) != cIt.end())
		{
			if(debug) cout << "two, OSC::checkAndUpdateCache: cIt:" << cIt.size() <<endl;
			numCacheHits++;
			if(numTotalQueries < 0) cout << "ERROR 3 numTotalQueries: "<< numTotalQueries <<endl;

			ci.updateKey(numTotalQueries);
			sort(cache.begin(), cache.end());
			cacheHit = true;
// 			int matchFrom = std::find(cIt.begin(), cIt.end(), query.first) - cIt.begin();
// 			int matchTo = std::find(cIt.begin(), cIt.end(), query.second) - cIt.begin();
			vector<int>::iterator matchFrom = std::find(cIt.begin(), cIt.end(), query.first);
			vector<int>::iterator matchTo = std::find(cIt.begin(), cIt.end(), query.second);

			if(matchFrom < matchTo)
				itemSubMatch.assign(matchFrom, matchTo);
			else
				itemSubMatch.assign(matchTo, matchFrom);

			updateNodeHits(itemSubMatch);
			break;
		}
	}
	if(debug) cout << "three, OSC::checkAndUpdateCache, cacheHit:" << cacheHit <<endl;

	if(!cacheHit)
	{
		if(debug) cout << "four0, OSC::checkAndUpdateCache " << query.first <<"," << query.second <<endl;
		vector<int> spResult  = RoadGraph::mapObject(testFile,testType)->dijkstraSSSP(query.first, query.second);
		numDijkstraCalls++;
		if(debug) cout << "four1, OSC::checkAndUpdateCache" <<endl;
		updateNodeHits(spResult);
		int querySize = spResult.size();
		if(debug) cout << "four2, OSC::checkAndUpdateCache" <<endl;
		vector<int> nodesInQueryResult(spResult);

		if(debug) cout << "four3, OSC::checkAndUpdateCache" <<endl;
		if(cache.size() != 0)
		{
			if(debug) cout << "five1, OSC::checkAndUpdateCache" <<endl;
			testToReplaceItem(querySize, nodesInQueryResult);
			if(debug) cout << "five2, OSC::checkAndUpdateCache" <<endl;
		}else{
			if(numTotalQueries < 0) cout << "ERROR 2 numTotalQueries: "<< numTotalQueries << endl;

			CacheItem cIt (numTotalQueries, nodesInQueryResult);

			if(debug) cout << "six1, OSC::checkAndUpdateCache" <<endl;
			cache.push_back(cIt);
			cacheUsed = cacheUsed + cIt.size;
		}
		if(debug) cout << "seven, OSC::checkAndUpdateCache" <<endl;
	}
}

void OSC::updateNodeHits(std::vector< int > queryResult)
{
	BOOST_FOREACH(int node, queryResult)
	{
		nodeHits[node]++;
	}
}

bool compFunc(int i,int j) {return (i>j);} //support for reverse sort in testToReplaceItem()

void OSC::testToReplaceItem(int querySize, std::vector< int > nodesInQueryResult)
{
	if(debug) cout << "zero, OSC::testToReplaceItem querySize:"<<querySize<<" cacheSize:" << cacheSize <<" cacheUsed: " << cacheUsed <<endl;
	if(querySize > cacheSize) return;

	bool notEnoughSpace = true;
	int qSum = 0;
	int qScore = 0;
	if(debug) cout << "one, OSC::testToReplaceItem" <<endl;
	//calculate score for query
	if(debug) cout << "one1" << endl;
	BOOST_FOREACH(int node, nodesInQueryResult){
		if(nodeHits.at(node) < 0 || qSum < 0) cout << "one2, OSC::testToReplaceItem node: " << node <<" qSum " <<qSum<< endl;
		qSum += nodeHits.at(node);
	}

	if(debug) cout << "one3, qSum: "<< qSum << endl;
	Heap removeCandidate;
	vector<int> ci (cache[0].item);
	int nodes = ci.size();
	int sum = 0;

	if(debug) cout << "two, OSC::testToReplaceItem" <<endl;
	BOOST_FOREACH(int node, ci){
		if(nodeHits.at(node) < 0 || sum < 0) cout << "two2, OSC::testToReplaceItem node: " << node <<" sum: " << sum<< endl;
		sum += nodeHits.at(node);
	}

	//calc score for current query based on test flags
	if(useNodeScore && useHitScore)
	{
		HeapEntry he(0,nodes+sum);
		removeCandidate.push(he);
		qScore = qSum+querySize;
	}
	else if(useNodeScore)
	{
		HeapEntry he (0,nodes);
		removeCandidate.push(he);
		qScore = querySize;
	}
	else if(useHitScore)
	{
		HeapEntry he(0,sum);
		removeCandidate.push(he);
		qScore = qSum;
	}
	else
	{
		cout << "Can't set both useNodeScore & useHitScore to false, setting both true";
		HeapEntry he(0,nodes+sum);
		removeCandidate.push(he);
		qScore = qSum+querySize;
	}

	if(debug) cout << "three, OSC::testToReplaceItem" <<endl;
	/** calculate scores of all items, and place them in min-heap */
	for(uint k=1; k < cache.size(); k++)
	{
		ci = cache[k].item;
		nodes = ci.size();
		sum = 0;

		BOOST_FOREACH(int i, ci){
			if(nodeHits.at(i) < 0 || sum < 0) cout << "three2, OSC::testToReplaceItem i: " << i <<" sum: " << sum<< endl;
			sum += nodeHits.at(i);
		}

		if(useNodeScore && useHitScore)
		{
			HeapEntry he(k,nodes+sum);
			removeCandidate.push(he);
		}
		else if(useNodeScore)
		{
			HeapEntry he (k,nodes);
			removeCandidate.push(he);
		}
		else if(useHitScore)
		{
			HeapEntry he(k,sum);
			removeCandidate.push(he);
		}
		else
		{
			cout << "Can't set both useNodeScore & useHitScore to false, setting both true";
			HeapEntry he(k,nodes+sum);
			removeCandidate.push(he);
		}

	}

	if(debug) cout << "four, OSC::testToReplaceItem" <<endl;
	if(qScore > removeCandidate.top().dist)
	{
		vector<int> removeIndexes;
		int tmpRemIndex = 0;

		do{
			if((cacheSize - cacheUsed) >= querySize)
			{
				//sort reverse order.
				sort(removeIndexes.begin(), removeIndexes.end(), compFunc);

				//remove all cacheItems marked for removal
				BOOST_FOREACH(uint i, removeIndexes)
				{
					if(i >= cache.size()) cout << "THIS WILL NOT WORK!!!" <<endl;
					else cache.erase(cache.begin()+i); //used to have +i-1!!!
				}

				//add new cache item to cache
				if(numTotalQueries <0) cout << "ERROR 1 numTotalQueries: "<< numTotalQueries <<endl;

				CacheItem newCacheItem (numTotalQueries, nodesInQueryResult);
				if(newCacheItem.s < -1 || newCacheItem.t < -1) cout << "ERROR 1 s: "<< newCacheItem.s << " t: " <<newCacheItem.t << endl;
				cache.push_back(newCacheItem);
				cacheUsed = cacheUsed + newCacheItem.size;
				notEnoughSpace = false;
			}else{
				if(!cacheFull){queryNumCacheFull = numTotalQueries; cacheFull = true;}
				HeapEntry tmp = removeCandidate.top();
				removeCandidate.pop();
				tmpRemIndex = tmp.id;
				removeIndexes.push_back(tmpRemIndex);
				cacheUsed -= cache.at(tmpRemIndex).size;
			}
		}while(notEnoughSpace);
	}
}

#endif
