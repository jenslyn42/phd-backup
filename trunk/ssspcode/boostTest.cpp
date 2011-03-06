/****************************************************************************************
 *   Copyright (C) 2011 by Jeppe Rishede 						*
 *   jenslyn42@gmail.com								*
 *											*
 *   All rights reserved.								*
 *											*
 *   Redistribution and use in source and binary forms, with or without 		*
 *   modification, are permitted provided that the following conditions are met:	*
 *   Redistributions of source code must retain the above copyright notice,		*
 *   this list of conditions and the following disclaimer. 				*
 *   Redistributions in binary form must reproduce the above copyright notice,		*
 *   this list of conditions and the following disclaimer in the documentation		*
 *   and/or other materials provided with the distribution. 				*
 *   Neither the name of the <ORGANIZATION> nor the names of its contributors 		*
 *   may be used to endorse or promote products derived from this software 		*
 *   without specific prior written permission						*
 *                                                                         		*
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   		*
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     		*
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 		*
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER		*
 *   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 		*
 *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   		*
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    		*
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 		*
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  		*
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    		*
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          		*
 ***************************************************************************************/
/*
So to compile as a single test:
g++ test_main.cpp test1.cpp test2.cpp -lboost_unit_test_framework -o tests

or to compile an individual test:
g++ test1.cpp -DSTAND_ALONE -lboost_unit_test_framework -o test

to run with better output:
./tests --log_level=test_suite
*/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Unit Tests
#include <boost/test/unit_test.hpp>

// struct Massive
// {
//     int m;
// 
//     Massive() : m(2)
//     {
//         BOOST_TEST_MESSAGE("setup mass");
//     }
// 
//     ~Massive()
//     {
//         BOOST_TEST_MESSAGE("teardown mass");
//     }
// };
// 
// BOOST_FIXTURE_TEST_SUITE(Physics, Massive)
// 
// BOOST_AUTO_TEST_CASE(specialTheory)
// {
//     int e = 32;
//     int c = 4;
// 
//     BOOST_CHECK(e == m * c * c);
// }
// 
// BOOST_AUTO_TEST_CASE(newton2)
// {
//     int f = 10;
//     int a = 5;
// 
//     BOOST_CHECK(f == m * a);
// }
// 
// BOOST_AUTO_TEST_SUITE_END()