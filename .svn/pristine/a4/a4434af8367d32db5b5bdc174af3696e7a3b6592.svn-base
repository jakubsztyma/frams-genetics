/*
 *  conv_f8_utils.h
 *  L-systemToF1
 *
 *  Created by Maciej Wajcht on 08-09-21.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CONV_F8_UTILS_H_
#define _CONV_F8_UTILS_H_

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <stack>
#include <vector>
#include "sstring.h"

using namespace std;

class BadConversion : public std::runtime_error {
public:
	BadConversion(const std::string &s) : std::runtime_error(s)	{ }
};

class ParseExpressionException : public std::runtime_error {
public:
	ParseExpressionException(const std::string &s) : std::runtime_error(s) { }
};

bool containsChars(const SString s, char c[]);
bool containsChar(std::vector<char> tab, char c);
int reverseFindInSString(const SString s, char c, int index = -1);
SString trimSString(const SString s);
SString trimSString(const SString s);
double parseDouble(const SString &s);
int parseInt(const SString &s);
SString stringToSString(string str);
string sstringToString(SString sstr);
SString convertReversePolishNotationToNatural(const SString &s);
double parseExpression(const SString &s);

#endif
