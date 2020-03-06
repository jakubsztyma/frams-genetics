// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FH_GENERAL_H_
#define _FH_GENERAL_H_

#include <vector>
#include "../../model/modelparts.h"
#include <frams/util/sstring.h>
#include <frams/param/mutableparam.h>
#include <frams/param/param.h>
#include <frams/model/model.h>
#include <frams/param/paramobj.h>

/** @name Constants used in fH methods */
//@{
#define HANDLE_VECTOR_TYPE "f -1.0 1.0 0.0" ///<Vector values type definition
#define STICKH_LENGTH_TYPE "f 0.001 1.999 1.0" ///<Length of stick handle. Minimum should not be equal to 0, because calculating direction of next part from current stick with length 0 would be impossible
#define FH_PART_PROPS_COUNT   4 ///<Count of part properties
#define FH_JOINT_PROPS_COUNT  3 ///<Count of joint properties
#define FH_PE_NEURO_DET       "d" ///<Id of details type definition in f0_neuro_paramtab
#define FH_PE_CONN_WEIGHT     "w" ///<Id of weight type definition in f0_neuroconn_paramtab
//@}

extern const char *fL_part_names[];

extern const char *fL_joint_names[];

class Geno_fH; // from oper.fH

/// Determines to which class Handle belongs.
enum fHBodyType
{
	JOINT, ///<joint type
	NEURON, ///<neuron type
	CONNECTION ///<connection type
};

/**
 * Base class for all kinds of handles in fH encoding.Each line of fH genotype
 * describes a handle. Handles can be divided into joints, neurons and
 * connections. Every handle is described with 2 vectors of 'dimensions' values.
 * During process of body development those vectors are compared in order to
 * connect sticks of creature, connect neurons to body parts, and create connections
 * between neurons.
 */
class fH_Handle
{
private:
	int dimensions; ///< number of elements in each vector of a handle. This is read-only field
protected:
	static double dist(std::vector<double> left, std::vector<double> right);
public:
	fHBodyType type; ///< type of handle, can be stick, neural connection or neuron
	std::vector<double> first; ///< first vector of handle
	std::vector<double> second; ///< second vector of handle

	int begin; ///<begin of handle definition in genotype, used in mapping
	int end; ///<end of handle definition in genotype, used in mapping

	void *obj; ///<pointer to all properties that can be accessed via Param object

	/**
	 * Prepares memory for both handle vectors, according to value in 'dimensions'
	 * variable.
	 * @param type type of handle
	 * @param dimensions number of elements in each vector of handle
	 */
	fH_Handle(fHBodyType type, int dimensions, int begin, int end) :
		dimensions(dimensions),
		type(type),
		first(dimensions, 0),
		second(dimensions, 0),
		begin(begin),
		end(end),
		obj(NULL)
	{}

	int getDimensions() { return dimensions; }

	virtual ~fH_Handle() { if (obj) ParamObject::freeObject(obj); }

	/**
	 * Loads properties of handle after parsing them from genotype.
	 * All properties are available from Param object. This base method only
	 * loads values for handle vectors. Other properties, like stiffness for
	 * stick require overloading this method. The method presumes that values
	 * for vectors are first in ParamTab.
	 * @param par Param object holding parsed properties from genotype
	 */
	void loadProperties(Param par);

	/**
	 * Saves properties from handle into ParamObject selected by Param.
	 * @param par Param object that has needed paramtab configured and selected ParamObject
	 */
	void saveProperties(Param &par);

	/**
	 * Calculates distance between current and given handle. Method should
	 * be reimplemented in inheriting classes in order to provide valid values.
	 * @param right second handle to which distance is calculated
	 * @return distance between this and second handle
	 */
	virtual double distance(fH_Handle * right) = 0;


	/**
	 * Computes average of first and second vector of Handle.
	 * @return vector with average values from both handle vectors
	 */
	std::vector<double> getVectorsAverage();
};

/**
 * Class representing stick handle of fH encoding. Every stick has following properties:
 *  - double stif -  joint property of a stick representing stiffness
 *  - double rotstif -  joint property of a stick representing rotation stiffness
 *  - double stamina -  joint property of a stick representing stamina
 *  - double density -  parts property of a stick representing density
 *  - double friction -  parts property of a stick representing friction
 *  - double ingest -  parts property of a stick representing ingestion
 *  - double assimilation -  parts property of a stick representing assimilation
 *  - double length -  length of a stick
 */
class fH_StickHandle : public fH_Handle
{
public:
	Part *firstpart; ///< first part of created stick
	Part *secondpart; ///< second part of created stick
	Joint *joint; ///< joint connecting both parts of created stick

	// vectors used for connecting neurons to parts
	vector<double> firstparthandle; ///< vector for first part. Used in connecting neurons to parts. Inherited from previous stick
	vector<double> secondparthandle; ///< vector for second part. Used in connecting neurons to parts. Vector is calculated as average of second vector in current stick handle and first vector of all sticks connected to this part

	/**
	 * This constructor initializes each property of joint and parts of a stick
	 * with default values represented in f0 genotype.
	 * @param dimensions number of elements in each vector of handle
	 */
	fH_StickHandle(int dimensions, int begin, int end) :
		fH_Handle(fHBodyType::JOINT, dimensions, begin, end),
		firstparthandle(dimensions, 0),
		secondparthandle(dimensions, 0)
	{
		firstpart = secondpart = NULL;
		joint = NULL;
	}

	/**
	 * Calculates distance between current and given handle. Distance from
	 * StickHandle to StickHandle is calculated as distance between second
	 * vector of current handle and first vector of given handle.
	 * Distance from StickHandle to NeuronHandle is designed only for neurons
	 * that need to be attached to joints and is computed as distance between
	 * average of first and second vector for each handle.
	 * Distance between StickHandle and ConnectionHandle cannot be calculated.
	 * @param right second handle to which distance is calculated
	 * @return Euclidean distance between two vectors
	 */
	double distance(fH_Handle *right);
	/**
	 * Creates new part from current stick and its children. Part properties
	 * are calculated as mean of properties of current stick and properties
	 * of other sticks that share this part. Method requires model to add
	 * created part and map genotype responsible for this part.
	 * @param tab Param tab object for access to variables
	 * @param children stick handles that share created part
	 * @param model pointer to model that is being developed in buildModel method
	 * @param createmapping true if mapping should be added to created Model element
	 * @return created part
	 */
	Part *createPart(ParamEntry *tab, std::vector<fH_StickHandle *> *children, Model *model, bool createmapping);

	/**
	 * Creates new joint from current stick. Method should be called after
	 * creating parts with createPart method. Method sets properties of joint
	 * according to genotype values and connects both parts of stick.
	 * @param tab Param tab object for access to variables
	 * @param model pointer to model that is being developed in buildModel method
	 * @param createmapping true if mapping should be added to created Model element
	 * @return pointer to created joint or NULL if one or both of stick parts were not created
	 */
	Joint *createJoint(ParamEntry *tab, Model *model, bool createmapping);

	~fH_StickHandle() {}
};

/**
 * Class representing neuron handle of fH encoding. It has one property, called
 * details.
 */
class fH_NeuronHandle : public fH_Handle
{
public:
	Neuro *neuron; ///< pointer to created neuron from handle

	fH_NeuronHandle(int dimensions, int begin, int end) :
		fH_Handle(fHBodyType::NEURON, dimensions, begin, end)
	{
		neuron = NULL;
	}

	/**
	 * Calculates distance between current and given handle.
	 * Distance from StickHandle to NeuronHandle is designed only for neurons
	 * that need to be attached to joints and is computed as distance between
	 * average of first and second vector for each handle.
	 * Distance from current NeuronHandle to a given ConnectionHandle is used
	 * only for neurons that should act as inputs. Distance is calculated
	 * as Euclidean distance between second vector of NeuronHandle and first
	 * vector of ConnectionHandle. In order to calculate distance between
	 * connection and output neuron use distance function implemented in
	 * ConnectionHandle class.
	 * Distance between two NeuronHandles cannot be calculated
	 * @param right second handle to which distance is calculated
	 * @return Euclidean distance between two vectors
	 */
	double distance(fH_Handle *right);

	/**
	 * Calculate distance between neuron and given part.
	 * In this case distance between neuron and part is calculated as Euclidean
	 * distance between average of two vectors of NeuronHandle and average of
	 * second vector of given StickHandle and all first vectors of StickHandles
	 * that share this part with given StickHandle. First parameter determines
	 * if method should calculate distance for first or second part.
	 * @param right second handle to which distance is calculated
	 * @param first true if method should calculate distance between current neuron and first part of StickHandle, false if method should calculate distance between current neuron and second part
	 * @return Euclidean distance between two vectors
	 */
	double distance(fH_StickHandle *right, bool first);
	~fH_NeuronHandle() {}
};

/**
 * Class representing connection handle of fH encoding. It has one property, called
 * weight.
 */
class fH_ConnectionHandle : public fH_Handle
{
public:

	fH_ConnectionHandle(int dimensions, int begin, int end) :
		fH_Handle(fHBodyType::CONNECTION, dimensions, begin, end) {}

	/**
	 * This method can calculate distance only between current connection and
	 * NeuronHandle.
	 * Distance from current ConnectionHandle to a given NeuronHandle is used
	 * only for neurons that should act as outputs. Distance is calculated
	 * as Euclidean distance between second vector of ConnectionHandle and first
	 * vector of NeuronHandle. In order to calculate distance between
	 * input neuron and connection use distance function implemented in
	 * NeuronHandle class.
	 * @param right second handle to which distance is calculated
	 * @return Euclidean distance between two vectors
	 */
	double distance(fH_Handle *right);

	~fH_ConnectionHandle() {}
};

/**
 * Main class for parsing fH genotypes. It reads all genotype, prepares
 * all objects required for parsing it and holds prepared handles for further
 * development of a creature.
 */
class fH_Builder
{
private:
	friend class ::Geno_fH; // Geno_fH class requires access to private fields of Builder for performing mutations and cross-over
	int dimensions; ///<number of dimensions for this genotype
	bool createmapping; ///<determines if mapping should be created or not

	std::vector<fH_StickHandle*> sticks; ///<vector holding sticks of a genotype
	std::vector<fH_NeuronHandle*> neurons; ///<vector holding neurons of a genotype
	std::vector<fH_ConnectionHandle*> connections; ///<vector holding connections of a genotype

	MutableParam stickmut; ///<object holding all generated properties for sticks in genotype
	MutableParam neuronmut; ///<object holding all generated properties for neurons in genotype
	MutableParam connectionmut; ///<object holding all generated properties for connections in genotype

	ParamEntry *stickparamtab; ///< ParamTab generated from stickmut
	ParamEntry *neuronparamtab; ///< ParamTab generated from neuronmut
	ParamEntry *connectionparamtab; ///< ParamTab generated from connectionmut

	std::vector<std::pair<fH_StickHandle *, fH_StickHandle *>> stickconnections; ///< this vector holds all connections determined by development of fH body in method buildBody
	std::vector<int> sticksorder; ///< vector holds ids of StickHandles in 'sticks' vector in order of adding sticks to a body during buildBody

	/**
	 * Processes single line of genotype. This line should begin with
	 * "j:", "n:" or "c:". Otherwise it will be considered as error.
	 * @param line line of a genotype
	 * @param linenumber number of line in genotype, required for error messages
	 * @return 0 if processing was successful, 1 if type of handle could not be determined or loading of properties failed
	 */
	int processLine(SString line, int linenumber, int begin, int end);

	/**
	 * Prepares ParamTab objects according to a given number of dimensions.
	 */
	void prepareParams();

	/**
	 * Connects floating sticks into consistent body. It does not create Model,
	 * it only fills 'stickconnections' with pairs of connected sticks and
	 * 'stickorder' with order of adding sticks to body.
	 */
	void buildBody();

	/**
	 * Creates neurons and neural connections between them, according to
	 * NeuronHandles and ConnectionHandles. Needs to be called after developing
	 * body with buildBody function and adding parts and joints to Model given
	 * in input parameter of method. Method connects neurons, attaches them
	 * to parts or joints and save mappings from genotype to model.
	 * @param model currently created model by Builder with parts and joints created by sticks in buildBody
	 * @param createmapping true if mapping should be added to created Model element
	 * @return 0 if brain was developed successfully, -1 if one of neurons had invalid class
	 */
	int developBrain(Model *model, bool createmapping);

	/**
	 * Calculates direction in which new joint should develop. When there
	 * is more than one joint coming out from current part, all parts with
	 * joints starting from the same point should be as far away from each
	 * other as possible. This problem can be defined as even distribution
	 * of points on a sphere. This method, called improved RSZ method offers
	 * non-iterative and fast calculation of such directions. Name of method
	 * comes from creators Rakhmanov, Saff and Zhou.
	 * Method works partially on spherical coordinates (r and theta is used).
	 * Z coordinate is from Cartesian coordinate system. Golden angle is used
	 * to "iterate" along spiral, while Z coordinate is used to move down the
	 * sphere.
	 * Method requires two parameters - count of all joints that share same
	 * part, and number of current joint to calculate.
	 *
	 * Full description of algorithm is in article:
	 * Mary K. Arthur, "Point Picking and Distributing on the Disc and Sphere",
	 * Army Research Laboratory, July 2015
	 *
	 * @param count number of joints that share same sphere
	 * @param number number of joint in array of joints
	 * @return direction where new joint should point
	 */
	static Pt3D getNextDirection(int count, int number);

	/** TODO move to the Orient class?
	 * Creates rotation matrix that aligns current vector to some expected
	 * vector. Method is used with RSZ method. When method calculates n+1
	 * evenly distributed points (that will act as ends of vectors representing
	 * n joints directions), 0 point is acting as direction of previous joint.
	 * In order to properly use directions of new joints they need to be
	 * multiplied by rotation matrix that aligns direction represented by
	 * 0 point to direction pointing from shared part to second part of parent
	 * joint.
	 * Let:
	 * - cross product of a and b
	 * \f[ v = a \times b \f]
	 * - sine of angle
	 * \f[ s = ||v|| \f]
	 * - cosine of angle
	 * \f[ c = a \cdot b \f]
	 * - skew-symmetric cross-product matrix of \f$ v \f$
	 * \f[
	 * [v]_\times =
	 * \begin{bmatrix}
	 * 0 & -v_3 & v_2 \\
	 * v_3 & 0 & -v_1 \\
	 * -v_2 & v_1 & 0
	 * \end{bmatrix}
	 * \f]
	 *
	 * Then rotation matrix is following:
	 * \f[ R = I + [v]_{\times} + [v]_{\times}^{2}\frac{1}{1+c} \f]
	 * @param currdir current direction of vector
	 * @param expecteddir where vector should point
	 * @return rotation matrix
	 */
	static Orient getRotationMatrixToFitVector(Pt3D currdir, Pt3D expecteddir);

public:
	fH_Builder(int dimensions = 0, bool createmapping = true) : dimensions(dimensions),
		createmapping(createmapping),
		stickmut("Stick handle", "Properties"),
		neuronmut("Neuron handle", "Properties"),
		connectionmut("Connection handle", "Properties")
	{
		stickparamtab = neuronparamtab = connectionparamtab = NULL;
		if (dimensions > 0)
		{
			prepareParams();
		}
	}

	~fH_Builder();

	/**
	 * Parses fH genotype, starting with line holding number of dimensions for
	 * this genotype. Secondly, it prepares ParamTabs for sticks, neurons and
	 * connections in order to parse following lines. During parsing method
	 * creates handle for each nonempty line and stores in vector matching
	 * its type.
	 * @param genotype fH genotype
	 * @return 0 if processing was successful, 1 if parsing of dimensions went wrong or one of genotype lines could not be parsed
	 */
	int parseGenotype(const SString &genotype);

	/**
	 * Removes neurons with invalid class names. This method is used in
	 * repairing the genotype. It should be called before building Model,
	 * otherwise Model will return failure and validation will show error.
	 * @return number of neurons removed from genotype
	 */
	int removeNeuronsWithInvalidClasses();

	/**
	 * Builds model for current fH genotype. Should be called after parseGenotype
	 * method.
	 * @param using_checkpoints true if checkpoints should be generated, false otherwise
	 */
	Model* buildModel(bool using_checkpoints);

	/**
	 * Converts Handle objects into strings with validated properties by
	 * Paramtab helper classes. To ensure toString method will return repaired
	 * genotype methods parseGenotype(...), remoeNeuronsWithInvalidClasses()
	 * and buildModel(...) should be called. If buildModel will return NULL
	 * then toString method will not return proper output.
	 * @return genotype held in Builder, with minor repairs of properties and removed neurons with invalid class names
	 */
	SString toString();

	/**
	 * Adds handle to proper handles vector.
	 * @param handle handle to add
	 */
	void addHandle(fH_Handle *handle);

	/**
	 * Checks if there is any stick in creature.
	 * @return true if there is at least one stick, false otherwise
	 */
	bool sticksExist() { return sticks.size() > 0; }

	/**
	 * Returns Param for a given handle type.
	 * @param type type of handle
	 * @return proper Param object
	 */
	ParamEntry* getParamTab(fHBodyType type);
};

#endif //_FH_GENERAL_H_
