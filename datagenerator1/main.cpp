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

if(argc < 6){
  cout << "Wrong usage!\nUsage: " << argv[0] << "<genChoice> <mapname> <numQueries> <numPoints> <radius> [<inputFn>]" << endl;
  exit(-1);
}
if(atoi(argv[1]) == 3 && argc < 7){
  cout << "Wrong usage!\nUsage: " << argv[0] << "<genChoice> <mapname> <numQueries> <numPoints> <radius> <inputFn>" << endl;
  exit(-1);
}
if(atoi(argv[1]) == 4 && argc < 7){
  cout << "Wrong usage!\nUsage: " << argv[0] << "<genChoice> <mapname> <numQueries> <numPoints> <radius> <inputFn>" << endl;
  exit(-1);
}

int genChoice = atoi(argv[1]); //1: manual clusters, 2: random clusters, 3:read cluster centers from file
string fn = argv[2];
int queriesToGenerate = atoi(argv[3]); //will be split into two files with half each
int numPoints = atoi(argv[4]);
int radius = atof(argv[5]);
string inputFn;
if(genChoice == 3 || genChoice == 4 )
  inputFn = argv[6]; 
bool constWeight = false;
boost::unordered_map<int,vector<int> > regionVerticelists;

int i=0;
long mapsize = RoadGraph::mapObject(fn)->getMapsize(); 
int centers[numPoints];

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

// NY data
// 78999 -74455311 40309206
// 61225 -74480997 40482404
// 90187 -74134986 40449407
// 82099 -74047383 40352009
// 59037 -74460597 40570703
// 
// 63116 -74415595 40506804
// 218834 -74137434 40616743
// 129872 -74250990 40641203
// 217498 -74070038 40613283
// 207549 -73807057 40689526
// 
// 157525 -73555071 40680001
// 95371 -74480099 40871299
// 48468 -74183589 40738901
// 139309 -73810603 40818779
// 190669 -74005091 40712174
// 
// 152661 -73610972 40710301
// 111669 -74169088 40912198
// 22560 -74000583 40925298
// 253154 -73788277 40911499
// 235397 -74027525 41117092


if(genChoice == 1){
  //MANUAL - uncomment the dataset to be used!
  //-------------------------------------------------
  //NY data
  int cent [] = {190669, 218834, 207549, 48468, 22560, 111669, 217498, 253154, 152661, 157525, 129872, 63116, 139309, 90187, 59037, 78999, 61255, 82099, 95371, 235397};
  int numCenters = numPoints;
  if(numCenters > 20) numCenters = 20; //limit, we only have 20 points to work with
  //-------------------------------------------------

  //-------------------------------------------------
  //NY data - 2 point special set
  //int cent [] = {260952, 91408};
  //int numCenters = numPoints;
  //if(numCenters > 2) {cout << "INVALID points"; exit(1);} //limit, we only have 2 points to work with
  //-------------------------------------------------

  //-------------------------------------------------
  ///Aalborg data
  //int cent [] = {17283, 9970, 18596, 17347, 127966, 94186, 119685, 24815, 31652, 33923};
  //int numCenters = 10;
  //-------------------------------------------------
  //
  cout << mapsize;
  
  BOOST_FOREACH(int c, cent){centers[i] = c; i++;}
  i=0; //just because i is used later on

  while(i<numCenters){
    regionVerticelists[centers[i]] = RoadGraph::mapObject(fn)->dijkstraSSSP(centers[i], -1, constWeight, radius);
    cout << "Region " << i << " size: " << regionVerticelists[centers[i]].size() << " S:(" << centers[i] << ")" <<  endl;
    i++;
  }
}
else if(genChoice == 2){ //Random choice of cluster center placement
  cout << "Calc regions" << endl;
  while(i<numPoints)
  {
    centers[i] = rand()% mapsize;
    regionVerticelists[centers[i]] = RoadGraph::mapObject(fn)->dijkstraSSSP(centers[i], -1, constWeight, radius);
    cout << "Region " << i << " size: " << regionVerticelists[centers[i]].size() << " S:(" << centers[i] << ")" <<  endl;
    i++;
  }
}
else if(genChoice == 3){  
  ifstream in_data (inputFn.c_str(), ios::in);
  std::vector<string> tokens;
  string str;
  
  if(in_data.is_open()) {
    while(getline(in_data, str)){
      boost::algorithm::split(tokens, str, boost::algorithm::is_space());
      
      if(i >= numPoints) break;
      centers[i] = atoi(tokens[1].c_str());
      regionVerticelists[centers[i]] = RoadGraph::mapObject(fn)->dijkstraSSSP(centers[i], -1, constWeight, radius);
      cout << "Region " << i << " size: " << regionVerticelists[centers[i]].size() << " S:(" << centers[i] << ")" <<  endl;
      
      centers[i+1] = atoi(tokens[2].c_str());
      regionVerticelists[centers[i+1]] = RoadGraph::mapObject(fn)->dijkstraSSSP(centers[i+1], -1, constWeight, radius);
      cout << "Region " << i+1 << " size: " << regionVerticelists[centers[i+1]].size() << " S:(" << centers[i+1] << ")" <<  endl;
      
      if(regionVerticelists[centers[i+1]].size() != 0 && regionVerticelists[centers[i]].size() != 0)
	i = i+2;
    }
    in_data.close();
  }
}
else if(genChoice == 4){  
  ifstream in_data (inputFn.c_str(), ios::in);
  std::vector<string> tokens;
  string str;
  
  if(in_data.is_open()) {
    while(getline(in_data, str)){
      boost::algorithm::split(tokens, str, boost::algorithm::is_space());
      
      centers[i] = atoi(tokens[1].c_str());
      regionVerticelists[centers[i]] = RoadGraph::mapObject(fn)->dijkstraSSSP(centers[i], -1, constWeight, radius);
//       cout << "Region " << i << " size: " << regionVerticelists[centers[i]].size() << " S:(" << centers[i] << ")" <<  endl;
      
      centers[i+1] = atoi(tokens[2].c_str());
      regionVerticelists[centers[i+1]] = RoadGraph::mapObject(fn)->dijkstraSSSP(centers[i+1], -1, constWeight, radius);
//       cout << "Region " << i+1 << " size: " << regionVerticelists[centers[i+1]].size() << " S:(" << centers[i+1] << ")" <<  endl;
      
      if(regionVerticelists[centers[i+1]].size() != 0 && regionVerticelists[centers[i]].size() != 0)
	i = i+2;
      else cout << "*";
    }
    in_data.close();
  }
}
    
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
  if(genChoice == 4){
    tmpPick1 = rand()%regionVerticelists.size();
  }
  tempList1 =regionVerticelists.at(centers[tmpPick1]);
  //make sure two clusters centers were chosen from the same query pair
  if(genChoice == 3 || genChoice == 4){
    if((tmpPick1+1)%2 == 0) tmpPick2 = tmpPick1-1;
    else tmpPick2 = tmpPick1+1;
  }else{
    do{
      tmpPick2=rand()%numPoints;
    }while(tmpPick1 == tmpPick2);
  }
  
  tempList2 =regionVerticelists.at(centers[tmpPick2]);
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
for(;i<queriesToGenerate;i++){
  tmpPick1 =rand()%numPoints;
  if(genChoice == 4){
    tmpPick1 = rand()%regionVerticelists.size();
  }
  tempList1 =regionVerticelists.at(centers[tmpPick1]);

  //make sure two clusters centers were chosen from the same query pair
  if(genChoice == 3 || genChoice == 4){
    if((tmpPick1+1)%2 == 0) tmpPick2 = tmpPick1-1;
    else tmpPick2 = tmpPick1+1;
  }else{
    do{
      tmpPick2=rand()%numPoints;
    }while(tmpPick1 == tmpPick2);
  }

  tempList2 =regionVerticelists.at(centers[tmpPick2]);

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
  cout << "POI write... ";
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
  cout << "done" << endl;
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
