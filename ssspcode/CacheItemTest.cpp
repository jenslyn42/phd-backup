/****************************************************************************************
*   Copyright (C) 2011 by Jeppe Rishede 						*
*   jenslyn42@gmail.com									*
*											*
*   All rights reserved.								*
*											*
*   Redistribution and use in source and binary forms, with or without 			*
*   modification, are permitted provided that the following conditions are met:		*
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

#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE CacheItemModule
#endif
#include <boost/test/unit_test.hpp>
#include "CacheItem.cpp"
#include <vector>

BOOST_AUTO_TEST_SUITE(ClassCacheItem)

BOOST_AUTO_TEST_CASE(shortConstructor)
{
	int key = 2;
	int myints[] = {16,2,77,29};
	std::vector<int> item (myints, myints + sizeof(myints) / sizeof(int) );
	CacheItem ci (key, item);

	BOOST_CHECK(ci.key() == key);
	BOOST_CHECK(ci.item.at(2) == 77);
	BOOST_CHECK(ci.item.size() == ci.size);
	BOOST_CHECK(ci.item.size() == 4);
	BOOST_CHECK(ci.s == -1);
	BOOST_CHECK(ci.t == -1);
}


BOOST_AUTO_TEST_CASE(longConstructor)
{
	int key = 2;
	int myints[] = {16,2,77,29};
	std::vector<int> item (myints, myints + sizeof(myints) / sizeof(int) );
	int queryStartNode = 4;
	int queryTargetNode =55;
	CacheItem ci (key, item, queryStartNode, queryTargetNode);

	BOOST_CHECK(ci.key() == key);
	BOOST_CHECK(ci.item.at(2) == 77);
	BOOST_CHECK(ci.item.size() == ci.size);
	BOOST_CHECK(ci.item.size() == 4);
	BOOST_CHECK(ci.s == queryStartNode);
	BOOST_CHECK(ci.t == queryTargetNode);
}

BOOST_AUTO_TEST_CASE(updateKey)
{
	int key = 2;
	int myints[] = {16,2,77,29};
	std::vector<int> item (myints, myints + sizeof(myints) / sizeof(int) );
	CacheItem ci (key, item);

	ci.updateKey(6);
	BOOST_CHECK(ci.key() != key);
	BOOST_CHECK(ci.key() == 6);
	BOOST_CHECK(ci.id == key);
}

BOOST_AUTO_TEST_SUITE_END()
