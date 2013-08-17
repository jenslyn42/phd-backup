/*
 * Takes as input a .co file 
 * outputs a .cnode file
 * 
 * It adds an extra empty line at the bottom of the output file!
 * 
 * execute as:
 * ./convertGr <filename>.co
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
typedef boost::unordered_map<int, pair<int,int> > intPairMap;

/*
 * fileformat 
 * .cnode: nid x y
 * e.g.: 0 0 1
 * .co: 
 * (first non-comment line) p aux sp co [num coordinate lines]
 * c [comment]
 * v [id x y]
*/
int main(int argc, char *argv[]) {
  if(argc !=2){
    cout << "Need to give a .co file as input: " << argv[0] << " filename.co" << endl; 
    return -1;
  }
  
  string fn = argv[1];
  string str;
  std::vector<string> tokens;
  boost::unordered_map<int, pair<int,int> > entry;
  
  ifstream in_data (fn.c_str(), ios::in);
  cout << "FN: " <<fn << endl;
  if(in_data.is_open()){
    while(getline(in_data, str))
    {
      boost::algorithm::split(tokens, str, boost::algorithm::is_space());
      if (tokens[0].compare("v") == 0)
	entry[atoi(tokens[1].c_str())] =  std::make_pair<int,int>(atoi(tokens[2].c_str()), atoi(tokens[3].c_str()));
//       else if(tokens[0].compare("c") == 0)
	//handle comments
//       else if(tokens[0].compare("p") == 0)
	//use info about number of nodes and edges
    }
    cout << "Reading file Done!" << endl;
    in_data.close();
  }
  
  ///file output
  int i=0;
  fn.replace ((fn.size()-2), 5, "cnode"); //change file extention from .gr to .cedge
  ofstream resultfile;
  resultfile.open(fn.c_str(), ios::out);
  
  BOOST_FOREACH(intPairMap::value_type it, entry){
    resultfile << it.first << " " << it.second.first << " " << it.second.second << endl;
      i++;
  }
  cout << fn << " written.\n" << i << " lines" << endl;
  resultfile.close();

}