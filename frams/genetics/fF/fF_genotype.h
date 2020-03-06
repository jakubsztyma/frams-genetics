// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2017  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FF_GENOTYPE_H_
#define _FF_GENOTYPE_H_

#include <stdio.h>
#include <frams/param/param.h>

//Growth parameters; see http://www.framsticks.com/foraminifera
struct fF_growth_params
{
	int number_of_chambers;
	double radius0x, radius0y, radius0z; //radius of 0th (initial) chamber
	double scalex, scaley, scalez; //(cumulative) scaling of consecutive chambers
	double translation;
	double angle1, angle2;
#define fF_PROPS_TO_MUTATE {0,4,5,6,7,8,9} //indexes of properties from paramtab; keep synchronized with fF_genotype.cpp

	static ParamEntry paramtab[];
	Param param;

	fF_growth_params() :param(paramtab, this)
	{
		reset();
	}

	void reset()
	{
		param.setDefault();
	}

	bool load(const char* serialized)
	{
		SString s = serialized;
		ParamInterface::LoadOptions opts;
		return ((param.load(ParamInterface::FormatSingleLine, s, &opts) == param.getPropCount()) && (opts.offset == s.len()));
	}

	string save()
	{
		SString tmp;
		param.saveSingleLine(tmp, NULL/*object containing default values for comparison*/, false/*add CR*/, false/*force field names*/);
		return string(tmp.c_str());
	}
};

#endif
