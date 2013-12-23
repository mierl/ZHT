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
 * StrTokenizer.h
 *
 *  Created on: Jul 6, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#ifndef STRTOKENIZER_H_
#define STRTOKENIZER_H_

#include <string>

/** The StrTokenizer class splits a string into a sequence of
 subtrings, called tokens, separated by delimiters.*/
class StrTokenizer {
public:

	/** Construct a StrTokenizer.
	 @param source The string to be split into tokens
	 @param delim The string containing the delimiters. If
	 this parameter is omitted, a space character is
	 assumed
	 */
	StrTokenizer(std::string source, std::string delim = " ") :
			the_source(source), the_delim(delim), start(0), end(0) {
		find_next();
	}

	/** Determine if there are more tokens.
	 @return true if there are more tokens
	 */
	bool has_more_tokens();

	/** Retrieve the next token.
	 @return The next token. If there are no more tokens, an empty
	 string is returned  */
	std::string next_token();

private:
	/** Position start and end so that start is the index of the start
	 of the next token and end is the end.
	 */
	void find_next();

	/** The string to be split into tokens */
	std::string the_source;

	/** The string of delimiters */
	std::string the_delim;

	/** The index of the start of the next token */
	size_t start;

	/** The index of the end of the next token */
	size_t end;
};

#endif /* STRTOKENIZER_H_ */
