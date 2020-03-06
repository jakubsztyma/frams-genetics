// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

/// This is a module included into neuroimpl-fuzzy.
/// It converts string parameters (fuzzy sets and fuzzy rules) into appropriate variables.

#ifndef _neuroimpl_fuzzy_f0_h
#define _neuroimpl_fuzzy_f0_h

#include <ctype.h>
#include <frams/util/sstring.h>

class FuzzyF0String
{
public:
	static int convertStrToSets(const SString& str, double numbers[], int nrOfSets);
	static int countInputsOutputs(const char* str, int ruldef[], int rulesNr);
	static int convertStrToRules(const SString& str, const int ruledef[], int **rules, int setsNr, int rulesNr, int &maxOutputNr);
};

#endif

