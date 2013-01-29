#include "Competitors.h"

#define debug false

///////////////////////////////////////////////////////////////////////////////////////////////////////////


LRU::LRU(TestSetting ts)
{
	this->ts = ts;
	cacheSize = ts.cacheSize;
	numTotalQueries = 0;
	numCacheHits = 0;
	cacheUsed = 0;
	numDijkstraCalls = 0;
	readMapData();
}

LRU::~ LRU()
{
}

void LRU::buildCache()
{
	cout<< "2.0 done cachesize:" << cacheSize << endl;
	readQueryLogData(QLOG_TEST);
	readQueryLogData(QLOG_TRAIN);
	cout << "test/training query pairs:" << testSTPointPairs.size() << "/" << trainingSTPointPairs.size() << endl;
}

void LRU::runQueryList()
{
	RoadGraph::mapObject(ts)->resetRoadGraph(); //as the roadgraph object has been used already we need to reset it to clear the statistics.
cout << "runQueryList 1" << endl;
    int same=0, notSame=0;
    intVector tmp1, tmp2;
    intPairIntMap sameMap;
    pair<int,double> tempPair;
    //<pair(pair(counter,longestCommenPathLength/longestParentSP), pair(x,y)), pair(pair(|SPone|, |SPtwo|), pair(|union|, |intersection|))>
    IntDoubleIntPairIntPairPointMap similarityMap;

cout << "runQueryList 2" << endl;
	BOOST_FOREACH(intPair q, testSTPointPairs ) {
		tmp1 = checkAndUpdateCache(q, false);
		tmp2 = checkAndUpdateCache(q, true);
		sameMap[q] = isSame(tmp1,tmp2)? 1:0;
		tempPair = make_pair<int,double>(numTotalQueries, lcp(tmp1,tmp2));
        similarityMap[make_pair<intPair,intPair>(tempPair,q)] = similarity(tmp1,tmp2);
		sameMap.at(q) == 1 ? same++ : notSame++;
		numTotalQueries++;
	}
cout << "runQueryList 3" << endl;
	BOOST_FOREACH(intPair q, trainingSTPointPairs ) {
		tmp1 = checkAndUpdateCache(q, false);
		tmp2 = checkAndUpdateCache(q, true);
		sameMap[q] = isSame(tmp1,tmp2)? 1:0;
		tempPair = make_pair<int,double>(numTotalQueries, lcp(tmp1,tmp2));
        similarityMap[make_pair<pair<int,double>,intPair>(tempPair,q)] = similarity(tmp1,tmp2);
		sameMap.at(q) == 1 ? same++ : notSame++;
		numTotalQueries++;
	}
cout << "runQueryList 4" << endl;


	cout << "Base::plotShortestPaths start!";
	string fn;
	fn.append( ts.testFile, 0, 3);
	fn.append("spfile.sp");

	ifstream ifile(fn.c_str());

	ofstream of(fn.c_str(), ios::out | ios::ate | ios::app);
	int counter = 0;
	Point tmp;
	of << "\t" << same << " , " << notSame << endl;


	BOOST_FOREACH(IntDoubleIntPairIntPairPointMap::value_type simi, similarityMap) {
	    tmp = simi.second.second;
        of << counter++ <<"\t(" << simi.first.second.first << "," << simi.first.second.second << ") [" << simi.second.first.first << "," << simi.second.first.second << "] "<< tmp.first << " " << tmp.second << " " << simi.first.first.second << " <" << tmp.second/tmp.first << ">" << endl;
	}
	cout << " ... Done!" << endl;
	of.close();


    cout << "------- Path similarity aggregation ------" << endl;
    aggregate(similarityMap);
    cout << "------- LCP aggregation ------------------" << endl;
    lcpAgg(similarityMap);
}

vector<int> LRU::checkAndUpdateCache(intPair query, bool constWeight)
{
		numDijkstraCalls++;
		return RoadGraph::mapObject(ts)->dijkstraSSSP(query.first, query.second, constWeight);
}


bool LRU::isSame(intVector& one, intVector& second ) {
    if(one.size() != second.size()) return false;

    if(one == second)
        return true;
    else
        return false;
}

pair<intPair,Point> LRU::similarity(intVector& one, intVector& second ) {
    double x,y;

    sort(one.begin(),one.end());
    sort(second.begin(),second.end());

    intVector result1, result2;
    int countOne=0, countSecond=0;
    int oneSize = one.size()-1;
    int secondSize = second.size()-1;

    while(true)
    {
        if(countOne == oneSize){
            for(; countSecond <= secondSize; countSecond++)
                result1.push_back(second.at(countSecond));
            x = result1.size();
            break;
        }
        if(countSecond == secondSize){
            for(;countOne <= oneSize; countOne++)
                result1.push_back(one.at(countOne));
            x = result1.size();
            break;
        }

        if(one.at(countOne) < second.at(countSecond)){
            result1.push_back(one.at(countOne));
            countOne++;
        }else if(one.at(countOne) > second.at(countSecond)){
            result1.push_back(second.at(countSecond));
            countSecond++;
        }else{
            result1.push_back(one.at(countOne));
            countOne++;
            countSecond++;
        }
    }

    countOne=0;
    countSecond=0;

    while(countOne <= oneSize && countSecond <= secondSize)
    {
        if(one.at(countOne) < second.at(countSecond)) countOne++;
        else if(one.at(countOne) > second.at(countSecond)) countSecond++;
        else{
            result2.push_back(one.at(countOne));
            countOne++;
            countSecond++;
        }
    }
    y = result2.size();


    return make_pair(make_pair(one.size(),second.size()),make_pair(x,y));
}

double LRU::lcp(intVector& one, intVector& second) {
    int counter=0;
    double lcp=0.0, tmplcp=1.0;
    intMap nodeplacement; //Keep track of the placement of each node
    intVector::iterator it;
    int secondsize = second.size();
    int onesize = one.size();
    BOOST_FOREACH(int node, one){
        nodeplacement[node] = counter;
        counter++;
    }

    for(int i=0; i< secondsize;i++){
        it = find(one.begin(), one.end(), second.at(i));
        if(it != one.end()){
            do{
               if(++it != one.end() && ++i != secondsize){
                    tmplcp++;
               }
            }while(i != secondsize && *it == second.at(i));

            if(tmplcp > lcp) lcp = tmplcp;
            tmplcp=1.0;
        }
    }
    if(onesize > secondsize){
        return lcp/one.size();
    }else{
        return lcp/second.size();
    }
}

void LRU::aggregate(IntDoubleIntPairIntPairPointMap& simMap){
  	long bucketsize = 10;
	long adjustment = 100000000; //Remove decimal so long datatype can be used
	boost::unordered_map<pair<long,long>, int> buckets;
	long rangeIncrement = ((1*adjustment)/bucketsize);
	pair<long,long> pnt;
	Point tmp;
	cout <<"calc buckets ";

    //Calculate number of paths which fall into each range, where the the range is 100/bucketsize
	for(long i=0; i < 1*adjustment; i = i+rangeIncrement)
	{
		pnt = make_pair<long,long>(i,i+rangeIncrement);
		BOOST_FOREACH(IntDoubleIntPairIntPairPointMap::value_type simi, simMap) {
			tmp = simi.second.second;
			long similar = (tmp.second/tmp.first)*adjustment;

			if(i < similar && similar <= (i + rangeIncrement)){
				buckets[pnt] = buckets[pnt] +1;
			}
	  	}
 		cout << ".";
	}

	cout << " done" <<endl;
	BOOST_FOREACH(PointIntMap::value_type bucket, buckets)
	{
		cout << "(" << bucket.first.first/(adjustment/100) << ":" << bucket.first.second/(adjustment/100) << ")" << " : " << bucket.second <<endl;
	}
}


void LRU::lcpAgg(IntDoubleIntPairIntPairPointMap& simMap){
  	long bucketsize = 10;
	long adjustment = 100000000; //Remove decimal so long datatype can be used
	boost::unordered_map<pair<long,long>, int> buckets;
	long rangeIncrement = ((1*adjustment)/bucketsize);
	pair<long,long> pnt;
	cout <<"calc buckets ";

    //Calculate number of paths which fall into each range, where the the range is 100/bucketsize
	for(long i=0; i < 1*adjustment; i = i+rangeIncrement){
		pnt = make_pair<long,long>(i,i+rangeIncrement);
		BOOST_FOREACH(IntDoubleIntPairIntPairPointMap::value_type simi, simMap) {
			long lcpNorm = (simi.first.first.second)*adjustment;

			if(i < lcpNorm && lcpNorm <= (i + rangeIncrement)){
				buckets[pnt] = buckets[pnt] +1;
			}
	  	}
 		cout << ".";
	}

	cout << " done" <<endl;
	BOOST_FOREACH(PointIntMap::value_type bucket, buckets)
	{
		cout << "(" << bucket.first.first/(adjustment/1000) << ":" << bucket.first.second/(adjustment/1000) << ")" << " : " << bucket.second <<endl;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////

