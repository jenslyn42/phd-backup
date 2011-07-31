// /****************************************************************************************
//  *   Copyright (C) 2011 by Jeppe Rishede 						*
//  *   jenslyn42@gmail.com								*
//  *											*
//  *   All rights reserved.								*
//  *											*
//  *   Redistribution and use in source and binary forms, with or without 		*
//  *   modification, are permitted provided that the following conditions are met:	*
//  *   Redistributions of source code must retain the above copyright notice,		*
//  *   this list of conditions and the following disclaimer. 				*
//  *   Redistributions in binary form must reproduce the above copyright notice,		*
//  *   this list of conditions and the following disclaimer in the documentation		*
//  *   and/or other materials provided with the distribution. 				*
//  *   Neither the name of the <ORGANIZATION> nor the names of its contributors 		*
//  *   may be used to endorse or promote products derived from this software 		*
//  *   without specific prior written permission						*
//  *                                                                         		*
//  *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   		*
//  *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     		*
//  *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 		*
//  *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER		*
//  *   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 		*
//  *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   		*
//  *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    		*
//  *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 		*
//  *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  		*
//  *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    		*
//  *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          		*
//  ***************************************************************************************/
// #ifndef WORKLOADMODEL_H
// #define WORKLOADMODEL_H
// 
// #include "stdio.h"
// #include <math.h>
// 
// #include "testsetting.h"
// #include "RoadGraph.h"
// #include "CacheItem.h"
// 
// #include "boost/unordered_map.hpp"
// 
// typedef std::pair<double, double> coordinate;
// typedef std::vector<coordinate > regionlist;
// typedef std::pair<int,int> intPair;
// typedef std::pair<coordinate, coordinate> coordinatePairs;
// typedef std::pair<intPair, intPair > intPairPairs;
// typedef boost::unordered_map<int, region> regionMap;
// typedef boost::unordered_map<int, intPair > intPairMap;
// typedef boost::unordered_map<intPair,int > pairIntMap;
// typedef boost::unordered_map<int, CacheItem> intCacheitemMap;
// 
// /**
// 	@author Jeppe Rishede <jenslyn42@gmail.com>
// */
// 
// /**
// Parameters needed:
// - test / training filename (can assume they just differ on extension)
// - map filename
// - filename for result file after running runQueries(..)
// - filename for statistic (different output files can just get different endings and/or extensions)
// -* filename if regions are to be printed (and maybe also which set of regions)
// -* number of queries
// */
// 
// 
// 
// class workloadmodel{
// public:
// 	workloadmodel(testsetting ts);
// 	~workloadmodel();
// 
// 	bool readTrainingData(string fn);
// 	bool makePartitions(int splits);
// 	void getStatistics(string outFn);
// 	double getProbability();
// 	void runQueries();
// 	void runQueries(int queries);
// 
// private:
// 	testsetting ts;
// 	boost::unordered_map<coordinate, int> coordinate2Nodeid;
// 	boost::unordered_map<int, coordinate> nodeid2coordinate;
// 	regionlist trainingQueryCoordinates; //holds all coordinates from training queries
// 	regionlist testQueryCoordinates; //holds all coordinates from test queries
// 	regionlist points; //holds all coordinates from the map
// 	boost::unordered_map<int, region> mapRegions; //holds all regions after a call to makePartitions()
// 
// 	int partitionsMade;
// 	boost::unordered_map<int, std::pair<double,double > > trainingNodeidsToCoordinates;
// 	boost::unordered_map<int, std::pair<double,double > > testNodeidsToCoordinates;
// 	std::vector<std::pair<int, int> > trainingSTPointPairs;
// 	std::vector<std::pair<int, int> > testSTPointPairs;
// 	std::vector<std::pair<coordinate, coordinate> > trainingSTCoordinatePairs;
// 	std::vector<std::pair<coordinate, coordinate> > testSTCoordinatePairs;
// 
// 	boost::unordered_map<int,std::pair<int,int> > testNtrainingCoordinatesPerRegion;
// 	//hold how often the two end regions of a SP query pair are seen for training and test data.
// 	boost::unordered_map<std::pair<int,int> ,std::pair<int,int> > testNtrainingCoordinatesPerRegionPair;
// 
// 	//same as above, but holds statistics for all subqueries too.
// 	boost::unordered_map<int, std::pair<int,int> > subTestNtrainingCoordinatesPerRegion;
// 	boost::unordered_map<std::pair<int,int>, std::pair<int,int> > subTestNtrainingCoordinatesPerRegionPair;
// 
// 	boost::unordered_map<std::pair<int,int> , int> subTestQueriesPerRegionPair;
// 	boost::unordered_map<std::pair<int,int> , int> subTrainingQueriesPerRegionPair;
// 
// 	boost::unordered_map<int, CacheItem> testSPpaths;
// 	boost::unordered_map<int, CacheItem> trainingSPpaths;
// 	maxHeap mhTrain, mhTest; //a heap to keep track of the score of each potential cache item
// 
// 	int mapCoordinate2RegionId(std::pair<double,double> coord);
// 	int mapNodeid2RegionId(int nid);
// 	std::vector<region> split(std::vector< region > regions, int axis);
// 	void printRegions(boost::unordered_map<int, region> regions);
// 	void populateStructures();
// 	void populateProbStructures();
// 	double calcScore(vector<int> spRes, boost::unordered_map<intPair, int > pointPairsSeen, int totalPairsSeen);
// 	void fillCache();
// };
// 
// #endif
