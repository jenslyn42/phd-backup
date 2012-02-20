/****************************************************************************************
 *   Copyright (C) 2011 by Jeppe Rishede 						*
 *   jenslyn42@gmail.com								*
 *											*
 *   All rights reserved.								*
 *											*
 *   Redistribution and use in source and binary forms, with or without 		*
 *   modification, are permitted provided that the following conditions are met:	*
 *   Redistributions of source code must retain the above copyright notice,		*
 *   this list of conditions and the following disclaimer. 				*
 *   Redistributions in binary form must reproduce the above copyright notice,		*
 *   this list of conditions and the following disclaimer in the documentation		*
 *   and/or other materials provided with the distribution. 				*
 *   Neither the name of the <ORGANIZATION> nor the names of its contributors 		*
 *   may be used to endorse or promote products derived from this software 		*
 *   without specific prior written permission						*
 *                                                                         		*
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   		*
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     		*
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 		*
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER		*
 *   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 		*
 *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   		*
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    		*
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 		*
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  		*
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    		*
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          		*
 ***************************************************************************************/

#include "Cache.h"

#define debug (false)

//////////////////////////////////////////////////////////////////////////////////////////

CacheItem::CacheItem(){};

// Feb 14 changed: by default, use the (front,back) item as (s,t)
CacheItem::CacheItem(int key, intVector& item)
{
	this->id = key;
	this->accessKey = key;
	this->item = item;		// copy
	this->size = item.size();
	
	this->s = -1;
	this->t = -1;
	
	if (item.size()>0) {
		this->s = item.front();
		this->t = item.back();
	}
	
	this->score = -1;
};

CacheItem::CacheItem(int key, intVector& item, int queryStartNode, int queryTargetNode)
{
	this->id = key;
	this->accessKey = key;
	this->item = item;		// copy
	this->size = item.size();
	this->s = queryStartNode;
	this->t = queryTargetNode;
	this->score = -1;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Test::readMapData() {
	cout << "one, Base::readMapData start!" << endl;
	mapSize = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->getMapsize();
	cout << "mapsize: " << mapSize << endl;
	string mapFile = ts.getTestFile();
	std::pair<double, double> tmpPair;
	string str;
	std::vector<string> tokens;

	mapFile.replace ((mapFile.size()-4), 4, "node"); //change file extention from .cedge to .cnode
	cout << "mapfile: " << mapFile << endl;
	ifstream in_data (mapFile.c_str(), ios::in); //*.cnode file

	//read in the mapping between coordinates and node ids from *.cnode file
	if (in_data.is_open()) {
		for(int i = 0; i < mapSize; i++)
		{
			getline(in_data, str);
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());
			tmpPair = std::make_pair(atof(tokens[1].c_str()),atof(tokens[2].c_str()));
			coordinate2Nodeid[tmpPair] = atoi(tokens[0].c_str());
			nodeid2coordinate[atoi(tokens[0].c_str())] = tmpPair;
			points.push_back(tmpPair);
			// WARNING: brittle code (error when the file contains one more line!)
		}
	}
	in_data.close();

	
	//writeoutCacheCoordinates(ts.getTestName(), cache.getCacheContentVector(), nodeid2coordinate, ts.getSplits());

	cout << "two, Base::readMapData end!" << endl;
}

void Test::writeoutCacheCoordinates(string testbasename, vector<CacheItem> cm)
{
	int numSplits=ts.getSplits();

	cout << "Base::writeoutCacheCoordinates start!";
	vector<int> sp;
	int i=0;
	coordinate c;
	ofstream of;
	string fn = testbasename;
	string app = "D" + boost::lexical_cast<std::string>(numSplits);
	string app2 = "C" + boost::lexical_cast<std::string>(cacheSize);
	app.append(app2);
 	app.append(".cache");
	fn.replace ((fn.size()-5), app.size(), app); //change file extention from .test to fnD#splits.cache
	of.open(fn.c_str(), ios::out | ios::ate | ios::app);

	BOOST_FOREACH(CacheItem ci, cm) {
		sp = ci.item;

		BOOST_FOREACH(int v, sp) {
			if(nodeid2coordinate.find(v) != nodeid2coordinate.end()) {
				c = nodeid2coordinate.at(v);
				of << ci.getScore() << " " << c.first << " " << c.second << "\n";
			} else
				cout << "\nBase::writeoutCacheCoordinates ERROR:  unknown node id." << endl;
		}
		of << endl;
		i++;
	}

	of.close();
	
	
	{	
		fn = testbasename;
		app = "D" + boost::lexical_cast<std::string>(numSplits);
		app2 = "C" + boost::lexical_cast<std::string>(cacheSize);
		app.append(app2);
		app.append(".statCache");
		fn.replace ((fn.size()-5), app.size(), app); //change file extention from .test to fnD#splits.cache
		of.open(fn.c_str(), ios::out | ios::ate | ios::app);

		BOOST_FOREACH(intPairPairs stats, cacheStats)// stats: (node1,node2) -> (seen count, sp length)
			of << stats.first.first << " " << stats.first.second << " " << stats.second.first << " " << stats.second.second << endl;

		of.close();
	}
	
	cout << " ... Done!" << endl;
}



int Test::writeoutTestCoordinates(string testbasename, std::vector<intPair> stPointPairs)
{
	cout << "Base::writeoutTestCoordinates start!";
	vector<int> sp;
	coordinate c;
	string fn = testbasename;
	string app = "TEST.cache";
	fn.replace ((fn.size()-5), app.size(), app); //change file extention from .test TEST.cache
	ifstream ifile(fn.c_str());
	if (ifile) {ifile.close(); return 0;} //file already exist

	ofstream of(fn.c_str(), ios::out | ios::ate | ios::app);

	BOOST_FOREACH(intPair ip, stPointPairs)
	{
		sp = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(ip.first, ip.second);

		BOOST_FOREACH(int v, sp)
		{
			if(nodeid2coordinate.find(v) != nodeid2coordinate.end())
			{
				c = nodeid2coordinate.at(v);
				of << c.first << " " << c.second << "\n";
			}else
				cout << "\nBase::writeoutTestCoordinates ERROR:  unknown node id." << endl;
		}
		of << endl;
	}
	cout << " ... Done!" << endl;
	of.close();
	return 1;
}


int Test::writeoutTrainingCoordinates(string testbasename, std::vector<intPair> stPointPairs)
{
	cout << "Base::writeoutTrainingCoordinates start!";
	vector<int> sp;
	coordinate c;
	string fn = testbasename;
	string app = "TRAIN.cache";
	fn.replace ((fn.size()-5), app.size(), app); //change file extention from .test TEST.cache
	ifstream ifile(fn.c_str());
	if (ifile) {ifile.close(); return 0;} //file already exist

	ofstream of(fn.c_str(), ios::out | ios::ate | ios::app);

	BOOST_FOREACH(intPair ip, stPointPairs)
	{
		sp = RoadGraph::mapObject(ts.getTestFile(),ts.getTestType())->dijkstraSSSP(ip.first, ip.second);

		BOOST_FOREACH(int v, sp)
		{
			if(nodeid2coordinate.find(v) != nodeid2coordinate.end())
			{
				c = nodeid2coordinate.at(v);
				of << c.first << " " << c.second << "\n";
			}else
				cout << "\nBase::writeoutTrainingCoordinates ERROR:  unknown node id." << endl;
		}
		of << endl;
	}
    cout << " ... Done!" << endl;
	of.close();
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////


aCache::aCache() {

}


aCache::aCache(testsetting ts) {
	init(ts);
}

void aCache::init(testsetting ts)
{
	if(ts.cacheType == GRAPH_CACHE || ts.cacheType == LIST_CACHE || ts.cacheType == COMPRESSED_G_CACHE)
	{
		cacheType = ts.cacheType;
		cacheSize = ts.getCacheSize();
		cacheUsed = 0;
		numberOfNodes = 0;
		numItems = 0;
		totalEntriesInCompressedBitsets = 0;
	}
	else
		cout << "invalid cache type set: " << ts.cacheType << endl;
}

bool aCache::insertItem(CacheItem ci)
{
	if(!hasEnoughSpace(ci)) return false;

	numItems++;

	cache.push_back(ci);
	updateCacheUsed(ci);
	return true;
}

bool aCache::insertItemWithScore(CacheItem ci, double score)
{
	ci.setScore(score);
	return insertItem(ci);
}

bool aCache::checkCache(int s, int t)
{
	vector<int> cItem;

	BOOST_FOREACH(CacheItem ci, cache )
	{
		cItem = ci.item;
		if(find(cItem.begin(),cItem.end(), s) != cItem.end() && find(cItem.begin(),cItem.end(), t) != cItem.end())
			return true;
	}
	return false;
}

bool aCache::checkCache(intPair query)
{
	return checkCache(query.first, query.second);
}

bool aCache::checkCache(CacheItem ci)
{
	return checkCache(ci.s, ci.t);
}

//assumes cache item ci has NOT been added to vector<CacheItem> cache
bool aCache::hasEnoughSpace(CacheItem ci)
{
    return hasEnoughSpace(ci.item);
}

bool aCache::hasEnoughSpace(vector<int> sp)
{
    if(cacheType == GRAPH_CACHE)
	{
		int newNodes = 0; //nodes in ci which is not already in graph

		BOOST_FOREACH(int v, sp)
			if(nodeIdsInCache.find(v) == nodeIdsInCache.end()){	newNodes++;	}

		if( (nodeIdsInCache.size() + newNodes ) * ( NODE_BITS + BIT*(cache.size()+1)) <= cacheSize) return true;
	}
	else if(cacheType == LIST_CACHE)
	{
		if(cacheUsed + sp.size()*NODE_BITS < cacheSize) return true;
	}
	else if(cacheType == COMPRESSED_G_CACHE)
	{
//        int newNodes = 0; //nodes in ci which is not already in graph
//        int bitsToAdd = 0;
//
//		BOOST_FOREACH(int v, ci.item)
//		{
//			if(nodeIdsInCache.find(v) == nodeIdsInCache.end())
//			{	newNodes++;
//				bitsToAdd += 2;
//			}
//			else
//				if(nodeIdsInCacheCompressed.at(v).back().second != cache.size())
//					bitsToAdd += 2;
//		}
//
//		if( (nodeIdsInCache.size() + newNodes ) * NODE_BITS + (totalEntriesInCompressedBitsets+bitsToAdd)* ceil(log(cache.size())/log(2)) <= cacheSize) return true;
        if(cacheUsed < cacheSize) return true;
	}
	else
		std::cout << "aCache::hasEnoughSpace! Invalid cache type set: " << cacheType << endl;

	return false;
}

//assumes cache item ci has already been added to vector<CacheItem> cache
void aCache::updateCacheUsed(CacheItem ci)
{
	if(cacheType == GRAPH_CACHE)
	{
		int nodesToBeAdded = 0;
		boost::dynamic_bitset<> bitset(0);

		BOOST_FOREACH(int v, ci.item)
		{
			if(nodeIdsInCache.find(v) == nodeIdsInCache.end())
			{
				boost::dynamic_bitset<> bitset(cache.size()-1); //set all bits to zero in the bitmap for the first cache.size()-1 bits
				nodesToBeAdded++;
				nodeIdsInCache[v] = bitset;
			}
		}

		BOOST_FOREACH(intDBitset::value_type nb, nodeIdsInCache)
		{
			if(find(ci.item.begin(), ci.item.end(), nb.first) != ci.item.end())
				nodeIdsInCache.at(nb.first).push_back(1);
			else
				nodeIdsInCache.at(nb.first).push_back(0);
		}

		cacheUsed =  nodeIdsInCache.size() * (NODE_BITS + BIT*cache.size()) ;
		numberOfNodes = nodeIdsInCache.size();
	}
	else if(cacheType == LIST_CACHE)
	{
		cacheUsed = cacheUsed + ci.size*NODE_BITS;
		numberOfNodes = numberOfNodes + ci.size;
	}
    else if(cacheType == COMPRESSED_G_CACHE)
	{
	    pidSets.insertPath(ci.item);

        int num_paths = cache.size();
        int num_tokens = pidSets.GetNumTokens();

        int pid_bits = ceil( log( num_paths ) / log(2) );
        int token_bits = ceil( log(  num_tokens ) / log(2) );

        cacheUsed =  pidSets.getNumNodes() * ( NODE_BITS + token_bits ) + pidSets.GetTotalNumItems()  * ( pid_bits + 1 )  +  num_tokens * token_bits * 2 ;
		numberOfNodes = pidSets.getNumNodes();
	}
	else
		std::cout << "aCache::hasEnoughSpace! Invalid cache type set: " << cacheType << endl;
}

void aCache::writeOutBitmaps()
{
	int nodeid=0;
	BOOST_FOREACH(intDBitset::value_type nb, nodeIdsInCache)
	{
		nodeid = nb.first;
		cout << nb.first << " " << nb.second << endl;
	}
	cout << nodeIdsInCache.size() << endl;
	cout << nodeIdsInCache.at(nodeid).size() << endl;
	cout << nodeIdsInCache.size()*nodeIdsInCache.at(nodeid).size() << endl;
}

void aCache::printNodesTokensPaths()
{
    pidSets.printNodesTokensPaths();
}


//////////////////////////////////////////////////////////////////////////////////////////


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

void compressedPidTokens::insertCompressedPid( int path_id, intVector& tmpModify, CompressPidType* prevcomppath )
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

//////////////////////////////////////////////////////////////////////////////////////////
