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




//  March 09	(10% difference)
//const bool isArrayInitOnce=true;
const bool isArrayInitOnce=false;

////////////////////////////////////////////////////////////////////////////////////////////////////



double lowerboundDist(Point* nodecoord,int nidA,int nidB) {
	double xdiff = nodecoord[nidA].first - nodecoord[nidB].first ;
	double ydiff = nodecoord[nidA].second - nodecoord[nidB].second ;
	return sqrt( xdiff*xdiff + ydiff*ydiff );
}


RoadGraph* RoadGraph::mapInstance = NULL;

RoadGraph* RoadGraph::mapObject(TestSetting& ts)
{
	// obtain parameter values
	int pt=ts.inputFileType;

	if (mapInstance==NULL){
		std::string testFilePrefix=ts.testFilePrefix;

		mapInstance = new RoadGraph();
		mapInstance->parseFileType = pt;
		mapInstance->ssspCalls = 0;
		mapInstance->numNodeVisits = 0;
printf("*** RoadGraph::read\n");

		cout << "[mapObject] testFilePrefix, parseFileType: " << testFilePrefix << " " << mapInstance->parseFileType << endl;

		switch( (mapInstance->parseFileType) ) {
			case 1:
				mapInstance->readRoadNetworkFile(testFilePrefix);
				break;
			case 2:
				mapInstance->readPPINetworkFile(testFilePrefix);
				break;
			case 3:
				mapInstance->readCedgeNetworkFile(testFilePrefix);
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

intVector RoadGraph::computeSP(TestSetting& ts,int s, int t) {
	intVector trace;
	RoadGraph* ins=mapObject(ts);

	if (ins==NULL)
		return trace;


	ins->ssspCalls++;
	ins->lastNodeVisits=0;

	if ( s<0 || t<0 || s >= ins->mapSize || t >= ins->mapSize ) {
		cout << "Invalid src/dest nodes, map\n";
		return trace;
	} else if (s==t) {
		trace.push_back(s);
		return trace;
	}


	if (ts.useDijkstra)
		trace = ins->dijkstraSP(s,t);
	else
		trace = ins->astarSP(s,t);


	return trace;
}

intVector RoadGraph::astarSP(int source, int dest) {	// March 7
	intVector trace;

	if(spDebug) cout << "one, dijkstraSSSP! " << endl;


	const double MAXREAL=1.0e10;	// hard-coded constant, OK for most maps
	double bestSPdist=MAXREAL;

	// gdist,hdist

	static int _mapsize=0;
	static int* backtrace=NULL;
	static double* ndists=NULL;

	// March 09
	if (isArrayInitOnce==false) {
		if (ndists!=NULL)
			delete[] ndists;
		if (backtrace!=NULL)
			delete[] backtrace;
		ndists=NULL;
		backtrace=NULL;
	}

	if (ndists==NULL) {
		_mapsize=getMapsize();
		backtrace=new int[_mapsize];
		ndists=new double[_mapsize];
		//printf("*** RoadGraph::init array\n");
	} else
		assert(_mapsize==getMapsize());

	// init
	fill( backtrace, backtrace+_mapsize, -1);
	fill( ndists, ndists+_mapsize, MAXREAL);

	//insert source node
	Heap hp;
	{
		HeapEntry root_he;
		root_he.id=source;

		root_he.gdist=0;	// dist. so far
		root_he.hdist=lowerboundDist(nodecoord,source,dest);
		root_he.dist=root_he.gdist+root_he.hdist;    // LB dist to destination

		root_he.prev_id=-1;
		hp.push(root_he);
	}

	if(spDebug) cout << "six, dijkstraSSSP! " << endl;

	while (hp.size()>0) {
		HeapEntry he=hp.top();
		hp.pop();

		int curnode=he.id;
		if (he.gdist>=ndists[curnode])  // current dist pruning, global dist pruning
			continue;
		if (he.dist>=bestSPdist)
			break;


		ndists[curnode]=he.gdist;
		backtrace[curnode]=he.prev_id;	// prev node
		numNodeVisits++;	// usage info
		lastNodeVisits++;

		if (curnode==dest) {
			bestSPdist=he.gdist;
			continue;
		}


		EdgeList& CurAdjList=map[curnode];
		BOOST_FOREACH (Edge neighbour, CurAdjList)
		{
			int NextNodeID = neighbour.first; //id
			double NextWeight = neighbour.second;

			HeapEntry new_he=he;    // copy ...
			new_he.id=NextNodeID;

			new_he.gdist=he.gdist+NextWeight; //weight
			new_he.hdist=lowerboundDist(nodecoord,NextNodeID,dest);
			new_he.dist=new_he.gdist+new_he.hdist;
			new_he.prev_id=curnode;

			if (new_he.gdist<ndists[NextNodeID])
				hp.push(new_he);    // propagation
		}
	}


	//printf("*** %f %d\n",bestSPdist,lastNodeVisits);
	//exit(0);

	if (bestSPdist<MAXREAL) {
		int prevNode = dest;
		trace.push_back(prevNode);
		while (prevNode!=source) {
			assert(prevNode!=-1);
			prevNode = backtrace[prevNode];
			trace.push_back(prevNode);
		}
		return trace;
	}



	return trace;
}

// Ken: updated the correctness issue [Feb 13]
intVector RoadGraph::dijkstraSP(int source, int dest) {
	intVector trace;


	if(spDebug) cout << "one, dijkstraSSSP! " << endl;


	static int _mapsize=0;
	static int* backtrace=NULL;
	static bool* isVisited=NULL;


	// March 09
	if (isArrayInitOnce==false) {
		if (isVisited!=NULL)
			delete[] isVisited;
		if (backtrace!=NULL)
			delete[] backtrace;
		isVisited=NULL;
		backtrace=NULL;
	}

	if (isVisited==NULL) {
		_mapsize=getMapsize();
		backtrace=new int[_mapsize];
		isVisited=new bool[_mapsize];
		//printf("*** RoadGraph::init array\n");
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
		lastNodeVisits++;

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
/*void RoadGraph::transformTrainOrTestFile(string cnodeFn, string trainTestFn)
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
*/

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
	filePrefix = fn;

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
	filePrefix = fn;

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
			//if(debug) cout << "five, readPPINetworkFile! getline:" << str  << " check: " << check++ << endl;
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
			for(uint i = 1; i< tokens.size(); i++)
			{
				//if(debug) cout << "five1, readPPINetworkFile! getline:" << str << " check: " << check<< endl;
				addEdge(atoi(tokens[0].c_str()),atoi(tokens[i].c_str())+woffset,1.0); //default weight of 1 since data does not provide any edge weights
				//if(debug) cout << "five2, readPPINetworkFile! getline:" << str << " check: " << check<< endl;
			}
		}
		if(debug) cout << "six, readPPINetworkFile!" << endl;
		in_data.close();
	}
}

void RoadGraph::readCedgeNetworkFile(string fn) {
///fileformat .cedge: 0 0 1 1.182663
	filePrefix = fn;


	string nodeFN = fn;
	string edgeFN = fn;

	nodeFN.append(".cnode"); // extension .cnode
	edgeFN.append(".cedge"); // extension .cedge

	string str;
	std::vector<string> tokens;

	ifstream edgeFile (edgeFN.c_str(), ios::in);

	if (debug) cout << "s1, readCedgeNetworkFile! edgeFN: " << edgeFN << endl;

	if (edgeFile.is_open()) {
		if(debug) cout << "zero, readCedgeNetworkFile! int maxsize: " << INT_MAX << endl;

 		mapSize = getFilelines(nodeFN.c_str()); //find number of lines in file

		map = new EdgeList[mapSize];
		if(debug) cout << "three, readCedgeNetworkFile! map: " << mapSize << endl;

		while(getline(edgeFile, str)) {
// 			cout << "five, readCedgeNetworkFile! getline:" << str  << " check: " << endl;
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
			if(debug) cout << "five1, readCedgeNetworkFile! getline:" << str << endl;
			addEdge(atoi(tokens[1].c_str()),atoi(tokens[2].c_str()),atof(tokens[3].c_str()));
			if(debug) cout << "five2, readCedgeNetworkFile! getline:" << str << endl;
		}
		if(debug) cout << "six, readCedgeNetworkFile!" << endl;
		edgeFile.close();
	}


	nodecoord=new Point[mapSize];

	ifstream nodeFile (nodeFN.c_str(), ios::in);
	if (nodeFile.is_open()) {
		while(getline(nodeFile, str)) {
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

			int nid=atoi(tokens[0].c_str());
			assert(nid>=0&&nid<mapSize);

			nodecoord[nid].first = atof(tokens[1].c_str());
			nodecoord[nid].second = atof(tokens[2].c_str());
		}
		nodeFile.close();
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
	// uncomment if you want to skip empty lines or lines that start with whitespace
	// fancier logic is probably called for

	if (!isspace(buff[0]) && buff[0] != 0)
	lastLine = buff;

	} while (is.getline(buff, _LL_BUFFSIZE_));

	is.close();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////



#endif
