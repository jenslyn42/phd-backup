#ifndef RANGE_H
#define RANGE_H

#include "Setting.h"
#include "RoadGraph.h"


typedef std::vector<Edge> EdgeList;
typedef std::pair<int,double> Edge;


class Range
{
    public:
        Range();
        virtual ~Range();

        TestSetting ts;

        intPairVector trainingSRQueryPairs, testSRQueryPairs;
        intPointMap qCoordMap, poiCoordMap;
        intVectorMap vertexToPathids;
        intintMapMap pathidMap;
        intMap vertexDistMap;

        void init(TestSetting ts);

        intPairVector evalQuery(int q, int R);
        intPairVector rangeQuery(int q, int R, intPairVector spQueryCandidates);
        void generatePOIlist();

    protected:

    private:
        intPairVector evalQueryFair(int q, int R);
        intPairVector evalQueryNaive(int q, int R);
        void readPOIlist();
        void readRangeQueries(QLOG_CHOICE qlog);
        int euclideanDist(Point s, Point t);
        int spDist(Point s, Point t);
        void populateDistMaps();
};

#endif // RANGE_H
