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

#include "CacheItem.h"
#include "testsetting.h"
#include "Test.h"
#include "RoadGraph.h"
#include "utility.h"
#include "acache.h"

#include <boost/foreach.hpp>
#include "boost/unordered_map.hpp"
#include <boost/lexical_cast.hpp>

#include <string>
#include <vector>
#include <fstream>
#include "stdio.h"
#include <math.h>
#include <iostream>
#include <ctime>

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
	
	void readQuery(std::pair<int,int> query);
	void readQueryList(std::vector< std::pair<int,int> > queryList);
	int getCacheHits(){return numCacheHits;}
	int getTotalQueries(){return numTotalQueries;}
	int getTotalDijkstraCalls(){return numDijkstraCalls;}
	int getQueryNumCacheFull(){return queryNumCacheFull;}

	void readQueries(int numQueries, string inFn);

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
	int mapSize;
	int calcScoreCounter;
	boost::unordered_map<int, int> calcScoreMap;	

	vector<pair<int,int> > queries;

	double startTime,endTime; 
	int** trainingQueriesPerRegionPair;
	

	boost::unordered_map<coordinate, int> coordinate2Nodeid;
	boost::unordered_map<int, coordinate> nodeid2coordinate;
	regionlist points; //holds all coordinates from the map
	boost::unordered_map<int, region> mapRegions; //holds all regions after a call to makePartitions()
 
	int partitionsMade;
	std::vector<std::pair<int, int> > trainingSTPointPairs;
	std::vector<std::pair<int, int> > testSTPointPairs;

	boost::unordered_map<intPair, int > subTrainingPointPairsSeen;
	int totalTrainingPairsSeen;

	void checkCache(std::pair<int,int> query);
	intPair findLargestQuery(vector<pair<int, int> > cCandidates, int k);
	double calcScore(vector<int> sp, boost::unordered_map<pair<int, int>, int> vSeen);
	void statistics(string outFn);

	bool readMapData();
	bool readData(string fn);
	bool readTestData(string fn);
	bool readTrainingData(string fn);
	bool makePartitions(int splits);
	std::vector<region> split(std::vector< region > regions, int axis);
	int mapNodeid2RegionId(int nid);
	int mapCoordinate2RegionId(std::pair< double, double > coord);
	void populateProbStructures();
	void fillCache(int numQueries, string inFn);
	void fillCacheFromQueriesFile(int numQueries, string inFn);
	void fillCacheFromTrainFile(int numQueries);
	void writeoutCacheCoordinates(string testbasename, std::vector<CacheItem> cm, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits);
	int writeoutTestCoordinates(string testbasename, std::vector<intPair > stPointPairs, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits);
	int writeoutTrainingCoordinates(string testbasename, std::vector<intPair > stPointPairs, boost::unordered_map<int, coordinate> nodeid2coordinate, int numSplits);
};

#endif