/* Copyright (C) 2005, 2006, 2007, 2008 
 * Robert Geisberger, Dominik Schultes, Peter Sanders,
 * Universitaet Karlsruhe (TH)
 *
 * This file is part of Contraction Hierarchies.
 *
 * Contraction Hierarchies is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * Contraction Hierarchies is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Contraction Hierarchies; see the file COPYING; if not,
 * see <http://www.gnu.org/licenses/>.
 */


#ifndef BINARYHEAP_H
#define BINARYHEAP_H

#include <vector>

/**
 * In case of the (FIFO)BinaryHeap, we distinguish between an ExternalKey,
 * which is provided by the application (which uses the heap) and an internal Key,
 * which is used inside the heap. The ExternalKey is always a part of the internal Key.
 * A "key extractor" extracts the ExternalKey out of the internal Key.
 * 
 * In the simple case of a BinaryHeap without the FIFO property, the internal Key just
 * corresponds to the ExternalKey. Thus, the SimpleKeyExtractor just returns the
 * given internal Key as the ExternalKey.
 * @param ExternalKey the type of the external key = the type of the internal key
 */
template < typename ExternalKey >
class SimpleKeyExtractor
{
public:
    /** Returns the given internal Key as the ExternalKey. */
    static ExternalKey key(ExternalKey k) {return k;}
};


/**
 * Represents an element of a BinaryHeap.
 * @param ExternalKey the type of the external key (see SimpleKeyExtractor)
 * @param MetaExtKey class that provides data about the external key, e.g. the max value
 * @param Data the type of the application-specific data that is associated with this element
 * @param Count the type of the counter that counts the number of heap elements / 
 *              the number of heap operations
 */
template < typename ExternalKey,
           typename MetaExtKey,
           typename Data,
           typename Count >
class BinaryHeapElement
{
public:
    /** Constructs a DUMMY element. */
    BinaryHeapElement() : _key(MetaExtKey::MAX_VALUE), _index(0) {}
            
    /**
     * Constructor.
     * @param key key of the new element
     * @param index index of the new element within the heap
     */
    BinaryHeapElement(ExternalKey key, Count index) : _key(key), _index(index) {}
    
    
    /** Returns the external key of this element. */
    ExternalKey key() const {return _key;}
    
    /** Sets the external key of this element. */
    void key(ExternalKey newKey) {_key = newKey;}

    bool isDummy() const {return key() == MetaExtKey::MAX_VALUE;}
    
    
    /** Returns a reference to the application-specific data object of this element. */
    Data& data() {return _data;}
    
    /** Returns a reference to the application-specific data object of this element. */
    const Data& data() const {return _data;}
    
    
    /** Marks that this element has been deleted from the heap. */
    void markDeleted() {index(0);}
    
    /** Returns true iff this element has been deleted from the heap. */
    bool hasBeenDeleted() const {return (_index==0);}

    
    /** Returns this element's index within the heap. */
    Count index() const {return _index;}
    
    /** Sets this element's index within the heap. */
    void index(Count newIndex) {_index = newIndex;}
    
private:
    /** The external key of this element. */
    ExternalKey _key;    
    /** The application-specific data that is associated with this element. */
    Data _data;
    /**
     * This element's index within the heap.
     * 0 is used to indicate that this element has been deleted from the heap.
     */
    Count _index;
};

/**
 * Represents a binary heap.
 * @param ExternalKey the type of the external key (see SimpleKeyExtractor)
 * @param MetaExtKey class that provides data about the external key, e.g. the max value
 * @param Data the type of the application-specific data that is associated with this element
 * @param Count the type of the counter that counts the number of heap elements / 
 *              the number of heap operations
 * @param Key the type of the internal key (see SimpleKeyExtractor)
 * @param KeyExtractor the "key extractor" that is used (see SimpleKeyExtractor)
 */
template < typename ExternalKey,
           typename MetaExtKey,
           typename Data,
           typename Count,
           typename Key = ExternalKey,
           typename KeyExtractor = SimpleKeyExtractor<ExternalKey> >
class BinaryHeap
{
public:
    /** The type of the elements stored in this heap. */
    typedef BinaryHeapElement<ExternalKey, MetaExtKey, Data, Count> PQElement;
    
    typedef Data PQData;
    
    /** Constructor. */
    BinaryHeap() {
        insertDummy();
        _heap.push_back( IndexKey() ); // add a dummy element
        _heap[0].second = MetaExtKey::MIN_VALUE;
    }

    /** Returns the size of this heap (= the number of elements). */
    Count size() const { 
        return _heap.size()-1; // subtract dummy element (index 0)
    }
    
    void reserve( size_t n )
    {
        _elements.reserve(n+1);
        _heap.reserve(n+1);
    }
                            
    /** Returns true iff the heap is empty. */
    bool empty() const { return (size() == 0); }

    /** 
     * Returns the (external) key of the minimum element in this heap
     * or the maximum value of the ExternalKey data type iff the heap is empty.
     */
    ExternalKey min() const {   
        if (empty()) return MetaExtKey::MAX_VALUE;
        return KeyExtractor::key(_heap[1].second);
    }

    /**
     * Creates a new element with the given (internal) key and inserts it into the heap.
     * @return the index of the new element.
     */
    Count insert(Key key) {
        Count element = _elements.size();
        Count index = _heap.size();
        _elements.push_back( PQElement(KeyExtractor::key(key), index) );
        _heap.push_back(IndexKey(element,key));
        upheap(index);
			
        return element;
    }
    
    /** Replaces a dummy element and inserts the actual element into the heap. */
    void insert(Key key, Count elementIndex) {          
        PQElement& element = _elements[elementIndex];
        Count index = _heap.size();
        element.key(KeyExtractor::key(key));        
        element.index(index);
        _heap.push_back(IndexKey(elementIndex,key));
        upheap(index);
    }
    
    /** Adds a dummy element, which is NOT inserted into the heap, yet. */
    void insertDummy() {        
        _elements.push_back( PQElement() );
    }

    /**
     * Initialises the _elements vector so that it contains the given number of dummy elements.
     * This is useful when a search is performed after another search has not cleaned up.
     * In particular, this is used when the main phase of the landmark query is executed
     * so that the shortest path tree of the initial phase can be kept in order to be able
     * to compose the complete path at the end.
     */
    void initDummies(Count number) {
        assert( empty() );
        _elements.resize(number);
    }
    
    bool isDummy(Count element) const {
        return (_elements[element].isDummy());
    }
    
    /** Returns a reference to the elements. */
    vector<PQElement>& elements() {return _elements;}
    
    /** Returns a reference to the elements. */
    const vector<PQElement>& elements() const {return _elements;}

    /**
     * Returns the index of the minimum element in this heap
     * or 0 iff the heap is empty.
     */
    Count minElement() const {
        if (empty()) return 0;
        return _heap[1].first;
    }

    /**
     * Deletes the minimum element in this heap.
     * Precondition: The heap is not empty.
     * @return the index of the deleted element
     */
    Count deleteMin() {
        assert( ! empty() );
        const Count element = _heap[1].first;
        _heap[1] = _heap[_heap.size()-1];
        Count index = 1;
        const Count droppingElement = _heap[index].first;
        _heap.pop_back();
        if (size() > 1) {
            // downheap:
            // Move the element at the top downwards
            // until the heap condition is restored.            
            
            Key k = _heap[index].second;
            Count nextIndex = 2*index;
            while (nextIndex < _heap.size()) {
                nextIndex += 
                    ((nextIndex+1 < _heap.size()) &&
                    (_heap[nextIndex].second > _heap[nextIndex+1].second));
                
                assert( _elements[_heap[nextIndex].first].key() == KeyExtractor::key(_heap[nextIndex].second) );
                
                if (k <= _heap[nextIndex].second) break;
                
                _heap[index] = _heap[nextIndex];
                _elements[_heap[nextIndex].first].index(index);
                index = nextIndex;
                nextIndex *= 2;
            }
            _heap[index].first = droppingElement;
            _heap[index].second = k;
            
            // end of downheap
        }
        _elements[droppingElement].index(index);
        
        _elements[element].markDeleted();
        COUNTING( counter.incDouble(COUNT_DEL_MIN) );
        return element;
    }

    /**
     * Deletes an arbitrary element in this heap.
     * Precondition: The heap is not empty.
     * @return the index of the deleted element
     */
    Count deleteArbitrary() {
        assert( ! empty() );         
        Count element = _heap.back().first;
        _heap.pop_back();
        _elements[element].markDeleted();
        return element;
    }

    /**
     * Increases the key of the given element:
     * sets the key to the given value.
     */
    void increaseKey(Count element, Key newKey) {
        Count index = _elements[element].index();
        assert( index < _heap.size() );
        assert( _heap[index].first == element );
        
        _elements[element].key(KeyExtractor::key(newKey));
        _heap[index].second = newKey;
        downheap(index);
        COUNTING( counter.incDouble(COUNT_INCR_KEY) );
    }
    
    /**
     * Decreases the key of the given element:
     * sets the key to the given value.
     */
    void decreaseKey(Count element, Key newKey) {
        Count index = _elements[element].index();
        assert( index < _heap.size() );
        assert( _heap[index].first == element );
        
        _elements[element].key(KeyExtractor::key(newKey));
        _heap[index].second = newKey;
        upheap(index);
        COUNTING( counter.incDouble(COUNT_DECR_KEY) );
    }
    
    /**
     * Updates the key of the given element:
     * sets the key to the given value.
     * The key can be larger or smaller than the previous key.
     */
    void updateKey(Count element, Key newKey) {
        Count index = _elements[element].index();
        assert( index < _heap.size() );
        assert( _heap[index].first == element );
        Key oldKey = _heap[index].second;

        _elements[element].key(KeyExtractor::key(newKey));
        _heap[index].second = newKey;
        if (newKey < oldKey)
        {        
            upheap(index);
            COUNTING( counter.incDouble(COUNT_DECR_KEY) );
        }
        else if (newKey > oldKey)
        {
            downheap(index);
            COUNTING( counter.incDouble(COUNT_INCR_KEY) );
        }
    }

    void clear() {
        _elements.clear();
        insertDummy();
    }
    
    /**
     * For debugging purpose.
     * @return _heap is still a binary heap.
     */
    bool checkHeapProperty() 
    {
        bool result = true;
        for ( unsigned int i = 2; i < _heap.size(); i++ )
        {
            if (!(_heap[i/2].second <= _heap[i].second))
            {
                cerr << "_heap[" << (i/2) << "] = " << _heap[i/2].second;
                cerr << " > " << _heap[i].second << " = _heap[" << i << "]" << endl;
                result = false;
                break;
            }            
        }
        return result;
    }
    
    
private:
    typedef pair<Count, Key> IndexKey;
    
    /**
     * The elements of this heap.
     * The order corresponds to the order of insertion and
     * is not changed by any heap operation. That implies
     * that an index of this vector can be used as a pointer
     * to the corresponding element.
     * It is possible that this vector contains holes (i.e.
     * dummy elements), namely for each element that has been
     * inserted into the other pqueue (for the other search direction)
     * but not to this one.
     * The first element (index 0) is a dummy element so that the index 0
     * can be used to mark elements that have not been inserted into the heap.
     */
    vector<PQElement> _elements;
    
    /** 
     * "Pointers" (first) to the elements of this heap in the right heap order.
     * In addition (second), the internal key of the corresponding element.
     * The first element (index 0) is a dummy element so that the index 0
     * can be used to mark elements that have been deleted from the heap.
     */
    vector<IndexKey> _heap;
    
    
    

    /** 
     * Move the element with the given index upwards
     * until the heap condition is restored.
     */
    void upheap(Count index) {
	
        Count risingElement = _heap[index].first;
        Key k = _heap[index].second;
        while (_heap[index / 2].second > k) {
            assert( index > 1 );
            assert( _elements[_heap[index / 2].first].key() == KeyExtractor::key(_heap[index / 2].second) );
            _heap[index] = _heap[index / 2];
            _elements[_heap[index].first].index(index);
            index /= 2;
        }
        _heap[index].first = risingElement;
        _heap[index].second = k;
        _elements[risingElement].index(index);
    }

    /** 
     * Move the element with the given index downwards.
     * until the heap condition is restored. 
     */
    void downheap(Count index) {
        Count descendingElement = _heap[index].first;
        Key k = _heap[index].second;        
        Count maxIndex;
        if (2*index < _heap.size() && _heap[2*index].second < k) 
        {
            maxIndex = 2*index;
        }
        else
        {
            maxIndex = index;
        }
        if ((2*index + 1) < _heap.size() && _heap[2*index+1].second < _heap[maxIndex].second) 
        {
            maxIndex = 2*index + 1;   
        }
        while (maxIndex != index) {
            assert( index >= 1 );
            assert( _elements[_heap[maxIndex].first].key() == KeyExtractor::key(_heap[maxIndex].second) );
            _heap[index] = _heap[maxIndex];
            _elements[_heap[index].first].index(index);
            index = maxIndex;
            _heap[index].second = k;

            if (2*index < _heap.size() && _heap[2*index].second < k) 
            {
                maxIndex = 2*index;
            }
            else
            {
                maxIndex = index;
            }
            if ((2*index + 1) < _heap.size() && _heap[2*index+1].second < _heap[maxIndex].second) 
            {
                maxIndex = 2*index + 1;   
            }
        }
        _heap[index].first = descendingElement;
        _elements[descendingElement].index(index);
    }

};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/** 
 * Encapsulates data that is associated with a node that is in the
 * pqueue, only stores NodeID
 */
class SimplePQueueNode
{
public:
    /**
     * Initializes this object.
     * @param nodeID the id of the node this object is associated with
     */
    void init(NodeID nodeID) {
        _nodeID = nodeID;
    }

    /** Returns the id of the node this object is associated with. */ 
    NodeID nodeID() const {return _nodeID;}
    
private:
    /** The id of the node this object is associated with. */
    NodeID _nodeID; // possibility to save memory: use parentEdge.target()
};

/** 
 * Encapsulates data that is associated with a node that is in the
 * pqueue (for a certain search direction).
 */
class PQueueNode
{
public:
    /**
     * Initializes this object.
     * @param nodeID the id of the node this object is associated with
     */
    void init(NodeID nodeID) {
        _nodeID = nodeID;
    }

    /**
     * Marks that this node is the start node of a search.
     * This is done by setting the parent pointer so
     * that a self-loop is created.
     */
    void setStartNode() { _parentNode = _nodeID; }

    bool isStartNode() const {return (_parentNode == _nodeID);}

    /**
     * This method is never used. However, the compiler is not able to realize this fact.
     * @see PQueueNodeConstruction::updateParent(...)
     */
    void updateParent(NodeID parentNodeID, EdgeID parentEdgeID, EdgeWeight parentEdgeWeight, EdgeWeight parentKey,
		      const PQueueNode& parentData, EdgeWeight dH, NodeID next) {
	assert(false);
    }

    /**
     * Set a new parent for this node.
     * @param parentNode the parent node
     * @param parentEdge the edge that leads from the parent to this node     
     */
    void updateParent(NodeID parentNode, EdgeID parentEdge) {
        _parentNode = parentNode;
        _parentEdge = parentEdge;
    }

    /** Returns the id of the node this object is associated with. */ 
    NodeID nodeID() const {return _nodeID;}
    /** Returns the id of the parent node. */
    NodeID parentNode() const {return _parentNode;}
    /** Returns the id of the edge that leads from the parent to this node. */
    EdgeID parentEdge() const {return _parentEdge;}

    
    /**
    * The stalling concept is only used in case of the HNR query.
    * In PQueueNodeDynQuery, this method is overwritten appropriately.
    * In all other cases (in particular during a HH query), there are
    * NO stalled nodes.
    * @see DijkstraTemplate::obtainRelevantSearchSpace
    */
    bool stalled() const {return false;}
    
    
    // The following dummy methods are required to provide the same interface
    // in all PQueueNode subclasses so that the compiler does not complain.
    // These dummy methods are never called.
    bool isCovered() const { assert(false); return false; }
    void unstall() { assert(false); }
    EdgeWeight stallKey() const { assert(false); return 0; }
    void stallKey(EdgeWeight k) { assert(false); }
    NodeID hwyNodeCount() const { assert(false); return 0; }
    void setHwyNodeCount(NodeID c) { assert(false); }
    void incHwyNodeCount() { assert(false); }
    void setMaverick() { assert(false); }
    bool isActive() const { assert( false ); return false; }
    void setActive(bool act) { assert( false ); }
    void setLevel(LevelID level, EdgeWeight dist) { assert( false ); }
    LevelID level() const { assert( false ); return 0; }
    EdgeWeight distanceToLevelBorder() const { assert( false ); return 0; }
    EdgeWeight potential() const { assert( false ); return 0; }
    void setPotential(EdgeWeight pot) { assert( false ); }
    void next(const NodeID n) { assert(false); }
    NodeID next() const { assert(false); return 0; }
    bool isTarget() const { assert( false ); return false; }
    void setTarget(bool value) { assert( false ); }
    bool isEliminated() const { assert(false); return false; }
    void setEliminated(bool value) { assert(false); }
    unsigned int hops() const { assert( false ); return 0; }
    void setHops(unsigned int v) { assert( false ); }    
    
private:
    /** The id of the node this object is associated with. */
    NodeID _nodeID; // possibility to save memory: use parentEdge.target()
    /** The id of the parent node. */
    NodeID _parentNode;
    /** The id of the edge that leads from the parent to this node. */
    EdgeID _parentEdge;
};


/**
 * Local search node. Stores several flags and a hop-counter
 * that is used to stop the search erlier.
 */
class PQueueNodeLocalSearch : public PQueueNode
{
public:
    PQueueNodeLocalSearch() 
    : _target(false), 
      _eliminated(false), 
      _hops(0) {}
    
    /** If node is a target of the local search */
    bool isTarget() const { return _target; }
    void setTarget(bool value) { _target = value; }

    
    /* If node is already elminated / contracted. */
    bool isEliminated() const { return _eliminated; }
    void setEliminated(bool value) { _eliminated = value; }
    
    /* Hops (edges) to the source node. */
    unsigned int hops() const { return _hops; }
    void setHops(unsigned int v) { _hops = v; }    
private:
    bool _target:1;
    bool _eliminated:1;
    unsigned int _hops:30;
};


/**
 * Encapsulates data that is associated with a node that is in the
 * pqueue (for a certain search direction) during a highway search.
 */
class PQueueNodeHwySearch : public PQueueNode
{
public:
    PQueueNodeHwySearch() : _potential(0), _active(true) {}
    
    /**
     * Sets the search level and the distance to the border
     * of the applicable neighbourhood.
     */
    void setLevel(LevelID level, EdgeWeight dist) {
        _level = level;
        _distToLevelBorder = dist;
    }

    void setPotential(EdgeWeight pot) {
        _potential = pot;
    }

    /** Returns the search level. */
    LevelID level() const {return _level;}
    
    /** Returns the distance to the border of the applicable neighbourhood. */
    EdgeWeight distanceToLevelBorder() const {return _distToLevelBorder;}

    EdgeWeight potential() const {return _potential;}

    bool isActive() const { return _active; }
    
    void setActive(bool act) { _active = act; }
    
private:
    /** The search level. */
    LevelID _level;
    
    /** 
     * The distance to the border of the applicable neighbourhood.
     * When this border is crossed, the current search level is left.
     */
    EdgeWeight _distToLevelBorder;

    /**
     * Used to cache the potential of this node during the main phase of a landmark query to
     * avoid multiple calls of the potential function.
     */
    EdgeWeight _potential;

    /**
     * A flag used to perform "active/passive searches".
     * Used for transit node routing: determine middle access points.
     */
    bool _active;
};


/**
 * Encapsulates data that is associated with a node that is in the
 * pqueue (for a certain search direction) during the construction
 * of the dynamic method.
 */
class PQueueNodeDynConstr : public PQueueNode
{
private:
    typedef unsigned short CounterType;
    
public:
    PQueueNodeDynConstr() : _hwyNodeCounter(0), _maverick(false) {}

    CounterType hwyNodeCount() const { return _hwyNodeCounter; }
    
    void setHwyNodeCount(const CounterType c) {
        _hwyNodeCounter = c;
        _maverick = false;
    }
    
    void incHwyNodeCount() { _hwyNodeCounter++; }

    bool isActive() const { return ((hwyNodeCount() == 0) && (!_maverick)); }

    bool isCovered() const { return (hwyNodeCount() > 0); }

    void setMaverick() { _maverick = true; }
    
private:
    /**
    * Counts the number of nodes that belong to the overlay graph
    * on the path from the root to this node. Used for the stall-
    * in-advance technique. A node with a counter value >= 1 is
    * covered. A node with a counter value of 1 whose parent is
    * not covered is a covering node.
    */
    CounterType _hwyNodeCounter;

    bool _maverick;
};


/**
 * Encapsulates data that is associated with a node that is in the
 * pqueue (for a certain search direction) during a query of the
 * dynamic method.
 */
class PQueueNodeDynQuery : public PQueueNode
{
public:
    PQueueNodeDynQuery() {unstall();}
    
    bool stalled() const { return (_stallKey != Weight::MAX_VALUE); }
    void unstall() { _stallKey = Weight::MAX_VALUE; }

    EdgeWeight stallKey() const { return _stallKey; }
    void stallKey(EdgeWeight k) { _stallKey = k; }
    
private:
    /**
    * If this node u has been stalled, some path from the source to u
    * has been found that is shorter than the key of u. The length of
    * this path is stored in the 'stallKey'. If the node has not been
    * stalled, the stallKey is 'infinity'.
    */
    EdgeWeight _stallKey;
};


/**
 * Encapsulates data that is associated with a node that is in the
 * pqueue during the construction.
 */
class PQueueNodeConstruction : public PQueueNode
{
private:
    // we rely on MAVERICK > ACTIVE > PASSIVE; thus, don't change the order
    static const char PASSIVE = 0;
    static const char ACTIVE = 1;
    static const char MAVERICK = 2;
    
public:
    /** Constructor. */
    PQueueNodeConstruction() :
        _distToBorderOFs1(Weight::SIGNED_MAX_VALUE), _dist_s0u(Weight::MAX_VALUE), _next(0), _status(ACTIVE), _slackDefined(false) {}

    /**
     * Set a new parent for this node.
     * @param parentNodeID the parent node
     * @param parentEdgeID the edge that leads from the parent to this node
     * @param parentEdgeWeight the weight of the edge from the parent to this node
     * @param parentKey the distance from source to parent
     * @param parentData a reference to the PQueueNodeConstruction object of the parent node
     * @param dH the neighbourhood radius of this node
     * @param next the index of the next parent or 0 if there is only a single parent
     */
    void updateParent(NodeID parentNodeID, EdgeID parentEdgeID, EdgeWeight parentEdgeWeight, EdgeWeight parentKey,
		      const PQueueNodeConstruction& parentData, EdgeWeight dH, NodeID next) {
	assert( dH <= Weight::SIGNED_MAX_VALUE );
	assert( parentEdgeWeight <= Weight::SIGNED_MAX_VALUE );
        
        // if this node is s_1 (i.e., its parent has key 0), then the distance to the neighbourhood border of s_1
        // is set to the neighbourhood radius of s_1; otherwise, we adapt the distance stored at the parent node
	SignedEdgeWeight currentDist = (parentKey == 0) ? dH : (parentData.distToBorderOFs1() - parentEdgeWeight);
	    
	if (next == 0) { // if there is only a single tentative parent...
            // set the single parent
	    PQueueNode::updateParent( parentNodeID, parentEdgeID );
	    distToBorderOFs1( currentDist );
	    // inherit data from the parent
	    dist_s0u( parentData.dist_s0u() );
	    _status = parentData._status;	    
	}
	else { // if there are multiple tentative parents...
            // do not just inherit data from the new parent, but take the maxima of the already stored data
            // and the data stored at the new parent
	    distToBorderOFs1( max(distToBorderOFs1(), currentDist) );
	    dist_s0u( max(dist_s0u(), parentData.dist_s0u()) );
	    _status = max(_status, parentData._status);
	}
	setNext(next);
    }

    /**
     * This method is never used. However, the compiler is not able to realize this fact.
     * @see PQueueNode::updateParent(...)
     */
    void updateParent(NodeID parentNode, EdgeID parentEdge) {
	assert(false);
    }

    /** Returns true iff the neighbourhood of s_1 has been left. */
    bool nbrhOFs1Left() const {return (_distToBorderOFs1 < 0);}

    /** 
     * Returns true iff the distance from s_0 to u has been defined.
     * @see _dist_s0u
     */
    bool defined_dist_s0u() const {return (_dist_s0u != Weight::MAX_VALUE);}
    /**
     * Returns the distance from s_0 to u.
     * @see _dist_s0u
     */
    EdgeWeight dist_s0u() const {return _dist_s0u;}
    /**
     * Sets the distance from s_0 to u.
     * @see _dist_s0u
     */
    void dist_s0u(EdgeWeight d) {_dist_s0u = d;}

        

    /**
     * Initializes the slack.
     * @param sl the dH value of the corresponding node
     * @see _slack
     */
    void initSlack(EdgeWeight sl) {
	assert( sl <= Weight::SIGNED_MAX_VALUE );
	SignedEdgeWeight signedSlack = (SignedEdgeWeight)sl;
	
        if (! _slackDefined) {
            // leaf
	    setSlack(signedSlack);
        }
        else {
            // inner node
            if (signedSlack < slack()) setSlack(signedSlack);
        }
    }
    
    /**
     * Updates the slack.
     * @param child the child from which the update is invoked
     * @param dist the distance from the child to this node
     * @return true iff the slack of this node GETS negative
     *         (note: false if the slack IS already negative, but this
     *          child does not cause a negative value)
     */
    bool updateSlack(const PQueueNodeConstruction& child, EdgeWeight dist) {
	assert( dist <= Weight::SIGNED_MAX_VALUE );
	const SignedEdgeWeight diff = child.slack() - (SignedEdgeWeight)dist;
        assert( (diff < 0) || (child.slack() >= (SignedEdgeWeight)dist) );
        if (diff < 0) {
            // the child causes a negative value of this slack
	    setSlack(diff);
            return true;
        }
        
        // if this slack is already negative, do nothing
        if ((_slackDefined) && (slack() < 0)) return false;
        
        // if this slack hasn't been defined yet
        // or the child causes a decrease of this slack...        
        if ((!_slackDefined) || (diff < slack())) setSlack(diff); // update this slack
        return false;
    }

    /** 
     * Deactivates this node iff the abort criterion is fulfilled. 
     * @param dist the distance from s_0 to p
     * @param dH the value d_H(p)
     */
    void abort(EdgeWeight dist, EdgeWeight dH) {        
        if (defined_dist_s0u()) { // d(s_0, u) has to be defined; otherwise, we can't abort
            assert( dist >= dist_s0u() );
            EdgeWeight w = dist - dist_s0u(); // compute d(u,p)            
            if (w > dH) _status = PASSIVE; // deactivate
        }
    }

    /** Returns true iff this node is active. */
    bool isActive() const { return (_status >= ACTIVE); }

    /** Returns true iff this node is a maverick. */
    bool isMaverick() const { return (_status == MAVERICK); }

    /** Marks that this node is a maverick. */
    void setMaverick() {
        // note: only an active node can be a maverick
        if (isActive()) _status = MAVERICK;
    }

    /** Sets the index of the next parent. */
    void setNext(const NodeID n) {
	_next = (unsigned short)n;
	assert( next() == n );
    }

    /** Returns the index of the next parent. */
    NodeID next() const {return _next;}
    
private:    
    /**
     * The distance to the border of the neighbourhood of s_1.
     * Also used to store the slack during the backward evaluation phase
     * of the construction, where the distance to the neighbourhood border
     * is no longer needed.
     */
    SignedEdgeWeight _distToBorderOFs1;
    
    /** 
     * The distance from s_0 to u.
     * u is the direct predecessor of v, which is the last node within the
     * neighbourhood of s_1.
     */
    EdgeWeight _dist_s0u;
    
    /** The index of the next parent. 0 if there is no next parent. */
    unsigned short _next;

    /** The status of the node (passive, active, maverick). */
    char _status;

    /** True iff the slack has already been defined. */
    bool _slackDefined;
    

    
    /** Returns the distance to the border of the neighbourhood of s_1. */
    SignedEdgeWeight distToBorderOFs1() const {return _distToBorderOFs1;}

    /** Sets the distance to the border of the neighbourhood of s_1. */
    void distToBorderOFs1(const SignedEdgeWeight d) {_distToBorderOFs1 = d;}

    /** Sets the slack. */
    void setSlack(const SignedEdgeWeight s) {
        // we use _distToBorderOFs1 to store the slack in order to save memory
	_distToBorderOFs1 = s;
	_slackDefined = true;
    }

    /** Returns the slack. */
    SignedEdgeWeight slack() const {
	assert( _slackDefined );
	return _distToBorderOFs1;
    }
};

/** Priority Queue that is used by the original version of Dijkstra's algorithm. */
typedef BinaryHeap< EdgeWeight, Weight, PQueueNode, NodeID > NormalPQueue;
/** Priority Queue that is used by the multilevel query algorithm ("highway search"). */
typedef BinaryHeap< EdgeWeight, Weight, PQueueNodeHwySearch, NodeID > HwyPQueue;
/** Priority Queue that is used during the construction. */
typedef BinaryHeap< EdgeWeight, Weight, PQueueNodeConstruction, NodeID > ConstrPQueue;

typedef BinaryHeap< EdgeWeight, Weight, PQueueNodeDynConstr, NodeID > DynConstrPQueue;
typedef BinaryHeap< EdgeWeight, Weight, PQueueNodeDynQuery, NodeID > DynQueryPQueue;

/** Priority Queue that is used during the node contraction of CH for the local searches. */
typedef BinaryHeap< EdgeWeight, Weight, PQueueNodeLocalSearch, NodeID > LocalSearchPQueue;

typedef NormalPQueue::PQElement NormalPQElement;
typedef HwyPQueue::PQElement HwyPQElement;
typedef ConstrPQueue::PQElement ConstrPQElement;
typedef DynConstrPQueue::PQElement DynConstrPQElement;
typedef DynQueryPQueue::PQElement DynQueryPQElement;


#endif // BINARYHEAP_H
