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


// #ifndef SUPPORT_H
// #define SUPPORT_H
// 
// /**
// 	@author Jeppe Rishede <jenslyn42@gmail.com>
// */
// 
// #include <vector>
// #include "CacheItem.h"
// #include "boost/foreach.hpp"
// 
// typedef vector<CacheItem> cacheVector;
// 
// void countDuplicates (vector < CacheItem > cItems);
// void writeOutCacheItems (vector < CacheItem > cItems);
// 
// 
// void countDuplicates(cacheVector cItems)
// {
// vector<int> iItem;
// int kMin,kMax;
// 
// 	for(int i=0; i< cItems.size(); i++) {
// 		iItem = cItems.at(i).item;
// 		for(int k=i+1; k< cItems.size(); k++) {
// 			kMin = cItems.at(k).item.at(0);
// 			kMax = cItems.at(k).item.at((cItems.at(k).size)-1);			
// // 			if(
// 		}
// 	}
// }
// 
// void writeOutCacheItems(cacheVector cItems)
// {
// 	BOOST_FOREACH (CacheItem e, cItems){ 
// 		cout << e.item.size() << " ";
// 		BOOST_FOREACH (int node, e.item){
// 			cout << node << " ";
// 		}
// 		cout << endl;
// 	}
// }
// 
// 
// #endif

