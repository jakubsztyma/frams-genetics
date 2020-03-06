// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _SYNTPATAM_H_
#define _SYNTPATAM_H_

#include "param.h"

/** Creates param + matching temporary object (ParamObject)
	using the supplied ParamEntry'ies as a template.
	This is mainly used for manipulating specialized neuron properties
	(Neuro.d field) in absence of live neuron implementations
	(these are only available in live creatures, but not when operating
	on Models and Genotypes).

	See also: genomanipulation.cpp
	*/
class SyntParam : public Param
{
	void* obj, *def_obj;
	ParamEntry *pe;
	SString* autostring;
public:
	/** @param handle_defaults_when_saving creates a second object holding the default values so Param::save2 can use it for omitting defaults. can be disabled for compatiblity with previous behavior (defaults were ignored in SyntParam)
	 */
	SyntParam(ParamEntry *pe, SString* autostring = 0, bool handle_defaults_when_saving = true);
	SyntParam(const SyntParam& src);
	~SyntParam();
	void setAutoUpdate(SString* autostr) { autostring = autostr; }
	void update(SString *s = 0);
	void revert(SString *s = 0);
};

#endif
