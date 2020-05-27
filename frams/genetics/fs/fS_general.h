//
// Created by jakub on 21.02.2020.
//

#ifndef _FS_GENERAL_H_
#define _FS_GENERAL_H_

#include <iostream>

#include <float.h>
#include <vector>
#include <map>
#include <set>
#include <math.h>
#include "common/Convert.h"
#include "common/nonstd_math.h"
#include "frams/genetics/genooperators.h"
#include "frams/util/3d.h"
#include "frams/util/sstring.h"
#include "frams/model/model.h"
#include "frams/util/multirange.h"
#include "frams/util/rndutil.h"
#include "frams/util/sstringutils.h"
#include "frams/util/extvalue.h"
#include "frams/neuro/neurolibrary.h"


/** @name Names of genotype modes */
//@{
#define MODIFIER_MODE 'M'
#define PARAM_MODE 'S'
#define CYCLE_MODE 'J'
//@}

/** @name Values of constants used in encoding */
//@{
#define BRANCH_START '('
#define BRANCH_END ')'
#define BRANCH_SEPARATOR ','
#define PARAM_START '{'
#define PARAM_END '}'
const char PARAM_SEPARATOR =';';
const char PARAM_KEY_VALUE_SEPARATOR='=';
#define NEURON_START '['
const char NEURON_END=']';
const char NEURON_SEPARATOR =';';
const SString NEURON_INPUT_SEPARATOR("_");
#define NEURON_I_W_SEPARATOR ':'
//@}

/** @name Every modifier changes the underlying value by this multiplier */
const float MODIFIER_MULTIPLIER = 1.1;
/**
 * Used in finding the proper distance between the parts
 * distance between spheres / sphere radius
 * That default value can be changed in certain cases
 * */
const float SPHERE_RELATIVE_DISTANCE = 0.25;
/**
 * Used in finding the proper distance between the parts
 * The maximal allowed value for
 * maximal radius of the node / sphere radius
 */
const int MAX_DIAMETER_QUOTIENT = 30;
/**
 * The tolerance of the value of distance between parts
 */
const double SPHERE_DISTANCE_TOLERANCE = 0.99;

/** @name Names of node parameters and modifiers*/
//@{
#define INGESTION "i"
#define FRICTION "f"
#define SIZE_X "x"
#define SIZE_Y "y"
#define SIZE_Z "z"
#define ROT_X "tx"
#define ROT_Y "ty"
#define ROT_Z "tz"
#define RX "rx"
#define RY "ry"
#define RZ "rz"
#define JOINT_DISTANCE "j"
//@}
/** @name Macros and values used in collision detection */
//@{
#define DISJOINT 0
#define COLLISION 1
#define ADJACENT 2
//@}

#define HINGE_X 'b'
#define HINGE_XY 'c'

const double DEFAULT_NEURO_CONNECTION_WEIGHT = 1.0;
const string PART_TYPES = "EPC";
const string JOINTS = "bc";
const int JOINT_COUNT = JOINTS.length();
const string MODIFIERS = "ifxyz";
const vector<string> PARAMS {INGESTION, FRICTION, ROT_X, ROT_Y, ROT_Z, RX, RY, RZ, SIZE_X, SIZE_Y, SIZE_Z,
							  JOINT_DISTANCE};

/** @name Number of tries of performing a mutation before GENOPER_FAIL is returned */
#define mutationTries  20

/**
 * Draws an integer value from given range
 * @param to maximal value
 * @param from minimal value
 * @return Drawn value
 */
int randomFromRange(int to, int from);

/**
 * Represents a substring of a larger string.
 * The reference to the original string is stored along with indexes of beginning end length of the substring.
 */
class Substring
{
public:
	char *str;		// Pointer to the beginning of the substring
	int start;        // The beginning index of substring
	int len;        // The length of substring

	Substring(const char *_str, int _start, int _len)
	{
		str = (char*)_str + _start;
		start = _start;
		len = _len;
	}

	const char *c_str()
	{
		return str;
	}

	SString substr(int relativeStart, int len)
	{
		const char *substrStart = str + relativeStart;
		return SString(substrStart, len);
	}

	int indexOf(char ch)
	{
		for(int i=0; i<len; i++)
			if(str[i] == ch)
				return i;
		return -1;
	}

	void startFrom(int index)
	{
		str += index;
		start += index;
		len -= index;
	}

	char at(int index)
	{
		return str[index];
	}

	/**
	 * Create a new instance of multirange, corresponding to the substring
	 * @return a created multirange
	 */
	MultiRange toMultiRange()
	{
		MultiRange range;
		range.add(start, start + len - 1);
		return range;
	}
};

/**
 * Stores the state of the node.
 * The state consists od current location, the direction in which the branch develops
 * and the current default values of the parameters (default values can be changed by modifiers).
 */
class State
{
public:
	Pt3D location;  /// Location of the node
	Pt3D v;         /// The normalised vector in which current branch develops
	double fr = 1.0;      /// Friction multiplier
	double ing = 1.0;      /// Ingestion multiplier
	double sx = 1.0, sy = 1.0, sz = 1.0;      /// Size multipliers

	State(State *_state); /// Derive the state from parent

	State(Pt3D _location, Pt3D _v); /// Create the state from parameters

	/**
	 * Add the vector of specified length to location
	 * @param length the length of the vector
	 */
	void addVector(const double length);

	/**
	 * Rotate the vector by specified values
	 * @param rx rotation by x axis
	 * @param ry rotation by y axis
	 * @param rz rotation by z axis
	 */
	void rotate(const Pt3D &rotation);
};

/**
 * Represent a neuron and its inputs
 */
class Neuron
{
public:
	SString cls;
	std::map<int, double> inputs;

	Neuron(const char* str, int length);

	Neuron(char neuronType);

	Neuron(){};
};

/**
 * Represents a node in the graph that represents a genotype.
 * A node corresponds to a single part.
 * However, it also stores attributes that are specific to fS encoding, such as modifiers and joint types.
 */
class Node
{
	friend class fS_Genotype;

	friend class fS_Operators;

private:
	Substring *partDescription;
	bool cycleMode, modifierMode, paramMode; /// Possible modes
	bool isStart;   /// Is a starting node of whole genotype
	char partType; /// The type of the part (E, P, C)
	Part *part;     /// A part object built from node. Used in building the Model
	unsigned int childSize = 0; /// The number of direct children
	unsigned int partCodeLen; /// The length of substring that directly describes the corresponding part

	std::map<string, double> params; /// The map of all the node params
	vector<Node *> children;    /// Vector of all direct children
	vector<char> modifiers;     /// Vector of all modifiers
	std::set<char> joints;           /// Set of all joints
	vector<Neuron*> neurons;    /// Vector of all the neurons

	Pt3D getSize();

	Pt3D getRotation();

	Pt3D getVectorRotation();

	/**
	 * Get the position of part type in genotype
	 *
	 * @return the position of part type
	 */
	int getPartPosition(Substring restOfGenotype);

	/**
	 * Extract the value of parameter or return default if parameter not exists
	 * @return the param value
	 */
	double getParam(string key);

	/**
	 * Extract modifiers from the rest of genotype
	 * @return the remainder of the genotype
	 */
	void extractModifiers(Substring &restOfGenotype);

	/**
	 * Extract part type from the rest of genotype
	 * @return the remainder of the genotype
	 */
	void extractPartType(Substring &restOfGenotype);

	/**
	 * Extract neurons from the rest of genotype
	 * @return the remainder of the genotype
	 */
	void extractNeurons(Substring &restOfGenotype);

	/**
	 * Extract params from the rest of genotype
	 * @return the length og the remainder of the genotype
	 */
	 void extractParams(Substring &restOfGenotype);

	/**
	 * Extract child branches from the rest of genotype
	 * @return vector of child branches
	 */
	vector<Substring> getBranches(Substring restOfGenotype);

	/**
	 * Get phenotypic state that derives from ancestors.
	 * Used when building model
	 * @param _state state of the parent
	 */
	void getState(State *_state, Pt3D parentSize);

	/**
	 * Build children internal representations from fS genotype
	 * @param restOfGenotype part of genotype that describes the subtree
	 */
	void getChildren(Substring restOfGenotype);

	/**
	 * Create part object from internal representation
	 */
	void createPart();

	/**
	 * Add joints between current node and the specified child
	 * Used in building model
	 * @param mode pointer to build model
	 * @param child pointer to the child
	 */
	void addJointsToModel(Model &model, Node *child);

	/**
	 * Get all the nodes from the subtree that starts in this node
	 * @param reference to vector which contains nodes
	 */
	void getAllNodes(vector<Node *> &allNodes);

	/**
	 * Build model from the subtree that starts in this node
	 * @param pointer to model
	 */
	Part *buildModel(Model &model);

public:
	State *state = nullptr; /// The phenotypic state that inherits from ancestors

	Node(Substring &genotype, bool _modifierMode, bool _paramMode, bool _cycleMode, bool _isStart);

	~Node();

	/**
	 * Get fS representation of the subtree that starts from this node
	 * @param result the reference to an object which is used to contain fS genotype
	 */
	void getGeno(SString &result);

	/**
	 * Counts all the nodes in subtree
	 * @return node count
	 */
	int getNodeCount();
};

/**
 * Represents an fS genotype.
 */
class fS_Genotype
{
	friend class Node;

	friend class fS_Operators;

private:
	Node *startNode = nullptr;    /// The start (root) node. All other nodes are its descendants

	/**
	 * Get all existing nodes
	 * @return vector of all nodes
	 */
	vector<Node *> getAllNodes();

	/**
	 * Draws a node that has an index greater that specified
	 * @param fromIndex minimal index of the node
	 * @return pointer to drawn node
	 */
	Node *chooseNode(int fromIndex);

	/**
	 * Draws a value from defined distribution
	 * @return Drawn value
	 */
	void randomFromDistribution();

	/**
	 * Find a node that is nearest (euclidean distance to specified node) and is not a child of specified node
	 * @return Nearest node
	 */
	Node *getNearestNode(vector<Node *> allNodes, Node *node);

public:
	/**
	 * Get all existing neurons
	 * @return vector of all neurons
	 */
	vector<Neuron *> getAllNeurons();

	/**
	 * Counts all the nodes in genotype
	 * @return node count
	 */
	int getNodeCount();

	/**
	 * Build internal representation from fS format
	 * @param genotype in fS format
	 */
	fS_Genotype(const SString &genotype);

	~fS_Genotype();

	/**
	 * Builds Model object from internal representation
	 * @param a reference to a model that will contain a built model
	 */
	void buildModel(Model &model);

	/**
	 * Adds neuro connections to model
	 * @param a reference to a model where the connections will be added
	 */
	void buildNeuroConnections(Model &model);

	/**
	 * @return genotype in fS format
	 */
	SString getGeno();

	/**
	 * After creating or deleting a new neuron, rearrange other neurons so that the inputs match
	 */
	void rearrangeNeuronConnections(Neuron *newNeuron, int shift);

	/**
	 * Performs add joint mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool addJoint();

	/**
	 * Performs remove mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool removeJoint();

	/**
	 * Performs add part mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool addPart();

	/**
	 * Performs change part type mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool changePartType();

	/**
	 * Performs remove part type mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool removePart();

	/**
	 * Performs add param mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool addParam();

	/**
	 * Performs remove param mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool removeParam();

	/**
	 * Performs change param mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool changeParam();

	/**
	 * Performs add modifier mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool addModifier();

	/**
	 * Performs remove modifier mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool removeModifier();

	bool addNeuro();

	bool removeNeuro();

	bool changeNeuroConnection();

	bool addNeuroConnection();

	bool removeNeuroConnection();

	bool changeNeuroParam();
};


#endif
