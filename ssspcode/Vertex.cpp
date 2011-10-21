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
#ifndef VERTEX_CPP
#define VERTEX_CPP

#include "Vertex.h"
#define debug false

Vertex::Vertex(int id, int nId, double weight)
{
	updateVertexID(id, nId, weight);
}

void Vertex::updateVertexID(int id, int nId, double weight)
{
	this->id = id;
	addNeighbour(nId, weight);
}

void Vertex::addNeighbour(int id, double weight)
{
	if(debug) std::cout << "one, vertex::addNeighbour! id: "<<  this->id << " " << id <<", w: " << weight << " size: " << Adjacencylist.size() << endl;
	//if(debug) {for(uint d = 0; d < Adjacencylist.size(); d++) {cout << d << ":"<< Adjacencylist[d] << " "; cout << endl;}}

	if(Adjacencylist.find(id) == Adjacencylist.end()){
		if(debug) std::cout << "@" << id << "#" <<  weight << "@"  << Adjacencylist.size() <<  endl;
		Adjacencylist[id] = weight;
		if(debug) std::cout << "three, vertex::addNeighbour!" << endl;
	}
	if(debug){
		std::cout << "four, vertex::addNeighbour!"<< endl;
		//for(uint d = 0; d < Adjacencylist.size(); d++) {cout<< d << ":"<< Adjacencylist[d] << " ";cout << endl;}
	}

}

boost::unordered_map<int, double>  Vertex::getAdjacencylist()
{
	return Adjacencylist;
}

int Vertex::getId()
{
	return this->id;
}

string Vertex::toString()
{
	string s;
	s += "Vertice id: ";
	s += id;
	s += "\n";
	s += "Adjacency list (vid,weight): ";
    BOOST_FOREACH(intDoubleMap::value_type idm, Adjacencylist)
    {
    	s += "(";
		s += idm.first;
		s += "," ;
		s += idm.second;
		s += ")";
    }
    s += "\n";
    return s;
}

#endif
