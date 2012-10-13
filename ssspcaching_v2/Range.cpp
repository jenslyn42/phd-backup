#include "Range.h"

#define debug false

Range::Range()
{

}

Range::~Range()
{
    //dtor
}


void Range::init(TestSetting ts){
    cout<< "R3.0 done" << endl;
    double refTime = clock();
    this->ts = ts;
    readPOIlist();
    //generatePOIlist();
    cout<< "R3.1R done, " << ts.getElapsedTime(refTime) << endl;
    readRangeQueries(QLOG_TEST);
    cout<< "R3.2R done, " << ts.getElapsedTime(refTime) << endl;
    readRangeQueries(QLOG_TRAIN);
    cout<< "R3.3R done, " << ts.getElapsedTime(refTime) << endl;

    if(!ts.useSPtree)
        populateDistMaps(); //doing this will be incredibly slow

}


intPairVector Range::evalQuery(int q, int R){
    switch( ts.testRangetype ){
		case RALG_FAIR:
            return evalQueryFair(q, R);
			break;
		case RALG_NAIVE:
			return evalQueryNaive(q, R);
			break;
        default:
			cout << "Wrong or no range algorithm type set!!" << endl;
			exit(1);
	}
}

intPairVector Range::evalQueryFair(int q, int R){
    intPairVector stQueryPairs;
    Point qCoordinate = qCoordMap.at(q);

    BOOST_FOREACH(intPointMap::value_type poi, poiCoordMap){
        if(euclideanDist(qCoordinate, poi.second) <= R)
            stQueryPairs.push_back(std::make_pair(q, poi.first));
    }
    return stQueryPairs;
}

intPairVector Range::evalQueryNaive(int q, int R){
    intPairVector stQueryPairs;

    BOOST_FOREACH(intPointMap::value_type p, poiCoordMap){
        stQueryPairs.push_back(std::make_pair(q, p.first));
    }
    return stQueryPairs;
}

intPairVector Range::rangeQuery(int q, int R, intPairVector spQueryCandidates){
 int s,t;
 intPairVector result;

    BOOST_FOREACH(intPair qc, spQueryCandidates){
        s = qc.first;
        t = qc.second;

        if(ts.useSPtree){
            int  spDistS, spDistT;
            spDistS = RoadGraph::mapObject(ts)->getTrackdist(s,q);
            spDistT = RoadGraph::mapObject(ts)->getTrackdist(t,q);

            if(abs(spDistT - spDistS) <= R)
                result.push_back(qc);
        }else{
            intVector& first = vertexToPathids.at(s);
            intVector& second = vertexToPathids.at(t);

            vector<int> v(first.size()+second.size()); //full of zeroe

            sort(first.begin(), first.end());
            sort(second.begin(), second.end());

            set_intersection (first.begin(), first.end(), second.begin(), second.end(), v.begin());

            if(v.front() != 0){
                intMap sp = pathidMap.at(v.front());

                if(abs(sp.at(t) - sp.at(s)) <= R)
                    result.push_back(qc);
            }
        }
    }
    return result;
}

///input .poi format: record_id, point_id, point_x, point_y
void Range::readPOIlist(){

	string str;
    std::pair<double, double> coord;
	std::vector<string> tokens;
	string fn=ts.testFilePrefix;
	fn.append(".poi"); //set the extension to be .poi
	ifstream file (fn.c_str(), ios::in); //*.poi file

	cout << "Range::readPOIlist start: " << fn << endl;

	//find all pairs of nodeids in the training set to have SP done for them. map nodeids to Points.
	if (file.is_open()) {
		if (debug)
			cout << "two, Range::readPOIlist opened! " << endl;

		while(getline(file, str)) {
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

			coord = std::make_pair(atof(tokens[2].c_str()),atof(tokens[3].c_str()));

			poiCoordMap[atoi(tokens[1].c_str())] = coord;
			if((unsigned int)atoi(tokens[0].c_str()) == ts.numpoi) {  //unsigned to make the compiler shut up...
			    cout << "numpoi REACHED (numpoi/useRange/testRangetype):\t(" << ts.numpoi <<")/("; ts.useRange ? cout << "TRUE" : cout << "FALSE"; cout <<")/(";
                if(ts.testRangetype == RALG_FAIR){cout << "FAIR";}
                else if(ts.testRangetype == RALG_NAIVE){cout << "NAIVE";}
                cout << ")" << endl;
                break;
            }
  		}
	}
	file.close();
	cout << "Range::readPOIlist end! number of POI/useRange/testRangetype:" << poiCoordMap.size() << "/";
	ts.useRange ? cout << "TRUE" : cout << "FALSE"; cout << "/";
	if(ts.testRangetype == RALG_FAIR) {cout << "FAIR";} else {cout << "NAIVE";} cout << endl;
//B****************************************************
//    intMap poiDub;
//    BOOST_FOREACH (intPointMap::value_type poi, poiCoordMap){
//        if(poiCoordMap.find(poi.first) != poiCoordMap.end()) poiDub[poi.first] = poiDub[poi.first] +1;
//        else poiDub[poi.first] = 1;
//    }
//
//    BOOST_FOREACH(intPair poiIDcount, poiDub){
//        if(poiIDcount.second > 1) cout << endl;
//        cout << "(" << poiIDcount.first << "," << poiIDcount.second << ") ";
//    }
//    cout << endl;
//
//    cout << "DUB train: ";
//    BOOST_FOREACH (intPointMap::value_type poi, poiCoordMap){
//        BOOST_FOREACH (intPair query, trainingSRQueryPairs){
//            if(query.first == poi.first) cout << "*" << query.first << "*";
//        }
//    }
//    cout << endl;
//    cout << "DUB test: ";
//    BOOST_FOREACH (intPointMap::value_type poi, poiCoordMap){
//        BOOST_FOREACH (intPair query, testSRQueryPairs){
//            if(query.first == poi.first) cout << "*" << query.first << "*";
//        }
//    }
//    cout << endl;

//E****************************************************
}

void Range::generatePOIlist(){
    cout << "Range::generatePOIlist start" << endl;
    std::string testFilePrefix=ts.testFilePrefix;

    string nodeFN = testFilePrefix + ".cnode";
	string str;
	std::vector<string> tokens;
    intPointMap tmpPoiCoordMap;
    Point coord;

	ifstream nodeFile (nodeFN.c_str(), ios::in);
	if (nodeFile.is_open()) {
		while(getline(nodeFile, str)) {
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

			coord = std::make_pair(atof(tokens[1].c_str()),atof(tokens[2].c_str()));

			tmpPoiCoordMap[atoi(tokens[0].c_str())] = coord;
		}
		nodeFile.close();
	}
	int poiPoolSize = RoadGraph::mapObject(ts)->getMapsize();
    int tmpNid;
    Point tmpPnt;
    int index = 0;
    vector<int> seenPOI;
    string fn=ts.testFilePrefix;
	fn.append(".poi"); //make the extension .poi
    ///file output
	ofstream rqueryfile;
	rqueryfile.open((fn).c_str(), ios::out | ios::ate);
	while(ts.numpoi >= (unsigned) index){
	    tmpNid = rand() % poiPoolSize;
        if (find(seenPOI.begin(), seenPOI.end(), tmpNid) == seenPOI.end()){
            tmpPnt = tmpPoiCoordMap.at(tmpNid);
            rqueryfile << index << " " << tmpNid << " " <<  tmpPnt.first << " " <<  tmpPnt.second << endl;
            seenPOI.push_back(tmpNid);
            index++;
        }
    }
    rqueryfile.close();
    cout << "Range::generatePOIlist end" << endl;
}


///input file format: record_id, point_id, range, point_x, point_y.
void Range::readRangeQueries(QLOG_CHOICE qlog){

    string app = boost::lexical_cast<string>( ts.range );
	intPairVector* srQueryPairs=NULL;

	if (qlog==QLOG_TRAIN) {
		srQueryPairs=&trainingSRQueryPairs;
        app.append(".rqtrain");
	} else if (qlog==QLOG_TEST) {
		srQueryPairs=&testSRQueryPairs;
        app.append(".rqtest");
	} else {
		printf("*** invalid qlog parameter\n");
		exit(0);
		return;
	}

    Point qPair;
    intPair queryPair;
	string str;
	std::vector<string> tokens;

	string fn=ts.testFilePrefix;
	fn.append(app); //change file extention to .rqtest / .rqtrain
	ifstream qlogFile (fn.c_str(), ios::in); //*.train file

	cout << "Range::readRangeQueries start: " << fn << endl;


	if (qlogFile.is_open()) {
		if (debug)
			cout << "two, Range::readRangeQueries opened! " << endl;

		while(getline(qlogFile, str)) {
			boost::algorithm::split(tokens, str, boost::algorithm::is_space());

            queryPair = std::make_pair(atoi(tokens[1].c_str()),atoi(tokens[2].c_str())); //(nid, range)
			qPair = std::make_pair(atof(tokens[3].c_str()),atof(tokens[4].c_str())); //(nid_x, nid_y)

            qCoordMap[atoi(tokens[1].c_str())] = qPair; //q_id, (q_x, q_y)
			(*srQueryPairs).push_back(queryPair);
		}
	}
	qlogFile.close();
	cout << "Range::readRangeQueries end! size:" << (*srQueryPairs).size() << endl;
//
//    BOOST_FOREACH (intPairVector::value_type rquery, (*srQueryPairs)){
//        cout << "(" << rquery.first << "," << rquery.second << ")\t";
//    }
//    cout << endl;
}

int Range::euclideanDist(Point s, Point t){
    int xd, yd;
    xd = t.first - s.first;
    yd = t.second - s.second;
    return ceil(sqrt(xd*xd + yd*yd));
}

int Range::spDist(Point s, Point t){

///TODO implement
    return ceil(1);
}

void Range::populateDistMaps(){

    EdgeList* edgelists = RoadGraph::mapObject(ts)->getMap();
    int pathid = 0, prevVertex, curLength = 0, locator;
    intVector sp;

	cout << "Range::populateDistMaps begin! " << endl;

    BOOST_FOREACH (intPointMap::value_type q, qCoordMap){
        BOOST_FOREACH (intPointMap::value_type poi, poiCoordMap){
            sp = RoadGraph::computeSP(ts, q.first, poi.first);
//            cout << "Range::populateDistMaps sp size: " << sp.size() << endl;
//            cout << "Range::populateDistMaps vertexToPathids size: " << vertexToPathids.size() << endl;
//            cout << "Range::populateDistMaps sp args: " << q.first << "," << poi.first << endl;
//            cout << "Range::populateDistMaps sp: {";  BOOST_FOREACH (int j, sp) {cout << j << ","; } cout << "}" << endl;
            if(vertexToPathids.find(q.first) == vertexToPathids.end() || vertexToPathids.find(poi.first) == vertexToPathids.end()){
                prevVertex = poi.first;
//                cout << "Range::populateDistMaps one" << endl;
                BOOST_FOREACH(int v, sp){
                    if(v != prevVertex){
//                        cout << "current v, prevVertex: " << v << "," << prevVertex << endl;
                        vertexToPathids[v].push_back(pathid);
//                        cout << "Range::populateDistMaps two v,pathid: " << v << "," << pathid << endl;
//                        vector<int>::iterator it=find(edgelists.begin(),edgelists.end(), prevVertex);
                        EdgeList& prevAdjList = edgelists[prevVertex];
//                        cout << "Range::populateDistMaps three prevAdjList size: " << prevAdjList.size() << endl;
//                        cout << "prevAdjList (id;coordinate): "; BOOST_FOREACH (Edge e, prevAdjList) {cout << "(" << e.first << ";" << e.second << ") "; } cout << endl;
                        for(uint i = 0; i < prevAdjList.size(); i++) if(prevAdjList.at(i).first == v){ locator = i; break; }
                        if(locator==-1) cout << "BAD ERROR!!!!" << endl;
                        vertexDistMap[v] = curLength + prevAdjList.at(locator).second;
//                        cout << "Range::populateDistMaps four" << endl;
                        curLength = curLength + prevAdjList.at(locator).second;
//                        cout << "Range::populateDistMaps five" << endl;
                        prevVertex = v;
                    }
                }
                pathidMap[pathid] = vertexDistMap;
                pathid++;
                vertexDistMap.clear();
                locator = -1;
//                cout << "Range::populateDistMaps six! sp size: " << sp.size() << endl;
            }
        }
        cout << "latest pathID(" << pathid << ") Range::populateDistMaps seven! (q, coordinate): (" << q.first << ",(" << q.second.first <<";" << q.second.second << ")" << endl;
    }
}
