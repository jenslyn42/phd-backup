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
boost::unordered_map<int, int*> RoadGraph::trackdist;
////////////////////////////////////////////////////////////////////////////////////////////////////

RoadGraph* RoadGraph::mapInstance = NULL;

RoadGraph* RoadGraph::mapObject(TestSetting& ts)
{
	// obtain parameter values
	std::string testFile=ts.getTestFile();
	int pt=ts.inputFileType;
	
	if (mapInstance==NULL){
		mapInstance = new RoadGraph();
		mapInstance->parseFileType = pt;
		mapInstance->ssspCalls = 0;
		mapInstance->numNodeVisits = 0;
		mapInstance->readSPTreeFileBinary(ts);
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
	if(!trace.empty()) return trace;
	
	
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
			return trace;
		}
	}
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

///fileformat .cedge: 0 0 1 1.182663
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

	while (!is.eof())
	{
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
	string prefixFn = ts.queryFileName;
	prefixFn.replace ((prefixFn.size())-6, 6, "");
    int token1,token2,token3;
	int curPoiNodeId=-1;

	string fn = prefixFn;
	fn.append(".cnode"); // extension .cnode
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
                trackdist[curPoiNodeId] = new int[mapsize];
			}
            spTrace[curPoiNodeId][token1] = token3;
            trackdist[curPoiNodeId][token1] = token2;
		}
//		if(debug)
            cout << "four, readSPTreeFile! ";
		sptFile.close();
	}
//	if(debug)
        cout << "five, readSPTreeFile END!";
        cout<< "@TIME2: " << ts.getElapsedTime(refTime)<< endl;
	
	cout << "query 1 " << spTrace[103973] <<" ;" << endl;
	cout << "query 1 " << spTrace[15929] <<" ;" << endl;
	cout << "query 6 " << spTrace[119270] <<" ;" << endl;
	cout << "query 6 " << spTrace[98515] <<" ;" << endl;

}

intVector RoadGraph::getSPfromSPTree(int source, int target){
	intVector trace;
    int* backtrace=spTrace[target];
    if(backtrace == NULL) {
        cout << "*!IN SPTree s/t: " << source <<"/" << target << "\t";
//        trace.push_back(target);;
        return trace;
    }
    cout << " (**||**) ";
    int prevNode = source;
    trace.push_back(prevNode);
    while (prevNode!=target) {
        assert(prevNode!=-1);

        prevNode = backtrace[prevNode];
        trace.push_back(prevNode);
    }
    return trace;
}
////////////////////////////////////////////////////////////////////////////////////////////////////



#endif
