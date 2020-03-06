// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <common/nonstd.h>
#include "syntparam.h"
#include "paramobj.h"
#include <math.h>

SyntParam::SyntParam(ParamEntry *init_pe, SString* autostr, bool handle_defaults_when_saving)
	:autostring(autostr)
{
	Param::setParamTab(init_pe);
	pe = ParamObject::makeParamTab(this);
	Param::setParamTab(pe);
	if (handle_defaults_when_saving)
	{
		def_obj = ParamObject::makeObject(pe);
		Param::select(def_obj);
		Param::setDefault();
	}
	else
		def_obj = NULL;
	obj = ParamObject::makeObject(pe);
	Param::select(obj);
	Param::setDefault();
	revert();
}

SyntParam::SyntParam(const SyntParam& src)
	:Param(), autostring(src.autostring)
{
	Param::setParamTab(src.pe);
	pe = ParamObject::makeParamTab(this);
	Param::setParamTab(pe);
	obj = ParamObject::dupObject(src.obj);
	def_obj = src.def_obj ? ParamObject::dupObject(src.def_obj) : NULL;
	Param::select(obj);
}

SyntParam::~SyntParam()
{
	update();
	ParamObject::freeParamTab(pe);
	ParamObject::freeObject(obj);
	ParamObject::freeObject(def_obj);
}

void SyntParam::update(SString *s)
{
	if (!s) s = autostring;
	if (s) { *s = ""; Param::saveSingleLine(*s, def_obj, 0); }
}

void SyntParam::revert(SString *s)
{
	if (!s) s = autostring;
	if (s)	{
		Param::load(ParamInterface::FormatSingleLine, *s);
	}
}
