// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <string>
#include <limits>
#include <algorithm>
#include <frams/util/multirange.h>
#include <utility>
#include "fH_general.h"

using namespace std;
#undef max //this macro is not needed here and it clashes with numeric_limits<>::max()



// Methods for loading handles

const char *fH_part_names[FH_PART_PROPS_COUNT] = { "dn", "fr", "ing", "as" };

const char *fH_joint_names[FH_JOINT_PROPS_COUNT] = { "stif", "rotstif", "stam" };

void fH_Handle::loadProperties(Param par)
{
	// loading values for vectors
	for (int i = 0; i < dimensions; i++)
	{
		first[i] = par.getDouble(i);
		second[i] = par.getDouble(dimensions + i);
	}
	obj = par.getSelected();
}

void fH_Builder::addHandle(fH_Handle *handle)
{
	switch (handle->type)
	{
	case fHBodyType::JOINT:
		sticks.push_back((fH_StickHandle*)handle);
		break;
	case fHBodyType::NEURON:
		neurons.push_back((fH_NeuronHandle*)handle);
		break;
	case fHBodyType::CONNECTION:
		connections.push_back((fH_ConnectionHandle*)handle);
		break;
	}
}

// Methods for saving properties of handles in params

void fH_Handle::saveProperties(Param &par)
{
	par.select(obj);
	for (int i = 0; i < dimensions; i++)
	{
		par.setDouble(i, first[i]);
		par.setDouble(dimensions + i, second[i]);
	}
}

// Destructor of Builder

fH_Builder::~fH_Builder()
{
	for (fH_StickHandle *obj : sticks)
	{
		delete obj;
	}
	sticks.clear();
	for (fH_NeuronHandle *obj : neurons)
	{
		delete obj;
	}
	neurons.clear();
	for (fH_ConnectionHandle *obj : connections)
	{
		delete obj;
	}
	connections.clear();

	if (stickparamtab) ParamObject::freeParamTab(stickparamtab);
	if (neuronparamtab) ParamObject::freeParamTab(neuronparamtab);
	if (connectionparamtab) ParamObject::freeParamTab(connectionparamtab);

}

// Methods for parsing genotype

void fH_Builder::prepareParams()
{
	for (int i = 0; i < dimensions; i++) // preparing first vector fields
	{
		string x = "x";
		x += to_string(i);
		stickmut.addProperty(NULL, x.c_str(), HANDLE_VECTOR_TYPE, x.c_str(), "", PARAM_CANOMITNAME, 0, -1);
		neuronmut.addProperty(NULL, x.c_str(), HANDLE_VECTOR_TYPE, x.c_str(), "", PARAM_CANOMITNAME, 0, -1);
		connectionmut.addProperty(NULL, x.c_str(), HANDLE_VECTOR_TYPE, x.c_str(), "", PARAM_CANOMITNAME, 0, -1);

	}
	for (int i = 0; i < dimensions; i++) // preparing second vector fields
	{
		string y = "y";
		y += to_string(i);
		stickmut.addProperty(NULL, y.c_str(), HANDLE_VECTOR_TYPE, y.c_str(), "", PARAM_CANOMITNAME, 0, -1);
		neuronmut.addProperty(NULL, y.c_str(), HANDLE_VECTOR_TYPE, y.c_str(), "", PARAM_CANOMITNAME, 0, -1);
		connectionmut.addProperty(NULL, y.c_str(), HANDLE_VECTOR_TYPE, y.c_str(), "", PARAM_CANOMITNAME, 0, -1);

	}

	Part p;
	for (int i = 0; i < FH_PART_PROPS_COUNT; i++)
	{
		stickmut.addProperty(&p.properties().getParamTab()[p.properties().findId(fH_part_names[i]) + p.properties().getGroupCount()], -1);
	}

	Joint j;
	for (int i = 0; i < FH_JOINT_PROPS_COUNT; i++)
	{
		stickmut.addProperty(&j.properties().getParamTab()[j.properties().findId(fH_joint_names[i]) + j.properties().getGroupCount()], -1);
	}
	stickmut.addProperty(NULL, "l", STICKH_LENGTH_TYPE, "length", "", 0, 0, -1);

	Neuro n;
	neuronmut.addProperty(&n.properties().getParamTab()[n.properties().findId(FH_PE_NEURO_DET) + n.properties().getGroupCount()], -1);

	Param tmp(f0_neuroconn_paramtab, NULL);
	connectionmut.addProperty(&tmp.getParamTab()[tmp.findId(FH_PE_CONN_WEIGHT) + tmp.getGroupCount()], -1);

	stickparamtab = ParamObject::makeParamTab((ParamInterface *)&stickmut, 0, 0, stickmut.firstMutableIndex());
	neuronparamtab = ParamObject::makeParamTab((ParamInterface *)&neuronmut, 0, 0, neuronmut.firstMutableIndex());
	connectionparamtab = ParamObject::makeParamTab((ParamInterface *)&connectionmut, 0, 0, connectionmut.firstMutableIndex());
}

int fH_Builder::processLine(SString line, int linenumber, int begin, int end)
{
	// Firstly, method determines if line describes joint, neuron or neural connection
	// and prepares corresponding ParamTab
	fH_Handle *handle = NULL;
	ParamEntry *tab = NULL;
	if (line.startsWith("j:")) //joint
	{
		handle = new fH_StickHandle(dimensions, begin, end);
		tab = stickparamtab;
	}
	else if (line.startsWith("n:")) //neuron
	{
		handle = new fH_NeuronHandle(dimensions, begin, end);
		tab = neuronparamtab;
	}
	else if (line.startsWith("c:")) //connection
	{
		handle = new fH_ConnectionHandle(dimensions, begin, end);
		tab = connectionparamtab;
	}
	else // could not determine type of a handle
	{
		string message = "Cannot determine handle type at line:  " + to_string(linenumber);
		logMessage("fH_Builder", "processLine", LOG_ERROR, message.c_str());
		return begin;
	}
	line = line.substr(2); // skip of "j:", "c:" or "n:"

	// Secondly, ParamObject for holding handle properties is created
	void *obj = ParamObject::makeObject(tab);
	Param par(tab, obj);
	par.setDefault();
	ParamInterface::LoadOptions opts;

	// After preparing Param objects, vector values and body properties are parsed
	par.load(ParamInterface::FormatSingleLine, line, &opts);

	// If parsing failed, method writes error message and ends processing
	if (opts.parse_failed)
	{
		string message = "Error in parsing handle parameters at line:  " + to_string(linenumber);
		logMessage("fH_Builder", "processLine", LOG_ERROR, message.c_str());
		delete handle;
		ParamObject::freeObject(obj);
		return begin;
	}

	// If parsing ended successfully, parsed properties are loaded into handle fields
	handle->loadProperties(par);

	// In the end, ready handle is stored in an appropriate vector
	addHandle(handle);
	return 0;
}

int fH_Builder::parseGenotype(const SString &genotype)
{
	// Firstly, number of dimensions is parsed
	int pos = 0;
	SString numdimensions;
	genotype.getNextToken(pos, numdimensions, '\n');
	if (!ExtValue::parseInt(numdimensions.c_str(), dimensions, true, false))
	{
		logMessage("fH_Builder", "parseGenotype", LOG_ERROR, "Could not parse number of dimensions");
		return 1;
	}
	if (dimensions < 1)
	{
		logMessage("fH_Builder", "parseGenotype", LOG_ERROR, "Number of dimensions cannot be lower than 1");
		return 1;
	}
	SString line;
	int linenumber = 2;

	// With known number of dimensions ParamTabs for handles are prepared
	prepareParams();

	// After preparing Builder for parsing, each line is processed with processLine
	int lastpos = pos;
	while (genotype.getNextToken(pos, line, '\n'))
	{
		if (line.len() > 0)
		{
			int res = processLine(line, linenumber, lastpos, pos - 1);
			if (res != 0)
			{
				return res;
			}
		}
		lastpos = pos;
		linenumber++;
	}
	if (sticks.size() == 0)
	{
		logMessage("fH_Builder", "parseGenotype", LOG_ERROR, "Genotype does not contain any stick");
		return 1;
	}
	return 0;
}

// Distance calculations

double fH_Handle::dist(vector<double> left, vector<double> right)
{
	double sum = 0;
	for (unsigned int i = 0; i < left.size(); i++)
	{
		sum += (left[i] - right[i]) * (left[i] - right[i]);
	}
	return sqrt(sum);
}

vector<double> fH_Handle::getVectorsAverage()
{
	vector<double> result(dimensions, 0);
	for (int i = 0; i < dimensions; i++)
	{
		result[i] = (first[i] + second[i]) / 2;
	}
	return result;
}

double fH_StickHandle::distance(fH_Handle *right)
{
	double distance = 0;
	switch (right->type)
	{
	case fHBodyType::JOINT:
		// distance is computed between second vector of current handle and first
		// vector of second handle
		distance = dist(second, right->first);
		break;
	case fHBodyType::NEURON:
	{
		// if neuron has to be connected to joint, then distance is calculated
		// between averages of both handles
		vector<double> avgs = getVectorsAverage();
		vector<double> avgn = right->getVectorsAverage();
		distance = dist(avgs, avgn);
		break;
	}
	case fHBodyType::CONNECTION:
		// it is impossible to calculate distance between Joint and Connection
		return numeric_limits<double>::quiet_NaN();
	}
	return distance;
}

double fH_NeuronHandle::distance(fH_Handle *right)
{
	double distance = 0;
	switch (right->type)
	{
	case fHBodyType::JOINT:
	{
		// if neuron has to be connected to joint, then distance is calculated
		// between averages of both handles
		vector<double> avgs = right->getVectorsAverage();
		vector<double> avgn = getVectorsAverage();
		distance = dist(avgs, avgn);
		break;
	}
	case fHBodyType::CONNECTION:
		// this calculation is meant for input neuron - it compares second vector
		// of neuron and first vector of connection
		distance = dist(second, right->first);
		break;
	case fHBodyType::NEURON:
		// it is impossible to calculate distance between two Neurons
		return numeric_limits<double>::quiet_NaN();
	}
	return distance;
}

double fH_NeuronHandle::distance(fH_StickHandle *right, bool first)
{
	vector<double> avgn = getVectorsAverage();
	double distance = 0;
	if (first)
	{
		distance = dist(avgn, right->firstparthandle);
	}
	else
	{
		distance = dist(avgn, right->secondparthandle);
	}
	return distance;
}

double fH_ConnectionHandle::distance(fH_Handle *right)
{
	double distance = 0;
	switch (right->type)
	{
	case fHBodyType::NEURON:
		// this calculation is meant for output neuron - it compares second vector
		// of connection and first vector of neuron
		distance = dist(second, right->first);
		break;
	case fHBodyType::JOINT:
	case fHBodyType::CONNECTION:
		// it is impossible to calculate distance between Connection and other
		// Connection or Joint
		return numeric_limits<double>::quiet_NaN();
	}
	return distance;
}

// Creature build functions

Part * fH_StickHandle::createPart(ParamEntry *tab, std::vector<fH_StickHandle *> *children, Model *model, bool createmapping)
{
	Param par(tab, obj);
	double partprops[FH_PART_PROPS_COUNT];
	for (int i = 0; i < FH_PART_PROPS_COUNT; i++)
	{
		partprops[i] = par.getDouble(2 * getDimensions() + i);
	}

	unsigned int stickscount = children->size() + 1;

	MultiRange ranges;
	ranges.add(begin, end);

	for (fH_StickHandle *child : (*children))
	{
		par.select(child->obj);
		for (int i = 0; i < FH_PART_PROPS_COUNT; i++)
		{
			partprops[i] += par.getDouble(2 * getDimensions() + i);
		}
		ranges.add(child->begin, child->end);
	}

	for (int i = 0; i < FH_PART_PROPS_COUNT; i++)
	{
		partprops[i] /= stickscount;
	}

	Part *newpart = new Part();

	model->addPart(newpart);

	newpart->density = partprops[0];
	newpart->friction = partprops[1];
	newpart->ingest = partprops[2];
	newpart->assim = partprops[3];

	if (createmapping) newpart->addMapping(ranges);

	return newpart;
}

Joint* fH_StickHandle::createJoint(ParamEntry *tab, Model *model, bool createmapping)
{
	Param par(tab, obj);
	if (firstpart == NULL || secondpart == NULL)
	{
		return NULL;
	}
	Joint *newjoint = new Joint();

	model->addJoint(newjoint);

	newjoint->stif = par.getDoubleById("stif");
	newjoint->rotstif = par.getDoubleById("rotstif");
	newjoint->stamina = par.getDoubleById("stam");
	newjoint->attachToParts(firstpart, secondpart);
	if (createmapping) newjoint->addMapping(IRange(begin, end));
	return newjoint;
}

void fH_Builder::buildBody()
{
	// stickconnections vector holds information about connections between sticks.
	// Left side of pair should hold pointer to stick that is connected with second
	// vector, and right side of pair should hold pointer to stick that is connected
	// with first vector
	stickconnections.clear();

	// if body consists of single stick, just add it to body
	if (sticks.size() == 1)
	{
		stickconnections.push_back(pair<fH_StickHandle *, fH_StickHandle *>(nullptr, sticks[0]));
		sticksorder.push_back(0);
		return;
	}

	vector<bool> remainingsticks(sticks.size(), true);

	// first we find two handles that have minimal distances between their second
	// and first vector
	fH_StickHandle *left = sticks[0];
	fH_StickHandle *right = sticks[1];
	double mindist = left->distance(right);
	int leftid = 0;
	int rightid = 1;
	for (unsigned int i = 0; i < sticks.size(); i++)
	{
		for (unsigned int j = i + 1; j < sticks.size(); j++)
		{
			double distance = sticks[i]->distance(sticks[j]);
			if (distance < mindist)
			{
				mindist = distance;
				left = sticks[i];
				right = sticks[j];
				leftid = i;
				rightid = j;
			}
			distance = sticks[j]->distance(sticks[i]);
			if (distance < mindist)
			{
				mindist = distance;
				left = sticks[j];
				right = sticks[i];
				leftid = j;
				rightid = i;
			}
		}
	}

	// two found handles are the beginning of creature body
	stickconnections.push_back(pair<fH_StickHandle *, fH_StickHandle *>(nullptr, left));
	stickconnections.push_back(pair<fH_StickHandle *, fH_StickHandle *>(left, right));

	// after selecting two handles as beginning of body, they are marked as used
	// in the list of remaining sticks
	remainingsticks[leftid] = false;
	remainingsticks[rightid] = false;

	sticksorder.push_back(leftid);
	sticksorder.push_back(rightid);

	// next stick is selected by minimum distance between first vector of its handle
	// and second vector of any existing StickHandle in body
	int remaining = sticks.size() - 2;
	while (remaining > 0)
	{
		leftid = -1;
		rightid = -1;
		mindist = numeric_limits<double>::max();
		for (unsigned int i = 0; i < sticks.size(); i++)
		{
			// if stick is not already in
			if (remainingsticks[i])
			{
				for (int stickid : sticksorder)
				{
					double distance = sticks[stickid]->distance(sticks[i]);
					if (distance < mindist)
					{
						mindist = distance;
						leftid = stickid;
						rightid = i;
					}
				}
			}
		}
		stickconnections.push_back(pair<fH_StickHandle *, fH_StickHandle *>(sticks[leftid], sticks[rightid]));
		remainingsticks[rightid] = false;
		sticksorder.push_back(rightid);
		remaining--;
	}
}

int fH_Builder::developBrain(Model *model, bool createmapping)
{
	Param par(neuronparamtab, NULL);
	// First of all, neurons are attached to body
	for (fH_NeuronHandle *currneu : neurons)
	{
		par.select(currneu->obj);
		// create Neuro object and set details
		currneu->neuron = new Neuro();
		SString det = par.getStringById("d");
		if (det != "")
		{
			currneu->neuron->setDetails(det);
		}
		else
		{
			currneu->neuron->setDetails("N");
		}

		// get class of neuron. If class with given name does not exist - return error
		NeuroClass *nclass = currneu->neuron->getClass();
		if (!nclass)
		{
			SString msg = "NeuroClass given in details \"";
			msg += det + "\" does not exist";
			logMessage("fH_Builder", "developBrain", LOG_ERROR, msg.c_str());
			delete currneu->neuron;
			return -1;
		}
		// add neuron to model -> required before attaching to body part
		model->addNeuro(currneu->neuron);
		if (nclass->getPreferredLocation() == 2) // attach to Joint
		{
			// find stick that has closest average handle to average handle of
			// neuron
			double mindist = currneu->distance(sticks[0]);
			fH_StickHandle *minstick = sticks[0];
			for (unsigned int i = 1; i < sticks.size(); i++)
			{
				double distance = currneu->distance(sticks[i]);
				if (distance < mindist)
				{
					mindist = distance;
					minstick = sticks[i];
				}
			}
			currneu->neuron->attachToJoint(minstick->joint);
		}
		else if (nclass->getPreferredLocation() == 1) // attach to Part
		{
			// in the beginning we take first part of first stick to calculate
			// distance between them as initial minimal distance
			double mindist = currneu->distance(sticks[0], true);
			Part *minpart = sticks[0]->firstpart;
			for (unsigned int i = 0; i < sticks.size(); i++)
			{
				// after this we take only second parts of following sticks to
				// avoid repetition (thats why we start from i = 0)
				double distance = currneu->distance(sticks[i], false);
				if (distance < mindist)
				{
					mindist = distance;
					minpart = sticks[i]->secondpart;
				}
			}
			currneu->neuron->attachToPart(minpart);
		}
		if (createmapping) currneu->neuron->addMapping(IRange(currneu->begin, currneu->end));
		model->checkpoint();
	}

	par.setParamTab(connectionparamtab);
	// Secondly, connections are created
	for (fH_ConnectionHandle *currcon : connections)
	{
		par.select(currcon->obj);
		// Connection is created as follows:
		//   beginneu ---> endneu
		// distance between beginneu and connection is calculated as distance
		// between second handle of beginneu and first handle of connection.
		// This is why calculation is written as beginneu->distance(currcon).
		// In case of connection and endneu distance between them is calculated
		// as distance between second handle of connection and first handle of
		// endneu. This is why calculation is written as currcon->distance(endneu).

		fH_NeuronHandle *beginneu = NULL;
		double mindist = numeric_limits<double>::max();
		// find beginning of connection
		for (fH_NeuronHandle *neuron : neurons)
		{
			// These method checked earlier if all neurons have valid classes.
			// If a neuron does not have output, then it's skipped from comparison.
			// Otherwise:
			if (neuron->neuron->getClass()->getPreferredOutput() > 0)
			{
				double distance = neuron->distance(currcon);
				if (distance < mindist)
				{
					mindist = distance;
					beginneu = neuron;
				}
			}
		}
		// if there was no neuron that could begin connection, then return warning
		if (!beginneu)
		{
			// due to often appearance of connection genes in fB encoding, this
			// log message is commented
			// logMessage("fH_Builder", "developBrain", LOG_DEBUG, "There are no available neurons with outputs, connection could not be established");
			continue;
		}

		fH_NeuronHandle *endneu = NULL;
		mindist = numeric_limits<double>::max();
		// find ending of connection
		for (fH_NeuronHandle *neuron : neurons)
		{
			// Method checked earlier if all neurons have valid classes.
			// If neuron does not accept input or all inputs are already connected,
			// then it's skipped from comparison.
			// Otherwise:
			if (neuron->neuron->getClass()->getPreferredInputs() == -1 ||
				neuron->neuron->getClass()->getPreferredInputs() > neuron->neuron->getInputCount())
			{
				double distance = currcon->distance(neuron);
				if (distance < mindist)
				{
					mindist = distance;
					endneu = neuron;
				}
			}
		}
		// if there was no neuron that could end connection, then return warning
		if (!endneu)
		{
			// due to often appearance of connection genes in fB encoding, this
			// log message is commented
			// logMessage("fH_Builder", "developBrain", LOG_DEBUG, "There are no available neurons with free inputs, connection could not be established");
			continue;
		}
		endneu->neuron->addInput(beginneu->neuron, par.getDoubleById("w"));
		if (createmapping) endneu->neuron->addMapping(IRange(currcon->begin, currcon->end));
		model->checkpoint();
	}
	return 0;
}

Pt3D fH_Builder::getNextDirection(int count, int number)
{
	// In order to get evenly distributed sticks coming from the same Part method
	// uses algorithm for even distribution of points on a sphere. There are several
	// methods to perform this, usually they are iterative. This method introduced
	// below offers not fully accurate, yet quite satisfying results. This is
	// RSZ method (Rakhmanov, Saff and Zhou method), with use of the golden angle.
	// This method is based on distribution of points along spiral that covers sphere
	// surface.

	// Following method works partially on spherical coordinates (r and theta is used).
	// The Z coordinate is from Cartesian coordinate system. The golden angle is used
	// to "iterate" along spiral, while Z coordinate is used to move down the
	// sphere.

	double golden_angle = M_PI * (3.0 - sqrt(5));
	double dz = 2.0 / (double)count;
	double z = 1 - ((double)number + 0.5) * dz;
	double r = sqrt(1 - z * z);
	double theta = golden_angle * number;
	Pt3D vec;
	// In the end X and Y coordinates are calculated with current values of
	// r and theta. Value z is already calculated
	vec.x = r * cos(theta);
	vec.y = r * sin(theta);
	vec.z = z;
	vec.normalize();
	return vec;
}

Orient fH_Builder::getRotationMatrixToFitVector(Pt3D currdir, Pt3D expecteddir)
{
	Orient res;
	// first method normalizes vectors for easy calculations
	currdir.normalize();
	expecteddir.normalize();
	double c = currdir.dotProduct(expecteddir); // dot product of both vectors
	// if the dot product of both vectors equals 0
	if (c == 0)
	{
		res.x.x = -1;
		res.x.y = 0;
		res.x.z = 0;

		res.y.x = 0;
		res.y.y = -1;
		res.y.z = 0;

		res.z.x = 0;
		res.z.y = 0;
		res.z.z = -1;
	}
	Pt3D v = Pt3D(0); // cross product of both vectors
	v.x = currdir.y * expecteddir.z - currdir.z * expecteddir.y;
	v.y = currdir.z * expecteddir.x - currdir.x * expecteddir.z;
	v.z = currdir.x * expecteddir.y - currdir.y * expecteddir.x;

	// Rotation matrix that enables aligning currdir to expecteddir comes from
	// following calculation
	// R = I + [v]_x + ([v]_x)^2 / (1+c)
	// where [v]_x is the skew-symmetric cross-product matrix of v
	res.x.x = 1 - (v.y * v.y + v.z * v.z) / (1 + c);
	res.x.y = v.z + (v.x * v.y) / (1 + c);
	res.x.z = -v.y + (v.x * v.z) / (1 + c);
	res.y.x = -v.z + (v.x * v.y) / (1 + c);
	res.y.y = 1 - (v.x * v.x + v.z * v.z) / (1 + c);
	res.y.z = v.x + (v.y * v.z) / (1 + c);
	res.z.x = v.y + (v.x * v.z) / (1 + c);
	res.z.y = -v.x + (v.y * v.z) / (1 + c);
	res.z.z = 1 - (v.x * v.x + v.y * v.y) / (1 + c);

	return res;
}

Model* fH_Builder::buildModel(bool using_checkpoints)
{
	Model *model = new Model();

	// At first, floating sticks are connected
	buildBody();

	model->open(using_checkpoints);

	// Secondly, parts and joints are created
	// For every stick in body, starting with initial
	Param par(stickparamtab, NULL);
	for (int currid : sticksorder)
	{
		fH_StickHandle *currstick = sticks[currid];
		fH_StickHandle *parent = NULL;
		// find parent of current stick - it is first element of pair, in which
		// current stick is second
		for (pair<fH_StickHandle *, fH_StickHandle *> conn : stickconnections)
		{
			if (conn.second == currstick)
			{
				parent = conn.first;
				break;
			}
		}

		// if parent is NULL, then create Part with current stick properties and
		// location at (0,0,0)
		if (!parent)
		{
			vector<fH_StickHandle *> emptylist;
			Part *firstpart = currstick->createPart(stickparamtab, &emptylist, model, createmapping);
			firstpart->p = Pt3D(0);
			currstick->firstpart = firstpart;
			currstick->firstparthandle = currstick->first; // this is used to calculate later distance between
			model->checkpoint();
		}
		else //otherwise first part of current stick is the second part of previous stick
		{
			currstick->firstpart = parent->secondpart;
			currstick->firstparthandle = parent->secondparthandle;
		}
		// position of second part depends on two things
		//  1. direction of previous joint
		//  2. how many sticks are connected to the same parent
		// default direction of growth (without parent) is (1,0,0)
		Pt3D direction(1, 0, 0);
		Pt3D secondposition(currstick->firstpart->p);
		// if parent does exist, then determine how many sticks are connected to
		// parent and distribute them evenly on a sphere surrounding second part
		if (parent)
		{
			// improved RSZ method creates vectors that starts in
			// center of sphere (which will act as shared part), so direction
			// calculated below should point from shared part to previous part
			// in order to perform proper aligning
			direction = parent->secondpart->p - parent->firstpart->p;
			direction.normalize();
			// determine how many sticks are connected to parent and when connection
			// between parent and current stick appear
			int count = 0;
			int id = -1;
			for (unsigned int i = 0; i < stickconnections.size(); i++)
			{
				if (stickconnections[i].first == parent)
				{
					if (stickconnections[i].second == currstick)
					{
						id = count;
					}
					count++;
				}
			}
			if (id == -1)
			{
				logMessage("fH_Builder", "buildModel", LOG_ERROR, "Invalid behaviour");
				delete model;
				return NULL;
			}

			// if there is only one child, then don't change direction - continue
			// along axis of parent. Otherwise calculate direction of id-th stick
			// (that is currstick) with use of RSZ/Vogel method of distributing points
			// evenly on a sphere
			if (count > 1)
			{
				direction = parent->firstpart->p - parent->secondpart->p;
				direction.normalize();
				// there has to be count+1 directions, so method needs to generate
				// count+1 evenly distributed points on a sphere to make vectors
				// from point (0,0,0) to those points. First generated vector
				// will act as parent joint direction vector
				Pt3D sphere0direction = getNextDirection(count + 1, 0);

				// First generated vector needs to be aligned to parent vector
				Orient rotmatrix = getRotationMatrixToFitVector(sphere0direction, direction);

				// Calculation of direction from sphere for currstick
				direction = getNextDirection(count + 1, id + 1);
				// Rotation matrix aligning
				direction = rotmatrix.transform(direction);
				direction.normalize();
			}
		}

		// calculate second position
		par.select(currstick->obj);
		secondposition += direction * par.getDoubleById("l");

		// find every stick connected to current stick in order to calculate second
		// part properties
		vector<fH_StickHandle *> children;
		currstick->secondparthandle = currstick->second;
		for (pair<fH_StickHandle *, fH_StickHandle *> conn : stickconnections)
		{
			if (conn.first == currstick)
			{
				children.push_back(conn.second);
				for (int i = 0; i < dimensions; i++)
				{
					currstick->secondparthandle[i] += conn.second->first[i];
				}
			}
		}
		// create part from current stick and other sticks connected to this part
		Part *secondpart = currstick->createPart(stickparamtab, &children, model, createmapping);
		secondpart->p = secondposition;
		currstick->secondpart = secondpart;
		double count = (double)children.size() + 1;
		for (int i = 0; i < dimensions; i++)
		{
			currstick->secondparthandle[i] /= count;
		}

		//after creating second part connect two parts with joint
		Joint * joint = currstick->createJoint(stickparamtab, model, createmapping);
		if (!joint)
		{
			logMessage("fH_Builder", "buildModel", LOG_ERROR, "Joint cannot be created");
			delete model;
			return NULL;

		}
		currstick->joint = joint;
		model->checkpoint();
	}
	// after creating a body, attach neurons to body and link them according to
	// connections
	if (developBrain(model, createmapping) == -1)
	{
		delete model;
		return NULL;
	}
	model->close();
	return model;
}

int fH_Builder::removeNeuronsWithInvalidClasses()
{
	int count = neurons.size();
	if (count == 0)
	{
		return 0;
	}
	vector<fH_NeuronHandle *>::iterator it = neurons.begin();
	Param par(neuronparamtab, NULL);
	while (it != neurons.end())
	{
		par.select((*it)->obj);
		SString det = par.getStringById("d");
		if (det == "")
		{
			it++;
		}
		else
		{
			Neuro *neu = new Neuro();
			neu->setDetails(det);
			if (neu->getClass())
			{
				it++;
			}
			else
			{
				fH_NeuronHandle *tmp = (*it);
				it = neurons.erase(it);
				delete tmp;
			}
			delete neu;
		}

	}
	return count - neurons.size();
}

SString fH_Builder::toString()
{
	SString result = "";
	result += to_string(dimensions).c_str();
	result += "\n";
	// first method stringifies parts
	Param par(stickparamtab, NULL);
	void *def = ParamObject::makeObject(stickparamtab);
	par.select(def);
	par.setDefault();
	for (fH_StickHandle *currstick : sticks)
	{
		currstick->saveProperties(par);
		SString props;
		par.saveSingleLine(props, def, true, false);
		result += "j:";
		result += props;
	}
	ParamObject::freeObject(def);
	par.setParamTab(neuronparamtab);
	def = ParamObject::makeObject(neuronparamtab);
	par.select(def);
	par.setDefault();
	for (fH_NeuronHandle *currneuron : neurons)
	{
		currneuron->saveProperties(par);
		SString props;
		par.saveSingleLine(props, def, true, false);
		result += "n:";
		result += props;
	}
	ParamObject::freeObject(def);
	par.setParamTab(connectionparamtab);
	def = ParamObject::makeObject(connectionparamtab);
	par.select(def);
	par.setDefault();
	for (fH_ConnectionHandle *currconnection : connections)
	{
		currconnection->saveProperties(par);
		SString props;
		par.saveSingleLine(props, def, true, false);
		result += "c:";
		result += props;
	}
	ParamObject::freeObject(def);
	return result;
}

ParamEntry* fH_Builder::getParamTab(fHBodyType type)
{
	switch (type)
	{
	case fHBodyType::JOINT:
		return stickparamtab;
		break;
	case fHBodyType::NEURON:
		return neuronparamtab;
		break;
	default:
		return connectionparamtab;
		break;
	}
}
