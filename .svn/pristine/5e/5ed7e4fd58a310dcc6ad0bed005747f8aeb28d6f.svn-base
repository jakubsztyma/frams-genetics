// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <frams/util/3d.h>
#include "3dobject.h"
#include <frams/param/param.h>
#include "collectionobj.h"

ParamEntry* Pt3D_Ext::getStaticParamtab()
{
#define FIELDSTRUCT Pt3D_Ext
	static ParamEntry paramtab[] =
	{
		{ "XYZ", 1, 18, "XYZ", "3D vector" },

		{ "x", 0, PARAM_NOSTATIC, "x", "f", FIELD(p.x), },
		{ "y", 0, PARAM_NOSTATIC, "y", "f", FIELD(p.y), },
		{ "z", 0, PARAM_NOSTATIC, "z", "f", FIELD(p.z), },
		{ "new", 0, 0, "create new XYZ object", "p oXYZ(f x,f y,f z)", PROCEDURE(p_new), "3D vectors objects can be also created using the (x,y,z) notation, i.e. var v=(1,2,3) is the same as var v=XYZ.new(1,2,3);", },
		{ "newFromVector", 0, 0, "create new XYZ object", "p oXYZ(oVector)", PROCEDURE(p_newFromVector), "used for deserialization" },
		{ "clone", 0, PARAM_NOSTATIC, "create new XYZ object copying the coordinates", "p oXYZ()", PROCEDURE(p_clone), "Note: copying object references does not create new objects. Use clone() if a new object is needed.\n\nExample:\nvar o1=(1,2,3), o2=o1, o3=o1.clone();\no1.y=9999;\n//o2 is now (1,9999,3) but o3 is still (1,2,3)", },
		{ "set", 0, PARAM_NOSTATIC, "set (copy) coordinates from another XYZ object", "p(oXYZ)", PROCEDURE(p_set), },
		{ "set3", 0, PARAM_NOSTATIC, "set individual 3 coordinates", "p(f x,f y,f z)", PROCEDURE(p_set3), },
		{ "add", 0, PARAM_NOSTATIC, "add", "p(oXYZ)", PROCEDURE(p_addvec), "Note: it does not return a new object, just modifies the existing one" },
		{ "sub", 0, PARAM_NOSTATIC, "subtract", "p(oXYZ)", PROCEDURE(p_subvec), "Note: it does not return a new object, just modifies the existing one" },
		{ "scale", 0, PARAM_NOSTATIC, "multiply by scalar", "p(f)", PROCEDURE(p_scale), },
		{ "length", 0, PARAM_READONLY | PARAM_NOSTATIC, "length", "f", GETONLY(length), },
		{ "normalize", 0, PARAM_NOSTATIC, "normalize", "p()", PROCEDURE(p_normalize), "scales the vector length to 1.0" },
		{ "toString", 0, PARAM_READONLY | PARAM_NOSTATIC, "textual form", "s", GETONLY(toString), },
		{ "toVector", 0, PARAM_READONLY | PARAM_NOSTATIC, "vector of [x,y,z]", "oVector", GETONLY(toVector), },
		{ "rotate", 0, PARAM_NOSTATIC, "rotate using Orient object", "p(oOrient)", PROCEDURE(p_rotate), },
		{ "revRotate", 0, PARAM_NOSTATIC, "reverse rotate using Orient object", "p(oOrient)", PROCEDURE(p_revrotate), },
		{ "get", 0, PARAM_NOSTATIC, "get one of coordinates", "p f(d index)", PROCEDURE(p_get), "this function makes the XYZ objects \"indexable\" (so you can use [] for accessing subsequent fields, like in Vector)", },
		{ 0, 0, 0, },
	};
#undef FIELDSTRUCT
	return paramtab;
}

void Pt3D_Ext::p_new(ExtValue *args, ExtValue *ret)
{
	*ret = makeDynamicObject(new Pt3D_Ext(args[2].getDouble(), args[1].getDouble(), args[0].getDouble()));
}

static double doubleFromVec(VectorObject *vec, int i)
{
	if (i >= vec->data.size()) return 0;
	ExtValue *v = (ExtValue*)vec->data.get(i);
	if (v)
		return v->getDouble();
	return 0;
}

static Pt3D pt3DFromVec(VectorObject* v, int offset = 0)
{
	return Pt3D(doubleFromVec(v, offset), doubleFromVec(v, offset + 1), doubleFromVec(v, offset + 2));
}

void Pt3D_Ext::p_newFromVector(ExtValue *args, ExtValue *ret)
{
	VectorObject *vec = VectorObject::fromObject(args->getObject());
	if (vec)
		*ret = makeDynamicObject(new Pt3D_Ext(pt3DFromVec(vec)));
	else
		ret->setEmpty();
}

void Pt3D_Ext::p_clone(ExtValue *args, ExtValue *ret)
{
	*ret = makeDynamicObject(new Pt3D_Ext(p.x, p.y, p.z));
}

void Pt3D_Ext::p_set3(ExtValue *args, ExtValue *ret)
{
	p.x = args[2].getDouble();
	p.y = args[1].getDouble();
	p.z = args[0].getDouble();
	ret->setEmpty();
}

void Pt3D_Ext::p_set(ExtValue *args, ExtValue *ret)
{
	Pt3D_Ext *other = fromObject(args[0]);
	if (other)
		p = other->p;
	ret->setEmpty();
}

void Pt3D_Ext::get_length(ExtValue *ret)
{
	ret->setDouble(p.length());
}

SString Pt3D_Ext::toString() const
{
	SString s = "(";
	s += SString::valueOf(p.x);
	s += ",";
	s += SString::valueOf(p.y);
	s += ",";
	s += SString::valueOf(p.z);
	s += ")";
	return s;
}

void Pt3D_Ext::get_toString(ExtValue *ret)
{
	ret->setString(toString());
}

static void add3Coords(VectorObject* vec, const Pt3D& p)
{
	vec->data += new ExtValue(p.x);
	vec->data += new ExtValue(p.y);
	vec->data += new ExtValue(p.z);
}

void Pt3D_Ext::get_toVector(ExtValue *ret)
{
	VectorObject *vec = new VectorObject;
	add3Coords(vec, p);
	ret->setObject(ExtObject(&VectorObject::par, vec));
}

void Pt3D_Ext::p_addvec(ExtValue *args, ExtValue *ret)
{
	Pt3D_Ext *other = fromObject(args[0]);
	if (other)
		p += other->p;
	ret->setEmpty();
}

void Pt3D_Ext::p_subvec(ExtValue *args, ExtValue *ret)
{
	Pt3D_Ext *other = fromObject(args[0]);
	if (other)
		p -= other->p;
	ret->setEmpty();
}

void Pt3D_Ext::p_scale(ExtValue *args, ExtValue *ret)
{
	double d = args[0].getDouble();
	p.x *= d; p.y *= d; p.z *= d;
	ret->setEmpty();
}

void Pt3D_Ext::p_normalize(ExtValue *args, ExtValue *ret)
{
	p.normalize();
	ret->setEmpty();
}

void Pt3D_Ext::p_rotate(ExtValue *args, ExtValue *ret)
{
	Orient_Ext *o = Orient_Ext::fromObject(args[0]);
	if (o)
	{
		Pt3D tmp = p;
		o->o.transform(p, tmp);
	}
	ret->setEmpty();
}

void Pt3D_Ext::p_revrotate(ExtValue *args, ExtValue *ret)
{
	Orient_Ext *o = Orient_Ext::fromObject(args[0]);
	if (o)
	{
		Pt3D tmp = p;
		o->o.revTransform(p, tmp);
	}
	ret->setEmpty();
}

void Pt3D_Ext::p_get(ExtValue *args, ExtValue *ret)
{
	int index = args->getInt();
	if ((index < 0) || (index > 2))
		ret->setEmpty();
	else
		ret->setDouble((&p.x)[index]);
}

Param& Pt3D_Ext::getStaticParam()
{
#ifdef __CODEGUARD__
	static Pt3D_Ext static_pt3dobj;
	static Param static_pt3dparam(getStaticParamtab(), &static_pt3dobj);
#else
	static Param static_pt3dparam(getStaticParamtab());
#endif
	return static_pt3dparam;
}

Pt3D_Ext* Pt3D_Ext::fromObject(const ExtValue& v, bool warn)
{
	return (Pt3D_Ext*)v.getObjectTarget(getStaticParam().getName(), warn);
}

ParamInterface* Pt3D_Ext::getInterface() { return &getStaticParam(); }

ExtObject Pt3D_Ext::makeStaticObject(Pt3D* p)
{
	return ExtObject(&getStaticParam(), ((char*)p) + (((char*)&p->x) - ((char*)&((Pt3D_Ext*)p)->p.x)));
}

ExtObject Pt3D_Ext::makeDynamicObject(Pt3D_Ext* p)
{
	return ExtObject(&getStaticParam(), p);
}

ExtObject Pt3D_Ext::makeDynamicObject(const Pt3D& p)
{
	Pt3D_Ext *pe = new Pt3D_Ext(p);
	return ExtObject(&getStaticParam(), pe);
}

//////////////////////////////////////

ParamEntry* Orient_Ext::getStaticParamtab()
{
#define FIELDSTRUCT Orient_Ext
	static ParamEntry paramtab[] =
	{
		{ "Orient", 1, 29, "Orient", "3D orientation, stored as 3x3 matrix." },

		{ "xx", 1, PARAM_NOSTATIC, "orientation.x.x", "f", FIELD(o.x.x), },
		{ "xy", 1, PARAM_NOSTATIC, "orientation.x.y", "f", FIELD(o.x.y), },
		{ "xz", 1, PARAM_NOSTATIC, "orientation.x.z", "f", FIELD(o.x.z), },
		{ "yx", 1, PARAM_NOSTATIC, "orientation.y.x", "f", FIELD(o.y.x), },
		{ "yy", 1, PARAM_NOSTATIC, "orientation.y.y", "f", FIELD(o.y.y), },
		{ "yz", 1, PARAM_NOSTATIC, "orientation.y.z", "f", FIELD(o.y.z), },
		{ "zx", 1, PARAM_NOSTATIC, "orientation.z.x", "f", FIELD(o.z.x), },
		{ "zy", 1, PARAM_NOSTATIC, "orientation.z.y", "f", FIELD(o.z.y), },
		{ "zz", 1, PARAM_NOSTATIC, "orientation.z.z", "f", FIELD(o.z.z), },

		{ "x", 0, PARAM_NOSTATIC | PARAM_READONLY, "x vector", "oXYZ", GETONLY(x), },
		{ "y", 0, PARAM_NOSTATIC | PARAM_READONLY, "y vector", "oXYZ", GETONLY(y), },
		{ "z", 0, PARAM_NOSTATIC | PARAM_READONLY, "z vector", "oXYZ", GETONLY(z), },

		{ "new", 0, 0, "create new Orient object", "p oOrient()", PROCEDURE(p_new), },
		{ "newFromVector", 0, 0, "create new Orient object", "p oOrient(oVector)", PROCEDURE(p_newFromVector), },
		{ "toVector", 0, PARAM_READONLY | PARAM_NOSTATIC, "vector representation", "oVector", GETONLY(toVector), "for serialization" },
		{ "clone", 0, PARAM_NOSTATIC, "create new Orient object", "p oOrient()", PROCEDURE(p_clone), },
		{ "set", 0, PARAM_NOSTATIC, "copy from another Orient object", "p(oOrient)", PROCEDURE(p_set), },
		{ "reset", 0, PARAM_NOSTATIC, "set identity matrix", "p()", PROCEDURE(p_reset), },
		{ "rotate3", 0, PARAM_NOSTATIC, "rotate around 3 axes", "p(f x,f y,f z)", PROCEDURE(p_rotate3), },
		{ "rotate", 0, PARAM_NOSTATIC, "rotate using Orient object", "p(oOrient)", PROCEDURE(p_rotate), },
		{ "revRotate", 0, PARAM_NOSTATIC, "reverse rotate using Orient object", "p(oOrient)", PROCEDURE(p_revrotate), },
		{ "lookAt", 0, PARAM_NOSTATIC, "calculate rotation from 2 vectors", "p(oXYZ direction,oXYZ up)", PROCEDURE(p_lookat), },
		{ "normalize", 0, PARAM_NOSTATIC, "normalize", "p()", PROCEDURE(p_normalize), },
		{ "between2", 0, PARAM_NOSTATIC, "interpolate orientation", "p(oOrient,oOrient,f amount)", PROCEDURE(p_between2), "The calling Orient receives the orientation interpolated from 2 input orientations.\nExample:\n"
		"var o1=Orient.new(), o2=Orient.new(), o3=Orient.new();\n"
		"o2.rotate3(0,Math.pi/2,0);\n"
		"o3.between2(o1,o2,0); // o3 equals o2\n"
		"o3.between2(o1,o2,1); // o3 equals o1\n"
		"o3.between2(o1,o2,0.5); // o3 is halfway between o1 and o2\n" },
		{ "betweenOV", 0, PARAM_NOSTATIC, "interpolate orientation", "p(oOrient,oXYZ,f amount)", PROCEDURE(p_betweenOV), "Like between2(), but the second Orient is composed of the supplied XYZ vector (X component) and Y Z vectors from the calling object.\n"
		"Example:\n"
		"var o=Orient.new();\n"
		"o.betweenOV(o,(0,1,0),1); //no change, o remains 100 010 001\n"
		"o.betweenOV(o,(0,1,0),0.9); //o is slightly rotated towards (0,1,0)\n"
		"o.betweenOV(o,(0,1,0),0); //o is completely transformed, o.x=(0,1,0)\n"
		},
		{ "localToWorld", 0, PARAM_NOSTATIC, "transform coordinates", "p oXYZ(oXYZ point,oXYZ center)", PROCEDURE(p_localToWorld), },
		{ "worldToLocal", 0, PARAM_NOSTATIC, "transform coordinates", "p oXYZ(oXYZ point,oXYZ center)", PROCEDURE(p_worldToLocal), },
		{ "angles", 0, PARAM_READONLY | PARAM_NOSTATIC, "Euler angles representation", "oXYZ", GETONLY(angles), },
		{ "toString", 0, PARAM_READONLY | PARAM_NOSTATIC, "textual form", "s", GETONLY(toString), },
		{ 0, 0, 0, },
	};
#undef FIELDSTRUCT
	return paramtab;
}

void Orient_Ext::p_new(ExtValue *args, ExtValue *ret)
{
	*ret = makeDynamicObject(new Orient_Ext());
}

void Orient_Ext::p_newFromVector(ExtValue *args, ExtValue *ret)
{
	VectorObject *vec = VectorObject::fromObject(args->getObject());
	if (vec)
		*ret = makeDynamicObject(new Orient_Ext(Orient(pt3DFromVec(vec, 0), pt3DFromVec(vec, 3), pt3DFromVec(vec, 6))));
	else
		ret->setEmpty();
}

void Orient_Ext::get_toVector(ExtValue *ret)
{
	VectorObject *vec = new VectorObject;
	add3Coords(vec, o.x);
	add3Coords(vec, o.y);
	add3Coords(vec, o.z);
	ret->setObject(ExtObject(&VectorObject::par, vec));
}

void Orient_Ext::p_clone(ExtValue *args, ExtValue *ret)
{
	*ret = makeDynamicObject(new Orient_Ext(o));
}

void Orient_Ext::p_set(ExtValue *args, ExtValue *ret)
{
	Orient_Ext *other = fromObject(args[0]);
	if (other)
		o = other->o;
	ret->setEmpty();
}

void Orient_Ext::p_reset(ExtValue *args, ExtValue *ret)
{
	o = Orient_1;
	ret->setEmpty();
}

void Orient_Ext::get_x(PARAMGETARGS)
{
	*ret = Pt3D_Ext::makeStaticObject(&o.x);
}

void Orient_Ext::get_y(PARAMGETARGS)
{
	*ret = Pt3D_Ext::makeStaticObject(&o.y);
}

void Orient_Ext::get_z(PARAMGETARGS)
{
	*ret = Pt3D_Ext::makeStaticObject(&o.z);
}

void Orient_Ext::p_lookat(ExtValue *args, ExtValue *ret)
{
	Pt3D_Ext *dir = Pt3D_Ext::fromObject(args[1]), *up = Pt3D_Ext::fromObject(args[0]);
	if (dir&&up)
		o.lookAt(dir->p, up->p);
	ret->setEmpty();
}

void Orient_Ext::p_rotate3(ExtValue *args, ExtValue *ret)
{
	Pt3D p(args[2].getDouble(), args[1].getDouble(), args[0].getDouble());
	o.rotate(p);
	ret->setEmpty();
}

void Orient_Ext::p_rotate(ExtValue *args, ExtValue *ret)
{
	Orient_Ext *obj = Orient_Ext::fromObject(args[0]);
	if (!obj)
	{
		Orient tmp = o;
		obj->o.transform(o, tmp);
	}
	ret->setEmpty();
}

void Orient_Ext::p_revrotate(ExtValue *args, ExtValue *ret)
{
	Orient_Ext *obj = Orient_Ext::fromObject(args[0]);
	if (obj)
	{
		Orient tmp = o;
		obj->o.revTransform(o, tmp);
	}
	ret->setEmpty();
}

void Orient_Ext::p_normalize(ExtValue *args, ExtValue *ret)
{
	o.normalize();
	ret->setEmpty();
}

void Orient_Ext::p_between2(ExtValue *args, ExtValue *ret)
{
	Orient_Ext *o1 = Orient_Ext::fromObject(args[2]);
	Orient_Ext *o2 = Orient_Ext::fromObject(args[1]);
	if (o1&&o2)
	{
		double q1 = args[0].getDouble(), q2 = 1.0 - q1;
		o.x.x = q1 * o1->o.x.x + q2 * o2->o.x.x;
		o.x.y = q1 * o1->o.x.y + q2 * o2->o.x.y;
		o.x.z = q1 * o1->o.x.z + q2 * o2->o.x.z;
		o.y.x = q1 * o1->o.y.x + q2 * o2->o.y.x;
		o.y.y = q1 * o1->o.y.y + q2 * o2->o.y.y;
		o.y.z = q1 * o1->o.y.z + q2 * o2->o.y.z;
		o.z.x = q1 * o1->o.z.x + q2 * o2->o.z.x;
		o.z.y = q1 * o1->o.z.y + q2 * o2->o.z.y;
		o.z.z = q1 * o1->o.z.z + q2 * o2->o.z.z;
		o.normalize();
	}
	ret->setEmpty();
}

void Orient_Ext::p_betweenOV(ExtValue *args, ExtValue *ret)
{
	Orient_Ext *o1 = Orient_Ext::fromObject(args[2]);
	Pt3D_Ext *p2 = Pt3D_Ext::fromObject(args[1]);
	if (o1&&p2)
	{
		double q1 = args[0].getDouble(), q2 = 1.0 - q1;
		o.x.x = q1 * o1->o.x.x + q2 * p2->p.x;
		o.x.y = q1 * o1->o.x.y + q2 * p2->p.y;
		o.x.z = q1 * o1->o.x.z + q2 * p2->p.z;
		o.normalize();
	}
	ret->setEmpty();
}

void Orient_Ext::p_localToWorld(ExtValue *args, ExtValue *ret)
{
	Pt3D_Ext *center, *point;
	point = Pt3D_Ext::fromObject(args[1]);
	center = Pt3D_Ext::fromObject(args[0]);
	if (center && point)
	{
		Pt3D d;
		Pt3D src = point->p;
		o.transform(d, src);
		d += center->p;
		*ret = Pt3D_Ext::makeDynamicObject(new Pt3D_Ext(d));
	}
	else
		ret->setEmpty();
}

void Orient_Ext::p_worldToLocal(ExtValue *args, ExtValue *ret)
{
	Pt3D_Ext *center, *point;
	point = Pt3D_Ext::fromObject(args[1]);
	center = Pt3D_Ext::fromObject(args[0]);
	if (center && point)
	{
		Pt3D d;
		Pt3D src = point->p;
		d -= center->p;
		o.revTransform(d, src);
		*ret = Pt3D_Ext::makeDynamicObject(new Pt3D_Ext(d));
	}
	else
		ret->setEmpty();
}

void Orient_Ext::get_angles(ExtValue *ret)
{
	*ret = Pt3D_Ext::makeDynamicObject(new Pt3D_Ext(o.getAngles()));
}

void Orient_Ext::get_toString(ExtValue *ret)
{
	Pt3D_Ext a(o.getAngles());
	ret->setString(SString("Orient@") + a.toString());
}

Param& Orient_Ext::getStaticParam()
{
#ifdef __CODEGUARD__
	static Orient_Ext static_orientobj;
	static Param static_orientparam(getStaticParamtab(), &static_orientobj);
#else
	static Param static_orientparam(getStaticParamtab());
#endif
	return static_orientparam;
}

Orient_Ext* Orient_Ext::fromObject(const ExtValue& v)
{
	return (Orient_Ext*)v.getObjectTarget(getStaticParam().getName());
}

ParamInterface* Orient_Ext::getInterface() { return &getStaticParam(); }

ExtObject Orient_Ext::makeStaticObject(Orient* o)
{
	return ExtObject(&getStaticParam(), ((char*)o) + (((char*)&o->x) - ((char*)&((Orient_Ext*)o)->o.x)));
}

ExtObject Orient_Ext::makeDynamicObject(Orient_Ext* p)
{
	return ExtObject(&getStaticParam(), p);
}

/////////////

REGISTER_DESERIALIZABLE(Pt3D_Ext)
REGISTER_DESERIALIZABLE(Orient_Ext)
