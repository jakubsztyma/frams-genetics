// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

///////////////////////////////     MODELPARTS.CPP     ///////////////

#include "modelparts.h"
#include "model.h"

#include <common/nonstd.h>
#include <frams/param/param.h>
#include <frams/neuro/neurolibrary.h>
#include <frams/util/multirange.h>
#include <frams/util/extvalue.h>
#include <frams/param/paramobj.h>

#include F0_DEFASSIGN_FILE

#ifndef SDK_WITHOUT_FRAMS
#include <frams/neuro/neuroclsobject.h>
#endif

/////////////////////////

PartBase::~PartBase()
{
	if (mapped) delete mapped;
}

void PartBase::notifyMappingChange()
{
	if (owner) owner->partmappingchanged = 1;
}

void PartBase::setMapping(const IRange &r)
{
	if (mapped) (*mapped) = r;
	else mapped = new MultiRange(r);
	notifyMappingChange();
}

void PartBase::clearMapping()
{
	if (mapped) { delete mapped; mapped = 0; }
}

void PartBase::addMapping(const IRange &r)
{
	if (mapped) mapped->add(r);
	else mapped = new MultiRange(r);
	notifyMappingChange();
}

void PartBase::setMapping(const MultiRange &mr)
{
	if (mapped) (*mapped) = mr;
	else mapped = new MultiRange(mr);
	notifyMappingChange();
}

void PartBase::addMapping(const MultiRange &mr)
{
	if (mapped) mapped->add(mr);
	else mapped = new MultiRange(mr);
	notifyMappingChange();
}

void PartBase::setInfo(const SString &name, const SString &value)
{
	strSetField(info, name, value);
}

void PartBase::setInfo(const SString &name, int value)
{
	setInfo(name, SString::valueOf(value));
}

void PartBase::setInfo(const SString &name, double value)
{
	setInfo(name, SString::valueOf(value));
}

SString PartBase::getInfo(const SString &name)
{
	return strGetField(info, name);
}

/////////////////////////

NeuroClass::NeuroClass(ParamEntry *_props, SString _description,
	int _prefinputs, int _prefoutput, int _preflocation,
	int *_vectordata, bool own_vd, int vhints)
	:ownedvectordata(own_vd),
	name(_props->name), longname(_props->id), description(_description),
	props(_props), ownedprops(false),
	prefinputs(_prefinputs),
	prefoutput(_prefoutput),
	preflocation(_preflocation),
	vectordata(_vectordata),
	visualhints(vhints), impl_count(0),/*impl(0),*/active(1), genactive(0)
{}

NeuroClass::~NeuroClass()
{
	setSymbolGlyph(0, 0);
	if (props && ownedprops)
		ParamObject::freeParamTab(props);
}

NeuroClass::NeuroClass()
	:ownedvectordata(0),
	name("Invalid"),
	props(empty_paramtab), ownedprops(false),
	prefinputs(0), prefoutput(0),
	preflocation(0), vectordata(0),
	visualhints(0), impl_count(0), /*impl(0),*/ active(1), genactive(0)
{}

void NeuroClass::resetActive()
{
	for (int i = 0; i < Neuro::getClassCount(); i++)
	{
		Neuro::getClass(i)->genactive = 0;
		Neuro::getClass(i)->active = 1;
	}
}

void NeuroClass::setGenActive(const char *genactive_classes[])
{
	for (const char **n = genactive_classes; *n; n++)
	{
		NeuroClass *cls = Neuro::getClass(*n);
		if (cls) cls->genactive = 1;
	}
}

SString NeuroClass::getSummary()
{
	SString t;
	t = getDescription();
	if (t.len()) t += "\n\n";
	t += "Characteristics:\n";
	if (getPreferredInputs())
	{
		if (getPreferredInputs() < 0) t += "   supports any number of inputs\n";
		else if (getPreferredInputs() == 1) t += "   uses single input\n";
		else t += SString::sprintf("   uses %d inputs\n", getPreferredInputs());
	}
	else t += "   does not use inputs\n";
	if (getPreferredOutput())
		t += "   provides output value\n";
	else
		t += "   does not provide output value\n";
	switch (getPreferredLocation())
	{
	case 0: t += "   does not require location in body\n"; break;
	case 1: t += "   should be located on a Part\n"; break;
	case 2: t += "   should be located on a Joint\n"; break;
	}
	Param p = getProperties();
	if (p.getPropCount())
	{
		if (t.len()) t += "\n\n";
		t += "Properties:\n";
		const char *h;
		int i;
		for (i = 0; i < p.getPropCount(); i++)
		{
			if (i) t += "\n";
			t += "   "; t += p.name(i); t += " ("; t += p.id(i); t += ") ";
			t += p.friendlyTypeDescr(i);
			if (h = p.help(i)) if (*h) { t += " - "; t += h; }
		}
	}
	return t;
}

/////////////////////////

/////////////////////////////////////

Neuro::Neuro(double _state, double _inertia, double _force, double _sigmo)
	:PartBase(getDefaultStyle()), state(_state)
{
	flags = 0;
	myclass = 0;
	knownclass = 1;
	part_refno = -1; joint_refno = -1;
}

Neuro::Neuro(void) :PartBase(getDefaultStyle())
{
	defassign();
	state = 0.0;
	myclass = NULL;
	myclassname = "N";//default d="N" but f0.def is unable to set this (d is GETSET, not a regular FIELD)
	knownclass = 0;
	refno = 0;
	pos = Pt3D_0; rot = Pt3D_0;
	parent = 0; part = 0; joint = 0;
	parentcount = 0;
	flags = 0;
	part_refno = -1; joint_refno = -1;
}


Neuro::~Neuro()
{
	int i;
	for (i = 0; i < inputs.size(); i++)
	{
		NInput &ni = inputs(i);
		if (ni.info) delete ni.info;
	}
}

SString **Neuro::inputInfo(int i)
{
	if (i >= getInputCount()) return 0;
	return &inputs(i).info;
}

void Neuro::setInputInfo(int i, const SString &name, const SString &value)
{
	SString **s = inputInfo(i);
	if (!s) return;
	if (!*s) *s = new SString();
	strSetField(**s, name, value);
}

void Neuro::setInputInfo(int i, const SString &name, int value)
{
	setInputInfo(i, name, SString::valueOf(value));
}

void Neuro::setInputInfo(int i, const SString &name, double value)
{
	setInputInfo(i, name, SString::valueOf(value));
}

SString Neuro::getInputInfo(int i)
{
	SString **s = inputInfo(i);
	if (!s) return SString();
	if (!*s) return SString();
	return **s;
}

SString Neuro::getInputInfo(int i, const SString &name)
{
	SString **s = inputInfo(i);
	if (!s) return SString();
	if (!*s) return SString();
	return strGetField(**s, name);
}

void Neuro::operator=(const Neuro &src)
{
	refno = src.refno;
	state = src.state;
	part_refno = -1;
	joint_refno = -1;
	pos = src.pos; rot = src.rot;
	parent = 0; part = 0; joint = 0;
	parentcount = 0;
	flags = 0;
	myclass = src.myclass;
	knownclass = src.knownclass;
	myclassname = src.myclassname;
	myclassparams = src.myclassparams;
}

void Neuro::attachToPart(int i)
{
	attachToPart((i >= 0) ? owner->getPart(i) : 0);
}

void Neuro::attachToJoint(int i)
{
	attachToJoint((i >= 0) ? owner->getJoint(i) : 0);
}

int Neuro::getClassCount()
{
	return NeuroLibrary::staticlibrary.getClassCount();
}

NeuroClass *Neuro::getClass(int classindex)
{
	return NeuroLibrary::staticlibrary.getClass(classindex);
}

NeuroClass *Neuro::getClass(const SString &classname)
{
	return NeuroLibrary::staticlibrary.findClass(classname);
}

int Neuro::getClassIndex(const NeuroClass *nc)
{
	return NeuroLibrary::staticlibrary.classes.find((void *)nc);
}

NeuroClass *Neuro::getClass()
{
	checkClass();
	return myclass;
}

void Neuro::setClass(NeuroClass *cl)
{
	myclass = cl;
	myclassname = cl->getName();
	knownclass = 1;
}

SString Neuro::getClassName(int classindex)
{
	NeuroClass *cl = NeuroLibrary::staticlibrary.getClass(classindex);
	return cl ? cl->getName() : SString();
}

void Neuro::setDetails(const SString &details)
{
	int colon = details.indexOf(':');
	if (colon >= 0) { myclassname = details.substr(0, colon); myclassparams = details.substr(colon + 1); }
	else { myclassname = details; myclassparams = 0; }
	knownclass = 0;
}

SString Neuro::getDetails()
{
	SString ret = getClassName();
	if (myclassparams.len()) { if (!ret.len()) ret = "N"; ret += ":"; ret += myclassparams; }
	return ret;
}

void Neuro::checkClass()
{
	if (knownclass) return;
	myclass = getClass(myclassname);
	knownclass = 1;
}

SyntParam Neuro::classProperties(bool handle_defaults_when_saving)
{
	NeuroClass *cl = getClass();
	ParamEntry *pe = cl ? cl->getParamTab() : emptyParamTab;
	return SyntParam(pe, &myclassparams, handle_defaults_when_saving);
}

SString Neuro::getClassName()
{
	return myclassname;
}

void Neuro::setClassName(const SString &clazz)
{
	myclassname = clazz;
	knownclass = 0;
}

int Neuro::addInput(Neuro *child, double weight, const SString *info)
{
	inputs += NInput(child, weight, (info && (info->len())) ? new SString(*info) : 0);
	child->parentcount++;
	if (child->parentcount == 1) { child->parent = this; }
	return inputs.size() - 1;
}

int Neuro::findInput(Neuro *child) const
{
	for (int i = 0; i < inputs.size(); i++)
		if (inputs(i).n == child) return i;
	return -1;
}

Neuro *Neuro::getInput(int i, double &weight) const
{
	if (i >= getInputCount()) return 0;
	NInput &inp = inputs(i);
	weight = inp.weight;
	return inp.n;
}

double Neuro::getInputWeight(int i) const
{
	return inputs(i).weight;
}

void Neuro::setInputWeight(int i, double w)
{
	inputs(i).weight = w;
}

void Neuro::setInput(int i, Neuro *n)
{
	NInput &inp = inputs(i);
	inp.n = n;
}

void Neuro::setInput(int i, Neuro *n, double w)
{
	NInput &inp = inputs(i);
	inp.n = n;
	inp.weight = w;
}

void Neuro::removeInput(int refno)
{
	Neuro *child = getInput(refno);
	child->parentcount--;
	if (child->parent == this) child->parent = 0;
	SString *s = inputs(refno).info;
	if (s) delete s;
	inputs.remove(refno);
}

int Neuro::removeInput(Neuro *child)
{
	int i = findInput(child);
	if (i >= 0) removeInput(i);
	return i;
}

int Neuro::getOutputsCount() const
{
	int c = 0;
	for (int i = 0; i < owner->getNeuroCount(); i++)
		for (int j = 0; j < owner->getNeuro(i)->getInputCount(); j++) c += owner->getNeuro(i)->getInput(j) == this;
	return c;
}

int Neuro::isOldEffector()
{
	static SString bend("|"), rot("@");
	return ((getClassName() == bend) || (getClassName() == rot));
}

int Neuro::isOldReceptor()
{
	static SString g("G"), t("T"), s("S");
	return ((getClassName() == g) || (getClassName() == t) || (getClassName() == s));
}

int Neuro::isOldNeuron()
{
	static SString n("N");
	return (getClassName() == n);
}

int Neuro::isNNConnection()
{
	static SString conn("-");
	return (getClassName() == conn);
}

int Neuro::findInputs(SList &result, const char *classname, const Part *part, const Joint *joint) const
{
	Neuro *nu;
	SString cn(classname);
	int n0 = result.size();
	for (int i = 0; nu = getInput(i); i++)
	{
		if (part)
			if (nu->part != part) continue;
		if (joint)
			if (nu->joint != joint) continue;
		if (classname)
			if (nu->getClassName() != cn) continue;
		result += (void *)nu;
	}
	return result.size() - n0;
}

int Neuro::findOutputs(SList &result, const char *classname, const Part *part, const Joint *joint) const
{ // not very efficient...
	Neuro *nu, *inp;
	SString cn(classname);
	SList found;
	int n0 = result.size();
	for (int i = 0; nu = getModel().getNeuro(i); i++)
	{
		if (part)
			if (nu->part != part) continue;
		if (joint)
			if (nu->joint != joint) continue;
		if (classname)
			if (inp->getClassName() != cn) continue;
		for (int j = 0; inp = nu->getInput(j); j++)
			if (inp == this)
			{
				result += (void *)nu;
				break;
			}
	}
	return result.size() - n0;
}

void Neuro::get_inputCount(PARAMGETARGS)
{
	ret->setInt(inputs.size());
}

void Neuro::p_getInputNeuroDef(ExtValue *args, ExtValue *ret)
{
	int i = args->getInt();
	if ((i < 0) || (i >= inputs.size()))
		ret->setEmpty();
	else
		ret->setObject(ExtObject(&Neuro::getStaticParam(), inputs(i).n));
}

void Neuro::p_getInputWeight(ExtValue *args, ExtValue *ret)
{
	int i = args->getInt();
	if ((i < 0) || (i >= inputs.size()))
		ret->setEmpty();
	else
		ret->setDouble(inputs(i).weight);
}

void Neuro::p_getInputNeuroIndex(ExtValue *args, ExtValue *ret)
{
	int i = args->getInt();
	if ((i < 0) || (i >= inputs.size()))
		ret->setInt(-1);
	else
		ret->setInt(inputs(i).n->refno);
}

void Neuro::get_classObject(PARAMGETARGS)
{
#ifndef SDK_WITHOUT_FRAMS
	NeuroClassExt::makeStaticObject(ret, getClass());
#endif
}

///////////////////////////////////////

SString Part::getDefaultStyle()
{
	return SString("part");
}
SString Joint::getDefaultStyle()
{
	return SString("joint");
}
/*
const SString& Neuro::getDefaultStyle()
{static SString s("neuro"); return s;}
const SString& NeuroItem::getDefaultStyle()
{static SString s("neuroitem"); return s;}
*/
SString Neuro::getDefaultStyle()
{
	return SString("neuro");
}

Part::Part(enum Shape s) :PartBase(getDefaultStyle())
{
	o = Orient_1;
	p = Pt3D_0;
	rot = Pt3D_0;
	flags = 0;
	defassign();
	shape = s;
	mass = 1;
}

void Part::operator=(const Part &src)
{
	p = src.p; o = src.o;
	flags = src.flags;
	mass = src.mass; density = src.density;
	friction = src.friction;
	ingest = src.ingest;
	assim = src.assim;
	size = src.size;
	rot = src.rot;
	refno = src.refno;
	vcolor = src.vcolor;
	vsize = src.vsize;
	vis_style = src.vis_style;
	shape = src.shape;
	scale = src.scale;
	hollow = src.hollow;
}

void Part::setOrient(const Orient &_o)
{
	o = _o;
	rot.getAngles(o.x, o.z);
}

void Part::setRot(const Pt3D &r)
{
	rot = r;
	o = Orient_1;
	o.rotate(rot);
}

void Part::setPositionAndRotationFromAxis(const Pt3D &p1, const Pt3D &p2)
{
	Pt3D x = p2 - p1;
	Pt3D dir(x.y, x.z, x.x);
	p = p1 + x * 0.5;
	rot.getAngles(x, dir);
}

Param &Part::getStaticParam()
{
	static Param p(f0_part_paramtab, 0, "Part");
	return p;
}


///////////////////////////

Joint::Joint() :PartBase(getDefaultStyle())
{
	rot = Pt3D_0;
	defassign();
	d.x = JOINT_DELTA_MARKER;
	d.y = JOINT_DELTA_MARKER;
	d.z = JOINT_DELTA_MARKER;
	part1 = 0; part2 = 0;
	flags = 0;
	usedelta = 0;
}

void Joint::operator=(const Joint &src)
{
	rot = src.rot;
	d = src.d;
	shape = src.shape;
	stamina = src.stamina;
	stif = src.stif; rotstif = src.rotstif;
	vis_style = src.vis_style;
	vcolor = src.vcolor;
	part1 = 0; part2 = 0;
	flags = src.flags;
	usedelta = src.usedelta;
	refno = src.refno;
}

void Joint::attachToParts(Part *p1, Part *p2)
{
	part1 = p1;
	part2 = p2;
	if (p1 && p2)
	{
		o = rot;
		if (usedelta)
		{
			p1->o.transform(p2->o, o);
			//		p2->o.x=p1->o/o.x; p2->o.y=p1->o/o.y; p2->o.z=p1->o/o.z;
			p2->p = p2->o.transform(d) + p1->p;
		}
	}
}

void Joint::attachToParts(int p1, int p2)
{
	attachToParts((p1 >= 0) ? owner->getPart(p1) : 0, (p2 >= 0) ? owner->getPart(p2) : 0);
}

void Joint::resetDelta()
{
	d = Pt3D(JOINT_DELTA_MARKER, JOINT_DELTA_MARKER, JOINT_DELTA_MARKER);
}

void Joint::resetDeltaMarkers()
{
	if (d.x == JOINT_DELTA_MARKER) d.x = 0;
	if (d.y == JOINT_DELTA_MARKER) d.y = 0;
	if (d.z == JOINT_DELTA_MARKER) d.z = 0;
}

void Joint::useDelta(bool use)
{
	usedelta = use;
}

bool Joint::isDelta()
{
	return usedelta;
}

Param &Joint::getStaticParam()
{
	static Param p(f0_joint_paramtab, 0, "Joint");
	return p;
}

/////////////////////////////////////////////////////////////////

#include F0_CLASSES_FILE

////////////////////////////////////////

ParamEntry Neuro::emptyParamTab[] =
{
	{ "Undefined Neuro", 1, 0, "?", },
	{ 0, 0, 0, },
};

Param Part::extraProperties()
{
	return Param(f0_part_xtra_paramtab, this);
}

Param Joint::extraProperties()
{
	return Param(f0_joint_xtra_paramtab, this);
}

Param Neuro::extraProperties()
{
	return Param(f0_neuro_xtra_paramtab, this);
}

Param Part::properties()
{
	return Param(f0_part_paramtab, this);
}

Param Joint::properties()
{
	return Param(usedelta ? f0_joint_paramtab : f0_nodeltajoint_paramtab, this);
}

Param Neuro::properties()
{
	return Param(f0_neuro_paramtab, this);
}

class NeuroExtParamTab : public ParamTab
{
public:
	NeuroExtParamTab() :ParamTab(f0_neuro_paramtab)
	{
#define FIELDSTRUCT NeuroExt
		ParamEntry entry = { "class", 2, 0, "neuro class", "s", GETSET(neuroclass) };
#undef FIELDSTRUCT
		add(&entry);

#define FIELDSTRUCT Neuro
		ParamEntry entry2 = { "state", 2, 0, "state", "f", FIELD(state) };
#undef FIELDSTRUCT
		add(&entry2);
	}
};

Param &Neuro::getStaticParam()
{
	static Param p(f0_neuro_paramtab, 0, "NeuroDef");
	return p;
}

////////////////////////

NeuroConn::NeuroConn()
{
	defassign();
}

//////////////////////////////////////

ParamEntry *NeuroExt::getParamTab()
{
	static NeuroExtParamTab tab;
	return tab.getParamTab();
}

void NeuroExt::get_neuroclass(PARAMGETARGS)
{
	ret->setString(getClassName());
}

int NeuroExt::set_neuroclass(PARAMSETARGS)
{
	setClassName(arg->getString()); return PSET_CHANGED;
}
