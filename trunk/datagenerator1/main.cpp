#include "Setting.h"
#include "RoadGraph.h"

// if true: write out all vertices of all regions in .poi file
#define WRITE_POI true 

//arg 1: input filename, no type (cedge/cnode assumed)
//arg 2: queries to generate, half in *.qtest, other half in *.qtrain
//arg 3: number of points to use as example regions to pick vertices from
//arg 4: radius of regions


int main(int argc, char *argv[]) {
	srand(0);	srand48(0);

if(argc != 5){
  cout << "Wrong usage!\nUsage: " << argv[0] << "<mapname> <numQueries> <numPoints> <radius>" << endl;
  exit(-1);
}
string fn = argv[1];
int queriesToGenerate = atoi(argv[2]); //will be split into two files with half each
int numPoints = atoi(argv[3]);
int radius = atof(argv[4]);
bool constWeight = false;
boost::unordered_map<int,vector<int> > regionVerticelists;

cout << "******************************************" << endl;
cout << "Map/Clusters/Radius: " << fn << "/" << numPoints << "/" << radius << " " << sizeof(int) << " " << sizeof(char) << " " << sizeof(char*)  <<endl;
cout << "******************************************" << endl;


//comment out below and uncomment following loop to have random locations
//---------------------------------------------------------
// 2130052639
///Aalborg data
// 17283 558701.625000 6321532.000000 (558700, 6321500)
// 9970 556530.375000 6325770.000000 (556500, 6325700)
// 18596 555510.375000 6322083.500000 (555500, 6322000)
// 17347 551536.562500 6314563.500000 (551500, 6314500)
// 127966 558209.000000 6323511.000000 (582000, 6323500)
// 94186 555708.812500 6348010.500000 (557000, 6348000)
// 119685 537062.000000 6345856.500000 (537000, 6345800)
// 24815 544502.937500 6335516.000000 (544500, 6335500)
// 31652 567145.125000 6305505.500000 (550500, 6305500)
// 33923 531641.562500 6295516.000000 (531600, 6295500)
/*
int random[10];
int i=0; //just because i is used later on
cout << RoadGraph::mapObject(fn)->getMapsize();


random[0] = 17283;
	regionVerticelists[random[0]] = RoadGraph::mapObject(fn)->dijkstraSSSP(random[0], -1, constWeight, radius);
	cout << "Region " << 0 << " size: " << regionVerticelists[random[0]].size() << " " <<  endl;
random[1] = 9970;
	regionVerticelists[random[1]] = RoadGraph::mapObject(fn)->dijkstraSSSP(random[1], -1, constWeight, radius);
	cout << "Region " << 1 << " size: " << regionVerticelists[random[1]].size() << " " <<  endl;
random[2] = 18596;
	regionVerticelists[random[2]] = RoadGraph::mapObject(fn)->dijkstraSSSP(random[2], -1, constWeight, radius);
	cout << "Region " << 2 << " size: " << regionVerticelists[random[2]].size() << " " <<  endl;
random[3] = 17347;
	regionVerticelists[random[3]] = RoadGraph::mapObject(fn)->dijkstraSSSP(random[3], -1, constWeight, radius);
	cout << "Region " << 3 << " size: " << regionVerticelists[random[3]].size() << " " <<  endl;
random[4] = 127966;
	regionVerticelists[random[4]] = RoadGraph::mapObject(fn)->dijkstraSSSP(random[4], -1, constWeight, radius);
	cout << "Region " << 4 << " size: " << regionVerticelists[random[4]].size() << " " <<  endl;
random[5] = 94186;
	regionVerticelists[random[5]] = RoadGraph::mapObject(fn)->dijkstraSSSP(random[5], -1, constWeight, radius);
	cout << "Region " << 5 << " size: " << regionVerticelists[random[5]].size() << " " <<  endl;
random[6] = 119685;
	regionVerticelists[random[6]] = RoadGraph::mapObject(fn)->dijkstraSSSP(random[6], -1, constWeight, radius);
	cout << "Region " << 6 << " size: " << regionVerticelists[random[6]].size() << " " <<  endl;
random[7] = 24815;
	regionVerticelists[random[7]] = RoadGraph::mapObject(fn)->dijkstraSSSP(random[7], -1, constWeight, radius);
	cout << "Region " << 7 << " size: " << regionVerticelists[random[7]].size() << " " <<  endl;
random[8] = 31652;
	regionVerticelists[random[8]] = RoadGraph::mapObject(fn)->dijkstraSSSP(random[8], -1, constWeight, radius);
	cout << "Region " << 8 << " size: " << regionVerticelists[random[8]].size() << " " <<  endl;
random[9] = 33923;
	regionVerticelists[random[9]] = RoadGraph::mapObject(fn)->dijkstraSSSP(random[9], -1, constWeight, radius);
	cout << "Region " << 9 << " size: " << regionVerticelists[random[9]].size() << " " <<  endl;
//---------------------------------------------------------
*/
//**********************************************************
int random[numPoints];
int i=0;

cout << "Calc regions" << endl;
while(i<numPoints)
{
	random[i] = rand()%RoadGraph::mapObject(fn)->getMapsize();
	regionVerticelists[random[i]] = RoadGraph::mapObject(fn)->dijkstraSSSP(random[i], -1, constWeight, radius);
	cout << "Region " << i << " size: " << regionVerticelists[random[i]].size() << " S:(" << random[i] << ")" <<  endl;
	i++;
}
//**********************************************************
cout << "regionVerticelists initialized" << endl;


boost::unordered_map<int,std::pair<string,string> > nodelist = RoadGraph::mapObject(fn)->getNodelist();
int tmpPick1, tmpPick2, sid, tid;
vector<int> tempList1, tempList2;

string filename = fn + "GP";
filename.append(boost::lexical_cast<std::string>(numPoints));
filename.append("R");
filename.append(boost::lexical_cast<std::string>(radius));
filename.append("Q");
filename.append(boost::lexical_cast<std::string>(queriesToGenerate));

filename.append(".qtrain");
pair<string,string> tmpPair;

boost::unordered_map<intPair, int> stats;
boost::unordered_map<intPair, int> trainstats;
boost::unordered_map<intPair, int> teststats;

///open file for output
ofstream resultfile;
resultfile.open(filename.c_str(), ios::out | ios::ate | ios::app);
i=0;

cout << "file writing started [" << filename << "]" << endl;
for(;i<queriesToGenerate/2;i++)
{
	tmpPick1 =rand()%numPoints;
	tempList1 =regionVerticelists.at(random[tmpPick1]);

	do{
	tmpPick2=rand()%numPoints;
	}while(tmpPick1 == tmpPick2);
	tempList2 =regionVerticelists.at(random[tmpPick2]);

	sid = tempList1[rand()%tempList1.size()];
	tid = tempList2[rand()%tempList2.size()];
	
	resultfile << i << " " << sid << " " << tid << " ";
	tmpPair = nodelist[sid];
	resultfile << tmpPair.first << " "<< tmpPair.second << " ";
	tmpPair = nodelist[tid];
	resultfile << tmpPair.first << " "<< tmpPair.second << " " << endl;
	
	int temp;
	if(sid>tid){temp=sid; sid=tid; tid=temp;}
	stats[make_pair<int,int>(sid,tid)] = stats[make_pair<int,int>(sid,tid)] + 1;
	trainstats[make_pair<int,int>(sid,tid)] = trainstats[make_pair<int,int>(sid,tid)] + 1;
}
cout << "file writing ended" << endl;
resultfile.close();


filename.replace ((filename.size())-5, 5, "test");
///open file for output
resultfile.open(filename.c_str(), ios::out | ios::ate | ios::app);

cout << "file writing started [" << filename << "]" << endl;
for(;i<queriesToGenerate;i++)
{
	tmpPick1 =rand()%numPoints;
	tempList1 =regionVerticelists.at(random[tmpPick1]);

	do{
	tmpPick2=rand()%numPoints;
	}while(tmpPick1 == tmpPick2);

	tempList2 =regionVerticelists.at(random[tmpPick2]);

	sid = tempList1[rand()%tempList1.size()];
	tid = tempList2[rand()%tempList2.size()];
	
	resultfile << i << " " << sid << " " << tid << " ";
	tmpPair = nodelist[sid];
	resultfile << tmpPair.first << " " << tmpPair.second << " ";
	tmpPair = nodelist[tid];
	resultfile << tmpPair.first << " " << tmpPair.second << " " << endl;	
	
	int temp;
	if(sid>tid){temp=sid; sid=tid; tid=temp;}
	stats[make_pair<int,int>(sid,tid)] = stats[make_pair<int,int>(sid,tid)] + 1;
	teststats[make_pair<int,int>(sid,tid)] = teststats[make_pair<int,int>(sid,tid)] + 1;
}
cout << "file writing ended" << endl;
resultfile.close();


if(WRITE_POI){
  int linenum = 0;
  pair<string,string> tempPair;
  filename.replace ((filename.size())-5, 5, "poi");
  ofstream poifile;
  poifile.open(filename.c_str(), ios::out | ios::ate | ios::app);

  BOOST_FOREACH(intVectorMap::value_type verticelist, regionVerticelists){
    BOOST_FOREACH(int vertex, verticelist.second){
      tempPair = nodelist[vertex];
      poifile << linenum << " " << vertex << " " << tempPair.first << " " << tempPair.second << endl;
      linenum++;
    }
  }
  poifile.close();
}

//cout << "Unique pairs in stats: " << stats.size() << endl;
//cout << "(x,y) - train / test / total" << endl;

// BOOST_FOREACH(intPairIntMap::value_type stat, trainstats)
// {
// 	if(teststats.find(stat.first) != teststats.end())
// 		cout << "(" << stat.first.first << "," << stat.first.second << ") - " << stat.second << "/" << teststats.at(stat.first) << "/" << stats.at(stat.first) << "\t";
// }

	return EXIT_SUCCESS;
};
