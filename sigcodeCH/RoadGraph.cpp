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
#ifndef ROADGRAPH_CPP
#define ROADGRAPH_CPP

#include "RoadGraph.h"

#define rgDebug false
//specifically for the dijkstraSSSP() method
#define spDebug false

#define conciseDebug true
#define recoverConciseDebug false

boost::unordered_map<int, int*> RoadGraph::spTrace;
// boost::unordered_map<int, int*> RoadGraph::trackdist;

RoadGraph* RoadGraph::mapInstance = NULL;

RoadGraph* RoadGraph::mapObject(TestSetting& ts){
  // obtain parameter values
  std::string testFile=ts.getTestFile(); //has .cedge at end
  int pt=ts.inputFileType;

  if (mapInstance==NULL){
    mapInstance = new RoadGraph();
    mapInstance->useConcisepath = ts.useConcisepath;
    mapInstance->measureConcisepathdegrees = ts.measureConcisepathdegrees;
    mapInstance->parseFileType = pt;
    mapInstance->ssspCalls = 0;
    mapInstance->numNodeVisits = 0;
    mapInstance->readSPTreeFileBinary(ts);
    stats("mem.use", "RoadGraph::mapObject()  object set");
    mapInstance->countSuccess=0;
    mapInstance->countFail=0;
    mapInstance->setupCH(testFile); //initialize CH
    stats("mem.use", "RoadGraph::mapObject()  setupCH done");
    mapInstance->testconcisetype = (CONCISETYPE_CHOICE) ts.testConcisetype;
    
    printf("*** RoadGraph::read\n");
    
    switch( (mapInstance->parseFileType) ){
      case 1:
	cout << "mapObject[case1], parseFileType: " << mapInstance->parseFileType << " " << testFile << endl;
	mapInstance->readRoadNetworkFile(testFile);
	break;
      case 2:
	cout << "mapObject[case2], parseFileType: " << mapInstance->parseFileType << " " << testFile << endl;
	mapInstance->readPPINetworkFile(testFile);
	break;
      case 3:
	cout << "mapObject[case3], parseFileType: " << mapInstance->parseFileType << " " << testFile;
	mapInstance->readCedgeNetworkFile(testFile);
	cout << " ... done" << endl;
	break;
    }
    stats("mem.use", "RoadGraph::mapObject()  map read into object");
  }else if((mapInstance->parseFileType) != pt){
    delete mapInstance;
    mapInstance = NULL; //if type of file to be parsed changes, delete mapInstance
  }

  return mapInstance;
}


vector<int> RoadGraph::dijkstraSSSP(int source, int dest) {
  if(useConcisepath)
    return (conciseDijkstraSSSP(source, dest)).second;
  return (conciseDijkstraSSSP(source, dest)).first;
}

// 	Dijkstra sp(updGraph);
// 	Path path;
// 	EdgeWeight spDist;
std::pair<intVector, intVector> RoadGraph::conciseDijkstraSSSP(int source, int dest) {
  std::vector<int> trace, trace2;
  std::vector<unsigned int> temp, temp2;
  Path path2;
  (useConcisepath)? cout << "TRUE\t" : cout << "FALSE\t";
  cout << "(" << source << "," << dest << ")\t";
  if (spDebug)    cout << "RoadGraph::dijkstraSSSP 1 " << endl;

  spDist = sp->bidirSearch(source, dest); //distance
  path2 = sp->pathTo(path, dest, -1, false, true); //shortest path
  temp = path.getNodeVector();
  
  for (int i=0;i<temp.size();i++)
    trace.push_back((int)(temp[i]));
  
    cout << spDist << "," << (path2.getNodeVector()).size() << "," << temp.size() << ", " << trace.size() << "(" << trace.front() << "," << trace.back() << "\t";
  
  if (spDebug)    cout << "RoadGraph::dijkstraSSSP 6 " << useConcisepath << " " << trace.size() << " " << useConcisepath << endl;

  ///////////////////
//   cout << "(" << source << "," << dest << ") [" << trace.size() << " - " << spDist << " / " << path.length() << "] " << endl;;
//   BOOST_FOREACH(int node, trace){ cout << node << ",";}
//   cout << " *1.1*" << endl;
  sp->clear();
  //////////////////
        
  if(useConcisepath){
    intVector conc = calcConsisePath(trace);
    cout << "[" << trace.size() << "," << conc.size() << "]\t";
    return make_pair<intVector, intVector>(trace,conc);    
//     return make_pair<intVector, intVector>(trace, calcConsisePath(trace));
  }else 
    return make_pair<intVector, intVector>(trace, trace2);
}

vector<int> RoadGraph::dijkstraSSSP2(int source, int dest) {
  std::vector<int> trace;
  if(spDebug)cout << "dijkstraSSSP: " << source << "," << dest << " : " <<  useConcisepath << endl;
  ssspCalls++;

  trace = getSPfromSPTree(source, dest);
  if(spDebug) cout << "RoadGraph::dijkstraSSSP 2 " << trace.empty() << " " << trace.size() << endl; 
  if(!trace.empty()){
    if(useConcisepath){
      if(spDebug) cout << "RoadGraph::dijkstraSSSP 3 " << endl;
      return calcConsisePath(trace);
    }else 
      return trace;
  }


  if (spDebug) 
    cout << "one, dijkstraSSSP! map:" << mapSize <<" s,t:" << source <<"," <<dest << endl;

  if ( source<0 || dest<0 || source>=mapSize || dest>=mapSize ) {
    cout << "Invalid src/dest nodes, map\n";
    return trace;
  }

  if (source==dest) {
    trace.push_back(source);
    return trace;
  }

  if(spDebug) cout << "four, dijkstraSSSP! " << endl;


  static bool isInit=false;
  static int _mapsize=0;
  static int* backtrace=NULL;
  static bool* isVisited=NULL;

  if (isInit==false) {
    isInit=true;// be careful with this line!
    _mapsize=getMapsize();
    backtrace=new int[_mapsize];
    isVisited=new bool[_mapsize];
    printf("*** RoadGraph::init array\n");
  } else
    assert(_mapsize==getMapsize());

  // init
  fill( backtrace, backtrace+_mapsize, -1);
  fill( isVisited, isVisited+_mapsize, false);

  //insert source node
  Heap hp;
  {
    HeapEntry root_he;
    root_he.id=source;
    root_he.dist=0.0;    // dist. so far
    root_he.prev_id=-1;
    hp.push(root_he);
  }

  if(spDebug) cout << "six, dijkstraSSSP! " << endl;

  while (hp.size()>0) {
    HeapEntry he=hp.top();
    hp.pop();

    int curnode=he.id;
    if (isVisited[curnode])
      continue;

    isVisited[curnode]=true;
    backtrace[curnode]=he.prev_id;// prev node
    numNodeVisits++;// usage info

    EdgeList& CurAdjList=map[curnode];
    BOOST_FOREACH (JEdge neighbour, CurAdjList){
      int NextNodeID = neighbour.first; //id
      double NextWeight = neighbour.second; 
      if (isVisited[NextNodeID]==false) {
	HeapEntry new_he=he;    // copy ...
	new_he.id=NextNodeID;
	new_he.dist=he.dist+NextWeight; //weight
	new_he.prev_id=curnode;
	hp.push(new_he);    // propagation
      }
    }

    if(curnode==dest) {
      int prevNode = dest;
      trace.push_back(prevNode);
      while (prevNode!=source) {
	assert(prevNode!=-1);

	prevNode = backtrace[prevNode];
	trace.push_back(prevNode);
      }

      if(useConcisepath)
	return calcConsisePath(trace);
      else 
	return trace;
      }
  }
  if(useConcisepath)
    return calcConsisePath(trace);
  else 
    return trace;
}

int RoadGraph::getMapsize()
{
  return mapSize;
}

//convert training and testfile format from [vertexID, vertexID] to [lineID, x1, y1, x2, y2]
//.cnode file has format [nodeID, x, y]
//assumes the full name of the cnode file "fn.cnode" but only assumes the name of the training/test file. "fn"
void RoadGraph::transformTrainOrTestFile(string cnodeFn, string trainTestFn)
{
  string nodeFile = cnodeFn;
  string testname = trainTestFn;
  string trainname = trainTestFn;
  string str;
  std::vector<string> tokens;
  boost::unordered_map<int, Point> vertexId2Point;
  std::vector<intPair> trainVector;
  std::vector<intPair> testVector;
  testname.replace ((testname.size()), 5, ".test");
  trainname.replace ((trainname.size()), 6, ".train");

  //load in all vertex ids with their Point
  ifstream nodeData (nodeFile.c_str(), ios::in);
  if(rgDebug) cout << "s1, transformTrainOrTestFile!"<< endl;
  if(nodeData.is_open())
  {
    while(getline(nodeData, str))
    {
      boost::algorithm::split(tokens, str, boost::algorithm::is_space()); //split last line of *.cnode file
        vertexId2Point[boost::lexical_cast<int>(tokens[0])] = std::make_pair(boost::lexical_cast<double>(tokens[1]),boost::lexical_cast<double>(tokens[2]));
    }
    nodeData.close();
  }

  //load in all start/end vertex ids from training file
  ifstream trainData (trainname.c_str(), ios::in);
  if(rgDebug) cout << "s1, transformTrainOrTestFile!"<< endl;
  if(trainData.is_open())
  {
    while(getline(trainData, str))
    {
      boost::algorithm::split(tokens, str, boost::algorithm::is_space()); //split last line of *.cnode file
      trainVector.push_back(std::make_pair(boost::lexical_cast<int>(tokens[0]),boost::lexical_cast<int>(tokens[1])));
    }
    trainData.close();
  }

  //load in all start/end vertex ids from test file
  ifstream testData (testname.c_str(), ios::in);
  if(rgDebug) cout << "s1, transformTrainOrTestFile!"<< endl;
  if(testData.is_open())
  {
    while(getline(testData, str))
    {
      boost::algorithm::split(tokens, str, boost::algorithm::is_space()); //split last line of *.cnode file
      testVector.push_back(std::make_pair(boost::lexical_cast<int>(tokens[0]),boost::lexical_cast<int>(tokens[1])));
    }
    testData.close();
  }

  testname.replace ((testname.size()-5), 7, "CV.test");
  trainname.replace ((trainname.size()-6), 8, "CV.train");
  int i = 0;

  //write out training file
  ofstream trainfile;
  trainfile.open(trainname.c_str(), ios::out | ios::ate | ios::app);

  BOOST_FOREACH (intPair coord, trainVector)
  {
    trainfile << i << " " << vertexId2Point.at(coord.first).first << " " << vertexId2Point.at(coord.first).second << " ";
    trainfile << vertexId2Point.at(coord.second).first << " " << vertexId2Point.at(coord.second).second << endl;
    i++;
  }
  trainfile.close();

  //write out test file
  ofstream testfile;
  testfile.open(testname.c_str(), ios::out | ios::ate | ios::app);
  i = 0;

  BOOST_FOREACH (intPair coord, testVector)
  {
    testfile << i << " " << vertexId2Point.at(coord.first).first << " " << vertexId2Point.at(coord.first).second << " ";
    testfile << vertexId2Point.at(coord.second).first << " " << vertexId2Point.at(coord.second).second << endl;
    i++;
  }
  testfile.close();
}

void RoadGraph::writeoutEdgedegree()
{
  ofstream degreefile;
  degreefile.open("nodeEdgedegree.txt", ios::out | ios::ate | ios::app);
  int degree;

  for(int i=0; i<mapSize; i++){
    degree = map[i].size();
    degreefile << i << " : " << degree << endl;
  }
}

/*
 * Counts the number of nodes which has edge degree 1-5 and writes them to std output
 */
void RoadGraph::findNode2degree()
{
  int degree5=0, degree4=0, degree3=0, degree2=0, degree1=0;
  
  for(int i=0; i<mapSize; i++){
    node2degree[i] = map[i].size();
    if(map[i].size() == 1) degree1++;
    if(map[i].size() == 2) degree2++;
    if(map[i].size() == 3) degree3++;
    if(map[i].size() == 4) degree4++;
    if(map[i].size() == 5) degree5++;
  }
  cout << "\n ##################################################\n" << degree1 <<" "<< degree2 <<" "<< degree3 <<" "<< degree4 <<" "<< degree5 << "\n ##################################################\n" << endl;
}

void RoadGraph::addEdge(int v1, int v2, double w) {
  assert( v1>=0 && v1<mapSize );
  assert( v2>=0 && v2<mapSize );

  if(rgDebug) cout << "RoadGraph::addEdge " << v1 << ", " << v2 << ", " << w << ", " << mapSize << endl;  
  EdgeList& eList1=map[v1];
  EdgeList& eList2=map[v2];
  
  if(rgDebug) cout << eList1.size() << " / " << eList2.size() << endl;
  bool isFound=false;
  BOOST_FOREACH (JEdge e, eList1) {
    if(rgDebug) cout << "(" << e.first << "," << e.second << "); " << endl;
    if (e.first==v2)
      isFound=true;
    if(rgDebug) cout << "(" << e.first << "," << e.second << "); " << "if1: " << isFound << endl;
  }
  
  BOOST_FOREACH (JEdge e, eList2) {
    if(rgDebug) cout << "(" << e.first << "," << e.second << "); " << endl;
    if (e.first==v1)
      isFound=true;
    if(rgDebug) cout << "(" << e.first << "," << e.second << "); " << "if2: " << isFound << endl;
  }
  
  if(rgDebug) cout << "isFound: " << isFound << endl;
  if (!isFound) {  // edge not inserted before
    eList1.push_back(make_pair(v2,w));
    eList2.push_back(make_pair(v1,w));
  }
}

void RoadGraph::readRoadNetworkFile(string fn)
{
  filename = fn;
  string str;
  std::vector<string> tokens;
  ifstream in_data (fn.c_str(), ios::in);
  if(in_data.is_open())
  {
    if(rgDebug) cout << "zero, readNetworkFile!" << endl;
    getline(in_data, str);
    mapSize = atoi(str.c_str());
    
    map=new EdgeList[mapSize];
    if(rgDebug) cout << "one, readNetworkFile! map: " << mapSize << endl;
    
    getline(in_data, str);
    edges = atoi(str.c_str());

    while( tokens.size() < 3 )
    {
      getline(in_data, str);
      if(rgDebug) cout << "three, readNetworkFile! getline:" << str << endl;

      boost::algorithm::split(tokens, str, boost::algorithm::is_space());
    }

    if(rgDebug) cout << "four, readNetworkFile! tokens ([0],[1],[2]): (" << atoi(tokens[0].c_str()) << "," << atoi(tokens[1].c_str()) << "," << atof(tokens[2].c_str()) << ")" << endl;

    addEdge(atoi(tokens[0].c_str()),atoi(tokens[1].c_str()),atof(tokens[2].c_str()));

    int check = 0;
    while(getline(in_data, str)) {
      if(rgDebug) cout << "five, readNetworkFile! getline:" << str  << " check: " << check++ << endl;
      boost::algorithm::split(tokens, str, boost::algorithm::is_space());
      if(rgDebug) cout << "five1, readNetworkFile! getline:" << str  << " check: "  << check << endl;
      addEdge(atoi(tokens[0].c_str()),atoi(tokens[1].c_str()),atof(tokens[2].c_str()));
      if(rgDebug) cout << "five2, readNetworkFile! getline:" << str  << " check: "  << check << endl;
    }
    if(rgDebug) cout << "six, readNetworkFile!" << endl;
    in_data.close();
  }
}

void RoadGraph::readPPINetworkFile(string fn)
{
  filename = fn;
  string str;
  int woffset;
  std::vector<string> tokens;
  ifstream in_data (fn.c_str(), ios::in);
  if(in_data.is_open())
  {
    if(rgDebug) cout << "zero, readPPINetworkFile!" << endl;
    getline(in_data, str); //read first line comment
    getline(in_data, str);
    boost::algorithm::split(tokens, str, boost::algorithm::is_space()); //read the size of the two node sets
    mapSize = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str());
    woffset = atoi(tokens[0].c_str());  
    
    map=new EdgeList[mapSize];
    if(rgDebug) cout << "one, readPPINetworkFile! map: " << mapSize << endl;
    
    while(getline(in_data, str))
    {
      boost::algorithm::split(tokens, str, boost::algorithm::is_space());
      for(uint i = 1; i< tokens.size(); i++)
        addEdge(atoi(tokens[0].c_str()),atoi(tokens[i].c_str())+woffset,1.0); //default weight of 1 since data does not provide any edge weights
    }
    if(rgDebug) cout << "six, readPPINetworkFile!" << endl;
    in_data.close();
  }
}

/// fileformat .cedge: linenum nid1 nid2 distance
/// e.g.: 0 0 1 1.182663
/// fileformat .cnode: nid x y
void RoadGraph::readCedgeNetworkFile(string fn)
{
  cout << "\n readCedgeNetworkFile begin [fn]: " << fn << endl;
  filename = fn;
  string str;
  string nodeFN = fn;
  nodeFN.replace ((nodeFN.size()-4), 4, "node"); //change file extention from .cedge to .cnode
  std::vector<string> tokens;
	  
  ifstream in_data (fn.c_str(), ios::in);
  if(rgDebug) cout << "s1, readCedgeNetworkFile! nodeFN: " <<nodeFN << endl;
  if(in_data.is_open())
  {
    if(rgDebug) cout << "zero, readCedgeNetworkFile! int maxsize: " << INT_MAX << endl;
    mapSize = getFilelines(nodeFN.c_str()); //find number of lines in file

    map=new EdgeList[mapSize+1]; //map can accomodate mapSize+1 nodes, with ids upto mapSize.
    if(rgDebug) cout << "three, readCedgeNetworkFile! map: " << mapSize << endl;

    while(getline(in_data, str)) {
      if(str.length() == 0) break;
      boost::algorithm::split(tokens, str, boost::algorithm::is_space());
      if(rgDebug) cout << "five1, readCedgeNetworkFile! getline:" << str << endl;
      addEdge(atoi(tokens[1].c_str()),atoi(tokens[2].c_str()),atof(tokens[3].c_str()));
      if(rgDebug) cout << "five2, readCedgeNetworkFile! getline:" << str << endl;
    }
    if(rgDebug) cout << "six, readCedgeNetworkFile!" << endl;
    in_data.close();
  }

  int count=0;
  if(useConcisepath){
    if(rgDebug) cout << "seven, readCedgeNetworkFile!" << endl;
    ifstream in_nodedist (nodeFN.c_str(), ios::in);
    if(in_nodedist.is_open()) {
      while(getline(in_nodedist, str)) {
	if(str.length() == 0) break;
	boost::algorithm::split(tokens, str, boost::algorithm::is_space());
	if(rgDebug) cout << "nine, readCedgeNetworkFile! getline:" << str << endl;
	nid2Point[atoi(tokens[0].c_str())] = make_pair<double,double>(atoi(tokens[1].c_str()),atof(tokens[2].c_str()));
	count++;
      }
      in_nodedist.close();
    }
  }
  cout << "-| " << nid2Point.size() << " |- " << useConcisepath << " - " << mapSize << " - " << count << endl;
  findNode2degree();
}

int RoadGraph::getFilelines(const char *filename)
{
  if(rgDebug) cout << "one, getFilelines! filename: "<<filename << endl;
  if (!filename || !*filename) // if no file to work on, return false
  return false;

  int size = 0;
  string line;

  ifstream is;
  is.open(filename);

  if (!is) // return false if couldn't open file
    return -1;

  while (!is.eof()){
    getline(is, line);	
    size++;
  }

  is.close();
  return size;
}

bool RoadGraph::getLastLine(const char *filename, string &lastLine)
{
  #define _LL_BUFFSIZE_ 2048
  if(rgDebug) cout << "one, getLastLine! filename: "<<filename << endl;
  lastLine.clear(); // regardless, zero out our return string
  if (!filename || !*filename) // if no file to work on, return false
    return false;

  char buff[_LL_BUFFSIZE_]; // our temporary input buffer

  ifstream is;
  is.open(filename);

  if (!is) // return false if couldn't open file
    return false;

  is.seekg (0, ios::end); // go to end of file
  int length = is.tellg(); // find out how large it is
  is.seekg(length-min(length,_LL_BUFFSIZE_),ios::beg); // seek back from end a short ways

  // read in each line of the file until we're done
  buff[0]=0;
  do {
    if (!isspace(buff[0]) && buff[0] != 0)
      lastLine = buff;

  } while (is.getline(buff, _LL_BUFFSIZE_));

  is.close();

  return true;
}

void RoadGraph::readSPTreeFileBinary(TestSetting& ts){
    double refTime = clock();
//   string prefixFn = ts.queryFileName;
//   prefixFn.replace ((prefixFn.size())-6, 6, "");
  string prefixFn = "VERIFYSecondRANDaalborgNEWGP10R3200";
  
    int token1,token2,token3;
  int curPoiNodeId=-1;

  string fn = ts.testFile;
  fn.replace ((fn.size())-4, 4, "node"); // extension .cnode
  cout << fn << endl;
  int mapsize = getFilelines(fn.c_str());

  prefixFn.append(".sptree"); // extension .sptree
  ifstream sptFile (prefixFn.c_str(), ios::in|ios::binary);

//  if (rgDebug)
        cout << "zero, readSPTreeFile! prefixFn: " << prefixFn << endl;

  if (sptFile.is_open()) {
//    if(rgDebug)
            cout << "one, readSPTreeFile! ";
        cout<< "@TIME1: " << ts.getElapsedTime(refTime)<< endl;

    while(!sptFile.eof()) {
      sptFile.read((char*)&token1, sizeof(int));
      sptFile.read((char*)&token2, sizeof(int));
      sptFile.read((char*)&token3, sizeof(int));
      if(token3 == -1337){
        curPoiNodeId = token1;
        spTrace[curPoiNodeId] = new int[token2];
//         trackdist[curPoiNodeId] = new int[mapsize];
      }
      spTrace[curPoiNodeId][token1] = token3;
//       trackdist[curPoiNodeId][token1] = token2;
    }
    if(rgDebug) cout << "four, readSPTreeFile! ";
    sptFile.close();
  }
  if(rgDebug) cout << "five, readSPTreeFile END!";
        cout<< "@TIME2: " << ts.getElapsedTime(refTime)<< endl;
}

intVector RoadGraph::getSPfromSPTree(int source, int target){
    intVector trace;
    int* backtrace=spTrace[target];
    if(backtrace == NULL) {
    countFail++;
//         cout << "*!SPTree s/t (" << countFail <<"/" << countSuccess <<"): "<< source <<"/" << target << "\t";
//        trace.push_back(target);
        return trace;
    }
    countSuccess++;
    int prevNode = source;
    trace.push_back(prevNode);
    while (prevNode!=target) {
        assert(prevNode!=-1);

        prevNode = backtrace[prevNode];
        trace.push_back(prevNode);
    }
    return trace;
}

//find angle between current heading (prevNode -> source) and new heading (source -> target)
double RoadGraph::getAngle(Point prevNode, Point source, Point target){
    double PI = 3.14159265359;
    double existingSlope = (prevNode.second - source.second) / (prevNode.first - source.first);
    double targetSlope = (source.second - target.second) / (source.first - target.first);
    
    double rad=atan((targetSlope-existingSlope)/(1+existingSlope*targetSlope));
    
//find if angle between prevNode and target >90 deg 
    double diffx,diffy, angle, asquare, bsquare, csquare;
    diffx = prevNode.first - target.first;
    diffy = prevNode.second - target.second;
    asquare = (diffx*diffx) + (diffy*diffy);
    
    diffx = prevNode.first - source.first;
    diffy = prevNode.second - source.second;
    bsquare = (diffx*diffx) + (diffy*diffy);
    
    diffx = source.first - target.first;
    diffy = source.second - target.second;
    csquare = (diffx*diffx) + (diffy*diffy);
    
    angle = acos((bsquare + csquare - asquare)/(2*sqrt(bsquare*csquare)));
    
    
    return 180-(angle*180/PI);
}

std::vector<int>  RoadGraph::calcConsisePath(std::vector<int>& trace){
  if(testconcisetype == CONCISETYPE_A)
    return calcConsisePathA(trace);
  else if(testconcisetype == CONCISETYPE_B)
    return calcConsisePathB(trace);
}


std::vector<int>  RoadGraph::calcConsisePathA(std::vector<int>& trace){
  int outdegree, prevNode;
  bool addnext = true, doadd = false, equalMinAngle=false;
  vector<int> concisepath;
  concisepath.push_back(trace.back());
// // //    if(conciseDebug) 
     cout << "calcConsisePathA S: " << trace.size() << " (" << trace.front() << "," << trace.back() << ")"<< endl;
  double angleToNextNode=0.0, minAngle=0.0, tmp;
  Point prev, curr;
  if(trace.empty()) cout << "EMPTY TRACE!!" << endl;
  if(trace.size() == 1) return trace;
  int degree2=0, degree2Added=0;
  
  
  ////////////////////////////////////////////
  // write out each node of full path, its nighbours and angle to each. 
  // add ** to indicate next node in path.
  ////////////////////////////////////////////
//   int nid;
//   double weight = 0.0;
//   Point tmpNode;
//   for(std::vector<int>::size_type i = trace.size()-2; i != 0; i--){
//     nid = trace[i];
//     curr = nid2Point[nid];
//     outdegree = map[nid].size();
//     cout << "NID: " << nid << ", " << outdegree << " ";
//     BOOST_FOREACH(JEdge edge, map[nid]){
//       tmpNode = nid2Point[edge.first];
//       tmp=getAngle((prev=nid2Point[trace[i+1]]), curr, tmpNode); 
//       if(edge.first == trace[i-1]) cout << "**"; //identify next node in trace
//       cout << "[" << edge.first << ", " << tmp << "](" << tmpNode.first << "," << tmpNode.second << ") ";
//     }
//     cout << endl;
    
//   }
  ////////////////////////////////////////////////////

  
  outdegree= map[trace.back()].size();
  if(outdegree < 3) degree2++;
//     for(std::vector<int>::size_type it = trace.size()-1; it != 0; it--){ 
// 	curr = nid2Point[trace[it]];
// 	cout << curr.first << " " << curr.second << endl;
//     }
  if(conciseDebug) cout << "RoadGraph::calcConsisePathA Q1" << endl;
  for(std::vector<int>::size_type i = trace.size()-2; i != 0; i--){ //size()-1 because we already added the first nodeid to concisepath
    outdegree= map[trace[i]].size();
    if(outdegree < 3) degree2++;
    if(conciseDebug) cout << "#" <<outdegree << "#";
    if(addnext || trace[i] == trace[0]){ //if only one node left add it to concisepath
//       outdegree= map[trace[i]].size();
      if(outdegree > 2){ //used in the following code to ensure current node is added to the concise path
	if(conciseDebug) cout << " *// * ";
	doadd = true;
      }else{
	concisepath.push_back(trace[i]);
	degree2Added++;
	doadd = false; //Special case can set doadd=true, but not set doadd=false after adding next node, leading to possible extra node added to concisepath.
      }
      addnext = false;
      if(conciseDebug)	cout << "1..0: (" << i << ") [" << trace[i] << ", " << concisepath.size() << "] " << outdegree << endl;
    }
    if(outdegree > 2){
      if(measureConcisepathdegrees){ //(testsetting) add nid to path based on the angle deviation from previous heading
	EdgeList& edges = map[trace[i]];
	prevNode = trace[i+1];
	prev = nid2Point[prevNode];
	curr = nid2Point[trace[i]];
	angleToNextNode=0.0;
	minAngle=std::numeric_limits<double>::max();
 	if(conciseDebug)  cout << "prev/cur Node: " << prevNode << "/" << trace[i] << " +" << nid2Point.size() << "+ " << endl;
	BOOST_FOREACH(JEdge edge, edges){
	  if(edge.first != prevNode){
	    tmp=getAngle(prev, curr, nid2Point[edge.first]);
	    if(conciseDebug) (tmp < minAngle)? (cout << "true: " << tmp <<"," << minAngle << endl) : (cout << "false: " << tmp <<"," << minAngle << "\t" << "{(" << prev.first << "," << prev.second << "),(" << curr.first << "," << curr.second << "),(" << nid2Point[edge.first].first << "," << nid2Point[edge.first].second << ")} " << prevNode << "," << trace[i] << endl);
	    if(tmp == minAngle)
	      equalMinAngle = true; //if two nodes share the same angle, and it is minAngle
	    if(tmp < minAngle)
	      minAngle = tmp;
	    if(trace[i-1] == edge.first)
	      angleToNextNode = tmp;
	  }
	  if(conciseDebug) cout <<"2..0: (" << edge.first << "," << tmp << ")[" << minAngle << "," << angleToNextNode << "]" << endl;
	}
	
	if(angleToNextNode > minAngle || equalMinAngle){
	  concisepath.push_back(trace[i]);
	  addnext = true;
	  equalMinAngle = false;
	  if(conciseDebug) cout << "3..10: (" << angleToNextNode <<", " << minAngle << ")[" << trace[i] << ", " << concisepath.size() << "]" << endl;
	  //cout << "3.10: (" << i << ") [" << trace[i] << ", " << concisepath.size() << "] " << outdegree << endl;
	}else if(doadd){ 
	  concisepath.push_back(trace[i]);
	  doadd = false;
	  if(conciseDebug) cout << "3..20: (" << i << ") [" << trace[i] << ", " << concisepath.size() << "] " << outdegree << endl;
	}
      }else{ //add node to path if outdegree larger than 2
	concisepath.push_back(trace[i]);
	addnext =true;
	if(conciseDebug) cout << "4..0: " << "[" << trace[i] << ", " << concisepath.size() << "]" << endl;
	//cout << "4.10: (" << i << ") [" << trace[i] << ", " << concisepath.size() << "] " << outdegree << endl;
      }
    }
  }
  concisepath.push_back(trace[0]); //in the loop above we put a guard i != 0, which prevents the last node from being added to the concise path.
  if(conciseDebug) cout << "5..0: [" << trace[0] << ", " << concisepath.size() << "] " << endl;

  outdegree= map[trace[0]].size();
  if(outdegree < 3){ degree2++; degree2Added++;}
  
  
  int originalDegree2 = 0; //++
  if(concisepath.empty()) cout << "EMPTY CONCISE PATH!!" << endl;
  vector<int> reverseTrace;
  for(std::vector<int>::size_type k = trace.size()-1; k < trace.size(); k--){
    reverseTrace.push_back(trace[k]);
    if(node2degree[trace[k]] < 3) originalDegree2++;
  }
//  
    vector<int> tmpRecpath;
    ////////////////////////////////////
//      if((tmpRecpath=recoverPath(concisepath)) != reverseTrace)
//        cout << "NEQ! "<< concisepath.size() << " / " << tmpRecpath.size() << " - " << reverseTrace.size() << " D2:: (" << degree2Added <<") " << degree2 << endl;
//      else
//        cout << "EQ " << concisepath.size() << " / " << reverseTrace.size() << " D2.:  (" << degree2Added <<") " << degree2 << endl;
//     
//     for(std::vector<int>::size_type i = concisepath.size()-1; i != 0; i--){cout << concisepath[i] << " "; }
//     cout << endl;
    //////////////////////////////////////
//    cout << "7..0: [" << originalDegree2 << ", " << reverseTrace.size() << "] " << endl;
  if(conciseDebug) {
//     if((tmpRecpath=recoverPath(concisepath)) != reverseTrace)
//       cout << "NEQ! " << concisepath.size() << " / " << reverseTrace.size() << " D2: (" << degree2Added <<") " << degree2 << endl;
//     else
      cout << "EQ " << concisepath.size() << " / " << reverseTrace.size() << " D2.: (" << degree2Added <<") " << degree2 << endl;
      
    for(std::vector<int>::size_type i = 0; i != concisepath.size(); i++){
      cout << concisepath[i] << " ";
    }

    cout << "\nORI: " << trace.size() << endl;
    for(std::vector<int>::size_type k = trace.size()-1; k < trace.size(); k--){
      cout << trace[k] << " ";
    }   
 
    cout << "\nREC: " << tmpRecpath.size() << endl;
    for(std::vector<int>::size_type j = 0; j != tmpRecpath.size(); j++){
      cout << tmpRecpath[j] << " ";
    }
 
    cout << "\nREVTRACE: " << reverseTrace.size() << endl;
    for(std::vector<int>::size_type z = 0; z != reverseTrace.size(); z++){
      cout << reverseTrace[z] << " ";
    }
    std::cin.ignore();
  }
  if(conciseDebug) cout << "calcConsisePath E " << concisepath.size() << endl;
  return concisepath;
}
 
 ////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////

std::vector<int>  RoadGraph::calcConsisePathB(std::vector<int>& trace){
  int outdegree, prevNode, targetNode = trace[0];
  bool addnext = true, doadd = false, equalMinAngle=false;
  vector<int> concisepath;
  concisepath.push_back(trace.back());
  if(conciseDebug) cout << "calcConsisePathB S: " << trace.size() << endl;
  double angleToNextNode=0.0, minAngle=0.0, tmp;
  Point prev, curr, target=nid2Point[targetNode];
  cout << "(" << target.first << "," << target.second << ")" << endl;
  if(trace.empty()) cout << "EMPTY TRACE!!" << endl;
  if(trace.size() == 1) return trace;
  int degree2=0, degree2Added=0;

  
    ////////////////////////////////////////////
  // write out each node of full path, its nighbours and angle to each. 
  // add ** to indicate next node in path.
  ////////////////////////////////////////////
//   int nid;
//   double weight = 0.0;
//   Point tmpNode;
//   for(std::vector<int>::size_type i = trace.size()-2; i != 0; i--){
//     nid = trace[i];
//     curr = nid2Point[nid];
//     outdegree = map[nid].size();
//     cout << "NID: " << nid << ", " << outdegree << " ";
//     BOOST_FOREACH(JEdge edge, map[nid]){
//       tmpNode = nid2Point[edge.first];
// //       tmp=getAngle(target, curr, tmpNode); 
//       tmp=getAngle(tmpNode, curr, target); 
//       if(edge.first == trace[i-1]) cout << "**"; //identify next node in trace
//       cout << "[" << edge.first << ", " << tmp << "](" << tmpNode.first << "," << tmpNode.second << ") ";
//     }
//     cout << endl;
//     
//   }
  ////////////////////////////////////////////////////
  
  outdegree= map[trace.back()].size();
  if(outdegree < 3) degree2++;
//     for(std::vector<int>::size_type it = trace.size()-1; it != 0; it--){ 
// 	curr = nid2Point[trace[it]];
// 	cout << curr.first << " " << curr.second << endl;
//     }
  
  if(conciseDebug) cout << "RoadGraph::calcConsisePathB Q1" << endl;
  for(std::vector<int>::size_type i = trace.size()-2; i != 0; i--){ //size()-1 because we already added the first nodeid to concisepath
    outdegree= map[trace[i]].size();
    if(outdegree < 3) degree2++;
    if(conciseDebug) cout << "#" <<outdegree << "#";
    if(addnext || trace[i] == trace[0]){ //if only one node left add it to concisepath
//       outdegree= map[trace[i]].size();
      if(outdegree > 2){ //used in the following code to ensure current node is added to the concise path
	doadd = true;
      }else{
	concisepath.push_back(trace[i]);
	degree2Added++;
	doadd = false; //Special case can set doadd=true, but not set doadd=false after adding next node, leading to possible extra node added to concisepath.
      }
      addnext = false;

      if(conciseDebug)	cout << "1..0: (" << i << ") [" << trace[i] << ", " << concisepath.size() << "] " << outdegree << endl;
    }
    if(outdegree > 2){
      if(measureConcisepathdegrees){ //(testsetting) add nid to path based on the angle deviation from previous heading
	EdgeList& edges = map[trace[i]];
	prevNode = trace[i+1];
	curr = nid2Point[trace[i]];
	angleToNextNode=0.0;
	minAngle=std::numeric_limits<double>::max();
	if(conciseDebug) cout << "prevNode: " << prevNode << endl;
	BOOST_FOREACH(JEdge edge, edges){
	  if(edge.first != prevNode){
	    tmp=getAngle(target, curr, nid2Point[edge.first]);
	    if(conciseDebug) (tmp < minAngle)? (cout << "true: " << tmp <<"," << minAngle << endl) : (cout << "false: " << tmp <<"," << minAngle << endl);
	    if(tmp == minAngle)
	      equalMinAngle = true; //if two nodes share the same angle, and it is minAngle
	    if(tmp < minAngle)
	      minAngle = tmp;
	    if(trace[i-1] == edge.first)
	      angleToNextNode = tmp;
	  }
	  if(conciseDebug) cout << "(" << edge.first << "," << tmp << ")[" << minAngle << "," << angleToNextNode << "]" << endl;
	}
	
	if(angleToNextNode > minAngle || equalMinAngle){
	  concisepath.push_back(trace[i]);
	  addnext = true;
	  equalMinAngle = false;
	  if(conciseDebug) cout << "3..0: (" << angleToNextNode <<", " << minAngle << ")[" << trace[i] << ", " << concisepath.size() << "]" << endl;
	  //cout << "3.10: (" << i << ") [" << trace[i] << ", " << concisepath.size() << "] " << outdegree << endl;
	}else if(doadd){
	  concisepath.push_back(trace[i]);
	  doadd = false;
	  //cout << "3.20: (" << i << ") [" << trace[i] << ", " << concisepath.size() << "] " << outdegree << endl;
	}
      }else{ //add node to path if outdegree larger than 2
	concisepath.push_back(trace[i]);
	addnext =true;
	if(conciseDebug) cout << "4..0: " << "[" << trace[i] << ", " << concisepath.size() << "]" << endl;
	//cout << "4.10: (" << i << ") [" << trace[i] << ", " << concisepath.size() << "] " << outdegree << endl;
      }
    }
  }
  concisepath.push_back(trace[0]);
  if(conciseDebug) cout << "5..0: [" << trace[0] << ", " << concisepath.size() << "] " << endl;

  outdegree= map[trace[0]].size();
  cout << "5..1 " << outdegree << endl;
  if(outdegree < 3){ degree2++; degree2Added++;}
  
  
  int originalDegree2 = 0; //++
  if(concisepath.empty()) cout << "EMPTY CONCISE PATH!!" << endl;
  vector<int> reverseTrace;
  cout << "5..2 " << outdegree << " " << degree2 << " " << degree2Added << endl;  
  for(std::vector<int>::size_type k = trace.size()-1; k < trace.size(); k--){
    reverseTrace.push_back(trace[k]);
    if(node2degree[trace[k]] < 3) originalDegree2++; //++
  }
  cout << "5..3 " << trace.size() << endl;

    vector<int> tmpRecpath;
    ////////////////////////////////////
//     tmpRecpath=recoverPath(concisepath);
//     cout << " 5..4 " << trace.size() << endl;
//     if(tmpRecpath != reverseTrace)
//       cout << "-NEQ! "<< concisepath.size() << " / " << tmpRecpath.size() << " - " << reverseTrace.size() << " D2:: (" << degree2Added <<") " << degree2 << endl;
    //////////////////////////////////////

  if(!conciseDebug) {
    if((tmpRecpath=recoverPath(concisepath)) != reverseTrace)
      cout << "NEQ! " << concisepath.size() << " / " << reverseTrace.size() << " D2: (" << degree2Added <<") " << degree2 << endl;
    else
      cout << "EQ " << concisepath.size() << " / " << reverseTrace.size() << " D2.: (" << degree2Added <<") " << degree2 << endl;
      
    for(std::vector<int>::size_type i = 0; i != concisepath.size(); i++){
      cout << concisepath[i] << " ";
    }

    cout << "\nORI: " << trace.size() << endl;
    for(std::vector<int>::size_type k = trace.size()-1; k < trace.size(); k--){
      cout << trace[k] << " ";
    }   
  
    cout << "\nREC: " << tmpRecpath.size() << endl;
    for(std::vector<int>::size_type j = 0; j != tmpRecpath.size(); j++){
      cout << tmpRecpath[j] << " ";
    }
  
//  cout << "\nREVTRACE: " << reverseTrace.size() << endl;
//  for(std::vector<int>::size_type z = 0; z != reverseTrace.size(); z++){
//    cout << reverseTrace[z] << " ";
//  }
//  std::cin.ignore();
  }
  if(conciseDebug) cout << "calcConsisePath E " << concisepath.size() << endl;
  return concisepath;
}

/*
 * Takes a concise path and returns the full paht
 * Used to check the correctness of calcConsisePath
 * takes the input trace in the order t,..,s from calcConsisePath()
 */
std::vector<int>  RoadGraph::recoverPath(std::vector<int>& conciseTrace){
  
//   if(recoverConciseDebug) 
    cout << " RoadGraph::recoverPath S " << conciseTrace.size() << endl; 
  if(recoverConciseDebug){
    cout << "CONCISE: ";
    for(std::vector<int>::size_type i = 0; i != conciseTrace.size(); i++){
      cout << conciseTrace[i] << " ";
    }
    cout << endl;
  
    //print out the coordinates for concise path for plotting
    Point curr;
    for(std::vector<int>::size_type i = 0; i != conciseTrace.size(); i++){
      curr = nid2Point[conciseTrace[i]];
      cout << curr.first << ", " << curr.second << endl;
    }
  }
 
  std::vector<int> rcPath;
  rcPath.push_back(conciseTrace[0]);
  if(recoverConciseDebug) cout << "TR: " << conciseTrace[0] << "=" << endl;
  bool added;
  double minAngle, tmp=-1.0, minAngleEdgeID;
  int prevNode, curNode;
  
  for(std::vector<int>::size_type i = 1; i != conciseTrace.size(); i++){
    if(recoverConciseDebug) cout << "§" << endl;
    added=false;
    EdgeList& elist=map[conciseTrace[i]];
    //Check if the i'th node in conciseTrace is also a neighbor node in the original path
    for(std::vector<int>::size_type k = 0; k != elist.size(); k++){
      if(elist[k].first == rcPath.back()){
	rcPath.push_back(conciseTrace[i]);
	if(recoverConciseDebug) cout << "1.0=" << rcPath.size() << "=" << conciseTrace[i] << endl;;
	added=true;
	break;
      }
    }
    
    if(recoverConciseDebug) cout << " RoadGraph::recoverPath R.02 " << conciseTrace.size() << " " << i << endl; 
    //Find and add the nodes missing from the full path
    if(!added){
      if(recoverConciseDebug) cout <<"£*1 ";
      while(!added){
	if(recoverConciseDebug) cout << "£*2" << endl; //if(rcPath.size() > 700) break;
	if(measureConcisepathdegrees){
	  if(recoverConciseDebug) cout <<"£*3.1" << endl; 
	  prevNode = rcPath[rcPath.size()-2];
	  curNode = rcPath.back();
	  EdgeList& edges = map[rcPath.back()];
	  minAngleEdgeID =-1;
	  minAngle = std::numeric_limits<double>::max();
	  if(recoverConciseDebug) cout << "third: " << endl;
	  
 	  if(recoverConciseDebug) 
	    cout << "R02.1 edge size/prevNode/curNode: " << edges.size() << " / " << prevNode << " / " << curNode << endl;
	  if(edges.size() == 2){
	    if(edges[0].first == rcPath[rcPath.size()-2]){
	      if(recoverConciseDebug) 
		cout << "R02.2.1=" << rcPath.size() << "=" << "(" << rcPath.back() << "," << rcPath[rcPath.size()-2] << "),(" << edges[0].first << "," << edges[1].first << ") " << edges[1].first << endl;
	      rcPath.push_back(edges[1].first);
	    }else{
	      if(recoverConciseDebug) 
		cout << "R02.2.2=" << rcPath.size() << "=" << "(" << rcPath.back() << "," << rcPath[rcPath.size()-2] << "),(" << edges[0].first << "," << edges[1].first << ") " << edges[0].first << endl;
	      rcPath.push_back(edges[0].first);
	    }
	  }else{  
	    if(recoverConciseDebug) cout << "(" << prevNode << "," << curNode << ") @";
	    BOOST_FOREACH(JEdge edge, edges){
	      if(edge.first != prevNode){
		tmp=getAngle(nid2Point[prevNode], nid2Point[curNode], nid2Point[edge.first]);
		if(recoverConciseDebug) (tmp < minAngle)? (cout << "true: " << tmp <<"," << minAngle << endl) : (cout << "false: " << tmp <<"," << minAngle << endl);
		if(tmp < minAngle){
		  minAngleEdgeID = edge.first;
		  minAngle = tmp;
		}
		if(recoverConciseDebug) 
		  cout << "R02.3 (" << edge.first << "," << tmp << ")[" << minAngleEdgeID << "," << minAngle << "]" << endl;
	      }
	    }   
	    if(recoverConciseDebug) cout << "fourth: " << endl;
	    rcPath.push_back(minAngleEdgeID);
	    if(recoverConciseDebug) cout << "3.0=" << rcPath.size() << "=" << minAngleEdgeID << endl;
	  }
	}else{//if measureConcisepathdegrees is false then nodes not in concise path only has outdegree of 2
	  if(recoverConciseDebug) cout <<"£*3.2" << endl; 
	  EdgeList& edges = map[rcPath.back()];
	  if(recoverConciseDebug) cout <<"£*4.2" << endl; 
	  if(edges.size() > 2){ 
	    cout << "RoadGraph::recoverPath: ERROR, node should only have outdegree 2!! " << edges.size() << endl;
	    exit(1);
	  }
	  
	 if(edges.size() == 2){
	    if(edges[0].first == rcPath[rcPath.size()-2]){
	      if(recoverConciseDebug) cout << "2.1=" << rcPath.size() << "=" << "(" << rcPath.back() << "," << rcPath[rcPath.size()-2] << "),(" << edges[0].first << "," << edges[1].first << ") " << edges[1].first << endl;
	      rcPath.push_back(edges[1].first);
	    }else{
	      if(recoverConciseDebug) cout << "2.2=" << rcPath.size() << "=" << "(" << rcPath.back() << "," << rcPath[rcPath.size()-2] << "),(" << edges[0].first << "," << edges[1].first << ") " << edges[0].first << endl;
	      rcPath.push_back(edges[0].first);
	    }
	  }
	}
 	if(recoverConciseDebug) 
	  cout << "sixth elist size/rcPath.size/rcPath.back/conciseTrace[i]: " << elist.size() << " " << rcPath.size() << " " << rcPath.back() << "," << conciseTrace[i] << endl;
	//check if the current node from concise path now can be added to the reconstructed rcPath
	for(std::vector<int>::size_type j = 0; j != elist.size(); j++){
	  if(recoverConciseDebug) cout << "R02.6 (" << elist[j].first << ")\t";
	  if(elist[j].first == rcPath.back()){
	    rcPath.push_back(conciseTrace[i]);
	    if(recoverConciseDebug) cout << "R02.7=" << rcPath.size() << "=" << conciseTrace[i] << endl;
	    added=true;
	    break;
	  }
	}if(recoverConciseDebug)  cout << endl;
      }
    }
  }
  if(recoverConciseDebug) cout << "@" << rcPath.size() << "@" << endl;
  if(recoverConciseDebug) cout << "recoverPath E " << conciseTrace.size() << endl; 
  return rcPath;
}


/*
 * To test contraction hiracies 
 * 
 */
void  RoadGraph::testCH(){ 
  // if you want to know path from 234 to 100, you may use "sp.pathTo(path, 100, -1, true, true);"

  ProcessingConstruct::ContractParameters contractParams;

  ProcessingConstruct* construct = NULL;
  UpdateableGraph* updGraph = NULL;

  ifstream in; // input file object

  
    ////////////////////////////////////////////
  //Assign input, should be coming from config file or hardcoded.
  
  string ddsgFile = "data/NY_t_0.ddsg";  // input graph file
  string hcnFile = "hcn/NY_t_0-EOS1235.hcn";// input level file

  // hop-/degree-limits specified as comma-separated list,
  // e.g. "1,3.3,2,10,3,10,5" means hop-limit 1 until degree 3.3,
  //                          then hop-limit 2 until degree 10,
  //                          then hop-limit 3 until degree 10,
  //   
  string kParam = "1,3.3,2,10,3,10,5"; //degree limits (the k parameter for CH)
  contractParams.maxHops.clear();
  Command::createVector(string(kParam),contractParams.maxHops,(double)0);
  ////////////////////////////////////////////        

  
  /* Read graph */
  in.open(ddsgFile.c_str());
  if (!in.is_open()) {
    cerr << "ddsgFile - Cannot open " << ddsgFile << endl; exit(1);
  }
  VERBOSE( cout << "Reading graph ..." << endl;)
  updGraph = importGraphListOfEdgesUpdateable(in, false, false, "");
  in.close(); in.clear();

  /* Read node levels */
  in.open(hcnFile.c_str());
  if (!in.is_open()) {
    cerr << "hcnFile - Cannot open " << hcnFile << endl; exit(1);
  }
  construct = new ProcessingConstruct(updGraph);
  VERBOSE( cout << "Reading node levels ..." << endl;)
  construct->readLevels(in);
  in.close(); in.clear();


  /* Build Contraction Hierarchy  */
  VERBOSE( cout << "Contracting graph ..." << endl;)
  construct->constructHierarchy(contractParams);
  
  Dijkstra sp(updGraph);
  Path p;
  EdgeWeight spDist;
  
  /*EdgeWeight spDist = sp.bidirSearch(0, 3);
  VERBOSE( cout << "SP Dist from " << 0 << " to " << 3 << " is " << spDist << endl; )
  sp.clear();*/
  
  
  srand(0);
  
  for (int i=0;i<100000;i++) {
    int s=rand()%230000;
    int t=rand()%230000;
    spDist = sp.bidirSearch(s, t);
    sp.pathTo(p, t, -1, true, true);
    //VERBOSE( cout << "SP Dist from " << s << " to " << t << " is " << spDist << endl; )
    if(i%1000==0) VERBOSE( cout << i << "@" << s << "," << t << " :" << spDist << endl; );
    
    //p
    
    sp.clear();
  }
  
  /*spDist = sp.bidirSearch(1, 1009);
  VERBOSE( cout << "SP Dist from " << 1 << " to " << 1009 << " is " << spDist << endl; )
  sp.clear();*/
}

void RoadGraph::setupCH(string& fn){
  ProcessingConstruct::ContractParameters contractParams;

  construct = NULL;
  updGraph = NULL;

  ifstream in; // input file object
  cout <<  "setupCH 1" << endl;
   //f parameter
  string ddsgfile = fn; //has .cedge at ending
  ddsgfile.replace ((ddsgfile.size())-5, 5, "ddsg"); // extension .ddsg
  cout <<  "setupCH 2" << endl;   
  //h parameter
  string hcnFile = ddsgfile;
  hcnFile.replace ((hcnFile.size())-4, 4, "hcn"); // extension .hcn
  cout <<  "setupCH 3" << endl;
  //k parameter:
  contractParams.maxHops.clear();
  Command::createVector("1,3.3,2,10,3,10,5",contractParams.maxHops,(double)0);
  cout <<  "setupCH 4" << endl;
  /* Read graph */
  in.open(ddsgfile.c_str());
  if (!in.is_open()) {
    cerr << ".cedge - CH Cannot open " << ddsgfile << endl; exit(1);
  }
  VERBOSE( cout << "Reading graph ..." << endl;)
  updGraph = importGraphListOfEdgesUpdateable(in, false, false, "");
  in.close(); in.clear();
  cout <<  "setupCH 5" << endl;
  /* Read node levels */
  in.open(hcnFile.c_str());
  if (!in.is_open()) {
    cerr << "hcnFile - Cannot open " << hcnFile << endl; exit(1);
  }
  construct = new ProcessingConstruct(updGraph);
  VERBOSE( cout << "Reading node levels ..." << endl;)
  construct->readLevels(in);
  in.close(); in.clear();

  /* Build Contraction Hierarchy  */
  VERBOSE( cout << "Contracting graph ..." << endl;)
  construct->constructHierarchy(contractParams); 

  VERBOSE( cout << "new Dijkstra(updGraph) ..." << endl;)  
  sp = new Dijkstra(updGraph);
}


// doesn't look nice, but required by the compiler (gcc 4)
const EdgeWeight Weight::MAX_VALUE;
const EliminationWeight::Type EliminationWeight::MAX_VALUE = __DBL_MAX__;
const EliminationWeight::Type EliminationWeight::MIN_VALUE = -__DBL_MAX__;
const EdgeWeight Weight::MAX_INTEGER;
const int datastr::graph::UpdateableGraph::LOOK_FOR_SECOND_EDGE_BACKWARD;
const int datastr::graph::UpdateableGraph::LOOK_FOR_SECOND_EDGE_FORWARD;

#endif
