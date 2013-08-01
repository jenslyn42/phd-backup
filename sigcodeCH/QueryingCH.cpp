/********************************************************************
* QuerySample.cpp
* Demo how to use CH for shortest path query
********************************************************************/

/* C Lib */
#include <stdlib.h>

/* C++ Lib */
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
using namespace std;

#include "packCH/config.h"
#include "packCH/utils.h"

/* Linux Lib */
#include <unistd.h>

/* Headers */ 
#include "packCH/Command.h"

/* 
	These two must be placed first as they include necessary typedef 
	statementm like NodeID, EdgeID etc.
*/
#include "packCH/graphIO.h"
#include "packCH/UpdateableGraph.h"
#include "packCH/SearchGraph.h"
#include "packCH/DijkstraCH.h"
#include "packCH/ConstructCH.h"

typedef datastr::graph::UpdateableGraph UpdateableGraph;
typedef processing::ConstructCH<UpdateableGraph> ProcessingConstruct;
typedef processing::DijkstraCH<UpdateableGraph, NormalPQueue, 2, false> Dijkstra;


int main(int argc, char *argv[]) {	


	// if you want to know path from 234 to 100, you may use "sp.pathTo(path, 100, -1, true, true);"


	string ddsgFile("");	// input graph file
	string hcnFile("");		// input level file
	string workloadFile("");	// input workload file
	string outputFile("");	// output stat file

	ProcessingConstruct::ContractParameters contractParams;

	ProcessingConstruct* construct = NULL;
	UpdateableGraph* updGraph = NULL;

	ifstream in; // input file object
	ofstream out; // output file object
	
	int opt = -1;
	while ((opt = getopt(argc, argv, "f:h:w:o:m:k:c:L")) != -1 ) {
		switch (opt) {
			// input graph file
			case 'f':
				ddsgFile = string(optarg);
				break;
				
			// input level file
			case 'h':
				hcnFile = string(optarg);
				break;
			
			// limit of settled nodes in local searches during contraction
			case 'm':
				contractParams.maxSettledElim = atoi(optarg);
				break;
				
			
			// hop-/degree-limits specified as comma-separated list,
			// e.g. "1,3.3,2,10,3,10,5" means hop-limit 1 until degree 3.3,
			//                          then hop-limit 2 until degree 10,
			//                          then hop-limit 3 until degree 10,
			//                          then hop-limit 5
			case 'k':
				contractParams.maxHops.clear();
				Command::createVector(string(optarg),contractParams.maxHops,(double)0);
				break;
			
			// Core size, the contraction aborts if the remaining graph
			// only contains this number of nodes. Used for assymetric 
			// many-to-many calculations.
			case 'c':
				contractParams.coreSize = atoi(optarg);
				break;
			
			// local edge reduction
			case 'L':
				contractParams.localReduceEdges = !contractParams.localReduceEdges;
				break;
			
			/*
			// Export contraction hierarchy including node order and shortcuts
			// to file
			case 'C':
				chFile = string(optarg);
				break;
			*/
		}
	}
	
	/* Read graph */
	in.open(ddsgFile.c_str());
	if (!in.is_open()) {
		cerr << "ddsgFile - Cannot open " << ddsgFile << endl; exit(1);
	}
	VERBOSE( cout << "Reading graph ..." << endl;)
	updGraph = importGraphListOfEdgesUpdateable(in, false, false, "");
	in.close(); in.clear();

	/* Read node levels */
	in.open(hcnFile.c_str());
	if (!in.is_open()) {
		cerr << "hcnFile - Cannot open " << hcnFile << endl; exit(1);
	}
	construct = new ProcessingConstruct(updGraph);
	VERBOSE( cout << "Reading node levels ..." << endl;)
	construct->readLevels(in);
	in.close(); in.clear();


	/* Build Contraction Hierarchy  */
	VERBOSE( cout << "Contracting graph ..." << endl;)
	construct->constructHierarchy(contractParams);
	
	/* 
		**** For Ken ****
		Run Some Sample SP Query
		You may change the source and target ID 
	*/
	Dijkstra sp(updGraph);
	Path p;
	EdgeWeight spDist;
	
	/*EdgeWeight spDist = sp.bidirSearch(0, 3);
	VERBOSE( cout << "SP Dist from " << 0 << " to " << 3 << " is " << spDist << endl; )
	sp.clear();*/
	
	
	srand(0);
	
	for (int i=0;i<1000;i++) {
		int s=rand()%230000;
		int t=rand()%230000;
		spDist = sp.bidirSearch(s, t);
		sp.pathTo(p, t, -1, true, true);
		//VERBOSE( cout << "SP Dist from " << s << " to " << t << " is " << spDist << endl; )
		VERBOSE( cout << i << "@" << s << "," << t << " :" << spDist << endl; );
		
		//p
		
		sp.clear();
	}
	
	/*spDist = sp.bidirSearch(1, 1009);
	VERBOSE( cout << "SP Dist from " << 1 << " to " << 1009 << " is " << spDist << endl; )
	sp.clear();*/
	
	return 0;
}

// doesn't look nice, but required by the compiler (gcc 4)
const EdgeWeight Weight::MAX_VALUE;
const EliminationWeight::Type EliminationWeight::MAX_VALUE = __DBL_MAX__;
const EliminationWeight::Type EliminationWeight::MIN_VALUE = -__DBL_MAX__;
const EdgeWeight Weight::MAX_INTEGER;
const int datastr::graph::UpdateableGraph::LOOK_FOR_SECOND_EDGE_BACKWARD;
const int datastr::graph::UpdateableGraph::LOOK_FOR_SECOND_EDGE_FORWARD;


