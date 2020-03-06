#include "modelobj.h"
#include <frams/vm/classes/genoobj.h>
#include <frams/util/extvalue.h>
#include <frams/vm/classes/3dobject.h>

#define FIELDSTRUCT ModelObj
ParamEntry modelobj_paramtab[] =
{
	{ "Model", 1, 21, "Model", },
	{ "se", 0, PARAM_NOSTATIC, "startenergy", "f", FIELD(startenergy), },
	{ "Vstyle", 0, PARAM_NOSTATIC, "vis_style", "s", FIELD(vis_style), },

	{ "geno", 0, PARAM_NOSTATIC | PARAM_READONLY, "Geno", "oGeno", GETONLY(geno), },
	{ "newFromString", 0, 0, "Create a new object", "p oModel(s genotype)", PROCEDURE(p_newfromstring), },
	{ "newFromGeno", 0, 0, "Create a new object", "p oModel(oGeno)", PROCEDURE(p_newfromgeno), },
	{ "newWithCheckpoints", 0, 0, "Create a new object", "p oModel(x Geno object or string genotype)", PROCEDURE(p_newwithcheckpoints), "Creates a Model with the \"Checkpoints\" option enabled. Genotype converters supporting Checkpoints provide a sequence of Models that reflects development stages of the creature (this sequence is used purely for debugging and visualization of phenotype growth/development). Checkpoint Models can be accessed using getCheckpoint(i) for i ranging from 0 to numcheckpoints-1. Models created without the Checkpoint option and Models coming from unsupported converters have numcheckpoints=0." },

	{ "numparts", 0, PARAM_DONTSAVE | PARAM_NOSTATIC | PARAM_READONLY, "Number of parts", "d", GETONLY(numparts), },
	{ "numjoints", 0, PARAM_DONTSAVE | PARAM_NOSTATIC | PARAM_READONLY, "Number of joints", "d", GETONLY(numjoints), },
	{ "numneurons", 0, PARAM_DONTSAVE | PARAM_NOSTATIC | PARAM_READONLY, "Number of neurons", "d", GETONLY(numneurons), },
	{ "numconnections", 0, PARAM_DONTSAVE | PARAM_NOSTATIC | PARAM_READONLY, "Number of neuron connections", "d", GETONLY(numconnections), },

	{ "getPart", 0, PARAM_USERHIDDEN | PARAM_NOSTATIC, "getPart (static model information)", "p oPart(d index)", PROCEDURE(p_getpart), },
	{ "getJoint", 0, PARAM_USERHIDDEN | PARAM_NOSTATIC, "getJoint (static model information)", "p oJoint(d index)", PROCEDURE(p_getjoint), },
	{ "getNeuroDef", 0, PARAM_USERHIDDEN | PARAM_NOSTATIC, "getNeuroDef", "p oNeuroDef(d index)", PROCEDURE(p_getneuro), },

	{ "size_x", 0, PARAM_READONLY | PARAM_NOSTATIC | PARAM_DEPRECATED, "Bounding box x size", "f", FIELD(size.x), "(size_x,size_y,size_z) are dimensions of the axis-aligned bounding box of the creature, including imaginary Part sizes (Part.s, usually 1.0). A creature consisting of a single default part has the size of (2.0,2.0,2.0) - twice the Part.s value (like a sphere diameter is twice its radius).\nSee also: Creature.moveAbs" },
	{ "size_y", 0, PARAM_READONLY | PARAM_NOSTATIC | PARAM_DEPRECATED, "Bounding box y size", "f", FIELD(size.y), "See Model.size_x" },
	{ "size_z", 0, PARAM_READONLY | PARAM_NOSTATIC | PARAM_DEPRECATED, "Bounding box z size", "f", FIELD(size.z), "See Model.size_x" },
	{ "bboxSize", 0, PARAM_READONLY | PARAM_NOSTATIC, "Bounding box size", "oXYZ", GETONLY(bboxsize) },
	{ "numcheckpoints", 0, PARAM_DONTSAVE | PARAM_READONLY | PARAM_NOSTATIC, "Number of checkpoints", "d", GETONLY(numcheckpoints) },
	{ "getCheckpoint", 0, PARAM_USERHIDDEN | PARAM_NOSTATIC, "getCheckpoint", "p oModel(d index)", PROCEDURE(p_getcheckpoint),
	"Checkpoint Model objects are only valid as long as the parent Model object exists.\n"
	"See also: Model.newWithCheckpoints()\n\n"
	"// incorrect usage - calling getCheckpoint() on a temporary object:\n"
	"var c=Model.newWithCheckpoints(\"XXX\").getCheckpoint(1).genotype.geno;\n\n"
	"// correct usage - keeping the parent Model reference in 'm':\n"
	"var m=Model.newWithCheckpoints(\"XXX\");\n"
	"var c=m.getCheckpoint(1).genotype.geno;\n"
	},
	{ "shape_type", 0, PARAM_DONTSAVE | PARAM_NOSTATIC | PARAM_READONLY, "Shape type", "d 0 3 ~Unknown~Illegal~Ball-and-stick~Solids", GETONLY(shape_type) },
	{ "solid_model", 0, PARAM_DONTSAVE | PARAM_NOSTATIC | PARAM_READONLY, "Solid shapes model", "oModel", GETONLY(solid_model), "Conversion of this Model to solid shapes. Note! Only available when this Model has shape_type==2 (Ball-and-stick)." },

	{ 0, 0, 0, },
};
#undef FIELDSTRUCT

void ModelObj::get_geno(ExtValue *ret)
{
	Geno *g;
	if ((!geno.isValid()) && isValid())
		g = new Geno(getF0Geno());
	else
		g = new Geno(geno);
	ret->setObject(GenoObj::makeDynamicObjectAndDecRef(g));
}

void ModelObj::p_newfromstring(ExtValue *args, ExtValue *ret)
{
	*ret = makeDynamicObject(new Model(Geno(args[0].getString())));
}

void ModelObj::p_newfromgeno(ExtValue *args, ExtValue *ret)
{
	Geno *g = GenoObj::fromObject(args[0].getObject());
	if (g)
		*ret = makeDynamicObject(new Model(*g));
	else
		ret->setEmpty();
}

void ModelObj::p_newwithcheckpoints(ExtValue *args, ExtValue *ret)
{
	Model *m = NULL;
	if (args[0].getType() == TString)
		m = new Model(Geno(args[0].getString()), false, true);
	else
	{
		Geno *g = GenoObj::fromObject(args[0].getObject(), false);
		if (g)
			m = new Model(*g, false, true);
		else
			logPrintf("Model", "newWithCheckpoints", LOG_ERROR, "Geno or string expected, %s found", args[0].typeDescription().c_str());
	}

	if (m != NULL)
		*ret = makeDynamicObject(m);
	else
		ret->setEmpty();
}

Param& ModelObj::getStaticParam()
{
#ifdef __CODEGUARD__
	static ModelObj static_modelobj;
	static Param static_modelparam(modelobj_paramtab, &static_modelobj);
#else
	static Param static_modelparam(modelobj_paramtab);
#endif
	return static_modelparam;
}

Param& ModelObj::getDynamicParam()
{
	static Param dynamic_modelparam(modelobj_paramtab);
	return dynamic_modelparam;
}

ParamInterface* ModelObj::getInterface()
{
	return &getStaticParam();
}

ExtObject ModelObj::makeStaticObject(Model* m)
{
	return ExtObject(&getStaticParam(), (void*)m);
}

ExtObject ModelObj::makeDynamicObject(Model* m)
{
	return ExtObject(&getDynamicParam(), (DestrBase*)m);
}

Model* ModelObj::fromObject(const ExtValue& v, bool warn)
{
	return (Model*)v.getObjectTarget(getStaticParam().getName(), warn);
}

void ModelObj::p_getpart(PARAMPROCARGS)
{
	int i = args->getInt();
	if ((i < 0) || (i >= getPartCount()))
	{
		ret->setEmpty();
		return;
	}
	ret->setObject(ExtObject(&Part::getStaticParam(), getPart(i)));
}

void ModelObj::p_getjoint(PARAMPROCARGS)
{
	int i = args->getInt();
	if ((i < 0) || (i >= getJointCount()))
	{
		ret->setEmpty();
		return;
	}
	ret->setObject(ExtObject(&Joint::getStaticParam(), getJoint(i)));
}

void ModelObj::p_getneuro(PARAMPROCARGS)
{
	int i = args->getInt();
	if ((i < 0) || (i >= getNeuroCount()))
	{
		ret->setEmpty();
		return;
	}
	ret->setObject(ExtObject(&Neuro::getStaticParam(), getNeuro(i)));
}

void ModelObj::get_bboxsize(ExtValue *ret)
{
	*ret = Pt3D_Ext::makeDynamicObject(new Pt3D_Ext(size));
}

void ModelObj::p_getcheckpoint(PARAMPROCARGS)
{
	int i = args->getInt();
	if ((i < 0) || (i >= getCheckpointCount()))
	{
		ret->setEmpty();
		return;
	}
	ret->setObject(makeStaticObject(getCheckpoint(i)));
}

void ModelObj::get_solid_model(ExtValue *ret)
{
	if (getShapeType() != Model::SHAPE_BALL_AND_STICK)
		ret->setEmpty();
	Model *m = new Model;
	m->open();
	m->buildUsingSolidShapeTypes(*this);
	m->close();
	*ret = makeDynamicObject(m);
}

void ModelObj::get_shape_type(ExtValue *ret)
{
	ret->setInt(getShapeType());
}
