/*
 * Takes as input a .cedge file 
 * outputs a .ddsg file
 * 
 * It adds an extra empty line at the bottom of the output file!
 * 
 * execute as:
 * ./convertGr <filename>.cedge
 * 
 */


#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <cstdlib>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>

using namespace std;
typedef boost::unordered_map<int, pair<pair<int,int>, int> > intIntPairIntMap;

/*
 * fileformat 
 * 
 * .cedge: 
 * linenum nid1 nid2 distance
 * e.g.: 0 0 1 1.182663
 * 
 * .ddsg
 * d [num nodes] [num edges] (first line)
 * [nid1] [nid2] [distance] [direction] 
 * direction types:
 * 0 = open in both directions
 * 1 = open only in forward direction (from s to t)
 * 2 = open only in backward direction (from t to s)
 * 3 = closed
*/
int main(int argc, char *argv[]) {
  if(argc !=2){
    cout << "need to give a .cedge file as input: " << argv[0] << " filename.cedge" << endl; 
    return -1;
  }
  
  string fn = argv[1];
  string str;
  std::vector<string> tokens;
  boost::unordered_map<int, std::pair<std::pair<int,int>, int > > entry;
  boost::unordered_map<int, int> nodes; //To identify the number of unique node ids.
  int i=0, numNodes=0, numEdges=0;
  
  ifstream in_data (fn.c_str(), ios::in);
  cout << "FN: " <<fn << endl;
  if(in_data.is_open()){
    while(getline(in_data, str)) {
      boost::algorithm::split(tokens, str, boost::algorithm::is_space());
      entry[i] = make_pair<pair<int,int>, int>(make_pair<int,int>(atoi(tokens[1].c_str()), atoi(tokens[2].c_str())), atoi(tokens[3].c_str()));
      nodes[atoi(tokens[1].c_str())] = 1;
      nodes[atoi(tokens[2].c_str())] = 1;
      i++;
    }
    cout << "Reading file Done!" << endl;
    in_data.close();
  }
  
  numEdges = i;
  numNodes = nodes.size();
  
  
  ///file output .ddsg
  i=0;
  fn.replace ((fn.size()-5), 5, "ddsg"); //change file extention from .cedge to .ddsg
  ofstream resultfile;
  resultfile.open(fn.c_str(), ios::out);
  
  resultfile << "d " << numNodes << " " << numEdges << endl; 

  BOOST_FOREACH(intIntPairIntMap::value_type it, entry){
    resultfile <<  it.second.first.first << " " << it.second.first.second << " " << it.second.second << " 0" << endl;
    i++;
  }
  cout << fn << " written.\n" << i << " lines" << endl;
  resultfile.close();
}


  
