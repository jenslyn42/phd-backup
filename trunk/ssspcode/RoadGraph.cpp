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

#include "limits.h"

RoadGraph* RoadGraph::mapInstance = NULL;

RoadGraph* RoadGraph::mapObject(std::string testFile, int pt)
{
	if(!mapInstance){
		mapInstance = new RoadGraph();
		mapInstance->parseFileType = pt;

		switch( (mapInstance->parseFileType) ){
			case 1:
			cout << "mapObject[case1], parseFileType: " << mapInstance->parseFileType << endl;
 			mapInstance->readRoadNetworkFile(testFile);
			break;
			case 2:
			cout << "mapObject[case2], parseFileType: " << mapInstance->parseFileType << endl;
 			mapInstance->readPPINetworkFile(testFile);
			break;
			case 3:
 			cout << "mapObject[case3], parseFileType: " << mapInstance->parseFileType << endl;
 			mapInstance->readCedgeNetworkFile(testFile);
 			cout << "mapObject[case3], parseFileType: " << mapInstance->parseFileType << " done" << endl;
			break;
		}
	}
	else if((mapInstance->parseFileType) != pt)
	{
		RoadGraph::mapInstance = NULL; //if type of file to  be parsed changes, delete mapInstance
		RoadGraph::mapObject(testFile, pt);
	}

	return mapInstance;
}

vector<Vertex> RoadGraph::getMap()
{
	return map;	
}

vector<int> RoadGraph::dijkstraSSSP(int source, int dest)
{	
	if(spDebug) cout << "one, dijkstraSSSP! map:" << (int) map.size() <<" s,t:" << source <<"," <<dest << endl;
	
	if(map[source].getId() == -1 || map[dest].getId() == -1 || source > mapSize || dest > mapSize)
	{
		if(spDebug) cout << "two, dijkstraSSSP!" << endl;
		if(map[source].getId() == -1)
			cout << "Source node invalid: " << source << "\n";
		if(map[dest].getId() == -1)
			cout << "Destination node invalid: " << dest << "\n";
	}
	if(spDebug) cout << "three, dijkstraSSSP!" << endl;
	if(source == dest)
	{
		std::vector<int> tmp;
		tmp.push_back(source);
		return tmp;
	}
	if(spDebug) cout << "four, dijkstraSSSP! " << endl;
	Heap hp;
	boost::unordered_map<int, int> backtrace;
	static BitStore isVisited;
	isVisited.assign(getMapsize(),false);
	std::vector<int> trace;

	if(spDebug) cout << "five, dijkstraSSSP! "<< endl;
	//insert source node
	HeapEntry root_he;
	root_he.id=source;
	root_he.dist=0.0;    // dist. so far
	hp.push(root_he);
	
	if(spDebug) cout << "six, dijkstraSSSP! " << endl;
	while (hp.size()>0) {
		HeapEntry he=hp.top();
		hp.pop();

		int curnode=he.id;
		if (isVisited[curnode])
		continue;

		if(totalNodeCalls.find(curnode) == totalNodeCalls.end()){
			totalNodeCalls[curnode] = 1;
		}else{ 
			totalNodeCalls[curnode] = totalNodeCalls.at(curnode)+1;
		}
		
		isVisited[curnode]=true;
		
		if(spDebug) cout << "seven, dijkstraSSSP! "<< curnode << endl;
		intDoubleMap CurAdjList= map[curnode].getAdjacencylist();
		if(spDebug) cout << "eight, dijkstraSSSP! " << endl;
		BOOST_FOREACH (intDoubleMap::value_type neighbour, CurAdjList)
		{
			int NextNodeID= neighbour.first; //id
			if (isVisited[NextNodeID]==false) {
				HeapEntry new_he=he;    // copy ...
				new_he.id= NextNodeID;
				new_he.dist= he.dist+neighbour.second; //weight
				hp.push(new_he);    // propagation
				if(backtrace.find(neighbour.first) == backtrace.end()){ //neighbour id not in backtrace
					backtrace[neighbour.first] = he.id;
				}
			}
		}
		//if(spDebug) cout << "nine, dijkstraSSSP! " << endl;
		if(curnode==dest)
		{
			int prevNode = dest;
			trace.push_back(prevNode);
			do{
				prevNode = backtrace.at(prevNode);
				trace.push_back(prevNode);
			}while(prevNode != source);			
		return trace;
		}
	}
	return trace;
}

int RoadGraph::getMapsize()
{
	return mapSize;
}

void RoadGraph::addEdge(int v1, int v2, double w)
{
	//if(debug) cout << "one, addEdge! (v1,v2,w): (" <<v1 <<"," << v2 <<"," << w <<") " << endl;
	if(map[v1].getId() == -1){
		//if(debug) cout << "one1, addEdge! updateVertex (v1,v2,w): (" <<v2 <<"," << v1 <<"," << w <<") " << endl;
		map[v1].updateVertexID(v1,v2,w);
		//if(debug) cout << "one2, addEdge! updateVertex (v1,v2,w): (" <<v2 <<"," << v1 <<"," << w <<") " << endl;
	}else{
		//if(debug) cout << "one3, addEdge! addNeighbour (v2,w): (" << v1 << "," << w << ") " << endl;
		map[v1].addNeighbour(v2,w);
	}

	//if(debug) cout << "two, addEdge! (v2,v1,w): (" <<v2 <<"," << v1 <<"," << w <<") " << endl;
	if(map[v2].getId() == -1) {
	//	if(debug) cout << "two1, addEdge! updateVertex (v2,v1,w): (" <<v2 <<"," << v1 <<"," << w <<") " << endl;
		map[v2].updateVertexID(v2,v1,w);
	//	if(debug) cout << "two2, addEdge! updateVertex (v2,v1,w): (" <<v2 <<"," << v1 <<"," << w <<") " << endl;
	}else{
	//	if(debug) cout << "two3, addEdge! addNeighbour (v1,w): (" << v1 << "," << w << ") " << endl;
		map[v2].addNeighbour(v1,w);
	}
	//if(debug) cout << "three, addEdge!" << endl;
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
		map.reserve(mapSize+1);
		if(debug) cout << "one, readNetworkFile! map:" << (int) map.size() <<" " << mapSize << endl;
		for(int i = 0; i < mapSize+1; i++)
			{
				map.push_back(Vertex(-1,0,0.0)); //ugly, but do not know how to fill with null objects..
				if(debug) cout << "two, readNetworkFile! map:" << (int) map.size() <<" " << mapSize <<"i: " << i << " map content: " << map[i].getId() << endl;
			}

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
		map.reserve(mapSize+1);
		if(debug) cout << "one, readPPINetworkFile! map:" << (int) map.size() <<" " << mapSize << endl;
		for(int i = 0; i < mapSize+1; i++)
			{
				map.push_back(Vertex(-1,0,0.0)); //ugly, but do not know how to fill with null objects..
				//if(debug) cout << "two, readPPINetworkFile! map:" << (int) map.size() <<" " << mapSize <<"i: " << i << " map content: " << map[i].getId() << endl;
			}


		int check = 0;
		while(getline(in_data, str))
		{
			//if(debug) cout << "five, readPPINetworkFile! getline:" << str  << " check: " << check++ << endl;
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
			for(int i = 1; i< tokens.size(); i++)
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

void RoadGraph::readCedgeNetworkFile(string fn)
{
///fileformat .cedge: 0 0 1 1.182663
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

 		getLastLine((nodeFN.c_str()),str); //find last line
		boost::algorithm::split(tokens, str, boost::algorithm::is_space()); //split last line of *.cnode file
		mapSize = atoi(tokens[0].c_str()); //mapSize = number of nodes (using the highest node id to determine)

		map.reserve(mapSize+1);
		if(debug) cout << "one, readCedgeNetworkFile! map:" << (int) map.size() <<" " << mapSize << endl;
		for(int i = 0; i < mapSize+1; i++)
			{
				map.push_back(Vertex(-1,0,0.0)); //ugly, but do not know how to fill with null objects...
				//if(debug) cout << "two, readCedgeNetworkFile! map:" << (int) map.size() <<" " << mapSize <<"i: " << i << " map content: " << map[i].getId() << endl;
			}
		if(debug) cout << "three, readCedgeNetworkFile! map:" << (int) map.size() <<" " << mapSize << endl;
		int check = 0;
		while(getline(in_data, str))
		{
// 			cout << "five, readCedgeNetworkFile! getline:" << str  << " check: " << check++ << endl;
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
			//if(debug) cout << "five1, readCedgeNetworkFile! getline:" << str << " check: " << check<< endl;
			addEdge(atoi(tokens[1].c_str()),atoi(tokens[2].c_str()),atof(tokens[3].c_str()));
			//if(debug) cout << "five2, readCedgeNetworkFile! getline:" << str << " check: " << check<< endl;
		}
		if(debug) cout << "six, readCedgeNetworkFile!" << endl;
		in_data.close();
	}
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

#endif