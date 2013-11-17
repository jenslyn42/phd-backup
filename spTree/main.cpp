#include <iostream>
#include "spTreeRG.h"

using namespace std;

int main(int argc, char *argv[]) {

if(argc != 3){
  cout << "Wrong usage!\nUsage: " << argv[0] << "<mapname> <poi name>" << endl;
  exit(-1);
}
string fn = argv[1];
string poifn = argv[2];

//   spTreeRG::mapObject("aalborgNEW")->writeSPtree(spTreeRG::mapObject("aalborgNEW")->readPOIlist());
    spTreeRG::mapObject(fn)->writeSPtree(spTreeRG::mapObject(fn)->readPOIlist(poifn));

    return 0;
}
