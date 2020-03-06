// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _GENOOBJ_H_
#define _GENOOBJ_H_

#include <frams/genetics/geno.h>

extern ParamEntry geno_paramtab[];

class GenoObj : public Geno
{
public:
#define STATRICKCLASS GenoObj
	PARAMGETDEF(isvalid);
	PARAMSETDEF(isvalid);
	PARAMGETDEF(genotype);
	PARAMGETDEF(name);
	PARAMSETDEF(name);
	PARAMGETDEF(autoname);
	PARAMGETDEF(info);
	PARAMSETDEF(info);
	PARAMGETDEF(format);
	PARAMGETDEF(string);
	PARAMPROCDEF(p_getconvert);
	PARAMPROCDEF(p_getconvert_ch);
	PARAMGETDEF(f0genotype);
	PARAMPROCDEF(p_new);
	PARAMPROCDEF(p_newfrom);
	PARAMPROCDEF(p_newfromstring);
	PARAMPROCDEF(p_newfromvector);
	PARAMGETDEF(toVector);
#undef STATRICKCLASS

	static ExtObject makeStaticObject(Geno* g);
	static ExtObject makeDynamicObject(Geno* g);
	static ExtObject makeDynamicObjectAndDecRef(Geno* g);
	static Geno* fromObject(const ExtValue& v, bool warn = true);
	static ParamInterface* getInterface();
	static char formatFromExtValue(ExtValue& v);

	static Param& getStaticParam();
	static Param& getDynamicParam();
};

#endif
