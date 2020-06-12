// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 2019-2020  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FS_CONV_H_
#define _FS_CONV_H_

#include "fS_general.h"
#include "frams/util/multimap.h"

/**
 * Genotype converter from fS to f0.
 */
class GenoConv_fS0 : public GenoConverter
{
public:
	GenoConv_fS0() : GenoConverter()
	{
		name = "Solid encoding";

		in_format = 'S';
		out_format = '0';
		mapsupport = 1;
	}

	/// Return empty string if can not convert
	SString convert(SString &i, MultiMap *map, bool using_checkpoints);

	~GenoConv_fS0()
	{};
};

#endif
