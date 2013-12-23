/*
 * Copyright 2010-2020 DatasysLab@iit.edu(http://datasys.cs.iit.edu/index.html)
 *      Director: Ioan Raicu(iraicu@cs.iit.edu)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of ZHT library(http://datasys.cs.iit.edu/projects/ZHT/index.html).
 *      Tonglin Li(tli13@hawk.iit.edu) with nickname Tony,
 *      Xiaobing Zhou(xzhou40@hawk.iit.edu) with nickname Xiaobingo,
 *      Ke Wang(kwang22@hawk.iit.edu) with nickname KWang,
 *      Dongfang Zhao(dzhao8@@hawk.iit.edu) with nickname DZhao,
 *      Ioan Raicu(iraicu@cs.iit.edu).
 *
 * StrTokenizer.cpp
 *
 *  Created on: Jul 6, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "StrTokenizer.h"
using std::string;

/** Position start and end so that start is the index of the start
 of the next token and end is the end.
 */
void StrTokenizer::find_next() {
	// Find the first character that is not a delimiter.
	start = the_source.find_first_not_of(the_delim, end);
	// Find the next delimiter.
	end = the_source.find_first_of(the_delim, start);
}

/** Determine if there are more tokens.
 @return true if there are more tokens
 */
bool StrTokenizer::has_more_tokens() {
	return start != string::npos;
}

/** Retrieve the next token.
 @return The next token. If there are no more
 tokens, an empty string is returned
 */
string StrTokenizer::next_token() {
	// Make sure there is a next token
	if (!has_more_tokens())
		return "";
	// Save the next token.
	string token = the_source.substr(start, end - start); //to debug
	// Find the following token.
	find_next();
	// Return the next token.
	return token;
}
