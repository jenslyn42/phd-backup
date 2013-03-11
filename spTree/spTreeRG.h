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
#ifndef SPTREE_H
#define SPTREE_H

#include <utility>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <typeinfo>
#include <iostream>
#include <fstream>

#include <algorithm>
#include <assert.h>

#include <ctime>
#include <cmath>
#include <cstdlib>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/dynamic_bitset.hpp>


typedef std::pair<int,double> Edge;
typedef std::vector<Edge> EdgeList;
typedef std::vector<int>  intVector;
typedef std::pair<double, double> Point;
typedef std::pair<int,int> intPair;

using namespace std;

class spTreeRG
{
public:
	static spTreeRG* mapObject(string filePrefix);

	void setMapFile(string file);
	EdgeList* getMap(){return map;}

	int getMapsize();
	int ssspCalls;
	unsigned long numNodeVisits, lastNodeVisits;

	void resetRoadGraph(){
		//mapInstance = NULL;
		ssspCalls=0;
		numNodeVisits=0;
		lastNodeVisits=0;
	}

    void writeSPtree(std::pair<int*, int> poiArray);
    std::pair<int*, int> readPOIlist(string poifn);

private:
	spTreeRG(){ };
	~spTreeRG(){delete mapInstance;}
	spTreeRG(spTreeRG const&){}; //privatre copy constructor
	spTreeRG& operator=(spTreeRG const&); //private assignment operator
	static spTreeRG* mapInstance;


	Point* nodecoord;
	EdgeList* map;

	int mapSize;
	int edges;
	int parseFileType;
	static std::string filePrefix;
	static int* backtrace; //backtract the SP route from a node to its original source.
	static int* trackdist; //all the distances from any node to a source node

	void dijkstraSPTree(int s);

	void addEdge(int v1, int v2, double w);
	void readRoadNetworkFile(string fn);
	void readPPINetworkFile(string fn);
	void readCedgeNetworkFile(string fn);
	int getFilelines(const char *filename);
	bool getLastLine(const char *filename, string &lastLine);
    int* getSPtreeTrace(){return backtrace;}
    int* getSPtreeWeights(){return trackdist;}
};


#include <queue>

struct HeapEntry {
	int id;
	double dist;
	double gdist,hdist;	// only used for A* search

	int length;
	int prev_id;
	intPair pID;

	HeapEntry(){};
	HeapEntry(int id, int d){
		this->id = id;
		this->dist = d;
	}
	HeapEntry(int id, int d, int l){
		this->id = id;
		this->dist = d;
		this->length = l;
	}
};

struct HeapWorkloadEntry
{
    int pathId; //path id
    int pathLength; //lenght of path
    std::pair<int, std::vector<int> > answeredPaths; //number of paths 'path id' can answer, and a vector holding the id of each of these.
};

///For minHeap
struct HeapEntryComp {
	bool operator () (HeapEntry left, HeapEntry right) const
	{ return left.dist > right.dist; }
};
///For MaxHeap
struct HeapEntryCompMax {
	bool operator () (HeapEntry left, HeapEntry right) const
	{ return left.dist < right.dist; }
};

///For MaxWorkloadHeap
struct HeapWorkloadEntryCompMax {
	bool operator () (HeapWorkloadEntry left, HeapWorkloadEntry right) const
	{ return left.answeredPaths.second.size()< right.answeredPaths.second.size(); }
};

struct HeapWorkloadEntryCompMaxLengthDevide {
	bool operator () (HeapWorkloadEntry left, HeapWorkloadEntry right) const
	{ return left.answeredPaths.second.size() / left.pathLength < right.answeredPaths.second.size() / right.pathLength ; }
};


template<typename _Tp, typename _Sequence, typename _Compare >
class FAST_HEAP
{

	public:
	typedef typename _Sequence::value_type                value_type;
	typedef typename _Sequence::reference                 reference;
	typedef typename _Sequence::const_reference           const_reference;
	typedef typename _Sequence::size_type                 size_type;

	protected:
	_Sequence  c;
	_Compare   comp;

	public:
	explicit
	FAST_HEAP(const _Compare& __x = _Compare(),
		const _Sequence& __s = _Sequence()) : c(__s), comp(__x)
	{ std::make_heap(c.begin(), c.end(), comp); }

	bool empty() const { return c.empty(); }

	size_type size() const { return c.size(); }

	const_reference top() const {
		return c.front();
	}

	void push(const value_type& __x) {
		c.push_back(__x);
		std::push_heap(c.begin(), c.end(), comp);
	}

	void pop() {
		std::pop_heap(c.begin(), c.end(), comp);
		c.pop_back();
	}
};


// not much difference in heap implementation

// *** slow heap implementation
typedef    priority_queue<HeapEntry, std::vector<HeapEntry>, HeapEntryComp> Heap;
typedef    priority_queue<HeapEntry, std::vector<HeapEntry>, HeapEntryCompMax> maxHeap;
typedef    priority_queue<HeapWorkloadEntry, std::vector<HeapWorkloadEntry>, HeapWorkloadEntryCompMax> maxWorkloadHeap;


// *** fast heap implementation
/*typedef    FAST_HEAP<HeapEntry, std::vector<HeapEntry>, HeapEntryComp> Heap;
typedef    FAST_HEAP<HeapEntry, std::vector<HeapEntry>, HeapEntryCompMax> maxHeap;
typedef    FAST_HEAP<HeapWorkloadEntry, std::vector<HeapWorkloadEntry>, HeapWorkloadEntryCompMax> maxWorkloadHeap;
*/

#endif
