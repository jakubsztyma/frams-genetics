// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _MODELPARTS_H_
#define _MODELPARTS_H_

#include <frams/util/3d.h>
#include <frams/genetics/genoconv.h>

#include <frams/util/extvalue.h>
#include <frams/util/list.h>
#include <frams/util/sstring.h>
#include <frams/util/sstringutils.h>
#include <frams/param/param.h>
#include <frams/param/syntparam.h>
#include <frams/util/usertags.h>
#include <frams/param/paramtabobj.h>

#include <stdio.h>

class Model;
class IRange;
class MultiRange;

typedef UserTags<Model, void *, 5> ModelUserTags;

/** Common base for model elements. */
class PartBase
{
public:
	SString vis_style;
	PartBase(const SString &s) :vis_style(s), mapped(0) {}
	~PartBase();
	static SString getDefaultStyle() { return SString("none"); }
	MultiRange *mapped;
	enum PartBaseFlags { Selected = 1 };
	int flags;
	Model *owner;	///< backlink to the model

	SString info;

	Model &getModel() const { return *owner; }

	ModelUserTags userdata;

	void notifyMappingChange();

	void clearMapping();
	MultiRange *getMapping() { return mapped; }
	void setMapping(const IRange &mr);
	void addMapping(const IRange &mr);
	void setMapping(const MultiRange &mr);
	void addMapping(const MultiRange &mr);

	void setInfo(const SString &name, const SString &value);
	void setInfo(const SString &name, int value);
	void setInfo(const SString &name, double value);
	SString getInfo(const SString &name);
};

/// Part is the only real physical object in the Framsticks creature.
/// You can use this class for querying and adjusting constructed
/// model properties
class Part : public PartBase
{
	friend class Model;
	static SString getDefaultStyle();
	Part(double _mass, double _size, double _density, double _friction, double _ingest, double _assim)
		:PartBase(getDefaultStyle()), mass(_mass), size(_size), density(_density), friction(_friction), ingest(_ingest), assim(_assim)
	{}
	void defassign();
public:
	// base properties - have special meaning and therefore are often accessed directly for convenience
	Pt3D p;    ///< 3d coordinates of the part
	Orient o;  ///< orientation in 3d space (rotation matrix)
	/// ParamInterface object is preferred way to get/set other properties.
	Param extraProperties();
	Param properties();
	paInt refno;
	Pt3D rot;///< rotation angles

	///
	paInt shape;///default=old Framsticks compatible, do not mix with shapes>0
	enum Shape { SHAPE_BALL_AND_STICK = 0, SHAPE_ELLIPSOID = 1, SHAPE_CUBOID = 2, SHAPE_CYLINDER = 3 };
	double mass, size, density, friction, ingest, assim, hollow;
	Pt3D scale;
	Pt3D food;
	//SList points; // collistion points
	//Slist neurons; // "select * from owner->neurons where part=this" ;-)

	Pt3D vcolor;
	double vsize;

	Part(enum Shape s = SHAPE_BALL_AND_STICK);
	Part(const Part &src) :PartBase(getDefaultStyle()) { operator=(src); }
	void operator=(const Part &src);

	void setPositionAndRotationFromAxis(const Pt3D &p1, const Pt3D &p2);
	void setOrient(const Orient &o);///< set part.o and calculates part.rot (rotation angles)
	void setRot(const Pt3D &r);///< set part.rot (rotation angles) and calculate part.o

	static Param &getStaticParam();
};

/// Imaginary connection between two parts.
/// Joint has no mass nor intertia but can transfer forces.
class Joint : public PartBase
{
	friend class Model;
	SString getDefaultStyle();
	Joint(double _stamina, double _stif, double _rotstif, double _d)
		:PartBase(getDefaultStyle()), stamina(_stamina), stif(_stif), rotstif(_rotstif)
	{
		d = Pt3D(_d, 0, 0);
	}
	void defassign();
	void resetDeltaMarkers();
public:
	// base properties:
	paInt p1_refno, p2_refno; ///< parts' reference numbers

	Part *part1, *part2;	///< references to parts
	class Pt3D d;		///< position delta between parts
	class Pt3D rot;	///< orientation delta between parts expressed as 3 angles
	enum Shape { SHAPE_BALL_AND_STICK = 0, SHAPE_FIXED = 1, SHAPE_B = 2, SHAPE_C = 3, SHAPE_D = 4 };
	paInt shape;///< ball-and-stick=old Framsticks compatible, creates a physical rod between parts (cylinder or cuboid), do not mix with shape>0,  fixed=merge parts into one physical entity

	Joint();
	Joint(const Joint &src) :PartBase(getDefaultStyle()) { operator=(src); }
	void operator=(const Joint &src);

	/** connect two parts with this joint.
		p2 position will be adjusted if delta option is in effect.
		@see isDelta()
		*/
	void attachToParts(Part *p1, Part *p2);
	/// @see attachToParts(Part*,Part*)
	void attachToParts(int p1, int p2);

	/** discard delta information but don't disable delta flag.
		delta will be calculated from parts positions during final consistency check.
		*/
	void resetDelta();

	/** enable or disable delta option.
		delta value is not changed.
		*/
	void useDelta(bool use);

	/** @return true if delta option is in effect.
		@see useDelta(), resetDelta(), useDelta()
		*/
	bool isDelta();

	/// ParamInterface object is preferred way to get/set other properties.
	Param extraProperties();
	Param properties();

	// do not touch these:
	paInt refno; ///< this joint's reference number
	double stamina;
	double stif, rotstif;	///< stiffness for moving and bending forces
	class Orient o;	///< orientation delta between parts as rotation matrix
	/** flag: generated f0 should include delta data.
		set by 'singlestep' if j: attributes use delta option */
	bool usedelta;
	Pt3D vcolor;

	static Param &getStaticParam();
};

#define JOINT_DELTA_MARKER 99999.0



////////////////// Neural Network /////////////////

class NeuroClass;

typedef UserTags<NeuroClass, void *, 5> NeuroClassUserTags;

/** Information about neuron class.
 */
class NeuroClass
{
	bool ownedvectordata;
	void operator=(const NeuroClass &nosuchthich) {}
public:
	SString name, longname, description;
	ParamEntry *props;
	bool ownedprops;//< destructor will free props using ParamObject::freeParamTab
	paInt prefinputs, prefoutput;
	paInt preflocation;
	int *vectordata;
	paInt visualhints;

	//void *impl;
	int impl_count;
	bool active;
	bool genactive;
	NeuroClassUserTags userdata;

	//////////////////////
	~NeuroClass();
	NeuroClass();
	NeuroClass(ParamEntry *_props, SString _description,
		int _prefinputs, int _prefoutput, int _preflocation, int *_vectordata, bool own_vd = 1, int vhints = 0);
	/** class name for use in Neuro::setClassName(), Neuro::setDetails() (former 'moredata' field),
		eg. "N","-",G" */
	const SString &getName() { return name; }
	/** human friendly name, eg. "Neuron","Link","Gyroscope"  */
	const SString &getLongName() { return longname; }
	/** long description */
	const SString &getDescription() { return description; }
	ParamEntry *getParamTab() { return props; }

	/** NeuroClass specific properties, recognized by all neurons of this class */
	Param getProperties() { return Param(props); }

	/** preferred number of inputs, -1 = no preference (any number will go).
		extra inputs may be ignored by the object (depends on the class).
		*/
	int getPreferredInputs() { return (int)prefinputs; }

	/** @return 0 if this object doesn't provide useful output signal. */
	int getPreferredOutput() { return (int)prefoutput; }

	/** @return 0 if the object doesn't need any assignment to the body element.
		@return 1 = it likes to be attached to the Part ( @see Neuro::attachToPart() )
		@return 2 = the object prefers to have the Joint ( @see Neuro::attachToJoint() )
		*/
	int getPreferredLocation() { return (int)preflocation; }
	/** vector drawing to be used in neuro net diagram.
		interpretation:
		{
		LEN = datalength (excluding this number)
		NL = number_of_lines
		line#1 ->  NS = number_of_segments, x1,y1, x2,y2, ... xNS-1,yNS-1,
		...
		line#NL -> NS = number_of_segments, x1,y1, x2,y2, ... xNS-1,yNS-1,
		}
		*/
	int *getSymbolGlyph()
	{
		return vectordata;
	}
	void setSymbolGlyph(int *data, bool owned = 1)
	{
		if (vectordata && ownedvectordata) delete[]vectordata;
		vectordata = data; ownedvectordata = owned;
	}
	/** additional information about how the neuron should be drawn
		used by structure view (and maybe some other components).
		return value is defined by the enum Hint
		@see enum Hint
		*/
	int getVisualHints()
	{
		return (int)visualhints;
	}

	enum Hint
	{
		/** don't draw neurons of this class */
		Invisible = 1,
		/** don't draw classname label below the neuron */
		DontShowClass = 2,
		/** draw the neuron at the first part when attached to joint (default is in the middle) */
		AtFirstPart = 4,
		/** draw the neuron at the second part when attached to joint (default is in the middle) */
		AtSecondPart = 8,
		/** use effector colour for this neuro unit */
		EffectorClass = 16,
		/** use receptor colour for this neuro unit */
		ReceptorClass = 32,
		V1BendMuscle = 64,
		V1RotMuscle = 128,
		LinearMuscle = 256
	};

	/** textual summary, automatically generated from other properties (like the neuro class tooltip) */
	SString getSummary();

	static void resetActive(); ///< set default values of active and genactive for all classes
	static void setGenActive(const char *classes[]); ///< set genactive for specified classes
};






/** Single processing unit in Framsticks neural network.  */
class Neuro : public PartBase
{
	friend class Model;
	static SString getDefaultStyle();

	struct NInput {
		Neuro *n; double weight; SString *info;
		NInput(Neuro *_n, double w, SString *i = 0) :n(_n), weight(w), info(i) {}
	};

	SListTempl<NInput> inputs;

	NeuroClass *myclass;
	bool knownclass;
	SString myclassname, myclassparams;
	/** set myclass and make knownclass=true */
	void checkClass();
	SString **inputInfo(int i);
	void defassign();

public:
	enum NeuroFlags { HoldState = 2 };
	Param properties();
	Param extraProperties();

	void setInputInfo(int i, const SString &name, const SString &value);
	void setInputInfo(int i, const SString &name, int value);
	void setInputInfo(int i, const SString &name, double value);
	SString getInputInfo(int i);
	SString getInputInfo(int i, const SString &name);

	NeuroClass *getClass();
	void setClass(NeuroClass *);

	SString getClassParams() { return myclassparams; }
	void setClassParams(const SString &cp) { myclassparams = cp; }

	SString getClassName();
	void setClassName(const SString &clazz);

	/** return neuro unit details encoded as <CLASS> ":" <PROPERTIES>

		new Neuro can be created as root object (without parent) or can be
		the child of existing Neuro. Children of the Neuro are its inputs.
		Standard Framsticks neuron calculates the sum of all input units - other processing
		units don't have to treat them equally and can even ignore some of them.
		There are hints about expected inputs in the class database, @see getClass

		Application should not assume anything about classes and its properties
		except for two standard classes: (information about all current classes
		can be retrieved with getClass/getClassProperties methods)
		- getClassName()="N" is the standard Framsticks neuron, accepts any number of inputs,
		compatible with old Neuro object
		- getClassName()="-" is the neuron link, compatible with old Neuro-Neuro link
		(NeuroItem with empty details)
		Empty details defaults to "-" if the parent unit is specified,
		and "N" if the unit has no parent.
		*/
	SString getDetails();

	/** details = classname + ":" + classparams
		@see getDetails()
		*/
	void setDetails(const SString &);

#define STATRICKCLASS Neuro
	PARAMGETDEF(details) { arg1->setString(getDetails()); }
	PARAMSETDEF(details) { setDetails(arg1->getString()); return PSET_CHANGED; }
	PARAMGETDEF(inputCount);
	PARAMPROCDEF(p_getInputNeuroDef);
	PARAMPROCDEF(p_getInputNeuroIndex);
	PARAMPROCDEF(p_getInputWeight);
	PARAMGETDEF(classObject);
#undef STATRICKCLASS

	///@param handle_defaults_when_saving see SyntParam
	SyntParam classProperties(bool handle_defaults_when_saving = true);
	// base properties:
	paInt refno; ///< unique reference number (former 'neuro' refno)

	paInt part_refno; ///< can be used by some items as the part ref#
	paInt joint_refno; ///< can be used by some items as the joint ref#

	Pt3D pos, rot;	///< default = zero

	ModelUserTags userdata;

	Neuro();
	Neuro(double _state, double _inertia, double _force, double _sigmo);
	Neuro(const Neuro &src) :PartBase(getDefaultStyle()) { operator=(src); }

	~Neuro();

	void operator=(const Neuro &src);

	/** Attach this Neuro to the specified Part or detach it from the body if p==NULL.
		Neuro can be attached to either Part or Joint, but not both.
		@see getPart()
		*/
	void attachToPart(Part *p) { part = p; joint = 0; }

	/** Attach this Neuro to the specified Joint or detach it from the body if p==NULL.
		Neuro can be attached to either Part or Joint, but not both.
		@see getJoint()
		*/
	void attachToJoint(Joint *j) { joint = j; part = 0; }

	void attachToPart(int i);
	void attachToJoint(int i);

	/** @return Part the Neuro is attached to, or NULL if it has no defined location on the body.
		@see attachToPart()
		*/
	Part *getPart() { return part; }

	/** @return Joint the Neuro is attached to, or NULL if it has no defined location on the body.
		@see attachToJoint()
		*/
	Joint *getJoint() { return joint; }

	int isOldEffector();
	int isOldReceptor();
	int isOldNeuron();
	int isNNConnection();

	/** @return the number of inputs connected to this Neuro.
		Functions like getInput(), getInputWeight() will accept connection number [0..InputCount-1]
		*/
	int getInputCount() const { return inputs.size(); }

	/// @return the number of output connections (including possible self-connections)
	int getOutputsCount() const;

	/** @return the Neuro connected as i-th input */
	Neuro *getInput(int i) const { return (i >= inputs.size()) ? 0 : inputs(i).n; }
	/** @return the Neuro connected as i-th input.
		@param weight
		*/
	Neuro *getInput(int i, double &weight) const;
	/** @return connectin weight for i-th input */
	double getInputWeight(int i) const;
	/** change connection weight for i-th input */
	void setInputWeight(int i, double weight);
	/** connect i-th input with another neuron */
	void setInput(int i, Neuro *n);
	/** connect i-th input with another neuron */
	void setInput(int i, Neuro *n, double weight);
	/** add new input. @return its reference number */
	int addInput(Neuro *child, double weight = 1.0, const SString *info = 0);
	/** @return reference number [0..InputCount-1] of the input
	   or -1 if 'child' is not connected with this Neuro.*/
	int findInput(Neuro *child) const;
	void removeInput(paInt refno);
	/**    @return reference number of the child connection, like findInput() */
	int removeInput(Neuro *child);

	int findInputs(SList &result, const char *classname = 0, const Part *part = 0, const Joint *joint = 0) const;
	int findOutputs(SList &result, const char *classname = 0, const Part *part = 0, const Joint *joint = 0) const;

	/* class database retrieval */
	static int getClassCount();
	/** @return Neuro class name.
		@param classindex 0 .. getClassCount()
		*/
	static SString getClassName(int classindex);
	static NeuroClass *getClass(int classindex);
	static NeuroClass *getClass(const SString &classname);
	static int getClassIndex(const NeuroClass *nc);

	// not really private, but you should not access those directly
	double state;

	/** may reference parent neuron if parentcount is exacty 1. parent is invalid otherwise. @sa parentcount */
	Neuro *parent;
	int parentcount; ///< @sa parent

	Part *part;	///< link to the Part
	Joint *joint;	///< link to the Joint - required by some objects (eg.muscles)
	Orient o;	///< rotation matrix calculated from "rot"
	static ParamEntry emptyParamTab[];
	static Param &getStaticParam();
};

class NeuroExt : public Neuro
{
public:
#define STATRICKCLASS NeuroExt
	PARAMGETDEF(neuroclass);
	PARAMSETDEF(neuroclass);
#undef STATRICKCLASS
	static ParamEntry *getParamTab();
};

class NeuroConn
{
	void defassign();
public:
	int n1_refno, n2_refno;
	double weight;
	SString info;
	NeuroConn();
};

extern ParamEntry f0_part_paramtab[], f0_joint_paramtab[], f0_nodeltajoint_paramtab[], f0_neuro_paramtab[], f0_neuroconn_paramtab[], f0_neuroitem_paramtab[];

#endif
