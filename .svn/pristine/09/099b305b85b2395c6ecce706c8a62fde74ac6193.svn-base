// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FH_CONV_H_
#define _FH_CONV_H_

#include <frams/genetics/genoconv.h>
#include "fH_general.h"

class GenoConv_fH0 : public GenoConverter
{
public:
	GenoConv_fH0() :GenoConverter()
	{
		name = "Similarity encoding";

		in_format = 'H';
		out_format = '0';
		mapsupport = 1;
	}
	SString convert(SString &i, MultiMap *map, bool using_checkpoints);
	~GenoConv_fH0() {};
};

#endif //_FH_CONV_H_
