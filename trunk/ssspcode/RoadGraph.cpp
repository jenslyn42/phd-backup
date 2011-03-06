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

#include "RoadGraph.h"
#include "string.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "fastheap.h"

#include "boost/unordered_map.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/foreach.hpp"


RoadGraph* RoadGraph::mapInstance = NULL;

RoadGraph* RoadGraph::mapObject()
{
	if(!mapInstance){
		mapInstance = new RoadGraph();
		readNetworkFile(	
	}
	return mapInstance;
}

vector<Vertex> RoadGraph::getMap()
{
	return map;	
}

vector<int> RoadGraph::dijkstraSSSP(int source, int dest)
{	
	bool debug = true;
	if(debug) cout << "one, dijkstraSSSP! map:" << (int) map.size() << endl;
	
	if(map[source].getId() == -1 || map[dest].getId() == -1 || source > mapSize || dest > mapSize)
	{
		if(debug) cout << "two, dijkstraSSSP!" << endl;
		if(map[source].getId() == -1)
			cout << "Source node invalid: " << source << "\n";
		if(map[dest].getId() == -1)
			cout << "Destination node invalid: " << dest << "\n";
	}
	if(debug) cout << "three, dijkstraSSSP!" << endl;
	if(source == dest)
	{
		std::vector<int> tmp;
		tmp.push_back(source);
		return tmp;
	}
	if(debug) cout << "four, dijkstraSSSP!" << endl;
	Heap hp;
	unorderedIntMap backtrace;
	static BitStore isVisited;
	isVisited.assign(getMapsize(),false);
	std::vector<int> trace;

	//insert source node
	HeapEntry root_he;
	root_he.id=source;
	root_he.dist=0.0;    // dist. so far
	hp.push(root_he);
	
	while (hp.size()>0) {
		HeapEntry he=hp.top();
		hp.pop();

		int curnode=he.id;
		if (isVisited[curnode])
		continue;

		isVisited[curnode]=true;
		
		unorderedIntMap CurAdjList= map[curnode].getAdjacencylist();
		BOOST_FOREACH (unorderedIntMap::value_type neighbour, CurAdjList)
		{
			int NextNodeID= neighbour.first; //id
			if (isVisited[NextNodeID]==false) {
				HeapEntry new_he=he;    // copy ...
				new_he.id= NextNodeID;
				new_he.dist= he.dist+neighbour.second; //weight
				hp.push(new_he);    // propagation
				if(backtrace.find(neighbour.first) == backtrace.end()) //neighbour id not in backtrace
					backtrace[neighbour.first] = he.id;
			}
		}
		if(curnode==dest)
		{
			//he.dist; //contains distance source->dest.
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

void RoadGraph::addEdge(int v1, int v2, int w)
{
	if(map[v1].getId() == -1) 
		map[v1] = Vertex(v1,v2,w);
	else
		map[v1].addNeighbour(v2,w);

	if(map[v2].getId() == -1) 
		map[v2] = Vertex(v2,v1,w);
	else
		map[v2].addNeighbour(v1,w);
}

void RoadGraph::readNetworkFile(string fn)
{
	string str;
	std::vector<string> tokens;	
	ifstream in_data (fn.c_str(), ios::in);
	if(in_data.is_open())
	{
		getline(in_data, str);
		mapSize = atoi(str.c_str());
		map.reserve(mapSize+1);
		for(int i = 0; i < mapSize+1; i++)
			{
				map[i] = Vertex(-1,0,0); //ugly, but do not know how to fill with null objects..
			}

		getline(in_data, str);
		edges = atoi(str.c_str());

		while( tokens.size() < 3 )
		{
			getline(in_data, str);
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());	
		}
		addEdge(atoi(tokens[0].c_str()),atoi(tokens[1].c_str()),atoi(tokens[2].c_str()));

		while(getline(in_data, str))
		{
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
			addEdge(atoi(tokens[0].c_str()),atoi(tokens[1].c_str()),atoi(tokens[2].c_str()));
		}
		
		in_data.close();
	}
}




