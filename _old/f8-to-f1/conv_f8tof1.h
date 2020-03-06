/*
 *  conv_f8tof1.h
 *  L-systemToF1
 *
 *  Created by Maciej Wajcht on 08-03-21.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CONV_F8_TO_F1_H
#define _CONV_F8_TO_F1_H

#include <map>
#include <vector>
#include <string>
#include <list>
#include <iostream>
#include "genoconv.h"
#include "model.h"

using std::ostream;
using std::vector;
using std::list;
using std::string;
using std::map;

enum RelationType {
	r_greater,
	r_greaterEqual,
	r_less,
	r_lessEqual,
	r_equal,
	r_different
};

class Condition {
public:
	RelationType relation;
	SString parameter;
	double value;
	
	friend ostream& operator<<(ostream& os, const Condition& c);
};

class ActionP; //only declaration

/// superclass of all actions, e.g. Production, PrimitiveProduction etc.
class Action {
public:
	/// action's name
	SString name;
	/// if true action should ignore params passed to it
	bool ignoreParams;
	/// gets string representaion of this action in f1 format
	virtual const SString getF1Genotype(vector<double> params) = 0;
	/// gets list of actions with double parameters to each of them
	/// @param params parameters passed to this action
	virtual const list<ActionP> getActionList(const vector<double> params) = 0;
	/// gets string representaion of this action in f8 format
	virtual const SString getF8Representation() = 0;
	virtual ~Action() {}
};

/// this class consists of pointer to some action and a list of parameters to that action;
/// parameters are kept as strings
class ActionStrP {
public:
	Action *action;
	vector<SString> params;
};

/// this class consists of pointer to some action and a list of parameters to that action;
/// parameters are kept as doubles
class ActionP {
public:
	Action *action;
	vector<double> params;
};

/// part of production which is used in translation only if all the conditions are met
class SubProduction {
public:
	/// list of conditions necessary to be met
	vector<Condition> conditions;
	/// actions with params (as strings)
	vector<ActionStrP> actions;
};

/// primitive production; it's directly translated to its f1 equivalent
/// e.g. X, r, R etc.
class PrimitiveProduction : public Action {
public:
	PrimitiveProduction(const SString command);
	const SString getF1Genotype(const vector<double> params);
	const list<ActionP> getActionList(const vector<double> params);
	const SString getF8Representation();
protected:
	SString f1command;
	SString f8command;
};

/// param production; all param productions during translation are replaced with the current value of parameter
/// pointed by this production
class ParamProduction : public Action {
public:
	ParamProduction(const SString paramName);
	const SString getF1Genotype(const vector<double> params);
	const list<ActionP> getActionList(const vector<double> params);
	const SString getF8Representation();
protected:
	SString paramName;
};

/// neuron production; it is directly translated to f1 without any change
class NeuronProduction : public Action {
public:
	NeuronProduction(SString body);
	const SString getF1Genotype(const vector<double> params);
	const list<ActionP> getActionList(const vector<double> params);
	const SString getF8Representation();
protected:
	SString body;
};

/// class which keeps all parameters of a production and enables to access them in convenient way
/// IMPORTANT! All indices (positions) begins with 1 (not 0)!
class ParameterCollection {
public:
	/// returns parameter's value
	/// @param position parameter's position (first parameter has an index 1!)
	const double getValue(int position);
	/// returns parameter's value
	/// @param name parameter's name
	const double getValue(SString name);
	/// returns parameter's name
	/// @param position parameter's position (first parameter has an index 1!)
	const SString getParameterName(int position);
	/// returns parameter's position (first parameter has an index 1!)
	/// @param name parameter's name
	const int getParameterPosition(SString name);
	/// sets parameter's value
	/// @param position parameter's position
	/// @param value parameter's new value
	void setValue(int position, double value);
	/// sets parameter's value
	/// @param name parameter's name
	/// @param value parameter's new value
	void setValue(SString name, double value);
	/// adds parameter
	/// @param name parameter's name
	/// @param position desired parameter's position; defualts to -1 which means it will have first available position
	/// @param value parameter's value; defaults to 0.0
	void addParameter(SString name, int position = -1, double value = 0.0);
	/// returns the number of parameters kept in this class
	const int size();
	/// removes a parameter
	/// @param position position of parameter to be deleted
	void removeParameter(int position);
	/// removes a parameter
	/// @param name name of parameter to be deleted
	void removeParameter(SString name);
	/// returns true if parameter with given name exists
	/// @param name parameter's name
	bool paramExist(SString name);
protected:
	vector<string> parameters;
	map<string, double> paramValues;
};

/// represents a general rule in L-systems
/// only "calls" to Production in genotype are replaced in translation procedure
class Production : public Action {
public:
	/// parameters of this production
	ParameterCollection parameters;
	/// list of subproductions
	vector<SubProduction> subproductions;
	
	Production();
	~Production() {};
	const SString getF1Genotype(const vector<double> params);
	const list<ActionP> getActionList(const vector<double> params);
	const SString getF8Representation();
};

/// Main class that represents a genotype in f8 format
class Lsystem {
public:
	/// number of iterations in f8->f1 translation procedure
	int iterations;
	/// map of parameters of start production; key - parameter's name, value - parameter's value
	map<string, double> startParams;
	/// map of productions of L-system; key - productions's name, value - pointer to production
	map<string, Production*> productions;
	/// collection of neuron productions held in L-system
	vector<NeuronProduction*> neuronProductions;
	/// first production's name
	string firstProductionName;
	
	Lsystem();
	~Lsystem();
	/// returns a primitive production of a given name
	/// @param name primitive production's name
	PrimitiveProduction* getPrimitiveProduction(SString name);
	/// returns a param production of a given name
	/// @param name param production's name
	ParamProduction* getParamProduction(SString name);
	/// gets string representation of this L-system (f8 genotype)
	SString toString();
	/// returns all actions
	/// @param normal if true all normal actions will be included
	/// @param primitives if true all primitive actions will be included
	/// @param params if true all param actions will be included
	/// @param neurons if true all neuron actions will be included
	vector<Action*> getAllActions(bool normal, bool primitives, bool params, bool neurons);
protected:
	map<string, PrimitiveProduction*> primitiveProductions;
	map<string, ParamProduction*> paramProductions;

	void removeEmptySubproductionsAndProductions();
};





/// Converter between f8 and f1 format
class GenoConv_F8ToF1 : public GenoConverter {
public:
        Param par;
        static Param staticpar; //needed to add 'par' to the list of simulator params (this field is initialized externally)
  
	GenoConv_F8ToF1();
	~GenoConv_F8ToF1() {}

	SString convert(SString &in, MultiMap *map);

	/// check syntax of given f8 genotype
	/// @param geno f8 genotype to be checked
	bool checkSyntax(const char *geno);

	/// returns names of productions in a given genotype
	/// @param in f8 genotype
	vector<SString> readProductionNames(const SString &in);
	//Lsystem* createLsystem(const SString &in);

	/// creates Lsystem object based on input genotype
	/// @param f8 genotype
	Lsystem* createLsystem(SString in);

	int maxF1Length;
        static const char* simpleprods; 

protected:
	bool parseInput(const char* src, Lsystem* lsys);
};

#endif
