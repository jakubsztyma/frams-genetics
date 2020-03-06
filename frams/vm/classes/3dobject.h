// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _3DOBJECT_H_
#define _3DOBJECT_H_

#include <frams/util/3d.h>
#include <frams/util/extvalue.h>

class Pt3D_Ext : public DestrBase
{
public:
	Pt3D p;

	Pt3D_Ext(double x, double y, double z) :p(x, y, z) {}
	Pt3D_Ext() :p(0, 0, 0) {}
	Pt3D_Ext(const Pt3D &i) :p(i) {}
#define STATRICKCLASS Pt3D_Ext
	PARAMPROCDEF(p_new);
	PARAMPROCDEF(p_newFromVector);
	PARAMPROCDEF(p_clone);
	PARAMGETDEF(length);
	PARAMPROCDEF(p_addvec);
	PARAMPROCDEF(p_subvec);
	PARAMPROCDEF(p_scale);
	PARAMPROCDEF(p_rotate);
	PARAMPROCDEF(p_revrotate);
	PARAMPROCDEF(p_normalize);
	PARAMGETDEF(toString);
	PARAMGETDEF(toVector);
	PARAMPROCDEF(p_set);
	PARAMPROCDEF(p_set3);
	PARAMPROCDEF(p_get);
#undef STATRICKCLASS
	SString toString() const;
	static ParamInterface* getInterface();
	static ExtObject makeStaticObject(Pt3D* p);
	static ExtObject makeDynamicObject(Pt3D_Ext* p);
	static ExtObject makeDynamicObject(const Pt3D& p);
	static Pt3D_Ext* fromObject(const ExtValue& v, bool warn = true);
	static Param& getStaticParam();
	static ParamEntry* getStaticParamtab();
};

class Orient_Ext : public DestrBase
{
public:
	Orient o;

	Orient_Ext() :o(Orient_1) {}
	Orient_Ext(const Orient& other) :o(other) {}
#define STATRICKCLASS Orient_Ext
	PARAMPROCDEF(p_new);
	PARAMPROCDEF(p_newFromVector);
	PARAMPROCDEF(p_clone);
	PARAMGETDEF(toString);
	PARAMGETDEF(toVector);
	PARAMGETDEF(x);
	PARAMGETDEF(y);
	PARAMGETDEF(z);
	PARAMPROCDEF(p_rotate3);
	PARAMPROCDEF(p_rotate);
	PARAMPROCDEF(p_revrotate);
	PARAMPROCDEF(p_lookat);
	PARAMPROCDEF(p_normalize);
	PARAMPROCDEF(p_between2);
	PARAMPROCDEF(p_betweenOV);
	PARAMPROCDEF(p_set);
	PARAMPROCDEF(p_reset);
	PARAMPROCDEF(p_localToWorld);
	PARAMPROCDEF(p_worldToLocal);
	PARAMGETDEF(angles);
#undef STATRICKCLASS

	static ParamInterface* getInterface();
	static ExtObject makeStaticObject(Orient* p);
	static ExtObject makeDynamicObject(Orient_Ext* p);
	static Orient_Ext* fromObject(const ExtValue& v);
	static Param& getStaticParam();
	static ParamEntry* getStaticParamtab();
};

#endif
