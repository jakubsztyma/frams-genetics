// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _DEFGENOCONV_H_
#define _DEFGENOCONV_H_

#include "genoconv.h"

/// This GenoConvManager subclass allows you to add all "standard" converters in one go
class DefaultGenoConvManager : public GenoConvManager
{
public:
	void addDefaultConverters();///< add all converters configured in gen-config file
};

#endif
