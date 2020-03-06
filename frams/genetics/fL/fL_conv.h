// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FL_CONV_
#define _FL_CONV_

#include <frams/genetics/genoconv.h>

class GenoConv_fL0 : public GenoConverter
{
public:
	GenoConv_fL0() : GenoConverter()
	{
		name = "L-System encoding";

		in_format = 'L';
		out_format = '0';
		mapsupport = 1;
	}
	SString convert(SString &i, MultiMap *map, bool using_checkpoints);
	~GenoConv_fL0() {}
};

#endif // _FL_CONV_
