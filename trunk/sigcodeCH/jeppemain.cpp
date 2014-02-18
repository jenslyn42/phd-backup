/********************************************************************************
 *   Copyright (C) 2011 by Jeppe Rishede 					*
 *   jenslyn42@gmail.com							*
 *										*
 *   All rights reserved.							*
 *										*
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
* These two must be placed first as they include necessary typedef 
* statementm like NodeID, EdgeID etc.
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
  
  string ss = std::string("TestObject::constructor: ") + std::string(MatchEnumString(ALGO_ENUM,ts.testAlgo)) + std::string(" ") + boost::lexical_cast<std::string>(ts.cacheSize);

  cout << "TestObject::constructor: " << MatchEnumString(ALGO_ENUM,ts.testAlgo) << " test choosen" << endl;
  stats("mem.use", ss);
 
  switch( ts.testAlgo ){
    case ALGO_NONE:
      ts.cacheSize = 0;// a special case with 0 cacheSize
      test = new LRU(ts);
      break;
    case ALGO_LRU:
      test = new LRU(ts);
      break;
    case ALGO_LRUplus:
      test = new LRUPLUS(ts);
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
  stats("mem.use", "buildCache done");
  if (debug) cout << "TestObject::runStaticTest: queries read" <<endl;

  start = clock();
  test->runQueryList();
  stats("mem.use", "runQueryList done");
  
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
  cout << "SP calc:\t" << ts.testSPtype << " " << MatchEnumString(SPTYPE_ENUM, ts.testSPtype) << " " << MatchEnumString(CONCISETYPE_ENUM, ts.testConcisetype) << "(" << MatchEnumString(OPTIMALTYPE_ENUM, ts.testOptimaltype) <<  ")" << endl;

  cout << "CacheSize:\t" << ts.cacheSize << endl;
  cout << "CacheItems:\t" << ts.getItemsInCache() << endl;
  cout << "Avg. ItemLength:\t" << ts.getAvgItemLength() << endl;

  cout << "Splits:\t" << ts.getSplits() << endl;
  cout << "Optimal Pct:\t" << ts.optiNum << endl;
  cout << "QueryFile:\t" << ts.queryFileName << endl;

  cout << "NonEmptyRegions:\t" << ts.getNonEmptyRegionPairs() << endl;
  cout << "CalcStatTime:\t" << ts.getBuildStatisticsTime() << " sec" <<endl;
  cout << "FillCacheTime:\t" << ts.getFillCacheTime() << " sec" << endl;
  cout << "Avg length opti/long: ";
  (ts.numOpti > 0) ? cout << ts.optiLength / ts.numOpti << " / " : cout << 0 << " / ";
  (ts.numLong > 0) ? cout << ts.longLength / ts.numLong : cout << 0;
  cout << " (" << ts.optiLength << ", " <<ts.numOpti << ") (" << ts.longLength << ", " << ts.numLong << ")" << endl;
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
    << "NonEmptyRegions\tCalcStatTime\tFillCacheTime\tuseConcisepath\tmeasureConcisepathdegrees\t"
    << "bitsInCache\tSPcalc\texecTrainWL\tConciseType\tOptimalType\tOptimalPct\tDivideScoreByLength" << endl;
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
  << ts.executeTrainingWorkload << "\t"
  << MatchEnumString(CONCISETYPE_ENUM,ts.testConcisetype) << "\t"
  << MatchEnumString(OPTIMALTYPE_ENUM, ts.testOptimaltype) << "\t"
  << ts.optiNum << "\t"
  << ts.divideScoreByLength
  << endl;

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
  ts.splits = ts.getConfigInt("splits");  // for Probcache (SPC)
  ts.scacheQueryType = ts.getConfigInt("scacheQueryType");  // for SCACHE
  ts.executeTrainingWorkload = ts.getConfigBool("executeTrainingWorkload");
  ts.useLRUbitmap = ts.getConfigBool("useLRUbitmap");
  ts.divideScoreByLength = ts.getConfigBool("divideScoreByLength");
  ts.optiNum = ts.getConfigInt("optiNum");  
  ts.cacheSize = ts.getConfigLong("cachesize");  // as number of bits
  ts.testAlgo = (ALGO_CHOICE) ts.getEnumCode(ALGO_ENUM,"testAlgo");
  ts.testScenario = (ARCH_CHOICE) ts.getEnumCode(ARCH_ENUM,"testScenario");
  ts.testSPtype = (SPTYPE_CHOICE) ts.getEnumCode(SPTYPE_ENUM,"testSPtype");
  ts.testConcisetype = (CONCISETYPE_CHOICE) 0;
  ts.testOptimaltype = (OPTIMALTYPE_CHOICE) ts.getEnumCode(OPTIMALTYPE_ENUM, "testOptimaltype");

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
      ts.useConcisepath = true;
      ts.measureConcisepathdegrees = true;
      break;
  }

  // format: "experiment"_"testAlgo"_"testFile (3 letters)".test
  //         "experiment" to be added later
  if (ts.getConfigBool("autoTestName")==true) {
    string& tname = ts.testName;
    tname="";
    tname.append(MatchEnumString(ALGO_ENUM,ts.testAlgo));
    tname.append("_");
    tname.append( ts.testFile, 0, 3);  // first 3 latters of testFile
    tname.append(".test");
  }
}


void ExperimentVaryOptimalLengthPct(TestSetting ts) {
  if (ts.getConfigBool("autoTestName")==true) {
    ts.testName.insert(0,"_");
    ts.testName.insert(0,MatchEnumString(SPTYPE_ENUM, ts.testSPtype));
    ts.testName.insert(0,"v_optimalpct_");
    cout << "(auto) testName: " << ts.testName << endl;
  }

  unsigned long lowPct = ts.getConfigLong("lowPct");
  unsigned long highPct = ts.getConfigLong("highPct");

  for (unsigned long psize = lowPct; psize <= highPct ; psize+=5) {
    ts.optiNum = psize;
    cout << "*** Now using ts.optiNum = " << ts.optiNum << endl;

    TestObject *expTest = new TestObject(ts);
    expTest->runStaticTest();
    delete expTest;
  }
}


void ExperimentVaryCacheSize(TestSetting ts) {
  if (ts.getConfigBool("autoTestName")==true) {
    ts.testName.insert(0,"_");
    ts.testName.insert(0,MatchEnumString(SPTYPE_ENUM, ts.testSPtype));
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

void ExperimentVaryCacheSizeFromFilesize(TestSetting ts) {
  if (ts.getConfigBool("autoTestName")==true) {
    ts.testName.insert(0,"_");
    ts.testName.insert(0,MatchEnumString(SPTYPE_ENUM, ts.testSPtype));
    ts.testName.insert(0,"v_cachesizeFile_");

  cout << "(auto) testName: " << ts.testName << endl;
  }
  
  double filesizeInBits = 8 * ts.getConfigLong("filesize"); //filesize assumed to be given in bytes

  for (double csize = 0.05; csize <= 0.20 ; csize+=0.05) {
    ts.cacheSize = filesizeInBits*csize;
    cout << "*** Now using ts.cacheSize = " << ts.cacheSize << endl;

    TestObject *expTest = new TestObject(ts);
    expTest->runStaticTest();
    delete expTest;
  }
  
  for (double csize = 0.25; csize <= 1 ; csize+=0.25) {
    ts.cacheSize = filesizeInBits*csize;
    cout << "*** Now using ts.cacheSize = " << ts.cacheSize << endl;

    TestObject *expTest = new TestObject(ts);
    expTest->runStaticTest();
    delete expTest;
  }
}


void ExperimentVarySplit(TestSetting ts) {
  if (ts.getConfigBool("autoTestName")==true) {
    ts.testName.insert(0,"_");
    ts.testName.insert(0,MatchEnumString(SPTYPE_ENUM, ts.testSPtype));
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
    ts.testName.insert(0,"_");
    ts.testName.insert(0,MatchEnumString(SPTYPE_ENUM, ts.testSPtype));
    ts.testName.insert(0,"v_single_");
    cout << "(auto) testName: " << ts.testName << endl;
  }

  TestObject *expTest = new TestObject(ts);
  expTest->runStaticTest();
  delete expTest;
}


int main(int argc, char *argv[]) {

  InitEnumMappings();  // initialization
  
  srand(0);  srand48(0);
  
cout << "******************************************" << endl;
cout << "BEGIN TESTS" << endl;
cout << "******************************************" << endl;

  //Test setting
  ///Load settings from config.prob
  //TestSetting ts;
  //ts.addConfigFromFile("config.prop");  // load dou parameter values
  //ts.addConfigFromCmdLine(argc,argv);    // override parameter values
  //ts.listConfig();    // list the content of the config
  
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
  
  
  if (experiment.compare("SINGLE")==0)
    ExperimentSingle(ts);
  else if (experiment.compare("SPLIT")==0)
    ExperimentVarySplit(ts);
  else if (experiment.compare("CACHESIZE")==0)
    ExperimentVaryCacheSize(ts);
  else if (experiment.compare("OPTIMALPCT")==0)
    ExperimentVaryOptimalLengthPct(ts);
  else if (experiment.compare("CACHESIZEFILE")==0)
    ExperimentVaryCacheSizeFromFilesize(ts);

  return EXIT_SUCCESS;
};
