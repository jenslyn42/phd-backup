#ifndef COMPRESSEDPIDTOKENS_H
#define COMPRESSEDPIDTOKENS_H

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <list>
//#include <ctime>
#include <vector>
#include <utility>
#include <map>
#include <assert.h>

#include <boost/foreach.hpp>
#include "boost/unordered_map.hpp"

#include "utility.h"

class compressedPidTokens{
public:
    compressedPidTokens();
    ~compressedPidTokens();

    void insertPath(vector<int>& vecpath);
    int GetTotalNumItems() {return _total_pid_items;}
    void printNodesTokensPaths();
    int getNumNodes() {return NodeTokens.size();}
    int GetNumTokens() { return _num_token;}

private:
    struct CompressPidType {
        int token;
        int prev_token,prev_size;
        intPairVector vecpair;	// CONSIDERED in the cache size
        vector<int> vecnodes;	// NOT CONSIDERED in the cache size (just for temporary use)
    };
    boost::unordered_map<int,int> NodeTokens;						// the token of each node
    boost::unordered_map<int,CompressPidType*> TokenCompressPids;	// the compressed path of each token

    int _num_token;
    int pathID;
    int _total_pid_items;

    int GenerateToken() {return (_num_token++);}
    void insertVecPair( intPairVector& vecpair, int path_id );
    void printVecPair( intPairVector& vecpair );
    void insertCompressedPid( int path_id, vector<int>& tmpModify, CompressPidType* prevcomppath );
};

#endif // COMPRESSEDPIDTOKENS_H
