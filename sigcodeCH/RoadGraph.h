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

typedef std::pair<int,double> JEdge;
typedef std::vector<JEdge> EdgeList;


class RoadGraph
{
public:
	static RoadGraph* mapObject(TestSetting& ts);
	void setMapFile(string file);
	std::vector<int> dijkstraSSSP(int s, int t);
	std::vector<int> dijkstraSSSP2(int s, int t);
	int getMapsize();
	int ssspCalls;
	unsigned long numNodeVisits;
	string checkMapFilename(){return filename;}
	void resetRoadGraph(){
		ssspCalls=0;
		numNodeVisits=0;
	}
	void transformTrainOrTestFile(string cnodeFn, string trainTestFn);
	void setConcisePathUse(bool use){useConcisepath = use;} //cout << "***/" << useConcisepath << "/***" << endl; }
	void testCH();
	
private:
	RoadGraph(){ };
	~RoadGraph(){
	  delete map;
	  spTrace.clear();
	  nid2Point.clear();
	  node2degree.clear();
	  delete mapInstance;
	}
	RoadGraph(RoadGraph const&){}; //private copy constructor
	RoadGraph& operator=(RoadGraph const&); //private assignment operator
	static RoadGraph* mapInstance;

	EdgeList* map;

	int mapSize;
	int edges;
	int parseFileType;
	std::string filename;
	static boost::unordered_map<int, int*> spTrace;//backtract the SP route from a node to its original source.
//	static boost::unordered_map<int, int*> trackdist; //all the distances from any node to a source node
	int countSuccess, countFail;
	bool useConcisepath;
	bool measureConcisepathdegrees;
	intPointMap nid2Point;
	intMap node2degree;
	CONCISETYPE_CHOICE testconcisetype;
	////////// CH /////////
	ProcessingConstruct* construct;
	UpdateableGraph* updGraph;
 	Dijkstra *sp;
	Path path;
	EdgeWeight spDist;

	
	void addEdge(int v1, int v2, double w);
	void readRoadNetworkFile(string fn);
	void readPPINetworkFile(string fn);
	void readCedgeNetworkFile(string fn);
	int getFilelines(const char *filename);
	bool getLastLine(const char *filename, string &lastLine);
	void readSPTreeFileBinary(TestSetting& ts);
	intVector getSPfromSPTree(int source, int target);
	double getAngle(Point prevNode, Point source, Point target);
	std::vector<int> calcConsisePath(std::vector<int>& trace);
	std::vector<int> calcConsisePathA(std::vector<int>& trace);
	std::vector<int> calcConsisePathB(std::vector<int>& trace);
	std::vector<int> recoverPath(std::vector<int>& conciseTrace);
	void writeoutEdgedegree();
	void findNode2degree();
	////////// CH /////////	
	void setupCH(string& fn);

};


#include <queue>

struct HeapEntry {
	int id;
	double dist;
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

struct HeapWorkloadEntry{
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
//typedef    priority_queue<HeapEntry,vector<HeapEntry>,HeapEntryComp> Heap;
typedef    FAST_HEAP<HeapEntry, std::vector<HeapEntry>, HeapEntryComp> Heap;
typedef    FAST_HEAP<HeapEntry, std::vector<HeapEntry>, HeapEntryCompMax> maxHeap;
typedef    FAST_HEAP<HeapWorkloadEntry, std::vector<HeapWorkloadEntry>, HeapWorkloadEntryCompMax> maxWorkloadHeap;

////////////////////////////////////////////////

#include "RoadGraph.cpp"

////////////////////////////////////////////////

#endif