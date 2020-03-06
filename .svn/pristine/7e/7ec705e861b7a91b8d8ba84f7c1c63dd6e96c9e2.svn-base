// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _Fn_CONV_H_
#define _Fn_CONV_H_

#include <frams/genetics/genoconv.h>

// The fn->f0 converter
class GenoConv_fn0 : public GenoConverter
{
public:
	GenoConv_fn0();
	//~GenoConv_fn0();
	//implementation of the GenoConverter method
	SString convert(SString &in, MultiMap *map, bool using_checkpoints);

	static vector<double> stringToVector(const char *input); //returns empty vector on error
	static string vectorToString(const vector<double> vec);
};

#endif
