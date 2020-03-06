/*
 *  conv_f8_utils.cpp
 *  L-systemToF1
 *
 *  Created by Maciej Wajcht on 08-09-21.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "conv_f8_utils.h"

using namespace std;

bool containsChars(const SString s, char c[]) {
const char* str=(const char*)s;
	for (int i = 0; i < s.len(); i++) {
		for (int j = 0; j < strlen(c); j++) {
			if (str[i] == c[j]) {
				return true;
			}
		}
	}
	return false;
}

bool containsChar(std::vector<char> tab, char c) {
	for (int i = 0; i < tab.size(); i++) {
		if (tab[i] == c) {
			return true;
		}
	}
	return false;
}

int reverseFindInSString(const SString s, char c, int index) {
	if (index > 0) {
		index = (s.len() - 1 > index) ? index : s.len() - 1; //min(index, s.len() - 1)
	} else {
		index = s.len() - 1;
	}
	
const char* str=(const char*)s;
	for (int i = index; i >= 0; i--) {
		if (str[i] == c) {
			return i;
		}
	}
	return -1;
}

SString trimSString(const SString s) {
	SString result = "";
	for (int i = 0; i < s.len(); i++) {
		char c = s.charAt(i);
		if (c != ' ' && c != '\t') {
			result += c;
		}
	}
	return result;
}

double parseDouble(const SString &s) {
	const char* str = s;
	std::istringstream i(str);
	double x;
	if (!(i >> x))
		throw BadConversion("parseDouble(\"" + string(str) + "\")");
	return x;
}

int parseInt(const SString &s) {
	const char* str = s;
	std::istringstream i(str);
	int x;
	if (!(i >> x))
		throw BadConversion("parseInt(\"" + string(str) + "\")");
	return x;
}

SString stringToSString(string str) {
	return SString(str.c_str());
}

string sstringToString(SString sstr) {
	const char* tmp = sstr;
	return string(tmp);
}

SString convertReversePolishNotationToNatural(const SString &s) {
	SString result = "";
	SString tok = "";
	int pos = 0;
	stack<SString> stck;
	
	while (s.getNextToken(pos, tok, ';')) {
		if (tok.len() == 1  && (tok[0] == '+' || tok[0] == '-' || tok[0] == '*' || tok[0] == '/')) {
			SString s1, s2;
			if (!stck.empty()) {
				s1 = stck.top();
				stck.pop();
			} else {
				throw ParseExpressionException("Not enough elements on stack: " + sstringToString(s));
			}
			if (!stck.empty()) {
				s2 = stck.top();
				stck.pop();
			} else {
				throw ParseExpressionException("Not enough elements on stack: " + sstringToString(s));
			}
			switch (tok[0]) {
				case '+': stck.push(s2 + "+" + s1); break;
				case '-': stck.push(s2 + "-" + s1); break;
				case '*': stck.push(s2 + "*" + s1); break;
				case '/': stck.push(s2 + "/" + s1); break;
			}
		} else if (tok.len() > 0) {
			stck.push(tok);
		}
	}
	
	result = stck.top();	
	
	return result;
}

double parseExpression(const SString &s) {
	double result = 0;
	SString tok = "";
	int pos = 0;
	stack<double> stck;
	
	while (s.getNextToken(pos, tok, ';')) {
		if (tok.len() == 1  && (tok[0] == '+' || tok[0] == '-' || tok[0] == '*' || tok[0] == '/')) {
			double n1, n2;
			if (!stck.empty()) {
				n1 = stck.top();
				stck.pop();
			} else {
				throw ParseExpressionException("Not enough elements on stack: " + sstringToString(s));
			}
			if (!stck.empty()) {
				n2 = stck.top();
				stck.pop();
			} else {
				throw ParseExpressionException("Not enough elements on stack: " + sstringToString(s));
			}
			switch (tok[0]) {
				case '+': stck.push(n2 + n1); break;
				case '-': stck.push(n2 - n1); break;
				case '*': stck.push(n2 * n1); break;
				case '/': 
					if (n1 == 0) {
						throw ParseExpressionException("Division by zero");
					}
					stck.push(n2 / n1); 
					break;
			}
		} else if (tok.len() > 0) {
			stck.push(parseDouble(tok));
		}/* else {
		 throw ParseExpressionException("Unrecognized element in input string '" + sstringToString(tmp) + "'");
		 }*/
	}
	
	result = stck.top();
	
	return result;
}
