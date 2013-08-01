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


#ifndef GRAPH_H
#define GRAPH_H


#include <iostream>
#include <iomanip>
#include <vector>


using namespace std;

/** 
 * Type of a node ID.
 * Also used for everything that is bounded by the number of nodes,
 * e.g. the number of components.
 */
typedef unsigned int NodeID;

/** Type of an edge ID. */
typedef unsigned int EdgeID;

/** Type of a source/target node pair. */
typedef pair<NodeID, NodeID> stPair;
typedef vector<stPair> stPairs;

/** 
 * Special node ID which is normally not used
 * so that it can be used as flag.
 */
static const NodeID SPECIAL_NODEID = __INT_MAX__ * 2U + 1;

static const EdgeID MAX_EDGEID = __INT_MAX__ * 2U;

/** Type of a level ID. */
typedef int LevelID;

///////////////////////////////////////////////////////////////////////////////////////////////
///		#include "edge.h"

#include "serialize.h"

/** Normal edge. */
const static NodeID EDGE_TYPE_ORIGINAL = 0;
/** Normal shortcut edge. */
const static NodeID EDGE_TYPE_SHORTCUT = 1;
/** Shortcut edge representing a witness path. Such shortcuts
 *  are used during construction if possible witnesses are known
 *  in advance.
 */
const static NodeID EDGE_TYPE_WITNESS_SHORTCUT = 2;

/** 
 * Represents a weighted edge assuming the source node is known,
 * i.e. only the target node is stored.
 * Base class for both EdgeCHFast
 */
template <NodeID TARGET_SHIFT>
class EdgeTemplate
{
    /** Outputs data about the edge for debugging purposes. */
    friend std::ostream& operator<<( std::ostream& os, const EdgeTemplate &edge ) {
        os << "(";
        if (edge.isDirected(1)) os << "<";
        os << "-";
        if (edge.isDirected(0)) os << ">";
        os << edge.target() << ", " << edge.weight() << ")";
        return os;
    }
    
public:
    /**
     * Returns true iff the given edge and this edge are equal with respect
     * to target and direction, but NOT necessarily weight.
     * Note that edge1 == edge2 does not exclude edge1 "<" edge2.
     */
    bool operator== (const EdgeTemplate& e) const {
        return ((target() == e.target()) &&
                (isDirected(0) == e.isDirected(0)) && (isDirected(1) == e.isDirected(1)));
    }
    
    /** 
     * Compares two edges first by target, in case of equality by weight.
     * Returns false if both edges are identical.
     */
    bool operator< (const EdgeTemplate& e) const {
        if (target() == e.target()) return (weight() < e.weight());
        return (target() < e.target());
    }

    /** Mark (during construction) that this edge belongs to the highway network. */
    void setHighwayEdge() {setFlag(HWY);}
    
    /** Reset the flag that this edge belongs to the highway network. */
    void unsetHighwayEdge() {unsetFlag(HWY);}

    NodeID target() const {return (_target >> TARGET_SHIFT);}
    EdgeWeight weight() const {return _weight;}

    bool isHighwayEdge() const {return (isFlag(HWY));}

    /** Returns true iff this edge is open in the given direction. */
    bool isDirected(int searchID) const {return (isFlag(direction(searchID)));}
    
    /** Returns true iff this edge is open in both directions (two-way). */
    bool isBidirected() const {return (isDirected(0) && isDirected(1));}

    bool isClosed() const {return ( (!isDirected(0)) && (!isDirected(1)) );}
    
    void makeTwoWay() {
        setFlag(direction(0));
        setFlag(direction(1));
    }
    
    void makeOneWay(int searchID) {
        setFlag(direction(searchID));
        unsetFlag(direction(1-searchID));
    }

    void makeClosed() {
        unsetFlag(direction(0));
        unsetFlag(direction(1));
    }

    /**
    * Ensures that this edge is at least open in the same
    * directions as the given edge.
    */
    void makeOpen(const EdgeTemplate& e) {
        if (e.isDirected(0)) setFlag(direction(0));
        if (e.isDirected(1)) setFlag(direction(1));
    }

    /**
    * Ensures that this edge is at least open in the opposite
    * directions as the given edge.
    */
    void makeOpenReversed(const EdgeTemplate& e) {
        if (e.isDirected(0)) setFlag(direction(1));
        if (e.isDirected(1)) setFlag(direction(0));
    }

    /**
    * Ensures that this edge is not open for any direction
    * that the given edge is open.
    */
    void makeClosed(const EdgeTemplate& e) {
        if (e.isDirected(0)) unsetFlag(direction(0));
        if (e.isDirected(1)) unsetFlag(direction(1));
    }

    /** Serializes this edge to a given stream. */
    void serialize(ostream& out) const {
        writePrimitive(out, _target);
        writePrimitive(out, _weight);
    }

    /** Deserializes this edge from a given stream. */
    void deserialize(istream& in) {
        readPrimitive(in, _target);
        readPrimitive(in, _weight);
    }
    
protected:
    // counting from LSB (1) to MSB (32):
    // bit 1 and 2
    NodeID direction(int searchID) const {
        assert( (searchID >= 0) && (searchID < 2) );
        return searchID+1;
    }
    // bit 3
    static const NodeID HWY = 4;
    
    bool isFlag(NodeID flag) const {return ((_target & flag) != 0);}
    void setFlag(NodeID flag) {_target |= flag;}
    void unsetFlag(NodeID flag) {_target &= (~flag);}
    
    void setTarget(const NodeID t) 
    {
        _target = t << TARGET_SHIFT; 
        // treat SPECIAL_NODEID for unused edges different
        assert( t == SPECIAL_NODEID || target() == t );
    }
    
    /**
     * The target node.
     * In addition, some flags.
     */
    NodeID _target;
    
    /** The edge weight. */
    EdgeWeight _weight;
};


/** 
 * Represents a weighted edge assuming the source node is known,
 * i.e. only the target node is stored.
 * Used for Contraction Hierachies.
 *
 * There are two different edge classes for CH, EdgeCHFast and EdgeCH.
 * As the name says, EdgeCHFast is faster than EdgeCH but also more compicated.
 * The difference is how to store multiple values in one integer:
 *   EdgeCHFast: manually write the code to perform shift and mask operations
 *   EdgeCH: let the compiler generate this code. This is done by
 *           adding the number of bytes to each variable with a colon,
 *           e.g. NodeID _type:2; requires only 2 bytes.
 *
 */
class EdgeCHFast : public EdgeTemplate<4>
{
public:
    /** Default constructor. target and weight are set to 0.*/
    EdgeCHFast() {
        _target = 0;
        _weight = 0;
        assert( isClosed() );
        setShortcutOriginalEdgeCount(1);
    }
    
    /** 
     * Constructor. 
     * @param target the target node
     * @param weight the weight
     * @param type edge type (see constants EDGE_TYPE_...)
     * @param forward from source to target open
     * @param backward from target to source open
     * @param shortcutMiddle Not used, see EdgeCHExpand.
     * @param shortcutEdge1 Not used, see EdgeCHExpand.
     * @param shortcutEdge2 Not used, see EdgeCHExpand.
     * @param shortcutEdgeOriginalEdgeCount used for a priority term during CH node ordering
     */
    EdgeCHFast(NodeID target, EdgeWeight weight, NodeID type, bool forward, bool backward,
        NodeID shortcutMiddle = SPECIAL_NODEID, EdgeID shortcutEdge1 = SPECIAL_NODEID, 
        EdgeID shortcutEdge2 = SPECIAL_NODEID, EdgeID shortcutEdgeOriginalEdgeCount = 1) {
        setTarget(target);
        if (forward) setFlag(direction(0));
        if (backward) setFlag(direction(1));
        setType(type);
        
        _weight = weight;
        setShortcutOriginalEdgeCount(shortcutEdgeOriginalEdgeCount);
    }

    /** Constructs the reverse edge of the given edge. */
    EdgeCHFast(NodeID target, const EdgeCHFast& reverseEdge) {
        setTarget(target);
        if (reverseEdge.isDirected(1)) setFlag(direction(0));
        if (reverseEdge.isDirected(0)) setFlag(direction(1));
        setType(reverseEdge.type());
        
        _weight = reverseEdge.weight();
        copyShortcutInfo(reverseEdge, true);
    }

    /** Weight of the edge */
    void setWeight(EdgeWeight w) {_weight = w;}

    /** Is a shortcut edge. */
    bool isShortcut() const {
        return (type() != 0);
    }

    /** Type of the edge, see EDGE_TYPE_... constanst above. */
    NodeID type() const {return ((_target >> TYPE_SHIFT) & 3);}
    void setType(NodeID k) {
        assert( k < 4 );
        _target -= ( type() << TYPE_SHIFT );
        _target += (k << TYPE_SHIFT);
        assert( type() == k );
    }

    bool isIdentical(const EdgeCHFast& e) const {
        return ((*this == e) && (weight() == e.weight()) && (type() == e.type()));
    }

    bool isReverse(NodeID u, const EdgeCHFast& e) const {
        return ((target() == u) && (weight() == e.weight()) &&
                (isDirected(0) == e.isDirected(1)) && (isDirected(1) == e.isDirected(0)) &&
                (type() == e.type()));
    }

    /** 
     *  Methods required to unpack shortcuts. Not supported.
     *  EdgeCHExpand supports them (switch USE_CH_EXPAND on in config.h)
     */
    void setShortcutMiddle(NodeID middle) { }
    NodeID shortcutMiddle() const { return SPECIAL_NODEID; }
    void setShortcutEdge1(EdgeID e) { }
    NodeID shortcutEdge1() const { return shortcutEdgeLimit(); }
    void setShortcutEdge2(EdgeID e) { }
    NodeID shortcutEdge2() const { return shortcutEdgeLimit(); }
    NodeID shortcutEdgeLimit() const { return SPECIAL_NODEID; }
    void copyShortcutInfo(const EdgeCHFast& e, bool reverse) 
    {
        #ifdef COUNT_SHORTCUT_ORIGINAL_EDGES
        _shortcutOriginalEdgeCount = e.shortcutOriginalEdgeCount();
        #endif
    }

    /** 
     * Counter for the number of original edges a shortcut represents.
     * Used as priority term during node ordering.
     * (switch COUNT_SHORTCUT_ORIGINAL_EDGES on in config.h)
     */
    void setShortcutOriginalEdgeCount(const EdgeID count)
    {
        #ifdef COUNT_SHORTCUT_ORIGINAL_EDGES
        _shortcutOriginalEdgeCount = count;           
        #endif
    }
    EdgeID shortcutOriginalEdgeCount() const
    {
        #ifdef COUNT_SHORTCUT_ORIGINAL_EDGES
        return _shortcutOriginalEdgeCount;
        #else
        return SPECIAL_NODEID;
        #endif
    }
    
protected:
    // counting from LSB (1) to MSB (32):
    // 1,2: direction
    // 3,4: type
    //
    static const NodeID TYPE_SHIFT = 2;
    
    #ifdef COUNT_SHORTCUT_ORIGINAL_EDGES
    EdgeID _shortcutOriginalEdgeCount;
    #endif
};


/** 
 * Represents a weighted edge assuming the source node is known,
 * i.e. only the target node is stored.
 * Used for Contraction Hierarchies.
 * Despite the name, EdgeCHExpand seems to be faster
 * than EdgeCHExpandFast. This class is currently Not
 * used.
 */
class EdgeCHExpandFast : public EdgeCHFast
{
public:
    /** Default constructor. target and weight are set to 0.*/
    EdgeCHExpandFast()
        : EdgeCHFast() {
        _shortcut = 0;
    }
    
    /** 
     * Constructor.
     * @param target the target node
     * @param weight the weight
     * @param type edge type (see constants EDGE_TYPE_...)
     * @param forward from source to target open
     * @param backward from target to source open
     * @param shortcutMiddle middle nod of the path a shortcut represents
     * @param shortcutEdge1 relative index into the edge array of the first edge of the path
     * @param shortcutEdge2 relative index into the edge array of the second edge of the path
     * @param shortcutEdgeOriginalEdgeCount used for a priority term during CH node ordering
     */
    EdgeCHExpandFast(NodeID target, EdgeWeight weight, NodeID type, bool forward, bool backward, 
        NodeID shortcutMiddle = SPECIAL_NODEID, EdgeID shortcutEdge1 = SPECIAL_NODEID, 
        EdgeID shortcutEdge2 = SPECIAL_NODEID, EdgeID shortcutOriginalEdgeCount = 1) :
        EdgeCHFast(target,weight,type,forward,backward) {
        setShortcutMiddle(shortcutMiddle);
        setShortcutEdge1(shortcutEdge1);
        setShortcutEdge2(shortcutEdge2);
        setShortcutOriginalEdgeCount(shortcutOriginalEdgeCount);
    }

    /** Constructs the reverse edge of the given edge. */
    EdgeCHExpandFast(NodeID target, const EdgeCHExpandFast& reverseEdge) :
        EdgeCHFast(target, reverseEdge) {
        copyShortcutInfo(reverseEdge, true);
    }
    EdgeWeight weight() const {return _weight & WEIGHT_MASK;}
    void setWeight(EdgeWeight w) {
        assert( w < (1 << (32-SHORTCUT_SHIFT)) );
        _weight = (_weight & ~WEIGHT_MASK) | w;
        assert( w == weight() );
    }
    

    // ***
    // To expand a path with a recursive unpacking routine,
    // three values are used, the middle node (shortcutMiddle)
    // and two relative indices into the edge array of the middle
    // node (shortcutEdge1, shortcutEdge2) that form the path
    // the shortcut represents.
    // ***

    void setShortcutMiddle(NodeID middle) { 
        assert( middle == SPECIAL_NODEID || middle < (1 << (32-SHORTCUT_SHIFT)) );
        _shortcut = (_shortcut & SHORTCUT_EDGE_LIMIT) | (middle << SHORTCUT_SHIFT); 
        assert( middle == SPECIAL_NODEID || shortcutMiddle() == middle );
    }
    NodeID shortcutMiddle() const { return _shortcut >> SHORTCUT_SHIFT; }
    
    void setShortcutEdge1(EdgeID shortcutEdge1) 
    {
        if (shortcutEdge1 < SHORTCUT_EDGE_LIMIT)
        {
            _weight = weight() | (shortcutEdge1 << (32-SHORTCUT_SHIFT));
            assert( this->shortcutEdge1() == shortcutEdge1 );
        }
        else
        {
            _weight = _weight | (SHORTCUT_EDGE_LIMIT << (32-SHORTCUT_SHIFT));
            assert( this->shortcutEdge1() == shortcutEdgeLimit() );
        }
    }
    EdgeID shortcutEdge1() const { return _weight >> (32-SHORTCUT_SHIFT); }

    void setShortcutEdge2(EdgeID shortcutEdge2) 
    { 
        if (shortcutEdge2 < SHORTCUT_EDGE_LIMIT)
        {
            _shortcut = (_shortcut & ~SHORTCUT_EDGE_LIMIT) | shortcutEdge2;
            assert( this->shortcutEdge2() == shortcutEdge2 );
        }
        else
        {
            _shortcut = _shortcut | SHORTCUT_EDGE_LIMIT;
            assert( this->shortcutEdge2() == shortcutEdgeLimit() );
        }
    }
    EdgeID shortcutEdge2() const { return _shortcut & SHORTCUT_EDGE_LIMIT; }
    
    EdgeID shortcutEdgeLimit() const { return SHORTCUT_EDGE_LIMIT; }

    void copyShortcutInfo(const EdgeCHExpandFast& e, bool reverse) 
    { 
        setShortcutMiddle(e.shortcutMiddle());
        if ( true || !reverse )
        {
            setShortcutEdge1(e.shortcutEdge1());
            setShortcutEdge2(e.shortcutEdge2());
        }
        else
        {
            setShortcutEdge1(e.shortcutEdge2());
            setShortcutEdge2(e.shortcutEdge1());
        }
        #ifdef COUNT_SHORTCUT_ORIGINAL_EDGES
        _shortcutOriginalEdgeCount = e.shortcutOriginalEdgeCount();           
        #endif
    }
    
    /** Serializes this edge to a given stream. */
    void serialize(ostream& out) const {
        // not tested
        out.write((char*)this,sizeof(EdgeCHExpandFast)/sizeof(char));
    }

    /** Deserializes this edge from a given stream. */
    void deserialize(istream& in) {
        // not tested
        in.read((char*)this,sizeof(EdgeCHExpandFast)/sizeof(char));
    }
    
    
    
protected:
    EdgeID _shortcut;
    static const NodeID SHORTCUT_SHIFT = 6;
    static const NodeID SHORTCUT_EDGE_LIMIT = (1 << SHORTCUT_SHIFT) - 1;
    static const NodeID WEIGHT_MASK = (1 << (32-SHORTCUT_SHIFT)) - 1;
};

/** 
 * Represents a weighted edge assuming the source node is known,
 * i.e. only the target node is stored.
 * Used for Contraction Hierachies.
 *
 * There are two different edge classes for CH, EdgeCHFast and EdgeCH.
 * See EdgeCHFast for the differences.
 */
class EdgeCH
{

    /** Outputs data about the edge for debugging purposes. */
    friend std::ostream& operator<<( std::ostream& os, const EdgeCH &edge ) {
        os << "(";
        if (edge.isDirected(1)) os << "<";
        os << "-";
        if (edge.isDirected(0)) os << ">";
        os << edge.target() << ", " << edge.weight() << ")";
        return os;
    }

public:
    /** Default constructor. target and weight are set to 0.*/
    EdgeCH() {
        _target = 0;
        _weight = 0;
        _type = 0;
        _flags = 0;
        setShortcutOriginalEdgeCount(1);
        assert( isClosed() );
    }
    
    /** 
     * Constructor.
     * @param target the target node
     * @param weight the weight
     * @param type edge type (see constants EDGE_TYPE_...)
     * @param forward from source to target open
     * @param backward from target to source open
     * @param shortcutMiddle Not used, see EdgeCHExpand.
     * @param shortcutEdge1 Not used, see EdgeCHExpand.
     * @param shortcutEdge2 Not used, see EdgeCHExpand.
     * @param shortcutEdgeOriginalEdgeCount used for a priority term during CH node ordering
     */
    EdgeCH(NodeID target, EdgeWeight weight, NodeID type, bool forward, bool backward, 
        NodeID shortcutMiddle = SPECIAL_NODEID, EdgeID shortcutEdge1 = SPECIAL_NODEID, 
        EdgeID shortcutEdge2 = SPECIAL_NODEID, EdgeID shortcutEdgeOriginalEdgeCount = 1) {
        setTarget(target);
        _flags = 0;
        if (forward) setFlag(direction(0));
        if (backward) setFlag(direction(1));
        setType(type);
        _weight = weight;
        setShortcutOriginalEdgeCount(shortcutEdgeOriginalEdgeCount);
        
    }

    /** Constructs the reverse edge of the given edge. */
    EdgeCH(NodeID target, const EdgeCH& reverseEdge) {
        setTarget(target);
        _flags = 0;
        makeOpenReversed(reverseEdge);
        setType(reverseEdge.type());
        _weight = reverseEdge.weight();
        copyShortcutInfo(reverseEdge, true);
    }


    /**
     * Returns true iff the given edge and this edge are equal with respect
     * to target and direction, but NOT necessarily weight.
     * Note that edge1 == edge2 does not exclude edge1 "<" edge2.
     */
    bool operator== (const EdgeCH& e) const {
        return ((target() == e.target()) &&
                (isDirected(0) == e.isDirected(0)) && (isDirected(1) == e.isDirected(1)));
    }
    
    /** 
     * Compares two edges first by target, in case of equality by weight.
     * Returns false if both edges are identical.
     */
    bool operator< (const EdgeCH& e) const {
        if (target() == e.target()) return (weight() < e.weight());
        return (target() < e.target());
    }

    NodeID target() const {return _target; }
    EdgeWeight weight() const {return _weight; }

    /** Returns true iff this edge is open in the given direction. */
    bool isDirected(int searchID) const {return (isFlag(direction(searchID)));}
    
    /** Returns true iff this edge is open in both directions (two-way). */
    bool isBidirected() const {return (isDirected(0) && isDirected(1));}

    bool isClosed() const {return ( (!isDirected(0)) && (!isDirected(1)) );}
    
    bool isHighwayEdge() const { assert( false ); return false; }
    void setHighwayEdge() { assert(false);}
    
    void makeTwoWay() {
        setFlag(direction(0));
        setFlag(direction(1));
    }
    
    void makeOneWay(int searchID) {
        setFlag(direction(searchID));
        unsetFlag(direction(1-searchID));
    }

    void makeClosed() {
        unsetFlag(direction(0));
        unsetFlag(direction(1));
    }

    /**
    * Ensures that this edge is at least open in the same
    * directions as the given edge.
    */
    void makeOpen(const EdgeCH& e) {
        if (e.isDirected(0)) setFlag(direction(0));
        if (e.isDirected(1)) setFlag(direction(1));
    }

    /**
    * Ensures that this edge is at least open in the opposite
    * directions as the given edge.
    */
    void makeOpenReversed(const EdgeCH& e) {
        if (e.isDirected(0)) setFlag(direction(1));
        if (e.isDirected(1)) setFlag(direction(0));
    }

    /**
    * Ensures that this edge is not open for any direction
    * that the given edge is open.
    */
    void makeClosed(const EdgeCH& e) {
        if (e.isDirected(0)) unsetFlag(direction(0));
        if (e.isDirected(1)) unsetFlag(direction(1));
    }

    /** Serializes this edge to a given stream. */
    void serialize(ostream& out) const {
        // not tested
        out.write((char*)this,sizeof(EdgeCH)/sizeof(char));
    }

    /** Deserializes this edge from a given stream. */
    void deserialize(istream& in) {
        // not tested
        in.read((char*)this,sizeof(EdgeCH)/sizeof(char));
    }

    void setWeight(EdgeWeight w) {
        _weight = w;
    }

    NodeID type() const {return _type;}
    
    void setType(NodeID k) {
        _type = k;
    }
    
    bool isShortcut() const { return type() != EDGE_TYPE_ORIGINAL; }

    bool isIdentical(const EdgeCH& e) const {
        return ((*this == e) && (weight() == e.weight()) && (type() == e.type()));
    }

    bool isReverse(NodeID u, const EdgeCH& e) const {
        return ((target() == u) && (weight() == e.weight()) &&
                (isDirected(0) == e.isDirected(1)) && (isDirected(1) == e.isDirected(0)) &&
                (type() == e.type()));
    }
    
    void setShortcutMiddle(NodeID middle) { }
    NodeID shortcutMiddle() const { return SPECIAL_NODEID; }

    void setShortcutEdge1(EdgeID e) { }
    EdgeID shortcutEdge1() const { return SPECIAL_NODEID; }

    void setShortcutEdge2(EdgeID e) { }
    EdgeID shortcutEdge2() const { return SPECIAL_NODEID; }
    EdgeID shortcutEdgeLimit() const { return SPECIAL_NODEID; }
    
    void copyShortcutInfo(const EdgeCH& e, bool reverse)
    {
        #ifdef COUNT_SHORTCUT_ORIGINAL_EDGES
        _shortcutOriginalEdgeCount = e.shortcutOriginalEdgeCount();           
        #endif
    }
    
    void setShortcutOriginalEdgeCount(const EdgeID count)
    {
        #ifdef COUNT_SHORTCUT_ORIGINAL_EDGES
        _shortcutOriginalEdgeCount = count;           
        #endif
    }
    
    EdgeID shortcutOriginalEdgeCount() const
    {
        #ifdef COUNT_SHORTCUT_ORIGINAL_EDGES
        return _shortcutOriginalEdgeCount;
        #else
        return SPECIAL_NODEID;
        #endif
    }
    
protected:
    // The colon behind the variable denotes the number of bytes
    // the variable uses in the main memory.
    NodeID _target:26;
    EdgeID _shortcutEdge1:6;
    EdgeWeight _weight:28;
    NodeID _type:2;
    NodeID _flags:2;

    #ifdef COUNT_SHORTCUT_ORIGINAL_EDGES
    EdgeID _shortcutOriginalEdgeCount;
    #endif

    // counting from LSB (1) to MSB (32):
    // bit 1 and 2
    NodeID direction(int searchID) const {
        assert( (searchID >= 0) && (searchID < 2) );
        return searchID+1;
    }
    
    bool isFlag(NodeID flag) const {return ((_flags & flag) != 0);}
    void setFlag(NodeID flag) {_flags |= flag;}
    void unsetFlag(NodeID flag) {_flags &= (~flag);}
    
    void setTarget(const NodeID t) {
        _target = t;
        assert( t == SPECIAL_NODEID || _target == t );
    }
        
};


/** 
 * Represents a weighted edge assuming the source node is known,
 * i.e. only the target node is stored.
 * Used for Contraction Hierarchies.
 *
 * Along with the information normally stored with an EdgeCH
 * three values are stored to expand shortcut edges during
 * a recurisve unpacking routine. Remember that each shortcut
 * represents a path consiting of exactly two edges. To reconstruct
 * this path, we need the middle node of this path (shortcutMiddle).
 * To speedup the unpacking, we also store the relative indices
 * to the two edges of the path into the edge array of the middle node.
 * See DikjstraCH::pathTo() for more details to path expansion.
 */
class EdgeCHExpand : public EdgeCH
{
public:
    /** Default constructor. target and weight are set to 0.*/
    EdgeCHExpand()
        : EdgeCH() {
        _shortcutMiddle = 0;
        _shortcutEdge1 = 0;
        _shortcutEdge2 = 0;
    }
    
    /** 
     * Constructor.
     * @param target the target node
     * @param weight the weight
     * @param type edge type (see constants EDGE_TYPE_...)
     * @param forward from source to target open
     * @param backward from target to source open
     * @param shortcutMiddle middle nod of the path a shortcut represents
     * @param shortcutEdge1 relative index into the edge array of the first edge of the path
     * @param shortcutEdge2 relative index into the edge array of the second edge of the path
     * @param shortcutEdgeOriginalEdgeCount used for a priority term during CH node ordering
     */
    EdgeCHExpand(NodeID target, EdgeWeight weight, NodeID type, bool forward, bool backward, 
        NodeID shortcutMiddle = SPECIAL_NODEID, EdgeID shortcutEdge1 = SPECIAL_NODEID, 
        EdgeID shortcutEdge2 = SPECIAL_NODEID, EdgeID shortcutOriginalEdgeCount = 1) :
        EdgeCH(target,weight,type,forward,backward) {
        setShortcutMiddle(shortcutMiddle);
        if (shortcutEdge1 < SHORTCUT_EDGE_LIMIT)
        {
            _shortcutEdge1 = shortcutEdge1;
        }
        else
        {
            _shortcutEdge1 = SHORTCUT_EDGE_LIMIT;
        }
        if (shortcutEdge2 < SHORTCUT_EDGE_LIMIT)
        {
            _shortcutEdge2 = shortcutEdge2;
        }
        else
        {
            _shortcutEdge2 = SHORTCUT_EDGE_LIMIT;
        }
        setShortcutOriginalEdgeCount(shortcutOriginalEdgeCount);
    }

    /** Constructs the reverse edge of the given edge. */
    EdgeCHExpand(NodeID target, const EdgeCHExpand& reverseEdge) :
        EdgeCH(target, reverseEdge) {
        copyShortcutInfo(reverseEdge, true);
    }

    // ***
    // To expand a path with a recursive unpacking routine,
    // three values are used, the middle node (shortcutMiddle)
    // and two relative indices into the edge array of the middle
    // node (shortcutEdge1, shortcutEdge2) that form the path
    // the shortcut represents.
    // ***

    void setShortcutMiddle(NodeID middle) { 
        _shortcutMiddle = middle; 
        assert( middle == SPECIAL_NODEID || _shortcutMiddle == middle );
    }
    NodeID shortcutMiddle() const { return _shortcutMiddle; }
    
    void setShortcutEdge1(EdgeID shortcutEdge1) 
    {
        if (shortcutEdge1 < SHORTCUT_EDGE_LIMIT)
        {
            _shortcutEdge1 = shortcutEdge1;
        }
        else
        {
            _shortcutEdge1 = SHORTCUT_EDGE_LIMIT;
        }
    }
    EdgeID shortcutEdge1() const { return _shortcutEdge1; }

    void setShortcutEdge2(EdgeID shortcutEdge2) 
    { 
        if (shortcutEdge2 < SHORTCUT_EDGE_LIMIT)
        {
            _shortcutEdge2 = shortcutEdge2;
        }
        else
        {
            _shortcutEdge2 = SHORTCUT_EDGE_LIMIT;
        }
    }
    EdgeID shortcutEdge2() const { return _shortcutEdge2; }
    
    EdgeID shortcutEdgeLimit() const { return SHORTCUT_EDGE_LIMIT; }

    void copyShortcutInfo(const EdgeCHExpand& e, bool reverse) 
    { 
        setShortcutMiddle(e.shortcutMiddle());
        if ( true || !reverse )
        {
            _shortcutEdge1 = e.shortcutEdge1();
            _shortcutEdge2 = e.shortcutEdge2();
        }
        else
        {
            _shortcutEdge1 = e.shortcutEdge2();
            _shortcutEdge2 = e.shortcutEdge1();
        }
        #ifdef COUNT_SHORTCUT_ORIGINAL_EDGES
        _shortcutOriginalEdgeCount = e.shortcutOriginalEdgeCount();           
        #endif
    }
    
    /** Serializes this edge to a given stream. */
    void serialize(ostream& out) const {
        // not tested
        out.write((char*)this,sizeof(EdgeCHExpand)/sizeof(char));
    }

    /** Deserializes this edge from a given stream. */
    void deserialize(istream& in) {
        // not tested
        in.read((char*)this,sizeof(EdgeCHExpand)/sizeof(char));
    }
    
    
    
protected:
    // The colon behind the variable denotes the number of bytes
    // the variable uses in the main memory.
    EdgeID _shortcutEdge2:6;
    NodeID _shortcutMiddle:26;
    static const NodeID SHORTCUT_EDGE_LIMIT = (1 << 6) - 1;
};

// Here, we decide which edge to use.
// Globally declaring the Edge class is unsatisfying and
// legacy of Dominik Schultes code. 
#ifdef USE_CH_EXPAND
typedef EdgeCHExpand Edge;
#else
typedef EdgeCHFast Edge;
#endif


/** Represents a weighted edge including the source node. */
class CompleteEdge : public Edge
{
    /** Outputs data about the edge for debugging purposes. */
    friend std::ostream& operator<<( std::ostream& os, const CompleteEdge &edge ) {
        os << "(" << edge.source();
        
        if (edge.isDirected(1)) os << "<";
        os << "-";
        if (edge.isDirected(0)) os << ">";
        
        os << edge.target() << ", " << edge.weight() << ")";
        return os;
    }
    
public:
    /** 
     * Compares two edges first by source, in case of equality by target, and
     * in case of equality by weight.
     * If all these values are identical, returns true iff this edge is two-way
     * and the given edge is at most one-way.
     */
    bool operator< (const CompleteEdge& e) const {
        if (source() == e.source()) {
            if (target() == e.target()) {
                if (weight() == e.weight()) {
                    return (isDirected(0) && isDirected(1) &&
                            ((! e.isDirected(0)) || (! e.isDirected(1))));
                }
                return (weight() < e.weight());
            }
            return (target() < e.target());
        }
        return (source() < e.source());
    }

    /** Constructor. */
    CompleteEdge() : Edge(), _source(0) {}
    
    /** Constructor. */
    CompleteEdge(NodeID s, NodeID t, EdgeWeight w, bool shortcut, bool forward, bool backward) : 
            Edge(t, w, shortcut, forward, backward), _source(s) {}
    
    /** Constructor. */
    CompleteEdge(const Edge& e, NodeID s) : Edge(e), _source(s) {}

    NodeID source() const {return _source;}

    void serialize(ostream& out) const {
        Edge::serialize(out);
        writePrimitive(out, _source);
    }

    void deserialize(istream& in) {
        Edge::deserialize(in);
        readPrimitive(in, _source);
    }

private:
    NodeID _source;
};

/** Represents an edge in the search space. */
class SearchSpaceEdge
{
public:
    SearchSpaceEdge()
    : _source(0), _target(0), _edgeID(0), _keyOfTarget(0), _searchLevel(0), _core(true) {}
    
    SearchSpaceEdge(NodeID s, NodeID t, EdgeID e, EdgeWeight k, LevelID lev = 0, bool c = true)
    : _source(s), _target(t), _edgeID(e), _keyOfTarget(k), _searchLevel(lev), _core(c) {}

    void overwriteSearchLevel(LevelID lev) {_searchLevel = lev;}
    
    NodeID source() const {return _source;}
    NodeID target() const {return _target;}
    EdgeID edgeID() const {return _edgeID;}
    EdgeWeight keyOfTarget() const {return _keyOfTarget;}
    LevelID searchLevel() const {return _searchLevel;}
    bool core() const {return _core;}
    
private:
    NodeID _source;
    NodeID _target;
    EdgeID _edgeID;
    EdgeWeight _keyOfTarget;
    LevelID _searchLevel;
    bool _core;
};



///////////////////////////////////////////////////////////////////////////////////////////////
///		#include "path.h"



/**
 * Represents a path in a graph.
 * Two types of path objects can be dealt with.
 * Type A: Normally, a path is a list of x+1 nodes and x edge weights
 * and shortcut flags.
 * Type B: In addition to the lists of Type A, a list of x edge IDs is managed.
 * It is important to ensure that both types are not mixed up.
 */
class Path
{
    /** Outputs data about the path for debugging purposes. */ 
    friend ostream& operator<<( ostream& os, const Path& p ) {
        if (p._notConnected) {
            os << "NOT CONNECTED" << endl;
            return os;
        }
        if (p.empty()) {
            os << "EMPTY" << endl;
            return os;
        }
        assert( p.checkInvariant() );
        os << setprecision(20);
        os << p.length() << ": " << p.node(0);
        for (EdgeID e = 0; e < p.noOfEdges(); e++)
            os << " --" << p._weights[e] << "-> " << p.node(e+1); 
        os << endl;
        return os;
    }
    
public:
    /** Returns true iff the given path is identical to this path. */
    bool operator== (const Path& p) const {
        if (_notConnected != p._notConnected) return false;
        if (_notConnected) return true;
        return ((_nodes == p._nodes) && (_weights == p._weights));
    }

    /** Compares two paths by length (=sum of weights). */
    bool operator< (const Path& p) const {
        return (length() < p.length());
    }

    /** 
     * Constructor.
     * Creates an empty path.
     */
    Path() {clear();}
    
    /**
     * Constructor.
     * Creates a path with a given start node.
     */
    Path(NodeID id) {
        clear();
        addFirstNode(id);
    }

    /** Clears this path (so that it becomes an empty path). */
    void clear() {
        _nodes.clear();
        _weights.clear();
        _edges.clear();
        _shortcuts.clear();
        _length = 0;
        _notConnected = false;
    }

    /** Adds a start node to this (empty) path. */
    void addFirstNode(NodeID id) {
        _nodes.push_back(id);
        assert( checkInvariant() );
    }

    /** 
     * Adds an edge to this path. 
     * Note: the source of the edge is the formerly last node in the path.
     * @param id the target of the edge
     * @param w the weight of the edge
     * @param shc the shortcut flag of the edge
     */
    void add(NodeID id, EdgeWeight w, bool shc) {
        _nodes.push_back(id);
        _weights.push_back(w);
        _shortcuts.push_back(shc);
        _length += w;
        assert( checkInvariant() );
    }

    /** 
     * Adds an edge to this path.
     * Note: the source of the edge is the formerly last node in the path.
     * @param nodeID the target of the edge
     * @param w the weight of the edge
     * @param edgeID the ID of the edge
     * @param shc the shortcut flag of the edge
     */
    void add(NodeID nodeID, EdgeWeight w, EdgeID edgeID, bool shc) {
        add(nodeID, w, shc);
        _edges.push_back(edgeID);
    }

    /**
     * Adds a path to this path.
     * Note: the first node is NOT added; we assume that it is
     * identical to the formerly last node of this path.
     */
    void add(const Path& path) {
        _nodes.insert( _nodes.end(), path._nodes.begin() + 1, path._nodes.end() );
        _weights.insert( _weights.end(), path._weights.begin(), path._weights.end() );
        _edges.insert( _edges.end(), path._edges.begin(), path._edges.end() );
        _shortcuts.insert( _shortcuts.end(), path._shortcuts.begin(), path._shortcuts.end() );
        _length += path.length();
        assert( checkInvariant() );
    }

    /**
     * Adds sequences of node ids and edge weights to this path.
     * Note: The shortcut flags are filled with dummy entries.
     * @param beginNodes begin of the node sequence
     * @param endNodes end of the node sequence
     * @param beginWeights begin of the weights sequence
     * @param endWeights end of the weights sequence
     * @param w sum of all weights in the weights sequence
     */
    void add(vector<NodeID>::const_iterator beginNodes, vector<NodeID>::const_iterator endNodes,
             vector<EdgeWeight>::const_iterator beginWeights, vector<EdgeWeight>::const_iterator endWeights,
             EdgeWeight w) {
        _nodes.insert( _nodes.end(), beginNodes, endNodes );
        _weights.insert( _weights.end(), beginWeights, endWeights );
        _shortcuts.resize( _weights.size() );
        _length += w;
        assert( checkInvariant() );
    }

    /**
     * Reverses this path. (The first node becomes the last one and vice versa.)
     * Note that this might lead to unwanted results if edge IDs are given since
     * only the order of the edge IDs is reversed; the IDs of the reverse edges
     * are not determined.
     */
    void reverse() {
        std::reverse( _nodes.begin(), _nodes.end() );
        std::reverse( _weights.begin(), _weights.end() );
        std::reverse( _edges.begin(), _edges.end() );
        std::reverse( _shortcuts.begin(), _shortcuts.end() );
    }

    /** 
     * When this path is used to store a path between two nodes s and t,
     * this flag can be set to indicate that s and t aren't connected.
     */
    void setNotConnected() {_notConnected = true;}

    /** Returns the number of nodes that belong to this path. */
    NodeID noOfNodes() const {return _nodes.size();}
    
    /** Returns the number of edges that belong to this path. */
    EdgeID noOfEdges() const {
        assert( _weights.size() == _shortcuts.size() );
        return _weights.size();
    }

    /** Returns true iff this path is empty. */
    bool empty() const {return noOfNodes() == 0;}
    
    /** Returns the length (sum of weights) of this path. */
    EdgeWeight length() const {
        // special case: length "infinity" means "not connected"
        if (_notConnected) return Weight::MAX_VALUE;
 
        return _length;
    }

    /** Returns the node with the given index within this path. */
    NodeID node(NodeID index) const {
        assert( index < _nodes.size() );
        return _nodes[index];
    }

    NodeID firstNode() const {
        assert( ! empty() );
        return node(0);
    }

    NodeID lastNode() const {
        assert( ! empty() );
        return node(noOfNodes() - 1);
    }

    EdgeWeight weight(EdgeID index) const {
        assert( index < _weights.size() );
        return _weights[index];
    }

    EdgeID edge(EdgeID index) const {
        assert( _edges.size() == _weights.size() );
        assert( index < _edges.size() );
        return _edges[index];
    }

    void setEdge(EdgeID index, EdgeID newID) {
	assert( index < _edges.size() );
	_edges[index] = newID;
    }
    
    bool isShortcut(EdgeID index) const {
        assert( index < _shortcuts.size() );
        return _shortcuts[index];
    }

    bool isNotConnected() const {return _notConnected;}
    

    /**
     * Computes the differences between this path and a given path.
     * @param p the path that this path is compared to
     * @param diff1 a subpath of this path that begins just before the first 
     *              occurence of a difference and ends just after the last
     * @param diff2 a subpath of p that begins just before the first 
     *              occurence of a difference and ends just after the last
     */
    void diff(const Path& p, Path& diff1, Path& diff2) const {
        diff1.clear();
        diff2.clear();

        if (_notConnected != p._notConnected) {
            // completely different
            diff1 = (*this);
            diff2 = p;
            return;
        }

        if (*this == p) return; // completely equal
        
        EdgeID i; // moves in both paths forwards
        EdgeID j; // moves in this path backwards
        EdgeID k; // moves in the path p backwards
        // look for the first difference:
        // move forwards until a difference occurs
        for (i=0; 
               (i<noOfEdges()) && (i<p.noOfEdges()) && 
               (node(i)==p.node(i)) && (_weights[i]==p._weights[i]);
             i++) ;
        // look for the last difference:
        // move backwards until a difference occurs
        for (j = noOfEdges(), k = p.noOfEdges();
             (j > 0) && (k > 0) && (node(j) == p.node(k)) && (_weights[j-1] == p._weights[k-1]);
             j--, k--) ;

        // if applicable, take one step back
        // because the output should begin just BEFORE the first difference
        // and end just AFTER the last difference
        if (i > 0) i--;
        if (j < noOfEdges()) j++;
        if (k < p.noOfEdges()) k++;
        
        subpath( diff1, i, j);   // copy the appropriate subpath of this path to diff1
        p.subpath( diff2, i, k); // copy the appropriate subpath of p to diff2
    }

    /**
     * Exports all edges of this path to the given stream using the given color
     * (in order to be able to draw the path).
     */
    void exportEdges(ostream& out, int color) {
        for (EdgeID e = 0; e < noOfEdges(); e++)
            out << node(e) << " " << node(e+1) << " " << _weights[e] << " " << color << endl;
    }
    
    /*
     * Returns a vector of all nodes in the path
     */
    vector<NodeID> getNodeVector(){
      return _nodes;
    }
    
private:
    /** The nodes of this path. */
    vector<NodeID> _nodes;
    
    /** The weights of the edges of this path. */
    vector<EdgeWeight> _weights;

    /** The IDs of the edges. Optional, i.e., may be left empty. */
    vector<EdgeID> _edges;

    /** For each edge in this path, a shortcut flag. */
    vector<bool> _shortcuts;
    
    /** The length (=sum of edge weights) of this path. */
    EdgeWeight _length;
    
    /** 
     * A flag that indicates that two nodes are not connected
     * in case that this path should represent a connection
     * between those two nodes.
     */
    bool _notConnected;

    
    /**
     * Checks the invariant that the number of nodes of a non-empty
     * path is always equal to the number of edges plus 1.
     */
    bool checkInvariant() const {
        return (noOfNodes() == noOfEdges() + 1);
    }

    /** Adds the specified subpath of this path to the given path p. */
    void subpath(Path& p, NodeID begin, NodeID end) const {
        p.addFirstNode(node(begin));
        for (NodeID u = begin+1; u <= end; u++) p.add( node(u), _weights[u-1], _shortcuts[u-1] );
    }
    
};

/** Represents a collection of paths. */
class Paths : public vector<Path>
{
public:
    /**
     * Compares two collections of paths.
     * @param os the stream that the result of the comparison/the differences are written to
     * @param p the path collection that this collection is compared to
     * @param verbose write details about the differences (true) / only a summary (false)
     */
    void diff(ostream& os, const Paths& p, bool verbose = false) const {
        if (size() != p.size()) {
            os << "Not the same number of paths." << endl;
            return;
        }

        NodeID countDifferentPaths = 0;
        NodeID countDifferentLengths = 0;
        Path diff1, diff2; // store the differences of one comparison of two paths
        vector< pair<Path, Path> > diffs; // stores the differences of all comparisons,
                                          // all entries are unique (duplicates aren't stored)
        
        // compare path i in this collection with path i in the collection p
        for (NodeID i = 0; i < size(); i++) {
            (*this)[i].diff( p[i], diff1, diff2 ); // perform comparison
            if ((! diff1.empty()) || (! diff2.empty())) { // if both paths aren't identical
                countDifferentPaths++;
                if (diff1.length() != diff2.length()) { // if the lengths of both paths differ
                    countDifferentLengths++;
                    if (verbose && (! p[i].empty())) {
                        os << "Different path lengths: " << p[i].node(0) << " -> "
                           << p[i].node(p[i].noOfNodes()-1)
                           << ": " << (*this)[i].length() << " != " << p[i].length() << endl;
                    }
                }
                pair<Path, Path> newDiff( diff1, diff2 );
                bool unique = true;
                // check whether the current differences pair is unique (or a duplicate)
                for (NodeID j = 0; j < diffs.size(); j++)
                    if (diffs[j] == newDiff) {
                        unique = false;
                        break;
                    }
                if (unique) diffs.push_back(newDiff);
            }
        }
        // write summary
        os << countDifferentPaths << " out of " << size() << " pairs of paths differ. ("
           << diffs.size() << " distinct differences)" << endl;

        if (countDifferentLengths == 0) {            
            os << "For each pair of paths, the lengths are equal." << endl << endl;
        }
        else {
            os << endl << "!!! WARNING !!! The path lengths differ in "
               << countDifferentLengths << " cases. !!! WARNING !!!" << endl << endl;
        }
        
        if (verbose) {
            // write details
            for (NodeID i = 0; i < diffs.size(); i++) {
                os << diffs[i].first << diffs[i].second << endl;
            }
        }
    }

    /** 
     * Exports all edges of all paths of this collection to the given stream 
     * using the given color (in order to be able to draw the paths).
     */
    void exportEdges(ostream& out, int color) {
        for (NodeID i = 0; i < size(); i++) (*this)[i].exportEdges(out, color);
    }
};


///////////////////////////////////////////////////////////////////////////////////////////////

#endif // GRAPH_H
