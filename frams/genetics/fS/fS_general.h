// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 2019-2020  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FS_GENERAL_H_
#define _FS_GENERAL_H_

#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <exception>
#include "frams/model/model.h"
#include "frams/util/multirange.h"

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
const char PARAM_SEPARATOR = ';';
const char PARAM_KEY_VALUE_SEPARATOR = '=';
#define NEURON_START '['
const char NEURON_END = ']';
const char NEURON_SEPARATOR = ';';
const SString NEURON_INTERNAL_SEPARATOR("_");
#define NEURON_I_W_SEPARATOR ':'
//@}

enum class SHIFT
{
	LEFT = -1,
	RIGHT = 1
};

/** @name Every modifier changes the underlying value by this multiplier */
const double MODIFIER_MULTIPLIER = 1.1;
/** @name In mutation parameters will be multiplied by at most this value */
const double PARAM_MAX_MULTIPLIER = 1.5;

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
#define SIZE "s"
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

const char ELLIPSOID = 'E';
const char CUBOID = 'C';
const char CYLINDER = 'R';
const std::unordered_map<Part::Shape, char> SHAPETYPE_TO_GENE = {
		{Part::Shape::SHAPE_ELLIPSOID, ELLIPSOID},
		{Part::Shape::SHAPE_CUBOID,    CUBOID},
		{Part::Shape::SHAPE_CYLINDER,  CYLINDER},
};

// This map is inverse to SHAPE_TO_SYMBOL. Those two should be compatible
const std::unordered_map<char, Part::Shape> GENE_TO_SHAPETYPE = {
		{ELLIPSOID, Part::Shape::SHAPE_ELLIPSOID},
		{CUBOID,    Part::Shape::SHAPE_CUBOID},
		{CYLINDER,  Part::Shape::SHAPE_CYLINDER},
};
const int SHAPE_COUNT = 3;    // This should be the count of SHAPETYPE_TO_GENE and GENE_TO_SHAPETYPE

const char DEFAULT_JOINT = 'a';
const string JOINTS = "bc";
const int JOINT_COUNT = JOINTS.length();
const string MODIFIERS = "IFS";
const char SIZE_MODIFIER = 's';
const vector<string> PARAMS {INGESTION, FRICTION, ROT_X, ROT_Y, ROT_Z, RX, RY, RZ, SIZE, SIZE_X, SIZE_Y, SIZE_Z,
							  JOINT_DISTANCE};

/** @name Default values of node parameters*/
static const Part defPart = Model::getDefPart();
const std::map<string, double> defaultParamValues = {
		{INGESTION,      defPart.ingest},
		{FRICTION,       defPart.friction},
		{ROT_X,          0.0},
		{ROT_Y,          0.0},
		{ROT_Z,          0.0},
		{RX,             0.0},
		{RY,             0.0},
		{RZ,             0.0},
		{SIZE,           1.0},
		{SIZE_X,         1.0},
		{SIZE_Y,         1.0},
		{SIZE_Z,         1.0},
		{JOINT_DISTANCE, 1.0}
};

/** @name Number of tries of performing a mutation before GENOPER_FAIL is returned */
#define mutationTries  20

class fS_Exception : public std::exception
{
	string msg;
public:

	int errorPosition;
	virtual const char *what() const throw()
	{
		return msg.c_str();
	}

	fS_Exception(string _msg, int _errorPosition)
	{
		msg = _msg;
		errorPosition = _errorPosition;
	}
};

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
	char *str;        // Pointer to the beginning of the substring
	int start;        // The beginning index of substring
	int len;        // The length of substring

	Substring(const char *_str, int _start, int _len)
	{
		str = (char *) _str + _start;
		start = _start;
		len = _len;
	}

	Substring(const Substring &other)
	{
		str = other.str;
		start = other.start;
		len = other.len;
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
		for (int i = 0; i < len; i++)
			if (str[i] == ch)
				return i;
		return -1;
	}

	void startFrom(int index)
	{
		str += index;
		start += index;
		len -= index;
	}

	void shortenBy(int charCount)
	{
		len = std::max(len - charCount, 0);
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
	double s = 1.0;      /// Size multipliers

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
class fS_Neuron: public Neuro
{
public:
	std::map<int, double> inputs;

	fS_Neuron(const char *str, int start, int length);

	bool acceptsInputs()
	{
		return getClass()->prefinputs < int(inputs.size());
	}
};

/**
 * Represents a node in the graph that represents a genotype.
 * A node corresponds to a single part.
 * However, it also stores attributes that are specific to fS encoding, such as modifiers and joint types.
 */
class Node
{
	friend class fS_Genotype;

	friend class GenoOper_fS;

private:
	Substring *partDescription = nullptr;
	bool cycleMode, modifierMode, paramMode; /// Possible modes
	Node *parent;
	Part *part;     /// A part object built from node. Used in building the Model
	int partCodeLen; /// The length of substring that directly describes the corresponding part

	std::map<string, double> params; /// The map of all the node params
	vector<Node *> children;    /// Vector of all direct children
	std::map<char, int> modifiers;     /// Vector of all modifiers
	char joint = DEFAULT_JOINT;           /// Set of all joints
	vector<fS_Neuron *> neurons;    /// Vector of all the neurons

	static double calculateRadiusFromVolume(Part::Shape partType, double volume)
	{
		double result;
		switch (partType)
		{
			case Part::Shape::SHAPE_CUBOID:
				result = std::cbrt(volume / 8.0);
				break;
			case Part::Shape::SHAPE_CYLINDER:
				result = std::cbrt(volume / (2.0 * M_PI));
				break;
			case Part::Shape::SHAPE_ELLIPSOID:
				result = std::cbrt(volume / ((4.0 / 3.0) * M_PI));
				break;
			default:
				logMessage("fS", "calculateVolume", LOG_ERROR, "Invalid part type");
		}
		return result;
	}

	void cleanUp();

	Pt3D getRotation();

	Pt3D getVectorRotation();

	bool isPartSizeValid();

	bool hasPartSizeParam();

	/**
	 * Get the position of part type in genotype
	 *
	 * @return the position of part type
	 */
	int getPartPosition(Substring &restOfGenotype);

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
	vector<Substring> getBranches(Substring &restOfGenotype);

	/**
	 * Get phenotypic state that derives from ancestors.
	 * Used when building model
	 * @param _state state of the parent
	 */
	void getState(State *_state, const Pt3D &parentSize);

	/**
	 * Build children internal representations from fS genotype
	 * @param restOfGenotype part of genotype that describes the subtree
	 */
	void getChildren(Substring &restOfGenotype);

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
	void addJointsToModel(Model &model, Node *parent);

	/**
	 * Get all the nodes from the subtree that starts in this node
	 * @param reference to vector which contains nodes
	 */
	void getAllNodes(vector<Node *> &allNodes);


	/**
	 * Build model from the subtree that starts in this node
	 * @param pointer to model
	 */
	void buildModel(Model &model, Node *parent);

public:
	Part::Shape partType;  /// The type of the part
	State *state = nullptr; /// The phenotypic state that inherits from ancestors

	Node(Substring &genotype, bool _modifierMode, bool _paramMode, bool _cycleMode, Node *parent);

	~Node();

	/**
	 * Get fS representation of the subtree that starts from this node
	 * @param result the reference to an object which is used to contain fS genotype
	 */
	void getGeno(SString &result);

	/**
	 * Calculate the effective size of the part (after applying all multipliers and params)
	 * @return The effective size
	 */
	Pt3D calculateSize();

	/**
	 * Calculate the effective volume of the part
	 * @return The effective volume
	 */
	double calculateVolume();

	/**
	 * Change the value of the size parameter by given multiplier
	 * Do not change the value if any of the size restrictions is not satisfied
	 * @param paramKey
	 * @param multiplier
	 * @param ensureCircleSection
	 * @return True if the parameter value was change, false otherwise
	 */
	bool changeSizeParam(string paramKey, double multiplier, bool ensureCircleSection);

	/**
	 * Counts all the nodes in subtree
	 * @return node count
	 */
	int getNodeCount();

	/**
	 * Extract the value of parameter or return default if parameter not exists
	 * @return the param value
	 */
	double getParam(string key);
};

/**
 * Represents an fS genotype.
 */
class fS_Genotype
{
	friend class Node;

	friend class GenoOper_fS;

private:
	/**
	 * Draws a node that has an index greater that specified
	 * @param fromIndex minimal index of the node
	 * @return pointer to drawn node
	 */
	Node *chooseNode(int fromIndex=0);

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
	Node *startNode = nullptr;    /// The start (root) node. All other nodes are its descendants


	static int precision;
	static bool TURN_WITH_ROTATION;

	/**
	 * Build internal representation from fS format
	 * @param genotype in fS format
	 */
	fS_Genotype(const string &genotype);

	~fS_Genotype();

	void getState();

	/**
	 * Get a random multiplier for parameter mutation
	 * @return Random parameter multiplier
	 */
	static double randomParamMultiplier();

	/**
	 * Get all existing nodes
	 * @return vector of all nodes
	 */
	vector<Node *> getAllNodes();

	/**
	 * Get all the neurons from the subtree that starts in given node
	 * @param node The beginning of subtree
	 * @return The vector of neurons
	 */
	static vector<fS_Neuron *> extractNeurons(Node *node);

	/**
	 * Get the index of the neuron in vector of neurons
	 * @param neurons
	 * @param changedNeuron
	 * @return
	 */
	static int getNeuronIndex(vector<fS_Neuron *> neurons, fS_Neuron *changedNeuron);

	/**
	 * Left- or right- shift the indexes of neuro connections by the given range
	 * @param neurons
	 * @param start The beginning of the range
	 * @param end The end of the range
	 * @param shift
	 */
	static void shiftNeuroConnections(vector<fS_Neuron *> &neurons, int start, int end, SHIFT shift);

	/**
	 * Get all existing neurons
	 * @return vector of all neurons
	 */
	vector<fS_Neuron *> getAllNeurons();

	/**
	 * Counts all the nodes in genotype
	 * @return node count
	 */
	int getNodeCount();

	/**
	 * Check if sizes of all parts in genotype are valid
	\retval error_position 1-based
	\retval 0 when all part sizes are valid
	 */
	int checkValidityOfPartSizes();

	void validateNeuroInputs();

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
	void rearrangeNeuronConnections(fS_Neuron *newNeuron, SHIFT shift);

};


#endif
