#include "compressedpidtokens.h"

#define debug false

compressedPidTokens::compressedPidTokens()
{
       _num_token = 0;
       pathID =0;
       _total_pid_items=0;
}

compressedPidTokens::~compressedPidTokens()
{
    //dtor
}

// pre-condition: assume that "path_id" must be larger than any existing id in "vecpair"
void compressedPidTokens::insertVecPair( intPairVector& vecpair, int path_id )
{
    if(debug) cout << "compressedPidTokens::insertVecPair! Start";
	if (vecpair.size()==0) {
		vecpair.push_back(std::make_pair(path_id,path_id));
		_total_pid_items++;	// single item only
	} else {
		intPair tmppair=vecpair.back();
		if (tmppair.second + 1 == path_id) {
			if (tmppair.first == tmppair.second)	// a item to a pair
				_total_pid_items++;
			vecpair.pop_back();		// remove the last pair
			tmppair.second=path_id;	// update that pair
		} else {
			tmppair=std::make_pair(path_id,path_id);	// make a new pair
			_total_pid_items++;	// single item only
		}
		vecpair.push_back(tmppair);
	}
	if(debug) cout << " ... Done" << endl;
}

void compressedPidTokens::compressedPidTokens::printVecPair( intPairVector& vecpair )
{
    if(debug) cout << "compressedPidTokens::printVecPair! Start";
	BOOST_FOREACH(intPair curpair, vecpair)
	{
		cout << curpair.first << "-" << curpair.second << " ";
	}
	cout << endl;
    if(debug) cout << "compressedPidTokens::printVecPair! Done" << endl;

}

void compressedPidTokens::printNodesTokensPaths()
{
    if(debug) cout << "compressedPidTokens::printNodesTokensPaths! Start";

	cout << "*** " << NodeTokens.size() << " nodes ***" << endl;

	BOOST_FOREACH(intMap::value_type nt, NodeTokens)
	{
	    cout << nt.first << ": " << nt.second << "$ " << endl;
	}

	cout << "*** " << TokenCompressPids.size() << " tokens ***" << endl;

	boost::unordered_map<int,CompressPidType*>::iterator itB;

	for (itB=TokenCompressPids.begin();itB!=TokenCompressPids.end();itB++)
	{
		cout << itB->first << "$: ";

		CompressPidType* comppids=itB->second;
		if (comppids->prev_token<0)
			cout << "[root] ";
		else
			cout << "[" << comppids->prev_token  << "$^" << comppids->prev_size << "] ";
		printVecPair( comppids->vecpair );
	}

	cout << "*** " << GetTotalNumItems() << " total items ***" << endl;

    if(debug) cout << "compressedPidTokens::printNodesTokensPaths! Done" << endl;
}

void compressedPidTokens::insertCompressedPid( int path_id, vector<int>& tmpModify, CompressPidType* prevcomppath )
{
    if(debug) cout << "compressedPidTokens::insertCompressedPid! Begin ";
	int next_token=GenerateToken();

	// create a compressed path
	CompressPidType* splitcomppath=new CompressPidType;
	splitcomppath->token=next_token;

	if (prevcomppath!=NULL) {
		splitcomppath->prev_token=prevcomppath->token;
		splitcomppath->prev_size=prevcomppath->vecpair.size();
		//splitcomppath->vecpair.assign(prevcomppath->vecpair.begin(),prevcomppath->vecpair.end());
	} else {
		splitcomppath->prev_token=-1;
		splitcomppath->prev_size=0;
	}

	insertVecPair(splitcomppath->vecpair,path_id);
	splitcomppath->vecnodes.assign(tmpModify.begin(),tmpModify.end());

	// update the structures
	TokenCompressPids[next_token]=splitcomppath;
	BOOST_FOREACH(int nodeid, tmpModify)
		NodeTokens[nodeid]=next_token;

    if(debug) cout << "compressedPidTokens::insertCompressedPid! Done " << endl;
}

void compressedPidTokens::insertPath(vector<int>& vecpath)
{
    if(debug) cout << "compressedPidTokens::insertPath! Begin ";
	vector<int> newnodes,oldnodes;	// temporary vectors
	vector<int> tmpRemain,tmpModify;
	int path_id = pathID++;

	BOOST_FOREACH(int nodeid, vecpath)
	{
		if (NodeTokens.find(nodeid)==NodeTokens.end())
			newnodes.push_back(nodeid);
		else
			oldnodes.push_back(nodeid);
	}

	// for new node
	if (newnodes.size()>0) {
		insertCompressedPid( path_id, newnodes, NULL );
	}

	while (oldnodes.size()>0) {
		tmpRemain.clear();	// init temp vectors to be used in this loop
		tmpModify.clear();

		assert(NodeTokens.find(oldnodes[0])!=NodeTokens.end());	// must be true
		int prevtoken=NodeTokens[oldnodes[0]];


		CompressPidType* prevcomppath=TokenCompressPids[prevtoken];

		// only consider the nodes with "prevtoken"
		BOOST_FOREACH(int nodeid, prevcomppath->vecnodes)
		{
			vector<int>::iterator it=find(oldnodes.begin(),oldnodes.end(), nodeid);
			if (it != oldnodes.end()) {
				oldnodes.erase(it);
				tmpModify.push_back(nodeid);
			} else {
				tmpRemain.push_back(nodeid);
			}
		}

		assert(tmpModify.size()>0);

		if (tmpRemain.size()==0) {
			// just change "prevcomppath"
			insertVecPair(prevcomppath->vecpair,path_id);
		} else {
			prevcomppath->vecnodes.clear();
			prevcomppath->vecnodes.assign(tmpRemain.begin(),tmpRemain.end());

			insertCompressedPid( path_id, tmpModify, prevcomppath );
		}
	}


	// clear temporary vectors
	newnodes.clear();
	oldnodes.clear();
	tmpRemain.clear();
	tmpModify.clear();

    if(debug) cout << "compressedPidTokens::insertPath! Done " << endl;
}


