// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2017  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "fF_genotype.h"

#define FIELDSTRUCT fF_growth_params
ParamEntry fF_growth_params::paramtab[] =
{
	//on changes synchronize with fF_PROPS_TO_MUTATE from .h

	{ "fF", 1, 10, "fF" },
	{ "n", 0, PARAM_CANOMITNAME, "number of chambers", "d 1 15 6", FIELD(number_of_chambers), },

	{ "rad0x", 0, PARAM_CANOMITNAME, "radius0 x", "f 0.01 2.0 1.0", FIELD(radius0x), },
	{ "rad0y", 0, PARAM_CANOMITNAME, "radius0 y", "f 0.01 2.0 1.0", FIELD(radius0y), },
	{ "rad0z", 0, PARAM_CANOMITNAME, "radius0 z", "f 0.01 2.0 1.0", FIELD(radius0z), },

	{ "sx", 0, PARAM_CANOMITNAME, "scale x", "f 1.0 1.1 1.05", FIELD(scalex), },
	{ "sy", 0, PARAM_CANOMITNAME, "scale y", "f 1.0 1.1 1.05", FIELD(scaley), },
	{ "sz", 0, PARAM_CANOMITNAME, "scale z", "f 1.0 1.1 1.05", FIELD(scalez), },

	{ "tr", 0, PARAM_CANOMITNAME, "translation factor", "f -1 1 0", FIELD(translation), },
	{ "a1", 0, PARAM_CANOMITNAME, "angle 1", "f -3.1415926 3.1415926 0", FIELD(angle1), },
	{ "a2", 0, PARAM_CANOMITNAME, "angle 2", "f -3.1415926 3.1415926 0", FIELD(angle2), },

	{ 0, 0, 0, },
};
#undef FIELDSTRUCT
