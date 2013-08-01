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
 
 
 
//////// CH codes and headers ///////////////////////////////////////


/* C Lib */
#include <stdlib.h>

/* C++ Lib */
using namespace std;
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>

#include "packCH/config.h"
#include "packCH/utils.h"

Counter counter;


/* Linux Lib */
#include <unistd.h>

/* Headers */ 
#include "packCH/Command.h"

/* 
	These two must be placed first as they include necessary typedef 
	statementm like NodeID, EdgeID etc.
*/
#include "packCH/graphIO.h"
#include "packCH/UpdateableGraph.h"
#include "packCH/SearchGraph.h"
#include "packCH/DijkstraCH.h"
#include "packCH/ConstructCH.h"
#include "packCH/Construct.h"

//////////////////////////////////////////////////////


#define debug false

#include "mysql.h"
#include "Setting.h"
#include "RoadGraph.h"
#include "Cache.h"

#include "Competitors.h"
#include "Probcache.h"
#include "ProtoStudy.h"




class TestObject {
  public:
    TestObject(TestSetting settings);
	
    ~TestObject() {
      delete test;
    };
	  
   void runStaticTest();
   void printResults();

   AbstractCache *test;

  private:
    TestSetting ts;
    clock_t start,end;
};



TestObject::TestObject(TestSetting settings) {	
  ts = settings;
	  
  cout << "TestObject:: constructor: " << MatchEnumString(ALGO_ENUM,ts.testAlgo) << " test choosen" <<endl;
	  
  switch( ts.testAlgo ){
    case ALGO_NONE:
      ts.cacheSize = 0;// a special case with 0 cacheSize
      test = new LRU(ts);
      break;
    case ALGO_LRU:
      test = new LRU(ts);
      break;
    case ALGO_SCACHE:
      test = new Scache(ts);
      break;
    case ALGO_SPC:
    case ALGO_SPCplus:
    case ALGO_SPCstar:
      test = new Probcache(ts);
      break;
    case ALGO_HQF:
      test = new HQF(ts);
      break;
    case ALGO_HQFLRU:
      test = new HybridHQFLRU(ts);
      break;
    case ALGO_ORACLE:
      test = new Oracle(ts);
      break;
  }
}

void TestObject::runStaticTest() {
  srand(0);srand48(0);// fix the random seed

  if (debug) cout << "TestObject::runStaticTest: static test started" <<endl;
  
  test->buildCache();
  if (debug) cout << "TestObject::runStaticTest: queries read" <<endl;

  start = clock();
  test->runQueryList();
  end = clock();

  if (debug) cout << "TestObject::runStaticTest: static test ended" <<endl;
  printResults();

  RoadGraph::mapObject(ts)->resetRoadGraph();
}

void TestObject::printResults() {
  ///Console output

  ts = test->ts;

  unsigned long numNodeVisits = RoadGraph::mapObject(ts)->numNodeVisits;
  int ssspCalls = RoadGraph::mapObject(ts)->ssspCalls;

  cout << "\n\n--------------------------" << endl;
  cout << "QueryTime:\t" << (double(end-start))/CLOCKS_PER_SEC << " sec" << endl;
  cout << "CacheHits:\t" << test->getCacheHits() << "(" << test->getTotalDijkstraCalls() << ")" << endl;
  cout << "SPcalls:\t" << ssspCalls << endl;
  cout << "NodesVisited:\t" << numNodeVisits << endl;

  //cout << "Class:\t" << typeid(*test).name() <<  endl;
  cout << "Algorithm:\t" << ts.testAlgo << " " << MatchEnumString(ALGO_ENUM,ts.testAlgo) << endl;
  cout << "Storage:\t" << ts.testStorage << " " << MatchEnumString(STORAGE_ENUM,ts.testStorage) << endl;
  cout << "Scenario:\t" << ts.testScenario << " " << MatchEnumString(ARCH_ENUM,ts.testScenario) << endl;
  cout << "SP calc:\t" << ts.testSPtype << " " << MatchEnumString(SPTYPE_ENUM, ts.testSPtype) << endl;

  cout << "CacheSize:\t" << ts.cacheSize << endl;
  cout << "CacheItems:\t" << ts.getItemsInCache() << endl;
  cout << "Avg. ItemLength:\t" << ts.getAvgItemLength() << endl;

  cout << "Splits:\t" << ts.getSplits() << endl;
  cout << "QueryFile:\t" << ts.queryFileName << endl;

  cout << "NonEmptyRegions:\t" << ts.getNonEmptyRegionPairs() << endl;
  cout << "CalcStatTime:\t" << ts.getBuildStatisticsTime() << " sec" <<endl;
  cout << "FillCacheTime:\t" << ts.getFillCacheTime() << " sec" << endl;
  cout << "--------------------------\n\n" << endl;


  bool fileExist = false;
  ifstream fin((ts.getTestName()).c_str());
  if (fin)
    fileExist = true;// check to see if file exists
  fin.close();

  ///file output
  ofstream resultfile;
  resultfile.open((ts.getTestName()).c_str(), ios::out | ios::ate | ios::app);
  if(!fileExist){
    resultfile << "QueryTime\tCacheHits\tDijkstraCalls\tSPcalls\tNodesVisited\t"
    << "Algorithm\tScenario\tCacheSize\tCacheItems\tAvg.ItemLength\tSplits\tQueryFile\t"
    << "NonEmptyRegions\tCalcStatTime\tFillCacheTime\tuseConcisepath\tmeasureConcisepathdegrees\tbitsInCache\tSPcalc\texecTrainWL" << endl;
  }

  // note: "typeid(*test).name()" no longer used
  resultfile << (double(end-start))/CLOCKS_PER_SEC << "\t" 
  << test->getCacheHits() << "\t" 
  << test->getTotalDijkstraCalls() << "\t" 
  << ssspCalls << "\t" 
  << numNodeVisits << "\t" 

  << MatchEnumString(ALGO_ENUM,ts.testAlgo)  << "\t" 
  << MatchEnumString(ARCH_ENUM,ts.testScenario) << "\t" 

  << ts.cacheSize << "\t" 
  << ts.getItemsInCache() << "\t"
  << ts.getAvgItemLength() << "\t"
  << ts.getSplits() << "\t" 
  << ts.queryFileName << "\t"

  << ts.getNonEmptyRegionPairs() << "\t" 
  << ts.getBuildStatisticsTime() << "\t"
  << ts.getFillCacheTime() << "\t"

  << ts.useConcisepath << "\t"
  << ts.measureConcisepathdegrees << "\t"
  << ts.getUnusedCacheBits() << "\t"
  << MatchEnumString(SPTYPE_ENUM, ts.testSPtype) << "\t"
  << ts.executeTrainingWorkload << endl;

  resultfile.close();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int calcAVGpathlengthInCache(std::string fn) {
	int totallength=0, numpaths=0;
	std::string str;
	std::vector<std::string> tokens;

	std::ifstream cacheContent (fn.c_str(), std::ios::in);
	if(cacheContent.is_open()) {
		while(getline(cacheContent, str)) {
		if(!str.empty())
			totallength++;
		else
			numpaths++;
		}
		cacheContent.close();
	}
	return totallength/numpaths;
}

void extractTestParameters(TestSetting& ts) {
	ts.testFile = ts.getConfigString("testFile");
	ts.testName = ts.getConfigString("testName");
	ts.queryFileName = ts.getConfigString("queryFileName");
	
	//1:graph_large, 2: ppi.dat, 3:*.cedge
	ts.inputFileType = ts.getConfigInt("inputFileType");
	ts.splits = ts.getConfigInt("splits");	// for Probcache (SPC)
	ts.scacheQueryType = ts.getConfigInt("scacheQueryType");	// for SCACHE
	ts.executeTrainingWorkload = ts.getConfigBool("executeTrainingWorkload");
	
	ts.cacheSize = ts.getConfigLong("cachesize");	// as number of bits
	ts.testAlgo = (ALGO_CHOICE) ts.getEnumCode(ALGO_ENUM,"testAlgo");
	ts.testScenario = (ARCH_CHOICE) ts.getEnumCode(ARCH_ENUM,"testScenario");
	ts.testSPtype = (SPTYPE_CHOICE) ts.getEnumCode(SPTYPE_ENUM,"testSPtype");
	
	// default storage method: the LIST cache 
	ts.testStorage = STORE_LIST;
	if ( ts.testAlgo == ALGO_SPCplus )
		ts.testStorage = STORE_GRAPH;
	else if ( ts.testAlgo == ALGO_SPCstar )
		ts.testStorage = STORE_COMPRESS;

	switch(ts.testSPtype){
	  case SPTYPE_FULL:
	    ts.useConcisepath = false;
	    ts.measureConcisepathdegrees = false;
	    break;
	  case SPTYPE_CONCISE:
	    ts.useConcisepath = true;
	    ts.measureConcisepathdegrees = true;
	    break;
	  case SPTYPE_OPTIMAL:
	    ts.useConcisepath = false;
	    ts.measureConcisepathdegrees = false;
	    break;
	}

	// format: "experiment"_"testAlgo"_"testFile (3 letters)".test
	// 		   "experiment" to be added later
	if (ts.getConfigBool("autoTestName")==true) {
		string& tname = ts.testName;
		tname="";
		tname.append(MatchEnumString(ALGO_ENUM,ts.testAlgo));
		tname.append("_");
		tname.append( ts.testFile, 0, 3);	// first 3 latters of testFile
		tname.append(".test");
	}	
}


void ExperimentVaryCacheSize(TestSetting ts) {
	if (ts.getConfigBool("autoTestName")==true) {
		ts.testName.insert(0,"v_cachesize_");
		cout << "(auto) testName: " << ts.testName << endl;
	}
	
	unsigned long lowCacheSize = ts.getConfigLong("lowCacheSize");
	unsigned long highCacheSize = ts.getConfigLong("highCacheSize");
	
	for (unsigned long csize = lowCacheSize; csize <= highCacheSize ; csize*=2) {
		ts.cacheSize = csize;
		cout << "*** Now using ts.cacheSize = " << ts.cacheSize << endl;
		
		TestObject *expTest = new TestObject(ts);
		expTest->runStaticTest();
		delete expTest;
	}
}


void ExperimentVarySplit(TestSetting ts) {
	if (ts.getConfigBool("autoTestName")==true) {
		ts.testName.insert(0,"v_split_");
		cout << "(auto) testName: " << ts.testName << endl;
	}
	
	int lowSplit = ts.getConfigInt("lowSplit");
	int highSplit = ts.getConfigInt("highSplit");
	
	for (int splits = lowSplit; splits <= highSplit ; splits+=2) {	
		ts.splits = splits;
		cout << "*** Now using ts.splits = " << ts.splits << endl;

		TestObject *expTest = new TestObject(ts);
		expTest->runStaticTest();
		delete expTest;
	}
}


void ExperimentSingle(TestSetting ts) {
	if (ts.getConfigBool("autoTestName")==true) {
		ts.testName.insert(0,"v_single_");
		cout << "(auto) testName: " << ts.testName << endl;
	}
	
	TestObject *expTest = new TestObject(ts);
	expTest->runStaticTest();
	delete expTest;
}

int main(int argc, char *argv[]) {


	InitEnumMappings();	// initialization
	
	srand(0);	srand48(0);
	
cout << "******************************************" << endl;
cout << "BEGIN TESTS" << endl;
cout << "******************************************" << endl;

	//Test setting
	///Load settings from config.prob
	//TestSetting ts;
	//ts.addConfigFromFile("config.prop");	// load dou parameter values
	//ts.addConfigFromCmdLine(argc,argv);		// override parameter values
	//ts.listConfig();		// list the content of the config
	
	///Load settings from commandline
	TestSetting ts;
	ts.addConfigFromCmdLine(argc,argv);    // get the "configName" parameter from command line
	string configName= ts.getConfigString("configName");

	ts.addConfigFromFile( configName.c_str() );    // load default parameter values
	ts.addConfigFromCmdLine(argc,argv);     // override parameter values

	
	
	extractTestParameters(ts);
	ts.printSetting();
	
	string experiment = ts.getConfigString("experiment");
	boost::to_upper(experiment);	
	
	cout << "HERE THE EXPERIMENTS SHOULD START!!\n" << endl;
	
////////////////////////////////////////	


	string fn=ts.queryFileName;
	string app="";
	intPairVector stPointPairs;
	vector<int> spPath;
	
	std::pair<double, double> firstPair, secondPair;
	int firstPnt, secondPnt, temp;
	string str;
	std::vector<string> tokens;
	ifstream qlogFile (fn.c_str(), ios::in);
	  
	cout << "readQueryLogData start: " << fn << endl;
	
	//find all pairs of nodeids in the training set to have SP done for them. map nodeids to Points.
	if (qlogFile.is_open()) {
	  if (debugCache) 
	    cout << "two, readQueryLogData opened! " << endl;
	  
	  while(getline(qlogFile, str)) {
	    boost::algorithm::split(tokens, str, boost::algorithm::is_space());
	    stPointPairs.push_back(std::make_pair(atoi(tokens[1].c_str()),atoi(tokens[2].c_str())));
	  }
	}
	qlogFile.close();

	BOOST_FOREACH(intPair q, testSTPointPairs) { 
	  RoadGraph::mapObject(ts)->dijkstraSSSP(q.first, q.second);
	}
/////////////////////////////////////////	
	
// 	if (experiment.compare("SINGLE")==0)
// 		ExperimentSingle(ts);
// 	else if (experiment.compare("SPLIT")==0)
// 		ExperimentVarySplit(ts);
// 	else if (experiment.compare("CACHESIZE")==0)
// 		ExperimentVaryCacheSize(ts);

	return EXIT_SUCCESS;
};