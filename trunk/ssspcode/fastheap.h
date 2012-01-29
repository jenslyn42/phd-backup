#ifndef FASTHEAP_H
#define FASTHEAP_H

#include <queue>

#define BitStore vector<bool>

struct HeapEntry {
	int id;
	std::pair<int,int> pID;
	double dist;
	int length;
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
	{ return left.answeredPaths.second.size() < right.answeredPaths.second.size() ; }
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

#endif
