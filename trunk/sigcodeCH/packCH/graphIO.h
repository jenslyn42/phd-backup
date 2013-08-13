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

#ifndef CREATEGRAPH_H
#define CREATEGRAPH_H


#include <cmath>

#include "graph.h"
#include "UpdateableGraph.h"


/** The factor that the weights from the input are multiplied with. */
double weightFactor = 1;

/** Returns a random node identifier between 0 and n-1. */
inline NodeID randomNodeID(NodeID n) {
    return (NodeID)(rand() / (double)(RAND_MAX+1.0) * n);
}

/** Returns a random edge weight. */
inline EdgeWeight randomEdgeWeight() {
    const EdgeWeight maxEdgeWeight = (EdgeWeight)1e2;
    return (EdgeWeight)(rand() / (double)(RAND_MAX+1.0) * maxEdgeWeight);
}

/**
 * Removes parallel edges and self loops from the input and writes
 * the unique edges to the output.
 */
void removeDuplicates(vector<CompleteEdge>& input, vector<CompleteEdge>& output) {
    // sort the edge list first by source, then by target and weight
    sort(input.begin(), input.end());
    
    output.reserve( input.size() );

    VERBOSE( EdgeID selfLoops = 0 );
    VERBOSE( EdgeID parallelEdges = 0 );
    VERBOSE( EdgeID weightDependsOnDir = 0 );
    VERBOSE( EdgeID mergeTwoOneWays = 0 );
    VERBOSE( EdgeID downgradeToOneWay = 0 );

    CompleteEdge lastEdge(0,0,0,false,true,true);
    for (EdgeID i=0; i<input.size(); i++) { // scan the original edge list
        CompleteEdge currentEdge = input[i];
        if (currentEdge.source() == currentEdge.target()) {
            // skip self loop
            VERBOSE( selfLoops++ );
            VERBOSE2(cout << "self-loop " << currentEdge << " skipped." << endl); 
        }
        else {
            if ((currentEdge.source() != lastEdge.source()) ||
                (currentEdge.target() != lastEdge.target())) {
                
                lastEdge = currentEdge;
                // add unique edge to the new edge list (that doesn't contain duplicates)
                output.push_back( currentEdge );
            }
            else {
                assert( i > 0 );
                // Does the current edge point in another direction
                // than the previous edge ?
                if (((currentEdge.isDirected(0)) && (! lastEdge.isDirected(0))) ||
                    ((currentEdge.isDirected(1)) && (! lastEdge.isDirected(1)))) {
                    
                    if (currentEdge.weight() == lastEdge.weight()) {
                        // same weight
                        // -> sorting order implies that both edges are one-way and
                        //    complementary
                        // -> don't add the current edge, but make the last edge two-way
                        assert( (currentEdge.isDirected(0) != lastEdge.isDirected(0)) &&
                                (currentEdge.isDirected(1) != lastEdge.isDirected(1)) &&
                                (currentEdge.isDirected(0) != currentEdge.isDirected(1)) );
                        output.back().makeTwoWay();
                        lastEdge.makeTwoWay();
                        VERBOSE( mergeTwoOneWays++ );
                        continue;
                    }
                    
                    if (currentEdge.isBidirected()) {
                        // Since the last (one-way) edge is shorter, 
                        // the current (two-way) edge is only useful 
                        // for the opposite direction
                        assert( currentEdge.weight() > lastEdge.weight() );                        
                        if (lastEdge.isDirected(0)) currentEdge.makeOneWay(1);
                        if (lastEdge.isDirected(1)) currentEdge.makeOneWay(0);
                        VERBOSE( downgradeToOneWay++ );
                    }
                    
                    // set both direction flags to indicate that no further
                    // edge is needed for the current s-t-pair
                    lastEdge.makeTwoWay();
                    
                    // add unique edge to the new edge list
                    output.push_back( currentEdge );
                    VERBOSE( weightDependsOnDir++ );
                    VERBOSE2(cout << "parallel edge " << currentEdge
                             << " kept; predecessor: " << input[i-1] << endl);
                }
                else {
                    // skip parallel edge
                    // note: since parallel edges are sorted by weight,
                    //       it is ensured that the shorter one "survives"
                    VERBOSE( parallelEdges++ );
                    VERBOSE2(cout << "parallel edge " << currentEdge
                             << " skipped; predecessor: " << input[i-1] << endl);
                }
            }           
        }
    }

    // print statistics
    VERBOSE( if (selfLoops > 0) cout << selfLoops << " self-loop(s) skipped." << endl );
    VERBOSE( if (parallelEdges > 0) cout << parallelEdges << " parallel edge(s) skipped." << endl );
    VERBOSE( if (weightDependsOnDir > 0) cout << weightDependsOnDir 
             << " parallel edge(s) kept (different directions -> different weights)." << endl );
    VERBOSE( if (downgradeToOneWay > 0) cout << "  " << downgradeToOneWay 
             << " two-way edge(s) downgraded to one-way "
             << "(since the other direction is covered by a shorter edge)." << endl );
    VERBOSE( if (mergeTwoOneWays > 0) cout << mergeTwoOneWays 
             << " one-way edge(s) merged with an opposite edge of the same length." << endl );
}





/**
 * Reads a list of edges from the given stream and adds them to the given vector.
 * @return the number of nodes
 */
NodeID readGraphFromStream(istream &in, const bool adaptWeight, const bool addIsolatedNode,
                           vector<CompleteEdge>& edgeList) {
    NodeID n; // number of nodes
    EdgeID m; // number of edges
    NodeID source, target;
    int dir; // direction (0 = open, 1 = forward, 2 = backward, 3 = closed) [adopted from PTV]
    char dummy;
    
    // directed ?
    bool directed = false;
    if (in.peek() == 'd') {
        directed = true;
        in >> dummy;
    }

    in >> n >> m;

    VERBOSE( cout << "Import Graph: n = " << n << ", m = " << m << endl );
    
    if (adaptWeight) {
        // read the edge list a first time 
        // in order to determine the maximum weight in the input
        
        double weight; // the current weight read from the input
        double maxWeight = 0; // the maximum weight
        double weightSum = 0; // the sum of all weights
        
        // if false, no overflow (w.r.t. the maximum integer) occured 
        //           when computing the sum of all weights
        // if true, there might be an overflow
        bool overflow = false;
                               

        for (EdgeID i=0; i<m; i++) { // scan the edge list
            in >> source >> target >> weight;
            if (directed) in >> dir;
            if (weight > maxWeight) maxWeight = weight; // update the maximum weight
            
            if ((weightSum > Weight::MAX_INTEGER / 2) || 
                 (weight > Weight::MAX_INTEGER / 2)) overflow = true; 
            weightSum += weight; // update the sum of weights
        }
        
        in.seekg(0); // re-read the input
        if (directed) in >> dummy;
        in >> n >> m;
        
        // the allowed weight per edge;
        // this upper bound is based on the fact that a shortest path always consists
        // of less than n edges
        double allowedWeight = trunc(Weight::MAX_INTEGER / n);
        
        // if multiplied by this factor, all edge weights (incl. the maximum one)
        // are less than or equal to the allowed weight
        weightFactor = allowedWeight / maxWeight;

        VERBOSE( cout << setprecision(20) << "  adapt weight: allowed weight = " << allowedWeight
                 << ", max weight in input = " << maxWeight << ", factor = " << weightFactor <<  endl );

        if (! overflow) {
            // if there was no overflow, we can try to use the sum of weights
            // in order to find a less restrict bound for the edge weights
            
            // if the edge weights are multiplied by this factor,
            // the sum of all edge weights does not exceed the maximum integer;
            // note that a shortest path does not contain any edge twice
            double weightFactor2 = Weight::MAX_INTEGER / weightSum;
            
            // if the second factor is better than the first one, use the second one;
            // "better" means that we exhaust the given range more carefully so that
            // the loss of precision by this initial rounding is less serious
            if (weightFactor2 > weightFactor) {
                weightFactor = weightFactor2;

                VERBOSE( cout << "                sum of weights = " << weightSum 
                              << ", factor = " << weightFactor2 << endl );
            }
        }       
    }
    
    VERBOSE( EdgeID onewayStreets = 0 );
    edgeList.reserve(2*m); // each edge will be added twice (once for each direction)
        
    for (EdgeID i=0; i<m; i++) {
        EdgeWeight weight;
        
        in >> source >> target;

        if (adaptWeight) {
            double tmp;
            in >> tmp;
            weight = static_cast<EdgeWeight>( round(tmp * weightFactor) );
        }
        else {
            in >> weight;
        }
        
        dir = 0;
        if (directed) in >> dir;
        bool forward = true;
        bool backward = true;
        if (dir == 1) backward = false;
        if (dir == 2) forward = false;
        VERBOSE( if ((dir == 1) || (dir == 2)) onewayStreets++ );
        // note: all CLOSED roads (dir = 3) are considered as OPEN !

        CompleteEdge edge1(source, target, weight, false, forward, backward);
        CompleteEdge edge2(target, source, weight, false, backward, forward);
        edgeList.push_back(edge1);
        edgeList.push_back(edge2);
        
        // Check wheter end of line is reached
        if ( in.peek() !=  '\n' && in.peek() != '\r' )
        {
          cerr << "After edge number " << i << " is no newline." << endl;
          cerr << "Input format error, aborting." << endl;
          exit(1);
        }

    }
    
    VERBOSE( cout << onewayStreets << " oneway street(s)." << endl );

    if (addIsolatedNode) n++;
    return n;
}
    
/**
 * Imports a list of edges from a given input stream and
 * builds an UpdateableGraph. The node levels are read from
 * the specified file 'filenameHwyNodes', otherwise the levels are set to n.
 * Note: To use the graph to create a contraction hierarchy (../processing/ConstructCH.h)
 *       initially all nodes must have level n in the graph. The node order is specified
 *       separately.
 */
datastr::graph::UpdateableGraph* importGraphListOfEdgesUpdateable(istream &inGraph, const bool adaptWeight,
                                        const bool addIsolatedNode, const string filenameHwyNodes) {
    vector<CompleteEdge> edgeList;
    const NodeID n = readGraphFromStream(inGraph, adaptWeight, addIsolatedNode, edgeList);
    vector<CompleteEdge> edgeListWithoutDuplicates;
    removeDuplicates(edgeList, edgeListWithoutDuplicates);
    
    // default level is for all n nodes is n
    vector<LevelID> nodeLevels(n, n);

    if (filenameHwyNodes != "")
    {
        VERBOSE( cout << "open file '" << filenameHwyNodes << "' to read hwy node levels" << endl );
        ifstream in(filenameHwyNodes.c_str());
        if (! in) {
            VERBOSE( cout << "failed" << endl );
            exit(-1);
        }
        NodeID n2;
        LevelID max = 0;
        readPrimitive(in, n2);
        assert( n == n2 );
        for (NodeID u = 0; u < n; u++) {
            LevelID k;
            readPrimitive(in, k);
            if (k > max) max = k;
            nodeLevels[u] = k;
        }
        VERBOSE( cout << "max hwy node level = " << max << endl );
    }

    return new datastr::graph::UpdateableGraph(edgeListWithoutDuplicates, nodeLevels);
}



/**
 * Export search graph to text file in ddsg-format.
 */
template< typename Graph >
void exportSearchGraph(ostream& out, const Graph* g)
{
    out << "d" << endl;
    EdgeID m = 0;
    vector<NodeID> mapIntToExtNodeID(g->noOfNodes());
    for ( NodeID u = 0; u < g->noOfNodes(); u++ )
    {
        m += g->lastEdge(g->mapExtToIntNodeID(u)) - g->firstLevelEdge(g->mapExtToIntNodeID(u));
        mapIntToExtNodeID[g->mapExtToIntNodeID(u)] = u;
    }
    
    out << g->noOfNodes() << " " << m << endl;
    for ( NodeID u = 0; u < g->noOfNodes(); u++ )
    {
        EdgeID lastEdge = g->lastEdge(g->mapExtToIntNodeID(u));
        for ( EdgeID e = g->firstLevelEdge(g->mapExtToIntNodeID(u)); e < lastEdge; e++ )
        {
            const Edge& edge = g->edge(e);
            int dir = 3;
            if (edge.isDirected(0)) dir -= 2;
            if (edge.isDirected(1)) dir -= 1;
            out << u << " " << mapIntToExtNodeID[edge.target()] << " " << edge.weight() << " " << dir << endl;
        }
    }
}

/**
 * Export a contraction hierarchy in a binary format, see ../docu/chDocu.html.
 * supported graph classes: datastr::graph::UpdateableGraph
 */
template< typename Graph>
void exportContractionHierarchy(ostream& out, const Graph* g)
{

    bool expand = false;
    CH_EXPAND( expand = true );
    if (!expand)
    {
        cerr << "No shortcut information stored into edges, cannot proceed with export!" << endl;
        cerr << "Please #define USE_CH_EXPAND in config.h" << endl;
        exit(1);
    }
    
    double time = timestamp();

    unsigned int version = 1;
    unsigned int n = g->noOfNodes();
    unsigned int m1 = 0;
    unsigned int m2 = 0;

    // edge flags    
    const unsigned int CH_EDGE_FLAG_FORWARD  = 1 << 0;
    const unsigned int CH_EDGE_FLAG_BACKWARD = 1 << 1;
    const unsigned int CH_EDGE_FLAG_SHORTCUT = 1 << 2;
    
    for ( NodeID u = 0; u < g->noOfNodes(); u++ )
    {
        EdgeID lastEdge = g->lastEdge(u);
        for ( EdgeID e = g->firstLevelEdge(u); e < lastEdge; e++ )
        {
            const Edge& edge = g->edge(e);
            if ( edge.isShortcut() )
            {
                m2++;
            }
            else
            {
                m1++;
            }
        }
    }
    
    VERBOSE( cout << "Contraction Hierarchy file format version " << version << endl; )
    VERBOSE( cout << n << " nodes" << endl; )
    VERBOSE( cout << m1 << " original edges" << endl; )
    VERBOSE( cout << m2 << " shortcut edges" << endl; )
    
    // === BEGIN OF EXPORT ===
    
    // header
    // ------
    // id
    out << "CH\r\n";
    // version
    writePrimitive( out, version );
    // n (number of nodes)
    writePrimitive( out, n );
    // m1 ( number of original (=non shortcut) edges )
    writePrimitive( out, m1 );
    // m2 ( number of shortcut edges )
    writePrimitive( out, m2 );
    
    // levels for nodes 0..(n-1)
    // -------------------------

    VERBOSE( cout << "Export levels..." << endl; );
    VERBOSE( Percent percent1( n ); )
    VERBOSE( unsigned int i1 = 0; )
    unsigned int level;
    for ( NodeID u = 0; u < n; u++ )
    {
        level = g->node(u).level();
        writePrimitive( out, level );
        VERBOSE( percent1.printStatus( i1++ ); )
    }
    
    // orignal (=non shortcut) edges
    // -----------------------------
    VERBOSE( cout << "Export original edges..." << endl; );
    VERBOSE( Percent percent2( m1 ); )
    VERBOSE( unsigned int i2 = 0; )
    for ( NodeID u = 0; u < g->noOfNodes(); u++ )
    {
        EdgeID lastEdge = g->lastEdge(u);
        for ( EdgeID e = g->firstLevelEdge(u); e < lastEdge; e++ )
        {
            const Edge& edge = g->edge(e);
            if ( !edge.isShortcut() )
            {
                unsigned int source = u;
                unsigned int target = edge.target();
                unsigned int weight = edge.weight();
                
                // edge flags
                unsigned int flags = 0;
                if ( edge.isDirected(0) ) flags |= CH_EDGE_FLAG_FORWARD;
                if ( edge.isDirected(1) ) flags |= CH_EDGE_FLAG_BACKWARD;
                
                // source node
                writePrimitive( out, source );
                // target node
                writePrimitive( out, target );
                // weight
                writePrimitive( out, weight );
                // flags
                writePrimitive( out, flags );
                
                VERBOSE( percent2.printStatus( i2++ ); )       
            }
        }
    }
    
    // shortcut edges
    // --------------
    VERBOSE( cout << "Export shortcut edges..." << endl; );
    VERBOSE( Percent percent3( m2 ); )
    VERBOSE( unsigned int i3 = 0; )
    for ( NodeID u = 0; u < g->noOfNodes(); u++ )
    {
        EdgeID lastEdge = g->lastEdge(u);
        for ( EdgeID e = g->firstLevelEdge(u); e < lastEdge; e++ )
        {
            const Edge& edge = g->edge(e);
            if ( edge.isShortcut() )
            {
                unsigned int source = u;
                unsigned int target = edge.target();
                unsigned int weight = edge.weight();
                unsigned int middle = edge.shortcutMiddle();
                
                // edge flags
                unsigned int flags = CH_EDGE_FLAG_SHORTCUT;
                if ( edge.isDirected(0) ) flags |= CH_EDGE_FLAG_FORWARD;
                if ( edge.isDirected(1) ) flags |= CH_EDGE_FLAG_BACKWARD;
                
                // source node
                writePrimitive( out, source );
                // target node
                writePrimitive( out, target );
                // weight
                writePrimitive( out, weight );
                // flags
                writePrimitive( out, flags );
                // middle node of shortcut
                writePrimitive( out, middle );
                
                VERBOSE( percent3.printStatus( i3++ ); )       
            }
        }
    }
    unsigned int end = 0x12345678;
    writePrimitive( out, end );    
    // === END OF EXPORT
    
    time = timestamp() - time;
    VERBOSE( cout << "Export done, took " << time << " seconds." << endl; )
    
}

#endif // CREATEGRAPH_H
