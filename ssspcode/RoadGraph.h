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
#ifndef ROADGRAPH_H
#define ROADGRAPH_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "Vertex.h"
#include "fastheap.h"
#include "testsetting.h"
#include "utility.h"

#include "boost/unordered_map.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/foreach.hpp"



class RoadGraph
{
public:
	static RoadGraph* mapObject(string testfile, int parseType);
	void setMapFile(string file);
	std::vector<Vertex> getMap();
	std::vector<int> dijkstraSSSP(int s, int t);
	int getMapsize();
	int ssspCalls;
	string checkMapFilename(){return filename;}
	boost::unordered_map<int, int> totalNodeCalls;
	void resetRoadGraph(){mapInstance = NULL;}
	void transformTrainOrTestFile(string cnodeFn, string trainTestFn);

private:
	RoadGraph(){ };
	~RoadGraph(){delete mapInstance;}
	RoadGraph(RoadGraph const&){}; //privatre copy constructor
	RoadGraph& operator=(RoadGraph const&){}; //private assignment operator
	static RoadGraph* mapInstance;
	std::vector<Vertex> map;
	int mapSize;
	int edges;
	int parseFileType;
	std::string filename;

	void parseType(int pt);
	void addEdge(int v1, int v2, double w);
	void readRoadNetworkFile(string fn);
	void readPPINetworkFile(string fn);
	void readCedgeNetworkFile(string fn);
	bool getLastLine(const char *filename, string &lastLine);
};

#endif
