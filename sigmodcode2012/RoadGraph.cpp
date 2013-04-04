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
#define debug false
//specifically for the dijkstraSSSP() method
#define spDebug false


boost::unordered_map<int, int*> RoadGraph::spTrace;
// boost::unordered_map<int, int*> RoadGraph::trackdist;
////////////////////////////////////////////////////////////////////////////////////////////////////

RoadGraph* RoadGraph::mapInstance = NULL;

RoadGraph* RoadGraph::mapObject(TestSetting& ts)
{
	// obtain parameter values
	std::string testFile=ts.getTestFile();
	int pt=ts.inputFileType;
	
	if (mapInstance==NULL){
		mapInstance = new RoadGraph();
		mapInstance->useConcisepath = ts.useConcisepath;
		if(ts.useConcisepath) mapInstance->measureConcisepathdegrees = ts.measureConcisepathdegrees;
		mapInstance->parseFileType = pt;
		mapInstance->ssspCalls = 0;
		mapInstance->numNodeVisits = 0;
		mapInstance->readSPTreeFileBinary(ts);
		mapInstance->countSuccess=0;
		mapInstance->countFail=0;
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
	} else if((mapInstance->parseFileType) != pt)
	{
		delete mapInstance;
		mapInstance = NULL; //if type of file to be parsed changes, delete mapInstance
	}
	
	return mapInstance;
}

vector<int> RoadGraph::dijkstraSSSP(int source, int dest) {
	std::vector<int> trace;
	
	trace = getSPfromSPTree(source, dest);
	if(!trace.empty()){
	  if(useConcisepath)
	    return calcConsisePath(trace);
	  else 
	    return trace;
	}
	
	ssspCalls++;
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
		isInit=true;	// be careful with this line!
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
		backtrace[curnode]=he.prev_id;	// prev node
		numNodeVisits++;	// usage info
		
		EdgeList& CurAdjList=map[curnode];
		BOOST_FOREACH (Edge neighbour, CurAdjList)
		{
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
	if(debug) cout << "s1, transformTrainOrTestFile!"<< endl;
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
	if(debug) cout << "s1, transformTrainOrTestFile!"<< endl;
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
	if(debug) cout << "s1, transformTrainOrTestFile!"<< endl;
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

void RoadGraph::addEdge(int v1, int v2, double w) {
	assert( v1>=0 && v1<mapSize );
	assert( v2>=0 && v2<mapSize );
	
	EdgeList& eList1=map[v1];
	EdgeList& eList2=map[v2];
	
	bool isFound=false;
	BOOST_FOREACH (Edge e, eList1) {
		if (e.first==v2)
			isFound=true;
	}
	BOOST_FOREACH (Edge e, eList2) {
		if (e.first==v1)
			isFound=true;
	}
		
	if (!isFound) {	// edge not inserted before
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
		if(debug) cout << "zero, readNetworkFile!" << endl;
		getline(in_data, str);
		mapSize = atoi(str.c_str());
		
		map=new EdgeList[mapSize];
		if(debug) cout << "one, readNetworkFile! map: " << mapSize << endl;
		
		getline(in_data, str);
		edges = atoi(str.c_str());

		while( tokens.size() < 3 )
		{
			getline(in_data, str);
			if(debug) cout << "three, readNetworkFile! getline:" << str << endl;

			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
		}

		if(debug) cout << "four, readNetworkFile! tokens ([0],[1],[2]): (" << atoi(tokens[0].c_str()) << "," << atoi(tokens[1].c_str()) << "," << atof(tokens[2].c_str()) << ")" << endl;

		addEdge(atoi(tokens[0].c_str()),atoi(tokens[1].c_str()),atof(tokens[2].c_str()));

		int check = 0;
		while(getline(in_data, str))
		{
			if(debug) cout << "five, readNetworkFile! getline:" << str  << " check: " << check++ << endl;
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
			if(debug) cout << "five1, readNetworkFile! getline:" << str  << " check: "  << check << endl;
			addEdge(atoi(tokens[0].c_str()),atoi(tokens[1].c_str()),atof(tokens[2].c_str()));
			if(debug) cout << "five2, readNetworkFile! getline:" << str  << " check: "  << check << endl;
		}
		if(debug) cout << "six, readNetworkFile!" << endl;
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
		if(debug) cout << "zero, readPPINetworkFile!" << endl;
		getline(in_data, str); //read first line comment
		getline(in_data, str);
		boost::algorithm::split(tokens, str, boost::algorithm::is_space()); //read the size of the two node sets
		mapSize = atoi(tokens[0].c_str()) + atoi(tokens[1].c_str());
		woffset = atoi(tokens[0].c_str());	
		
		map=new EdgeList[mapSize];
		if(debug) cout << "one, readPPINetworkFile! map: " << mapSize << endl;
		
		while(getline(in_data, str))
		{
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
			for(uint i = 1; i< tokens.size(); i++)
				addEdge(atoi(tokens[0].c_str()),atoi(tokens[i].c_str())+woffset,1.0); //default weight of 1 since data does not provide any edge weights
		}
		if(debug) cout << "six, readPPINetworkFile!" << endl;
		in_data.close();
	}
}

/// fileformat .cedge: linenum nid1 nid2 distance
/// e.g.: 0 0 1 1.182663
/// fileformat .cnode: nid x y
void RoadGraph::readCedgeNetworkFile(string fn)
{
	filename = fn;
	string str;
	string nodeFN = fn;
	nodeFN.replace ((nodeFN.size()-4), 4, "node"); //change file extention from .cedge to .cnode
	std::vector<string> tokens;
	
	ifstream in_data (fn.c_str(), ios::in);
	if(debug) cout << "s1, readCedgeNetworkFile! nodeFN: " <<nodeFN << endl;
	if(in_data.is_open())
	{
		if(debug) cout << "zero, readCedgeNetworkFile! int maxsize: " << INT_MAX << endl;
 		mapSize = getFilelines(nodeFN.c_str()); //find number of lines in file

		map=new EdgeList[mapSize];
		if(debug) cout << "three, readCedgeNetworkFile! map: " << mapSize << endl;

		while(getline(in_data, str))
		{
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
			if(debug) cout << "five1, readCedgeNetworkFile! getline:" << str << endl;
			addEdge(atoi(tokens[1].c_str()),atoi(tokens[2].c_str()),atof(tokens[3].c_str()));
			if(debug) cout << "five2, readCedgeNetworkFile! getline:" << str << endl;
		}
		if(debug) cout << "six, readCedgeNetworkFile!" << endl;
		in_data.close();
	}
	
	if(useConcisepath){
		if(debug) cout << "seven, readCedgeNetworkFile!" << endl;
		ifstream in_nodedist (nodeFN.c_str(), ios::in);
		if(in_nodedist.is_open())
		{
			while(getline(in_nodedist, str))
			{
				boost::algorithm::split(tokens, str, boost::algorithm::is_space());
				if(debug) cout << "nine, readCedgeNetworkFile! getline:" << str << endl;
				nid2Point[atoi(tokens[0].c_str())] = make_pair<double,double>(atoi(tokens[1].c_str()),atof(tokens[2].c_str()));
			}
			in_nodedist.close();
		}
	}
}

int RoadGraph::getFilelines(const char *filename)
{
	if(debug) cout << "one, getFilelines! filename: "<<filename << endl;
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
	if(debug) cout << "one, getLastLine! filename: "<<filename << endl;
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
// 	string prefixFn = ts.queryFileName;
// 	prefixFn.replace ((prefixFn.size())-6, 6, "");
	string prefixFn = "VERIFYSecondRANDaalborgNEWGP10R1600";
	
    int token1,token2,token3;
	int curPoiNodeId=-1;

	string fn = ts.testFile;
	fn.replace ((fn.size())-4, 4, "node"); // extension .cnode
	cout << fn << endl;
	int mapsize = getFilelines(fn.c_str());

	prefixFn.append(".sptree"); // extension .sptree
	ifstream sptFile (prefixFn.c_str(), ios::in|ios::binary);

//	if (debug)
        cout << "zero, readSPTreeFile! prefixFn: " << prefixFn << endl;

	if (sptFile.is_open()) {
//		if(debug)
            cout << "one, readSPTreeFile! ";
        cout<< "@TIME1: " << ts.getElapsedTime(refTime)<< endl;

		while(!sptFile.eof()) {
		  sptFile.read((char*)&token1, sizeof(int));
		  sptFile.read((char*)&token2, sizeof(int));
		  sptFile.read((char*)&token3, sizeof(int));
		  if(token3 == -1337){
		    curPoiNodeId = token1;
		    spTrace[curPoiNodeId] = new int[token2];
// 		    trackdist[curPoiNodeId] = new int[mapsize];
		  }
		  spTrace[curPoiNodeId][token1] = token3;
// 		  trackdist[curPoiNodeId][token1] = token2;
		}
		if(debug) cout << "four, readSPTreeFile! ";
		sptFile.close();
	}
	if(debug) cout << "five, readSPTreeFile END!";
        cout<< "@TIME2: " << ts.getElapsedTime(refTime)<< endl;
}


intVector RoadGraph::getSPfromSPTree(int source, int target){
    intVector trace;
    int* backtrace=spTrace[target];
    if(backtrace == NULL) {
	countFail++;
        cout << "*!SPTree s/t (" << countFail <<"/" << countSuccess <<"): "<< source <<"/" << target << "\t";
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
double RoadGraph::getAngle(Point prevNode, Point source, Point target)
{
    double PI = 3.14159265359;
    double existingSlope = (prevNode.second - source.second) / (prevNode.first - source.first);
    double targetSlope = (source.second - target.second) / (source.first - target.first);
    
    double rad=atan((targetSlope-existingSlope)/(1+existingSlope*targetSlope));
    
//find if angle between prevNode and target >90 deg 
    int diffx,diffy;
    double a,b,c, angle;
    diffx = prevNode.first - target.first;
    diffy = prevNode.second - target.second;
    a = sqrt((diffx*diffx) + (diffy*diffy));
    
    diffx = prevNode.first - source.first;
    diffy = prevNode.second - source.second;
    b = sqrt((diffx*diffx) + (diffy*diffy));
    
    diffx = source.first - target.first;
    diffy = source.second - target.second;
    c = sqrt((diffx*diffx) + (diffy*diffy));
        
    angle = acos((b*b + c*c - a*a)/(2*b*c));
    
    if(angle < 90)
	return abs(rad/PI*180.0)+180;
   
    return abs(rad/PI*180.0); //only works for 180 degree, fix with check if 180 should be added.
}



std::vector<int>  RoadGraph::calcConsisePath(std::vector<int>& trace){
	
  int somePath[] = {24508, 24509, 24510, 7929, 7930, 16220, 16221, 24101, 24105, 24106, 24097, 11615, 24085, 24086, 23885, 24127, 24128, 16138, 24403, 24461, 24493, 24549, 24603, 24664, 47716, 37986, 38542, 38421, 38422, 38423, 38424, 38425, 38295, 38296, 38297, 38261, 38032, 38033, 38034, 38120, 38309, 38630, 100973, 100974, 100975, 100976, 69376, 69377, 83443, 106571, 106838, 106159, 107043, 107042, 107041, 107040, 129571, 106532, 129589, 107314, 107315, 107316, 86315, 70760, 70886, 70885, 70884, 107701, 107106, 107902, 108076, 108093, 108188, 59112, 59111, 108367, 108368, 71505, 71506, 87264, 87262, 87263, 99711, 87323, 87324, 108381, 22732, 22730, 27781, 22705, 22704, 22701, 10796, 9704, 9705, 9706, 11842, 30114, 30115, 18435, 27782, 27783, 27785, 30071, 22629, 22627, 32669, 13938, 22574, 22425, 22426, 12746, 29843, 22286, 22143, 34639, 22181, 22068, 21996, 32121, 21773, 21774, 21632, 21633, 31713, 105881, 105926, 44779, 44778, 44777, 44776, 44775, 44774, 43148, 43149, 75318, 40057, 40058, 74608, 39589, 39588, 42691, 41675, 49522, 63281, 63280, 63279, 63278, 41119, 41118, 86168, 50699, 99062, 99061, 99060, 57214, 57215, 127704, 98550, 129171, 129170, 97985, 97986, 62581, 62582, 62583, 67813, 67812, 67811, 67810, 67660, 67809, 42267, 40981, 38335, 38336, 38636, 38289, 38288, 38186, 38187, 48099, 48098, 51118, 45360, 39690, 93239, 46407, 93137, 93138, 126884, 126885, 93081, 92963, 92964, 49964, 92365, 38402, 38403, 38192, 38193, 38495, 12574, 12575, 16776, 16614, 15942, 16435, 15905, 16275, 28796, 16036, 16039, 15989, 15963, 15929, 15928, 15853, 15806, 15644, 15628, 15569, 15512, 19535, 19536, 66540, 129235, 53321, 129227, 53251, 53252, 59958, 40261, 40262, 39669, 86544, 86545, 86546, 70063, 70064, 70065, 70066, 70067, 70068, 70069, 129326, 57479, 129324, 127392, 40670, 21593, 21592, 38255, 27056, 35254, 12876, 26970, 38669, 16769, 14539, 12091, 12211, 15252, 38042, 38434, 13603, 37993, 13195, 30686, 10707, 26157, 30538, 38655, 38654, 25982, 38540, 38541, 18064, 17939, 38252, 38579, 129458, 128184, 126890, 127703, 127499, 127584, 38353, 38278, 34184, 13279, 13278, 127574, 127493, 127492, 129091, 128618, 24216, 126847, 15730, 11364, 11365, 9194, 7030, 7031, 15057, 10696, 15459, 11991, 15339, 9310, 9311, 28268, 9440, 9441, 9442, 26105, 26106, 28132, 11848, 11847, 10263, 10262, 9117, 11203, 11202, 13299, 13298, 13070, 13069, 7044, 7043, 16863, 21291, 7547, 10012, 9102, 9101, 11926, 24184, 24185, 11010, 7919, 7920, 38417, 38418, 38559, 126947, 126948, 128547, 129051, 128549, 127168, 127169, 6835, 127852, 128356, 127340, 13073, 127293, 127255, 7088, 126857, 33045, 129470, 9623, 128176, 10278, 127846, 38382, 38240, 38239, 38539, 38538, 23179, 38552, 38580, 126886, 127061, 127060, 128399, 128396, 128323, 128322, 128083, 129358, 129357, 129525, 129524, 129523, 32286, 127115, 126994, 126861, 126860, 127938, 127403, 128314, 128315, 38476, 37645, 38623, 38344, 8568, 31196, 7746, 38512, 37535, 34170, 38058, 38585, 129021, 127445, 14761, 127136, 30273, 126938, 30352, 128431, 128697, 38878, 25741, 38776, 8113, 127730, 127834, 127835, 29693, 128266, 38936, 38870, 38871, 39271, 63424, 127166, 63419, 129424, 76280, 62752, 62753, 62636, 74859, 74860, 74821, 62014, 61435, 38348, 38347, 61434, 127404, 61422, 127280, 60602, 38213, 38212, 38211, 38460, 38068, 38067, 38124, 27901, 27900, 22507, 22506, 22505, 25550, 13684, 13683, 27246, 27244, 33628, 33627, 26596, 26595, 55240, 126267, 127459, 122842, 127031, 101920, 69791, 101742, 122556, 122477, 52539, 52538, 51100, 65265, 128572, 128573, 128574, 127995, 128548, 125959, 125958, 128378, 64975, 127484, 58723, 126859, 58465, 56475, 53808, 53807, 53806, 53805, 53804, 49632, 77401, 129202, 129198, 129197, 127362, 127568, 56499, 48612, 47883, 47856, 75022, 82708, 47038, 47039, 74477, 124577, 124576, 73684, 53749, 47423, 47424, 44764, 44765, 38450, 38030, 38029, 38592, 108365, 71243, 71242, 71241, 71240, 71239, 71238, 71237, 57564, 57563, 57562, 57561, 38803, 38802, 38801, 38902, 38892, 38891, 38907, 97483, 45048, 45047, 66754, 53887, 81540, 81539, 81538, 65948, 65921, 52616, 52617, 65669, 128995, 126987, 94841, 94840, 94839, 94838, 64744, 64743, 50416, 50415, 50414, 50413, 50412, 50411, 76941, 76940, 76939, 76938, 76937, 53204, 104739, 42626, 42625, 42624, 45579, 50358, 50357, 60167, 60168, 59681, 59589, 87789, 118956, 118955, 118954, 54325, 54324, 118120, 118119, 118118, 118117, 118116, 118115, 63465, 63464, 63463, 117528, 117527, 117526, 83153, 83154, 83155, 83156, 83157, 83158, 83159, 26653, 26654, 12001, 12002, 14010, 14011, 14012, 19489, 19490, 19491, 19492, 60169, 66454, 51320, 51321, 41924, 41923, 41922, 41921, 41920, 48822, 48821, 48820, 48819, 43965, 112650, 112649, 112648, 75674, 75673, 75672, 75671, 75670, 54634, 54635, 54636, 54637, 54638};
 
  //54638 54637 24509 
  std::vector<int> examplePath (somePath, somePath + sizeof(somePath) / sizeof(int) );
  trace = examplePath;
  
	int outdegree, prevNode;
	bool addnext = true;
	vector<int> concisepath;
	concisepath.push_back(trace.back());
	double angleToNextNode=0.0, minAngle=0.0, tmp;
	Point prev, curr;


	for(std::vector<int>::size_type i = trace.size()-2; i != 0; i--){ //size()-1 because we already added the first nodeid to concisepath
		if(addnext || i==1){ //if only one node left add it to concisepath
			concisepath.push_back(trace[i]);
			addnext = false;
		}else if(outdegree= map[trace[i]].size() > 2){
			if(measureConcisepathdegrees){ //add nid to path based on the angle deviation from previous heading
				EdgeList& edges = map[trace[i]];
				prevNode = trace[i-1];
				prev = nid2Point[prevNode];
				curr = nid2Point[trace[i]];
				angleToNextNode=0.0;
				minAngle=std::numeric_limits<double>::max();
				BOOST_FOREACH(Edge edge, edges){
					if(edge.first != prevNode){
						if(tmp=getAngle(prev, curr, nid2Point[edge.first]) < minAngle)
							minAngle = tmp;
						if(trace[i+1] == edge.first)
							angleToNextNode = tmp;
					}	  
				}
				
				if(angleToNextNode > minAngle){
					concisepath.push_back(trace[i]);
					addnext = true;
				}
			}else{ //add node to path if outdegree larger than 2
				concisepath.push_back(trace[i]);
				addnext =true;
			}
		}
	}
	vector<int> tmpRecpath;
	if((tmpRecpath=recoverPath(concisepath)) == trace)
	  cout << "EQ ";
	else{
	  cout << "\nNEQ! ";
	  for(std::vector<int>::size_type i = 0; i != concisepath.size(); i++){
	    cout << concisepath[i] << " ";
	  }
	  cout << endl;
	  
	  cout << "ORI: ";
	  for(std::vector<int>::size_type k = 0; k != trace.size(); k++){
	    cout << trace[k] << " ";
	  }
	  cout << endl;	   
	  
	   cout << "REC: ";
	  for(std::vector<int>::size_type j = 0; j != tmpRecpath.size(); j++){
	    cout << tmpRecpath[j] << " ";
	  }
	  cout << endl;	   
	}
	std::cin.ignore();
	return concisepath;
}



//takes the input trace in the order t,..,s from calcConsisePath()
std::vector<int>  RoadGraph::recoverPath(std::vector<int>& conciseTrace){
 //check with map  
 
  std::vector<int> rcPath;
  rcPath.push_back(conciseTrace[0]);
  cout << "TR: " << conciseTrace[0] << "=" << endl;
  bool added;
  double minAngle, tmp, minAngleEdgeID;
  int prevNode, curNode;
  
  for(std::vector<int>::size_type i = 1; i != conciseTrace.size(); i++){
    cout << "¾" << endl;
    added=false;
    EdgeList& elist=map[conciseTrace[i]];
    //Check if the i'th node in conciseTrace is also a neighbor node in the original path
    for(std::vector<int>::size_type k = 0; k != elist.size(); k++){
      if(elist[k].first == rcPath.back()){
	rcPath.push_back(conciseTrace[i]);
	cout << "1.0=" << rcPath.size() << "=" << conciseTrace[i] << endl;;
	added=true;
	break;
      }
    }
    //Find and add the nodes missing from the full path
    if(!added){
      cout <<"£";
      while(!added){
	cout << "&";
	if(measureConcisepathdegrees){
	  prevNode = rcPath[rcPath.size()-2];
	  curNode = rcPath.back();
	  EdgeList& edges = map[rcPath.back()];
	  minAngleEdgeID =-1;
	  minAngle = std::numeric_limits<double>::max();
	  cout << "third: " << endl;
	  
	  cout << "edge size: " << edges.size() << endl;
	  if(edges.size() == 2){
	    if(edges[0].first == rcPath[rcPath.size()-2]){
	      cout << "2.1=" << rcPath.size() << "=" << "(" << rcPath.back() << "," << rcPath[rcPath.size()-2] << "),(" << edges[0].first << "," << edges[1].first << ") " << edges[1].first << endl;
	      rcPath.push_back(edges[1].first);
	    }else{
	      cout << "2.2=" << rcPath.size() << "=" << "(" << rcPath.back() << "," << rcPath[rcPath.size()-2] << "),(" << edges[0].first << "," << edges[1].first << ") " << edges[0].first << endl;
	      rcPath.push_back(edges[0].first);
	    }
	  }else{  
	    BOOST_FOREACH(Edge edge, edges){
	      if(edge.first != prevNode){
		if(tmp=getAngle(nid2Point[prevNode], nid2Point[curNode], nid2Point[edge.first]) < minAngle){
		  minAngleEdgeID = edge.first;
		  minAngle = tmp;
		}
	      }
	    }   
	    cout << "fourth: " << endl;
	    rcPath.push_back(minAngleEdgeID);
	    cout << "3.0=" << rcPath.size() << "=" << minAngleEdgeID << endl;
	  }
	}else{//if measureConcisepathdegrees is false then nodes not in concise path only has outdegree of 2
 	  EdgeList& edges = map[rcPath.back()];
	  if(edges.size() > 2) cout << "RoadGraph::recoverPath: ERROR, node should only have outdegree 2!! " << edges.size() << endl;
	  exit(1);
 	  if(edges[0].first == rcPath[rcPath.size()-2]){
 	    rcPath.push_back(edges[1].first);
 	    cout <<"4.1=" << rcPath.size() << "=" << conciseTrace[1] << endl;
 	  }else{
 	    rcPath.push_back(edges[0].first);
 	    cout << "4.2=" << rcPath.size() << "=" << conciseTrace[0] << endl;
 	  }
 	}
 	cout << "sixth" << endl;
	//check if the current node from concise path now can be added to the reconstructed rcPath
	for(std::vector<int>::size_type j = 0; j != elist.size(); j++){
	  if(elist[j].first == rcPath.back()){
	    rcPath.push_back(conciseTrace[i]);
	    cout << "5.0=" << rcPath.size() << "=" << conciseTrace[i] << endl;
	    added=true;
	    break;
	  }
	}
      }
    }
  }
  cout << "@" << rcPath.size() << "@" << endl;
  return rcPath;
}


////////////////////////////////////////////////////////////////////////////////////////////////////



#endif
