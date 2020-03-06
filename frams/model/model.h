// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _MODEL_H_
#define _MODEL_H_

#include <common/nonstd_math.h>
#include <stdlib.h>
#include <stdio.h>

#include "modelparts.h"
#include <frams/util/advlist.h>
#include <frams/util/usertags.h>

extern ParamEntry f0_model_paramtab[];

enum ModelBuildStatus { empty, building, invalid, valid };

class MultiMap;

class VisualModel;

/**
	"Model" is the skeleton of the Framsticks creature.
	This object can be used for 2 purposes:
	- you can build a creature from any supported Framsticks genotype
	format
	- or generate low level f0 genotype from existing construct.

	In both cases you have access to geometry and neuron net data.
	Using this standard class assures compatibility and good
	integration with core Framsticks engine.

	Model contains 3 kinds of objects:
	- Parts (class Part).
	- Joints (class Joint). Each Joint is connected with 2 Parts. (@see Joint::attachToParts()).
	- Neurons (class Neuro). Neuron can have 0 or more inputs - other neurons. (@see Neuro::addInput()).
	Each Neuron can be located on the physical structure, i.e. it can ba attached to Part or Joint
	(@see Neuro::attachToPart(), Neuro::attachToJoint()).

	\f[(dot)
	digraph Model
	{
	Joint1; Joint2;
	node [ shape=box ]
	Part1; Part2; Part3;
	Joint1 -> Part1; Joint1 -> Part2; Joint2 -> Part2; Joint2 -> Part3
	node [ shape=diamond ]
	Neuro1 -> Neuro2; Neuro1 -> Neuro3; Neuro2 -> Neuro2; Neuro3 -> Neuro2;
	Neuro1 -> Part1; Neuro2 -> Joint2;
	}
	\f]
	*/

class Model : public DestrBase
{
protected:
	Geno geno, f0geno;
	char modelfromgenotype;
	char f0genoknown;
	/// make model map in build()
	bool autobuildmaps;
	/// supports adding checkpoints
	bool using_checkpoints;
	/// means less strict validation
	bool is_checkpoint;
	/// valid if build from f0 genotype
	int f0errorposition;
	/// valid if build from f0 genotype
	int f0warnposition;

	ModelBuildStatus buildstatus;
	/// NULL if the map is not (yet) created
	MultiMap *map, *f0map;

	SList parts, joints, neurons;
	char partmappingchanged;
	vector<Model *> checkpoints;

	void internalCopy(const Model &mod);

	/// make the model from current genotype
	void build();

	friend class NeuroNetFactory;
	friend class VisualModel;
	friend class GLVisualModel;
	friend class Creature;
	friend class PartBase;

	int checklevel;

public:
	enum ShapeType { SHAPE_UNKNOWN, SHAPE_ILLEGAL, SHAPE_BALL_AND_STICK, SHAPE_SOLIDS };
	/// used in internalCheck()
	enum CheckType {
		EDITING_CHECK, ///< Used in Model::validate(). Default validation - does not modify elements of the Model.
		FINAL_CHECK,   ///< Used in Model::close() when a Model is built from a genotype. Like EDITING_CHECK, but also calculates Joint::d and Joint::rot.
		LIVE_CHECK,     ///< used in Model::close() when a Model is built from a Creature. Like FINAL_CHECK but does not limit joint length which could make some liveModels invalid.
		CHECKPOINT_CHECK     ///< used when storing checkpoint models. Like LIVE_CHECK, excluding consistency check (disjoint parts are acceptable)
	};
protected:
	ShapeType shape;

	SString nameForErrors() const;
	int internalcheck(CheckType check);

	void init(const Geno &srcgen, bool _using_checkpoints, bool _is_checkpoint);
	void init();

	void delMap();
	void delF0Map();
	void initMap();
	void initF0Map();

public:
	/** get current model state.
	\f[(dot)
	digraph M
	{
	node [fontsize=12]
	edge [fontsize=10]
	building [label="building = can be modified"]
	valid -> building [label="open()"]
	building -> valid [label="close()"]
	invalid -> building [label="open()"]
	building -> invalid [label="close() [failed]"]
	empty -> building [label="open()"]
	}
	\f]
	*/
	ModelBuildStatus getStatus() const { return buildstatus; }
	int isValid() const { return buildstatus == valid; }
	int getErrorPosition(bool includingwarnings = false);
	ShapeType getShapeType() const { return shape; }
	bool isUsingCheckpoints() const { return using_checkpoints; }
	bool isCheckpoint() const { return is_checkpoint; }

	void updateRefno(); // set ::refno for all elements

	int getCheckpointCount();
	Model *getCheckpoint(int i);

	/// The bounding box size. Valid if the model is valid. Read only.
	Pt3D size;

	SString vis_style;
	double startenergy;
	Callback delmodel_list;
	ModelUserTags userdata;

	/// Create empty model with invalid empty genotype
	Model();

	/** Create a model based on provided genotype
	   @param buildmaps if not 0, generate mapping information for the model.
	   default is 0, because mapping uses additional time and memory.
	   @see getMap()
	   */
	Model(const Geno &src, bool buildmaps = false, bool _using_checkpoints = false, bool _is_checkpoint = false);
	Model(const Model &mod, bool buildmaps = false, bool _using_checkpoints = false, bool _is_checkpoint = false);
	/** duplicate the model.
		the resulting object's status is 'building' (opened).
		@see getStatus()
		*/
	void operator=(const Model &source);

	/** move all elements from 'source' into our model object.
		'source' becomes empty after this operation.
		the model will be opened if it is not already open.
		@see addElementsFrom(const Model &source);
		*/
	void moveElementsFrom(Model &source);

	/** copy all elements from 'source' into our model object
		without affecting the 'source'.
		the model will be opened if it is not already open.
		@see moveElementsFrom(Model &source);
		*/
	void addElementsFrom(const Model &source)
	{
		Model m(source); moveElementsFrom(m);
	}

	void operator+=(const Model &source)
	{
		addElementsFrom(source);
	}

	~Model();

	/** @return source genotype.
		@warn source genotype will not automatically change
		when the model is modified. this behaviour is inconsistent
		with the previous release. use getF0Geno() if you need
		the updated genotype.
		@see getF0Geno(), setGeno()
		*/
	const Geno &getGeno() const;

	/// change source genotype
	void setGeno(const Geno &newgeno);

	/** @return f0 genotype - generated from current model state
		don't use between open()-close()
		*/
	const Geno getF0Geno();

	/// make f0 genotype from current construction (low level version of getF0Geno)
	void makeGeno(Geno &, MultiMap *map = 0, bool handle_defaults = true);

	/** @return Mapping from source genotype (0-based position in text) to model elements reference numbers.
		Read about how mappings work: http://www.framsticks.com/files/common/GeneticMappingsInArtificialGenomes.pdf
		The map can be empty if the mapping hasn't been requested earlier (in constructor)
		or the converters don't support mapping.
		If you create or modify the model using singleStepBuild() or direct manipulation
		the map will be not changed or created automatically - it is your responsibility.
		@see Model(const Geno &src,int buildmaps=0), singleStepBuild(), PartBase::addMapping()
		@see clearMap()
		@see convmap

		*/
	MultiMap &getMap();

	/** Read about how mappings work: http://www.framsticks.com/files/common/GeneticMappingsInArtificialGenomes.pdf
		@return mapping from f0 genotype (0-based position in text) to model elements reference numbers
		*/
	const MultiMap &getF0Map();

	/** discard all mapping information for this model.
		getMap().clear() also works, but it doesn't remove mappings from model elements.
		If there are any mappings, they will be incorporated into model map during close().
		@see close(), getMap(), PartBase::clearMapping()
		*/
	void clearMap();

	/** Generate mapping from the current genotype to the f0 genotype.
		This works only if both current and f0 maps are already known.
		Read about how mappings work: http://www.framsticks.com/files/common/GeneticMappingsInArtificialGenomes.pdf
		@see convmap
		*/
	void getCurrentToF0Map(MultiMap &m);

	void setValidationLevel(int level)
	{
		checklevel = level;
	}

	/// calculate location of the new part connected to the existing one
	/// using delta option
	Pt3D whereDelta(const Part &start, const Pt3D &rot, const Pt3D &delta);

	/// create the whole model from scratch, using current genotype
	void rebuild(bool buildmaps);

	/// setGeno(newgeno); rebuild();
	void rebuild(const Geno &newgeno, bool buildmaps) { setGeno(newgeno); rebuild(buildmaps); }

	/// reuse current model object but discard all model data
	void clear();

	enum ItemType { UnknownType, ModelType, PartType, JointType, NeuronType, NeuronConnectionType, CheckpointType };
	static ItemType itemTypeFromLinePrefix(const char *line);
	/** Execute single line of <B>f0</B> genotype.
		Return value is non-negative reference number of the created item,
		or negative value. reference number can be used to access
		the item using getPart(int), getJoint(int) and getNeuroItem(int) methods.
		@param line_num optional line number used in error messages
		@param srcrange source genotype range which will be mapped to this element
		*/
	int addFromString(ItemType item_type, const SString &singleline, int line_num, const MultiRange *srcrange = NULL);
	/** Execute single line of <B>f0</B> genotype - compatiblity variant */
	int addFromString(ItemType item_type, const SString &singleline, const MultiRange *srcrange = NULL);
	/** Execute single line of <B>f0</B> genotype - low level variant, used by Model::build(), error messages returned as string instead of calling logger */
	int addFromStringNoLog(ItemType item_type, const SString &singleline, SString &error_message, const MultiRange *srcrange = 0);

	/// separate build stages (for future use)
	void checkpoint();

	/// call resetDelta() on all joints
	void resetAllDelta();

	/// call useDelta() on all joints
	void useAllDelta(bool yesno);

	/// Final validity check of the model, all model data has to be available at this point.
	/// If the model was modified, the genotype will be also updated.
	/// It also calls "validate" with all side effects.
	/// @return > 0 means "valid"
	int close(bool building_live_model = false);

	/// Enable model building.
	/// You should use it if you need to create new model, modify the model after close
	/// or modify the model created from the genotype.
	/// Between open() and close() the model is not fully usable.
	void open(bool _using_checkpoints = false, bool _is_checkpoint = false);

	/// Current model written as f0 genotype while building
	/// (not cached, not validated, probably unusable and bad if used before close(). But good for debugging.)
	Geno rawGeno();

	/// partial validity check - you can use this call
	/// anytime between open - close.
	/// this function will check (and repair)
	/// - part-joint-neuro connections
	/// - model geometry (if "delta option" was used)
	/// - physical/biological limits
	/// @return 1 = valid
	/// @return 0 = invalid
	/// validate doesn't make the model fully usable (for simulation)
	/// you still need to use close if you have changed anything
	int validate();

	int getPartCount() const;
	/// you can access parts 0 .. getPartCount()-1.
	Part *getPart(int i) const;

	int getJointCount() const;
	/// you can access joints 0 .. getJointCount()-1.
	Joint *getJoint(int i) const;

	int getNeuroCount() const;
	int getConnectionCount() const;
	/// you can access neurons 0 .. getNeuroCount()-1.
	Neuro *getNeuro(int i) const;

	/** create new Part and add it to the model. @see addPart()  */
	Part *addNewPart(Part::Shape shape = Part::SHAPE_BALL_AND_STICK) { return addPart(new Part(shape)); }
	/** create new Joint and add it to the model. @see addJoint() */
	Joint *addNewJoint(Part *p1 = NULL, Part *p2 = NULL, Joint::Shape shape = Joint::SHAPE_BALL_AND_STICK) { Joint *j = addJoint(new Joint()); j->shape = shape; if ((p1 != NULL) && (p2 != NULL)) j->attachToParts(p1, p2); return j; }
	/** create new Neuro and add it to the model. @see addNeuro() */
	Neuro *addNewNeuro() { return addNeuro(new Neuro()); }

	/** add p to the model. p->refno is adjusted. @return the Part just added (==p). */
	Part *addPart(Part *p);
	/** add j to the model. j->refno is adjusted. @return the Joint just added (==j). */
	Joint *addJoint(Joint *j);
	/** add n to the model. n->refno is adjusted. @return the Neuro just added (==n). */
	Neuro *addNeuro(Neuro *n);

	/** remove the part from model.
		@param removeattachedjoints if not 0 -> remove all joints connected with this part
		@param removeattachedneurons if not 0 -> remove neurons attached to this part */
	void removePart(int partindex, int removeattachedjoints = 1, int removeattachedneurons = 1);

	/** remove the joint from model.
		@param removeattachedneurons if not 0 -> remove neurons attached to this joint */
	void removeJoint(int jointindex, int removeattachedneurons = 1);

	/** remove the neuron from model.
		@param removereferences if true -> look for references to this neuron
		(i.e. connections from other neurons) and remove them as well */
	void removeNeuro(int neuroindex, bool removereferences = true);

	void removeNeuros(SList &nlist);

	/// @return part index or -1 if not found in the model
	int findPart(Part *p);
	/// @return joint index or -1 if not found in the model
	int findJoint(Joint *j);
	/// @return neuro index or -1 if not found in the model
	int findNeuro(Neuro *nu);
	/// @return joint index or -1 if not found in the model
	int findJoint(Part *p1, Part *p2);

	/** make the list of neuros satisfying given search criteria: classname,part,joint
		@param result objects will be appended here
		@return number of objects found  */
	int findNeuros(SList &result, const char *classname = 0, const Part *part = 0, const Joint *joint = 0);

	/** search for joints connected to the part
		@param result objects will be appended here
		@return number of objects found  */
	int findJoints(SList &result, const Part *part = 0);

	void disturb(double amount);
	void move(const Pt3D &shift);
	/// rotate around the origin (move-rotate-move to rotate around arbitrary point)
	void rotate(const Orient &rotation);
	/// rotate around the origin (move-rotate-move to rotate around arbitrary point)
	void rotate(const Pt3D &angles) { Orient o = Orient_1; o.rotate(angles); rotate(o); }

	/// build this model using solid shape types, based on the provided ball-and-stick model. See also shapeconvert.cpp.
	void buildUsingSolidShapeTypes(const Model &src_ballandstick_shapes, Part::Shape use_shape = Part::SHAPE_CYLINDER, double thickness = 0.2);

protected:
	static const int MODEL_MAPPING_OFFSET = 0x10000000;
public:
	static int elementToMap(ItemType t, int i);
	struct TypeAndIndex
	{
		ItemType type; int index;
		TypeAndIndex() :type(UnknownType), index(0) {}
		TypeAndIndex(ItemType _type, int _index) :type(_type), index(_index) {}
	};
	static TypeAndIndex mapToElement(int i);
	static int partToMap(int i);
	static int jointToMap(int i);
	static int neuroToMap(int i);
	static int mapToPart(int i);
	static int mapToJoint(int i);
	static int mapToNeuro(int i);

	static void makeGenToGenMap(MultiMap &result, const MultiMap &gen1tomodel, const MultiMap &gen2tomodel);

	///////////////////////////

	static Part &getMinPart();
	static Part &getMaxPart();
	static Part &getDefPart();
	static Joint &getMinJoint();
	static Joint &getMaxJoint();
	static Joint &getDefJoint();
	static Neuro &getMinNeuro();
	static Neuro &getMaxNeuro();
	static Neuro &getDefNeuro();
};

/**
   An object of this class is created from a Model and returns the solids-type const Model& regardless of the source Model shape type.
   For solids-type Models, the same source Model reference is returned.
   For ball-and-stick-type Models, the new temporary Model is created (using Model::buildUsingSolidShapeTypes).
   Useful for making the solids-only code work for both solids Models and ball-and-stick Models, without if's and special cases, like in this example:

   void fun(const Model& input) // 'input' can be any shape type
   {
   SolidsShapeTypeModel converted(input); // 'converted' contains either 'input' or the new solids-type Model created from 'input'
   functionAcceptingSolidsTypeModel(converted); // operator const Model&() is called automatically because of the function signature
   int n=converted.getModel().getPartCount(); // getting the const Model& explicitly (converted.getPartCount() would fail)
   }
   */
class SolidsShapeTypeModel
{
public:
	Model *converted_model;
	Model *using_model;
	SolidsShapeTypeModel(Model &m, Part::Shape use_shape = Part::SHAPE_CYLINDER, double thickness = 0.2);
	operator Model &() { return *using_model; }
	Model &getModel() { return *using_model; }
	~SolidsShapeTypeModel() { if (converted_model) delete converted_model; }
};

#endif
