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


////////////////////////////////////////////////////////////////////////////////////////////////////

RoadGraph* RoadGraph::mapInstance = NULL;

RoadGraph* RoadGraph::mapObject(std::string fn)
{
	// obtain parameter values
	std::string mapFile=fn;

	if(mapInstance==NULL){
		mapInstance = new RoadGraph();
		mapInstance->ssspCalls = 0;
		mapInstance->numNodeVisits = 0;
cout << "*** RoadGraph::reading file: " << mapFile << " ";
		mapInstance->readCedgeNetworkFile(mapFile);
		cout << "... done" << endl;
	}

	return mapInstance;
}

vector<int> RoadGraph::dijkstraSSSP(int source, int dest, bool constWeight, double radius) {
	if(debug) cout << "DijkstraSSSP(" << source << ", " << dest << ", " << constWeight << ", " << radius << endl;
	std::vector<int> vertexlist;
	double radiusGuard = radius;
	
	ssspCalls++;
	if (spDebug) cout << "one, dijkstraSSSP! map:" << mapSize <<" s,t:" << source <<"," <<dest << endl;

	if ( source<0 || source>=mapSize) {
		cout << "Invalid src node, map\n";
		return vertexlist;
	}

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
		vertexlist.push_back(curnode);
		numNodeVisits++;	// usage info

		EdgeList& CurAdjList=map[curnode];
		BOOST_FOREACH (Edge neighbour, CurAdjList)
		{
			int NextNodeID = neighbour.first; //id
			double NextWeight;
			constWeight? NextWeight = 1 : NextWeight = neighbour.second;
			if (isVisited[NextNodeID]==false && he.dist+NextWeight <= radiusGuard) {
				HeapEntry new_he=he;    // copy 
				new_he.id=NextNodeID;
				new_he.dist=he.dist+NextWeight; //weight
				new_he.prev_id=curnode;
				hp.push(new_he);    // propagation
			}
		}
	}
	return vertexlist;
}

int RoadGraph::getMapsize()
{
	return mapSize;
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

///fileformat .cedge: 0 0 1 1.182663 (linenum, vid1, vid2, dist)
void RoadGraph::readCedgeNetworkFile(string fn)
{
	filename = fn + ".cedge";
	string str;
	string nodeFN = filename;
	nodeFN.replace ((nodeFN.size()-4), 4, "node"); //change file extention from .cedge to .cnode
	std::vector<string> tokens;
	ifstream in_data (filename.c_str(), ios::in);
	if(debug) cout << "s1, readCedgeNetworkFile! nodeFN / filename: " <<nodeFN << "/" << filename << endl;
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
	if(debug) cout << "s7, readCedgeNetworkFile loop end" << endl;
}

//Returns number of lines, and write out the size of the map to the screen
int RoadGraph::getFilelines(const char *filename)
{
	if(debug) cout << "one, getFilelines! filename: "<<filename << endl;
	if (!filename || !*filename) // if no file to work on, return false
        return false;

	int size = 0;
	double minY=std::numeric_limits<double>::max(),minX=std::numeric_limits<double>::max(),maxY=0,maxX=0;
	string line;
	std::vector<string> tokens;
	
	ifstream is;
	is.open(filename);

	if (!is) // return false if couldn't open file
		return -1;

	while (!is.eof())
	{
		getline(is, line);
		size++;
		
		boost::algorithm::split(tokens, line, boost::algorithm::is_space());
		if(atof(tokens[1].c_str()) < minX) minX = atof(tokens[1].c_str());
		if(atof(tokens[1].c_str()) > maxX) maxX = atof(tokens[1].c_str());
		if(atof(tokens[2].c_str()) < minY) minY = atof(tokens[2].c_str());
		if(atof(tokens[2].c_str()) > maxY) maxY = atof(tokens[2].c_str());
		
		nodelist[atoi(tokens[0].c_str())] = make_pair<string,string>(tokens[1],tokens[2]);
	}

	is.close();

	cout << "\nxmin, xmax: " << minX << ", " << maxX << endl;
	cout << "ymin, ymax: " << minY << ", " << maxY << endl;
	cout << "x-range, y-range: " << maxX-minX << ", " << maxY-minY << endl;
	
	return size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


#endif
