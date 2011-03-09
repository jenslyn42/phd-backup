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
#ifndef FASTHEAP_H
#define FASTHEAP_H

#include <queue>

#define BitStore vector<bool>

struct HeapEntry {
	int id;
	int dist;
	HeapEntry(){};
	HeapEntry(int id, int d){
		this->id = id;
		this->dist = d;
	}
};

struct HeapEntryComp {
	bool operator () (HeapEntry left, HeapEntry right) const
	{ return left.dist > right.dist; }
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

#endif