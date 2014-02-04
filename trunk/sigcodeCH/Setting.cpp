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

// NO NEED TO INCLUDE "Setting.h" anymore

//////////////////////////////////////////////////////////////////////////////////////////////

LookupList ALGO_ENUM, STORAGE_ENUM, ARCH_ENUM, SPTYPE_ENUM, CONCISETYPE_ENUM, OPTIMALTYPE_ENUM;


void InsertEnum(LookupList& list,int type,string name) {
	// only upper-case string is stored (for case-insensitive matching later)
	boost::to_upper(name);
	list.push_back( make_pair( type, name ) );
}

void InitEnumMappings() {
	ALGO_ENUM.clear();
	InsertEnum(ALGO_ENUM,	ALGO_NONE,	"NONE");
	InsertEnum(ALGO_ENUM,	ALGO_SPC,	"SPC");
	InsertEnum(ALGO_ENUM,	ALGO_SPCplus,	"SPCPLUS");
	InsertEnum(ALGO_ENUM,	ALGO_SPCstar,	"SPCSTAR");
	InsertEnum(ALGO_ENUM,	ALGO_LRU,	"LRU");
	InsertEnum(ALGO_ENUM,	ALGO_LRUplus,	"LRUPLUS");
	InsertEnum(ALGO_ENUM,	ALGO_HQF,	"HQF");
	InsertEnum(ALGO_ENUM,	ALGO_SCACHE,	"SCACHE");
	InsertEnum(ALGO_ENUM,	ALGO_HQFLRU,	"HQFLRU");
	InsertEnum(ALGO_ENUM,	ALGO_ORACLE,	"ORACLE");
	
	STORAGE_ENUM.clear();
	InsertEnum(STORAGE_ENUM, STORE_LIST,	"LIST");
	InsertEnum(STORAGE_ENUM, STORE_GRAPH,	"GRAPH");
	InsertEnum(STORAGE_ENUM, STORE_COMPRESS,"COMPRESS");
	
	ARCH_ENUM.clear();
	InsertEnum(ARCH_ENUM,	ARCH_SERVER,	"SERVER");
	InsertEnum(ARCH_ENUM,	ARCH_PROXY,	"PROXY");
	
	SPTYPE_ENUM.clear();
	InsertEnum(SPTYPE_ENUM,	SPTYPE_FULL,	"FULL");
	InsertEnum(SPTYPE_ENUM,	SPTYPE_CONCISE,	"CONCISE");
	InsertEnum(SPTYPE_ENUM,	SPTYPE_OPTIMAL,	"OPTIMAL");	

  	CONCISETYPE_ENUM.clear();
	InsertEnum(CONCISETYPE_ENUM, CONCISETYPE_A, "A");
	InsertEnum(CONCISETYPE_ENUM, CONCISETYPE_B, "B");
	
	OPTIMALTYPE_ENUM.clear();
	InsertEnum(OPTIMALTYPE_ENUM, OPTIMALTYPE_ORG,	"ORIGINAL");
	InsertEnum(OPTIMALTYPE_ENUM, OPTIMALTYPE_KSKIP,	"KSKIP");
	InsertEnum(OPTIMALTYPE_ENUM, OPTIMALTYPE_RAND,	"RAND");
}


void error(string error_msg) {
	cout << "\t[ERROR] " << error_msg << "\n";
	exit(0);
}

int MatchEnumCode(LookupList& list,string str) {		// lookup an enum value by a string
	// prerequisite: all strings in "list" are in upper-case
	// convert the string to upper-case for matching
	boost::to_upper(str);	

	BOOST_FOREACH(LookupPair& cpair, list) {
		if (cpair.second==str)
			return cpair.first;
	}
	
	cout << str;
	error("INVALID ENUM STRING");
	return -1;
}

string MatchEnumString(LookupList& list,int code) { 	// lookup a string by an enum value
	BOOST_FOREACH(LookupPair& cpair, list) {
		if (cpair.first==code)
			return cpair.second;
	}
	
	cout << code;
	error("INVALID ENUM CODE");
	return "";
}

int TestSetting::getEnumCode(LookupList& list,string key) {
  return MatchEnumCode(list,getConfigString(key));
}

TestSetting::TestSetting() {
  //InitEnumMappings();  // call "InitEnumMappings"
  
  testName = "";
  testFile = "";
  cacheSize = 0;

  splits = -1;
  itemsInCache = 0;
  nonEmptyRegionPairs = -1;
  
  fillCacheTime = buildStatisticsTime = 0;
  
  optiLength=longLength=0;
  numOpti=numLong=0;
}


TestSetting::~TestSetting() {
  testName = "";
  testFile = "";
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TestSetting::printSetting() {
  cout << "\n\n--------------------------" << endl;
  cout << "testName: " << testName << endl;
  cout << "testFile, Type: " << testFile << " " << inputFileType << endl;
  cout << "queryFileName: " << queryFileName << endl;
  
  cout << "testAlgo: " << testAlgo << " " << MatchEnumString(ALGO_ENUM,testAlgo) << endl;
  cout << "testStorage: " << testStorage << " " << MatchEnumString(STORAGE_ENUM,testStorage) << endl;
  cout << "testScenario: " << testScenario << " " << MatchEnumString(ARCH_ENUM,testScenario) << endl;
  cout << "SP calc:\t" << MatchEnumString(SPTYPE_ENUM, testSPtype) << " ";
  if(testSPtype == SPTYPE_OPTIMAL || testSPtype == SPTYPE_CONCISE) 
    cout << MatchEnumString(CONCISETYPE_ENUM, testConcisetype);
  cout << " (" << MatchEnumString(OPTIMALTYPE_ENUM, testOptimaltype) << ")" << endl;

  cout << "cacheSize: " <<  cacheSize << endl;
  cout << "scacheQueryType: " <<  scacheQueryType << endl;
  cout << "splits, itemsInCache, nonEmptyRegionPairs: " <<  splits << " " << itemsInCache << " " << nonEmptyRegionPairs << endl;
  cout << "fillCacheTime, buildStatisticsTime: " <<  fillCacheTime << " " << buildStatisticsTime << endl;
  cout << "useConcisePath, measureConcisepathdegrees, devideScoreByLength: " << useConcisepath << " " << measureConcisepathdegrees << " " << devideScoreByLength << endl;
  cout << "--------------------------\n\n" << endl;
}

  
void TestSetting::trimSpace(char* str) {
  if (str==NULL) return;

  static char spaces[]={'\t','\n','\f','\r',' '};
  int pos=0;
  for (int i=0;i<strlen(str);i++) {
    bool found=false;
    for (int j=0;j<5;j++)
      if (str[i]==spaces[j]) found=true;

    if (!found) {
      str[pos]=str[i];
      pos++;
    }
  }
  str[pos]='\0';
}

void TestSetting::addConfigFromFile(const char* filename) {
  const int LINE_LEN=1024;
  char line[LINE_LEN],key[LINE_LEN],value[LINE_LEN];

  ifstream br(filename);
    if (! br.is_open())
    { printf("Error opening file \"%s\"",filename); exit (1); }

  while (br.getline(line,LINE_LEN)){
    if (strstr(line,"//")!=NULL) continue; // remove comments
    char* chPos=strchr(line,'=');
    if (chPos!=NULL) {
      int pos=((int)(chPos-line))/sizeof(char);
      int keyLen=pos;
      int valueLen=strlen(line)-1-keyLen;
      memcpy(key,&line[0],keyLen);  key[keyLen]='\0';
      memcpy(value,&line[pos+1],valueLen);  value[valueLen]='\0';
      trimSpace(key);  trimSpace(value);
      cr[key]=value;
    }
  }
  br.close();
}

void TestSetting::addConfigFromCmdLine(int argc,char** argv) {
  int i=0;
  while (i<argc) {
    while ((i<argc)&&(argv[i][0]!='-')) i++;  // shortcut condition
    if (i+1<argc) {
      char* key=&(argv[i][1]);
      char* value=argv[i+1];
      trimSpace(key);  trimSpace(value);
      cr[key]=value;
      i+=2;
    } else
      return;
  }
}

void TestSetting::listConfig() {
  ConfigType::iterator p=cr.begin();
  while (p!=cr.end()) {
    printf("%s=%s\n",p->first.c_str(),p->second.c_str());
    p++;
  }
}


void TestSetting::printConfigError(string key,bool required) {
  if (required) {
    printf("Config key \"%s\" not found\n",key.c_str());
    exit(1);
  }
}

float TestSetting::getConfigFloat(string key,bool required,float _default) {
  float value=_default;
  if (cr.count(key))
    value=atof(cr[key].c_str());
  else 
    printConfigError(key,required);
  return value;
}

int TestSetting::getConfigInt(string key,bool required,int _default) {
  int value=_default;
  if (cr.count(key))
    value=atoi(cr[key].c_str());
  else 
    printConfigError(key,required);  
  return value;
}

long TestSetting::getConfigLong(string key,bool required,long _default) {
  long value=_default;
  if (cr.count(key))
    value=atol(cr[key].c_str());
  else 
    printConfigError(key,required);
  return value;
}

string TestSetting::getConfigString(string key,bool required,string _default) {
  string value=_default;
  if (cr.count(key))
    value=cr[key];
  else 
    printConfigError(key,required);
  return value;
}

bool TestSetting::getConfigBool(string key,bool required,bool _default) {
  bool value=_default;
  if (cr.count(key)) {
    string bstr=cr[key];
    boost::to_upper(bstr);
    if (bstr.compare("TRUE")==0)
      value=true;
    else if (bstr.compare("FALSE")==0)
      value=false;
    else {
      printf("Config key \"%s\" invalid\n",key.c_str());
      exit(1);  
    }
  } else 
    printConfigError(key,required);
  return value;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdlib.h"
#include "stdio.h"
// #include "string.h"
#include "sys/times.h"
#include "sys/vtimes.h"
#include "sys/types.h"
#include "sys/sysinfo.h"

int parseLine(char* line){
  int i = strlen(line);
  while (*line < '0' || *line > '9') line++;
  line[i-3] = '\0';
  i = atoi(line);
  return i;
}

/// Virtual Memory currently used by current process ////

int getVirtValue(){ //Note: this value is in KB!
  FILE* file = fopen("/proc/self/status", "r");
  int result = -1;
  char line[128];
    
  while (fgets(line, 128, file) != NULL){
    if (strncmp(line, "VmSize:", 7) == 0){
      result = parseLine(line);
      break;
    }
  }
  fclose(file);
  return result;
}

/// Physical Memory currently used by current process ///

int getPhysValue(){ //Note: this value is in KB!
  FILE* file = fopen("/proc/self/status", "r");
  int result = -1;
  char line[128];
    
  while (fgets(line, 128, file) != NULL){
    if (strncmp(line, "VmRSS:", 6) == 0){
      result = parseLine(line);
      break;
    }
  }
  fclose(file);
  return result;
}

void stats(string fn, string identifier){
  
  ///file output
  ofstream resultfile;
  resultfile.open(fn.c_str(), ios::out | ios::ate | ios::app);

  resultfile <<  identifier << endl;
  resultfile << "virtual memory used by process: " << getVirtValue() / 1024 << " MB" << endl;
  resultfile << "physical memory used by process: " << getVirtValue() / 1024 << " MB" << endl;
  
  //// Total physical memory and memory used (RAM) ////////////
  struct sysinfo memInfo;
    
  sysinfo (&memInfo);
  long long totalVirtualMem = memInfo.totalram;
  //Add other values in next statement to avoid int overflow on right hand side...
  totalVirtualMem += memInfo.totalswap;
  totalVirtualMem *= memInfo.mem_unit;
  
  ///Total physical memory
  long long totalPhysMem = memInfo.totalram;
  //Multiply in next statement to avoid int overflow on right hand side...
  totalPhysMem *= memInfo.mem_unit;
  
  ///Total physical memory used
  long long physMemUsed = memInfo.totalram - memInfo.freeram;
  //Multiply in next statement to avoid int overflow on right hand side...
  physMemUsed *= memInfo.mem_unit;
  
  resultfile << "Total physical memory (RAM): " << totalPhysMem/(1024*1024) << " MB"<< endl;
  resultfile << "Total physical memory used: " << physMemUsed/(1024*1024) << " MB\n"<< endl;

  resultfile.close();
}
///////////////////////////////////////////////////////////
