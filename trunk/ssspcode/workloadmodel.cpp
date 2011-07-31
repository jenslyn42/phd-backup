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
// #include "workloadmodel.h"
// 
// workloadmodel::workloadmodel(testsetting ts)
// {
// 	this->ts = ts;
// }
// 
// workloadmodel::~workloadmodel()
// {
// }
// 
// //file on the form: 
// //record_id, S_x, S_y, T_x, T_y.
// bool workloadmodel::readTrainingData(string fn)
// {
// 	int mapSize = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->getMapsize();
// 	std::pair<double, double> firstPair, secondPair;
// 	int firstPnt, secondPnt;
// 	string mapFile = ts.getTestFile();
// 	std::pair<double, double> tmpPair;
// 	string str;
// 	std::vector<string> tokens;
// 
// 	mapFile.replace ((mapFile.size()-4), 4, "node"); //change file extention from .cedge to .cnode
// 	ifstream in_data (mapFile.c_str(), ios::in); //*.cnode file
// 
// 	ifstream trainingData (fn.c_str(), ios::in); //*.train file
// 
// 	fn.replace ((fn.size()-5), 5, "test"); //change file extention from .train to .test
// 	ifstream testData (fn.c_str(), ios::in); //*.test file
// 
// 	//read in the mapping between coordinates and node ids from *.cnode file
// 	if(in_data.is_open())
// 	{
// 		for(int i = 0; i < mapSize+1; i++)
// 			{
// 				getline(in_data, str);
// 				boost::algorithm::split(tokens, str, boost::algorithm::is_space());
// 				tmpPair = std::make_pair(atof(tokens[1].c_str()),atof(tokens[2].c_str()));
// 				coordinate2Nodeid[tmpPair] = atoi(tokens[0].c_str());
// 				nodeid2coordinate[atoi(tokens[0].c_str())];
// 				points.push_back(tmpPair);
// 			}
// 	}
// 	in_data.close();
// 
// 	//find all pairs of nodeids in the training set to have SP done for them. map nodeids to coordinates.
// 	if(trainingData.is_open())
// 	{
// 		while(getline(trainingData, str))
// 		{
// 			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
// 
// 			firstPair = std::make_pair(atof(tokens[1].c_str()),atof(tokens[2].c_str()));
// 			secondPair = std::make_pair(atof(tokens[3].c_str()),atof(tokens[4].c_str()));
// 
// 			firstPnt = coordinate2Nodeid[firstPair];
// 			secondPnt = coordinate2Nodeid[secondPair];
// 			
// 			trainingNodeidsToCoordinates[firstPnt] = firstPair;
// 			trainingNodeidsToCoordinates[secondPnt] = secondPair;
// 			
// 			trainingQueryCoordinates.push_back(firstPair);
// 			trainingQueryCoordinates.push_back(secondPair);
// 			trainingSTPointPairs.push_back(std::make_pair(firstPnt,secondPnt));
// 			trainingSTCoordinatePairs.push_back(std::make_pair(firstPair,secondPair));
// 		}
// 	}
// 	trainingData.close();
// 
// 	//find all pairs of nodeids in the test set to have SP done for them. map nodeids to coordinates.
// 	if(testData.is_open())
// 	{
// 		while(getline(testData, str))
// 		{
// 			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
// 
// 			firstPair = std::make_pair(atof(tokens[1].c_str()),atof(tokens[2].c_str()));
// 			secondPair = std::make_pair(atof(tokens[3].c_str()),atof(tokens[4].c_str()));
// 
// 			firstPnt = coordinate2Nodeid[firstPair];
// 			secondPnt = coordinate2Nodeid[secondPair];
// 			
// 			testNodeidsToCoordinates[firstPnt] = firstPair;
// 			testNodeidsToCoordinates[secondPnt] = secondPair;
// 
// 			testQueryCoordinates.push_back(firstPair);
// 			testQueryCoordinates.push_back(secondPair);
// 			testSTPointPairs.push_back(std::make_pair(firstPnt,secondPnt));
// 			testSTCoordinatePairs.push_back(std::make_pair(firstPair,secondPair));
// 		}
// 	}
// 	testData.close();
// 
// }
// 
// bool xCompfunc(std::pair<double, double> i,std::pair<double, double> j) {return (i.first<j.first);} //sort based on x values
// bool yCompfunc(std::pair<double, double> i,std::pair<double, double> j) {return (i.second<j.second);} //sort based on y values
// 
// bool workloadmodel::makePartitions(int splits)
// {
// 	int axis = 0, j = 0;
// 
// 	double mapXmin,mapXmax,mapYmin,mapYmax;
// 	partitionsMade = pow(2,splits);
// 	vector<region> regionsVector, tmpRegions;
// 	sort(points.begin(), points.end(), xCompfunc);
// 	mapXmin = points.front().first;
// 	mapXmax = points.back().first;
// 	sort(points.begin(), points.end(), yCompfunc);
// 	mapYmin = points.front().second;
// 	mapYmax = points.back().second;
// 
// // 	region reg(0,trainingQueryCoordinates,mapXmin,mapXmax,mapYmin,mapYmax);
// 	region reg(0,points,mapXmin,mapXmax,mapYmin,mapYmax);
// 	regionsVector.push_back(reg);
// 
// 	for(int i=0; i<splits; i++)
// 	{
// 		tmpRegions = split(regionsVector, axis);
// 		regionsVector = tmpRegions;
// 		axis++;
// 	}
// 	
// 	//set the id of each region.
// 	BOOST_FOREACH(region r, regionsVector)
// 	{
// 		r.id = j;
// 		mapRegions[j] = r;
// 		j++;
// 	}
// 
// 	if(!mapRegions.empty()) return true;
// 
// 	return false;
// }
// 
// void workloadmodel::getStatistics(string outFn)
// {  
// 	ofstream statisticsFileRN;
// 	ofstream statisticsFileRRN;
// 	outFn.replace (outFn.size(), 2, "RN");
// 	statisticsFileRN.open((outFn).c_str(), ios::out | ios::ate | ios::app);
// 	outFn.replace ((outFn.size()-2), 3, "RRN");
// 	statisticsFileRRN.open((outFn).c_str(), ios::out | ios::ate | ios::app);	
// 	populateStructures(); //fill in all supporting structures
// 
//  	statisticsFileRN << "Regions: " << partitionsMade << endl;
// 	statisticsFileRRN << "Regions: " << partitionsMade << endl;
// 
// 	std::pair<int,int> emptyRegion (-2,-2);
// 	statisticsFileRN << "Region\t#testN\t#trainN" << endl;
// 	
// 	BOOST_FOREACH(intPairMap::value_type reg, testNtrainingCoordinatesPerRegion)
// 	{
// 		if(reg.second != emptyRegion)
// 			statisticsFileRN << reg.first <<"\t" << reg.second.first <<"\t" << reg.second.second << endl;
// 	}
// 
// 
// 	statisticsFileRRN << "R1\tR2\t#testN\t#trainN" << endl;
// 	pair<int,int> r,num;
// 	BOOST_FOREACH(intPairPairs statistic, testNtrainingCoordinatesPerRegionPair)
// 	{
// 		r = statistic.first;
// 		num = statistic.second;
// 		statisticsFileRRN << r.first << "\t" << r.second << "\t" << num.first << "\t" << num.second << endl; 
// 	}
// 	
// 	statisticsFileRN.close();
// 	statisticsFileRRN.close();
// }
// 
// double workloadmodel::getProbability()
// {
// 	populateProbStructures();
// }
// 
// std::vector<region> workloadmodel::split(std::vector< region > regions, int axis)
// {
// 	std::vector< region > tmpRegions;
// 
// 	regionlist leftdata, rightdata;
// 
// 	BOOST_FOREACH (region reg, regions)
// 	{
// 		if(axis%2)
// 			sort(reg.points.begin(), reg.points.end(), xCompfunc);
// 		else
// 			sort(reg.points.begin(), reg.points.end(), yCompfunc);
// 
// 		region leftReg(axis,leftdata,reg.xmin,reg.xmax,reg.ymin,reg.ymax);
// 		region rightReg(axis,rightdata,reg.xmin,reg.xmax,reg.ymin,reg.ymax);
// 
// 		int size = reg.points.size();
// 		int middle = size/2;
// 		leftdata.resize(middle);
// 		rightdata.resize(size-middle);
// 		copy(reg.points.begin(), reg.points.begin()+middle, leftdata.begin());
// 		copy(reg.points.begin()+middle+1, reg.points.end(), rightdata.begin());
// 
// 
// 		
// 		leftReg.points = leftdata;
// 		rightReg.points = rightdata;
// 
// 		if(axis%2){
// 			leftReg.xmax = (rightdata.front().first)-0.000001;
// 			rightReg.xmin = rightdata.front().first;
// 		}else{
// 			leftReg.ymax = (rightdata.front().second)-0.000001;
// 			rightReg.ymin = rightdata.front().second;
// 		}
// 	
// 		tmpRegions.push_back(leftReg);
// 		tmpRegions.push_back(rightReg);
// 	}
// 
// 	return tmpRegions;	
// }
// 
// 
// int workloadmodel::mapNodeid2RegionId(int nid)
// {
// 	return mapCoordinate2RegionId(nodeid2coordinate.at(nid));
// }
// 
// int workloadmodel::mapCoordinate2RegionId(std::pair< double, double > coord)
// {
// 	region reg;
// 	double x,y;
// 	BOOST_FOREACH(regionMap::value_type r, mapRegions) 
// 	{ 
// 		reg = r.second;
// 		x = coord.first;
// 		y = coord.second;
// 		if(x <= reg.xmax && x >= reg.xmin && y <= reg.ymax && y >= reg.ymin)
// 			return r.first;
// 	}
// 	
//  	return -1;
// }
// 
// void workloadmodel::printRegions(boost::unordered_map<int, region> regions)
// {
// 	ofstream file;
// 	file.open("rec16.txt", ios::out | ios::ate | ios::app);
// 	file << "R_id\txMax\txMax\tyMin\tyMax" << endl;
// 	region r;
// 	BOOST_FOREACH(regionMap::value_type rm, regions)
// 	{
// 		r = rm.second;
// 		file << r.id <<"\t" << r.xmin << "\t" << r.xmax << "\t" << r.ymin << "\t" << r.ymax << endl;
// 	}
// 	file.close();
// }
// 
// void workloadmodel::runQueries()
// {
// 	
// }
// 
// void workloadmodel::runQueries(int queries)
// {
// }
// 
// void workloadmodel::populateStructures()
// {
// 	boost::unordered_map<int,int> trainingCoordinatesPerRegion;
// 	boost::unordered_map<int,int> testCoordinatesPerRegion;
// 
// 
//  	BOOST_FOREACH(coordinate coord, trainingQueryCoordinates)
// 	{
// 		int rid = mapCoordinate2RegionId(coord);
// 		trainingCoordinatesPerRegion[rid] = trainingCoordinatesPerRegion[rid] +1;
// 	}
// 
//  	BOOST_FOREACH(coordinate coord, testQueryCoordinates)
// 	{
// 		int rid = mapCoordinate2RegionId(coord);
// 		testCoordinatesPerRegion[rid] = testCoordinatesPerRegion[rid] +1;
// 	}
// 
// 	//start populating testNtrainingCoordinatesPerRegion so that I may write out "Region\t#testN\t#trainN" to file
// 	BOOST_FOREACH(regionMap::value_type rm, mapRegions)
// 	{
// 		testNtrainingCoordinatesPerRegion[rm.first] = std::make_pair<int,int>(-2,-2);
// 	}
//  	testNtrainingCoordinatesPerRegion[-1] = std::make_pair<int,int>(-2,-2); //needed until I figure out how to partition correctly..
// 
// 	BOOST_FOREACH(unorderedIntMap::value_type reg, trainingCoordinatesPerRegion)
// 	{
// 		(testNtrainingCoordinatesPerRegion.at(reg.first)).first = reg.second;
// 	}
// 
// 	BOOST_FOREACH(unorderedIntMap::value_type reg, testCoordinatesPerRegion)
// 	{
// 		(testNtrainingCoordinatesPerRegion.at(reg.first)).second = reg.second;
// 	}
// 
// 
// 	//start populating testNtrainingCoordinatesPerRegionPair so that I may write out "R1\tR2\t#testN\t#trainN" to file
// 	int r1,r2,tmp;
// 	
// 	BOOST_FOREACH(coordinatePairs c, testSTCoordinatePairs)
// 	{
// 		r1 = mapCoordinate2RegionId(c.first);
// 		r2 = mapCoordinate2RegionId(c.second);
// 		if(r1 > r2){tmp = r1; r1 = r2; r2 = tmp;}
// 		
// 		if(testNtrainingCoordinatesPerRegionPair.find(std::make_pair<int,int>(r1,r2)) == testNtrainingCoordinatesPerRegionPair.end())
// 			testNtrainingCoordinatesPerRegionPair[std::make_pair<int,int>(r1,r2)] = std::make_pair(1,0);
// 		else
// 			(testNtrainingCoordinatesPerRegionPair.at(std::make_pair<int,int>(r1,r2))).first += 1;
// 	}
// 
// 	BOOST_FOREACH(coordinatePairs c, trainingSTCoordinatePairs)
// 	{
// 		r1 = mapCoordinate2RegionId(c.first);
// 		r2 = mapCoordinate2RegionId(c.second);
// 		if(r1 > r2){tmp = r1; r1 = r2; r2 = tmp;}
// 		
// 		if(testNtrainingCoordinatesPerRegionPair.find(std::make_pair<int,int>(r1,r2)) == testNtrainingCoordinatesPerRegionPair.end())
// 			testNtrainingCoordinatesPerRegionPair[std::make_pair<int,int>(r1,r2)] = std::make_pair(0,1);
// 		else
// 			(testNtrainingCoordinatesPerRegionPair.at(std::make_pair<int,int>(r1,r2))).second += 1;
// 	}
// }
// 
// void workloadmodel::populateProbStructures()
// {
// 	int i=0, r1, r2, temp;
// 	double tmpScore;
// 	vector<int> spResult;
// 	vector<vector<int> > trainingSPresults, testSPresults;
// 	boost::unordered_map<int,int> trainingCoordinatesPerRegion;
// 	boost::unordered_map<int,int> testCoordinatesPerRegion;
// 	pair<int,int> tmpPair;
// 	boost::unordered_map<int, CacheItem> tmpTestCache, tmpTrainCache;
// 
// 	HeapEntry tmp, tmp2;
// 	CacheItem tmpItem;
// 
// 	boost::unordered_map<intPair, int > subTestPointPairsSeen;
// 	boost::unordered_map<intPair, int > subTrainingPointPairsSeen;
// 	int totalTrainingPairsSeen=0, totalTestPairsSeen=0;
// 
// 	BOOST_FOREACH(intPair c, testSTPointPairs)
// 	{		
// 		spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(c.first, c.second);
// 		testSPresults.push_back(spResult);
// 
// 		BOOST_FOREACH(int v1, spResult)
// 		{
// 			BOOST_FOREACH(int v2, spResult)
// 			{
// 				if(v1 != v2 && v1 < v2)
// 				{
// 					subTestPointPairsSeen[std::make_pair(v1,v2)] =+ 1;
// 					totalTestPairsSeen++;
// 					subTestQueriesPerRegionPair[std::make_pair<int,int>(mapNodeid2RegionId(v1),mapNodeid2RegionId(v2))] =+ 1;
// 				}
// 			}
// 		}
// 	}
// 
// 
// 	BOOST_FOREACH(intPair c, trainingSTPointPairs)
// 	{
// 		spResult = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(c.first, c.second);
// 		trainingSPresults.push_back(spResult);
// 
// 		BOOST_FOREACH(int v1, spResult)
// 		{
// 			BOOST_FOREACH(int v2, spResult)
// 			{
// 				if(v1 != v2 && v1 < v2)
// 				{
// 					subTrainingPointPairsSeen[std::make_pair(v1,v2)] =+ 1;
// 					totalTrainingPairsSeen++;
// 					subTrainingQueriesPerRegionPair[std::make_pair<int,int>(mapNodeid2RegionId(v1),mapNodeid2RegionId(v2))] =+ 1;
// 				} 
// 			}
// 		}
// 	}
// 
// 	
// 	//calc scores per coordinate pair
// 	BOOST_FOREACH(vector<int> spResult, trainingSPresults)
// 	{
// 		CacheItem e (i++, spResult,  spResult.front(), spResult.back());
// 
// 		//add cache item to map
// 		trainingSPpaths[i] = e;
// 		tmp.id = i;
// 		tmp.dist = calcScore(spResult, subTrainingPointPairsSeen, totalTrainingPairsSeen);
// 		mhTrain.push(tmp);
// 	}
// 
// 	i = 0;
// 	BOOST_FOREACH(vector<int> spResult, testSPresults)
// 	{		
// 		CacheItem e (i++, spResult, spResult.front(), spResult.back());
// 
// 		//add cache item to map
// 		testSPpaths[i] = e;
// 		tmp.id = i;
// 		tmp.dist = calcScore(spResult, subTestPointPairsSeen, totalTestPairsSeen);
// 		mhTest.push(tmp);
// 	}
// }
// 
// double workloadmodel::calcScore(vector<int> spResult, boost::unordered_map<intPair, int > pointPairsSeen, int totalPairsSeen)
// {
// 	double score = 0.0;
// 
// 	BOOST_FOREACH(int v1, spResult)
// 		BOOST_FOREACH(int v2, spResult)
// 			if(v1 != v2 && v1 < v2)
// 				score =+ (double) pointPairsSeen.at(std::make_pair(v1,v2));
// 
// 	return score / totalPairsSeen;
// }
// 
// void workloadmodel::fillCache()
// {
// 	vector<int> tmpSP;
// 	int score;
// 	maxHeap mhCache;
// 	HeapEntry tmp;	
// 
// 	BOOST_FOREACH(intCacheitemMap::value_type icm, trainingSPpaths)
// 	{
// 		score = 0;
// 		tmpSP = icm.second.item;
// 		
// 		BOOST_FOREACH(int nid1, tmpSP)
// 		{
// 			BOOST_FOREACH(int nid2, tmpSP)
// 			{
// 				score =+ subTrainingQueriesPerRegionPair.at(std::make_pair(nid1,nid2));
// 			}		
// 		}
// 
// 		tmp.id = icm.first;
// 		tmp.dist = (double) score;
// 		mhCache.push(tmp);	
// 	}
// 
// 	
// }