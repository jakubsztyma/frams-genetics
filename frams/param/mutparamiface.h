// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _MUTPARAM_H_
#define _MUTPARAM_H_

#include <frams/util/callbacks.h>
#include "param.h"

class MutableParamInterface : public virtual ParamInterface
{
public:
	Callback onadd;
	Callback ondelete;
	Callback onchange;
	Callback ongroupadd;
	Callback ongroupdelete;
	Callback ongroupchange;
	Callback onactivate;
};

#endif
