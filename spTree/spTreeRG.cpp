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

#include "spTreeRG.h"
#define debug true
//specifically for the dijkstraSSSP() method
#define spDebug false



//const bool isArrayInitOnce=true;
const bool isArrayInitOnce=false;
int* spTreeRG::backtrace=NULL;
int* spTreeRG::trackdist=NULL;
spTreeRG* spTreeRG::mapInstance = NULL;
std::string spTreeRG::filePrefix;
////////////////////////////////////////////////////////////////////////////////////////////////////



double lowerboundDist(Point* nodecoord,int nidA,int nidB) {
	double xdiff = nodecoord[nidA].first - nodecoord[nidB].first ;
	double ydiff = nodecoord[nidA].second - nodecoord[nidB].second ;
	return sqrt( xdiff*xdiff + ydiff*ydiff );
}

spTreeRG* spTreeRG::mapObject(string fnPrefix){
//     if(mapInstance == NULL) cout << "HELP ERROR" <<endl;
// 
//     if(strcmp((fnPrefix).c_str(), (filePrefix).c_str()) !=0){ //true if strings are not equal
//         cout << "an error I believe arg1/arg2: " << fnPrefix <<"/" << filePrefix << endl;
//         delete mapInstance;
//         mapInstance = NULL;
// 		if (backtrace!=NULL)
// 			delete[] backtrace;
//         if (trackdist!=NULL)
// 			delete[] trackdist;
//         backtrace=NULL;
//         trackdist=NULL;
//     }

	if (mapInstance==NULL){
		filePrefix=fnPrefix;
		mapInstance = new spTreeRG();
        mapInstance->ssspCalls = 0;
		mapInstance->numNodeVisits = 0;
		cout << "[mapObject] filePrefix, parseFileType: " << filePrefix << "; " << mapInstance->parseFileType << endl;

        mapInstance->readCedgeNetworkFile(filePrefix);
        cout << " ... done" << endl;
    }
	return mapInstance;
}

void spTreeRG::dijkstraSPTree(int source) {

//    cout << "spTreeRG::dijkstraSPTree start!" << endl;
	spTreeRG* ins=mapObject(filePrefix);

	static int _mapsize=0;
	static bool* isVisited=NULL;


	if (ins==NULL){
        cout << "Error, mapObject is NULL!!" << endl;
        exit(1);
	}

	ins->ssspCalls++;
	ins->lastNodeVisits=0;

	if ( source<0 || source >= ins->mapSize) {
		cout << "source/mapSize: " << source <<"/" << mapSize << "\nInvalid src node, map\n";
	}

	if(spDebug) cout << "one, dijkstraSPTree! " << endl;


//	if (isArrayInitOnce==false) {
//		if (isVisited!=NULL)
//			delete[] isVisited;
//cout << "spTreeRG::dijkstraSPTree 4" << endl;
//		if (backtrace!=NULL)
//			delete[] backtrace;
//cout << "spTreeRG::dijkstraSPTree 5" << endl;
//        if (trackdist!=NULL)
//			delete[] trackdist;
//cout << "spTreeRG::dijkstraSPTree 6" << endl;
//		isVisited=NULL;
//		backtrace=NULL;
//		trackdist=NULL;
//	}

	if (isVisited==NULL) {
		_mapsize=getMapsize();
		backtrace=new int[_mapsize];
		trackdist=new int[_mapsize];
		isVisited=new bool[_mapsize];
	} else
		assert(_mapsize==getMapsize());

	// init
	fill( backtrace, backtrace+_mapsize, -1);
    fill( trackdist, backtrace+_mapsize, -1);
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

	if(spDebug) cout << "six, dijkstraSPTree! " << endl;

	while (hp.size()>0) {
		HeapEntry he=hp.top();
		hp.pop();
		int curnode=he.id;
		if (isVisited[curnode])
			continue;

		isVisited[curnode]=true;
		backtrace[curnode]=he.prev_id;	// prev node
		trackdist[curnode]=he.dist; //curnode sp dist
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
	}
//	cout << "done building" << endl;
}

int spTreeRG::getMapsize() {
	return mapSize;
}

void spTreeRG::addEdge(int v1, int v2, double w) {
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

void spTreeRG::readRoadNetworkFile(string fn) {
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

void spTreeRG::readCedgeNetworkFile(string fn) {
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
	else{cout << "file: " << edgeFN <<" could not be opended" << endl;}


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

int spTreeRG::getFilelines(const char *filename) {
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

bool spTreeRG::getLastLine(const char *filename, string &lastLine) {
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

///file format (space separated, blank line between different source nodes):
///first line: source node_id, map size, -1337
///remaining lines: node_id, node_distance, node_prev_id
void spTreeRG::writeSPtree(std::pair<int*, int> poiArray){

    int numPOI = poiArray.second;
    int source, devider=-1337;
 	string fn = filePrefix;
    bool fileExist = false;

	fn.append(".sptree"); // extension .sptree

    ifstream fin((fn).c_str());
    if (fin)
		fileExist = true;// check to see if file exists
    fin.close();

    ///file output
    ofstream outFile;
	outFile.open((fn).c_str(), ios::out | ios::binary);

    for(int i=0; i < numPOI; i++)
    {
        source = poiArray.first[i];
        dijkstraSPTree(source);
        outFile.write((char*)&source, sizeof(int));
        outFile.write((char*)&mapSize, sizeof(int));
        outFile.write((char*)&devider, sizeof(int));

        for(int k=0; k<mapSize; k++){
            outFile.write((char*)&k, sizeof(int));
            outFile.write((char*)&trackdist[k], sizeof(int));
            outFile.write((char*)&backtrace[k], sizeof(int));
        }
    }
    outFile.close();
}

/// input format of .poi format: 
/// record_id, point_id, point_x, point_y
std::pair<int*, int> spTreeRG::readPOIlist(string poifn){

	string str;
	int i=0;
	std::pair<double, double> coord;
	std::vector<string> tokens;
	int* poiIDs = new int[mapSize];

	poifn.append(".poi"); //set the extension to be .poi
	ifstream file (poifn.c_str(), ios::in); //*.poi fil

	cout << "spTreeRG::readPOIlist start: " << poifn << endl;

	//find all pairs of nodeids in the training set to have SP done for them. map nodeids to Points.
	if (file.is_open()) {
		if (debug)
			cout << "two, spTreeRG::readPOIlist opened! " << endl;
		cout << i << endl;
		while(getline(file, str)) {
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
			poiIDs[i] = atoi(tokens[1].c_str());
			i++;
			cout << i << endl;
  		}
	}
	file.close();
	cout << "spTreeRG::readPOIlist end! number of POI/testRangetype:" << i << endl;
	return std::make_pair<int*, int>(poiIDs, i);
}
#endif
