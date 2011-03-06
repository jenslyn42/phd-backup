/********************************************************************************
 *   Copyright (C) 2011 by Jeppe Rishede 					*
 *   jenslyn42@gmail.com							*
 *										*
 *   All rights reserved.							*
 *										*
 *   Redistribution and use in source and binary forms, with or without 	*
 *   modification, are permitted provided that the following conditions are met:*
 *   Redistributions of source code must retain the above copyright notice,	*
 *   this list of conditions and the following disclaimer. 			*
 *   Redistributions in binary form must reproduce the above copyright notice,	*
 *   this list of conditions and the following disclaimer in the documentation	*
 *   and/or other materials provided with the distribution. 			*
 *   Neither the name of the <ORGANIZATION> nor the names of its contributors 	*
 *   may be used to endorse or promote products derived from this software 	*
 *   without specific prior written permission					*
 *                                                                         	*
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   	*
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     	*
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 	*
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER	*
 *   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 	*
 *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   	*
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    	*
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 	*
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  	*
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    	*
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          	*
 ********************************************************************************/

#include <iostream>
#include <cstdlib>
#include <list>
#include "boost/math/common_factor.hpp"
#include "boost/unordered_map.hpp"
#include <boost/foreach.hpp>

using namespace std;


int main(int argc, char *argv[])
{
//int numQueries, int cacheSize, bool skewedData, bool optimalSubstructure, bool useNodeScore, bool useHitScore,
//bool useGaussian, double sigma, int activeTests, String testName
typedef boost::unordered_map<std::string, int> map;

  cout << "Hello, world!" << endl;

  cout << "gcd:" << boost::math::gcd(4,12) << endl;
  cout << "lcm:" << boost::math::lcm(2,10) << endl;


map x;
x["one"] = 1;
x["two"] = 2;
x["three"] = 3;

cout << x["one"] << endl;

  BOOST_FOREACH(map::value_type ii, x) 
{
    cout<<ii.first<<","<<ii.second<<"\n";
}

    std::string hello( "Hello, world!" );
    
BOOST_FOREACH( char ch, hello )
{
    std::cout << ch << " ";
}

list<int> list_int;
list_int.push_front(200);
list_int.push_front(2000);
list_int.push_front(400);
BOOST_FOREACH( int i, list_int )
{
   cout << i << " ";
}


short array_short[] = {1,2,3};
BOOST_FOREACH( int i, array_short )
{
    cout << i << " ";
}



assert(x.at("one") == 1);
assert(x.find("missing") == x.end());



cin.get();



  return EXIT_SUCCESS;
};