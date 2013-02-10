#include "Setting.h"
#include "RoadGraph.h"

//arg 1: input filename, no type (cedge/cnode assumed)
//arg 2: queries to generate in, half in *.qtest, other half in *.qtrain
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
cout << "Map/Clusters/Radius: " << fn << "/" << numPoints << "/" << radius << endl;
cout << "******************************************" << endl;



int random[numPoints];
int i=0;

while(i<numPoints)
{
	random[i] = rand()%RoadGraph::mapObject(fn)->getMapsize();
	regionVerticelists[random[i]] = RoadGraph::mapObject(fn)->dijkstraSSSP(random[i], -1, constWeight, radius);
	cout << "Region " << i << " size: " << regionVerticelists[random[i]].size() << " " <<  endl;
	i++;
}
cout << "regionVerticelists initialized" << endl;

int tmpPick1, tmpPick2, sid, tid;
vector<int> tempList1, tempList2;

boost::unordered_map<int,std::pair<string,string> > nodelist = RoadGraph::mapObject(fn)->getNodelist();
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

//cout << "Unique pairs in stats: " << stats.size() << endl;
//cout << "(x,y) - train / test / total" << endl;

// BOOST_FOREACH(intPairIntMap::value_type stat, trainstats)
// {
// 	if(teststats.find(stat.first) != teststats.end())
// 		cout << "(" << stat.first.first << "," << stat.first.second << ") - " << stat.second << "/" << teststats.at(stat.first) << "/" << stats.at(stat.first) << "\t";
// }

	return EXIT_SUCCESS;
};