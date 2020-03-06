// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "fH_oper.h"

#include <common/loggers/loggers.h>
#include <frams/param/syntparam.h>
#include <frams/param/paramobj.h>

#define FIELDSTRUCT Geno_fH

static ParamEntry GENOfHparam_tab[] =
{
	{ "Genetics: fH", 1, FH_OPCOUNT + FH_ADD_OPCOUNT, },
	{ "fH_mut_addition", 0, 0, "Add element", "f 0 1 0.3", FIELD(operations[FH_ADD]), "Probability of adding new element to genotype", },
	{ "fH_mut_add_joint", 0, 0, " - add joint", "f 0 1 0.33", FIELD(addoperations[FH_ADD_STICK]), "Probability of adding new stick handle", },
	{ "fH_mut_add_neuron", 0, 0, " - add neuron", "f 0 1 0.33", FIELD(addoperations[FH_ADD_NEURO]), "Probability of adding new neuron handle", },
	{ "fH_mut_add_connection", 0, 0, " - add connection", "f 0 1 0.33", FIELD(addoperations[FH_ADD_CONN]), "Probability of adding new connection handle", },
	{ "fH_mut_deletion", 0, 0, "Delete element", "f 0 1 0.1", FIELD(operations[FH_DEL]), "Probability of removing element from genotype", },
	{ "fH_mut_handle", 0, 0, "Modify vectors of handles", "f 0 1 0.3", FIELD(operations[FH_HANDLE]), "Probability of changing values in vectors of handle", },
	{ "fH_mut_property", 0, 0, "Modify properties of handles", "f 0 1 0.3", FIELD(operations[FH_PROP]), "Probability of changing properties of handles", },
	{ 0, },
};

#undef FIELDSTRUCT

Geno_fH::Geno_fH()
{
	par.setParamTab(GENOfHparam_tab);
	par.select(this);
	par.setDefault();
	supported_format = 'H';
}

int Geno_fH::checkValidity(const char* geno, const char* genoname)
{
	LoggerToMemory eh(LoggerBase::Enable | LoggerToMemory::StoreAllMessages, LOG_WARN);
	fH_Builder builder;
	// during parsing method tries to approximate error position
	int err = builder.parseGenotype(geno);
	if (err != 0)
	{
		return err;
	}
	if (builder.sticks.size() == 0)
	{
		return 1;
	}
	int amount = builder.removeNeuronsWithInvalidClasses();
	if (amount > 0)
	{
		return 1;
	}
	return 0;
}

int Geno_fH::validate(char *&geno, const char *genoname)
{
	// 'eh' variable is used for "hiding" error and warning messages generated during fH
	// genotype parsing
	LoggerToMemory eh(LoggerBase::Enable | LoggerToMemory::StoreAllMessages, LOG_WARN);
	fH_Builder builder;
	int err = builder.parseGenotype(geno);
	// if parsing failed, then it is impossible to repair genotype
	if (err != 0)
	{
		return GENOPER_OPFAIL;
	}
	// method removes definitions of neurons that have invalid genotype
	int amount = builder.removeNeuronsWithInvalidClasses();
	// if there were any warnings, then rewrite genotype
	if (eh.getWarningCount() > 0 || amount > 0)
	{
		free(geno);
		geno = strdup(builder.toString().c_str());
	}
	return GENOPER_OK;
}

int Geno_fH::crossOver(char *&g1, char *&g2, float& chg1, float& chg2)
{
	fH_Builder *parent1 = new fH_Builder();
	fH_Builder *parent2 = new fH_Builder();

	// first of all, both parents need to be parsed. If parents cannot be
	// parsed or their dimensionality differs, then method returns GENOPER_OPFAIL
	if (parent1->parseGenotype(g1) != 0 || parent2->parseGenotype(g2) != 0 ||
		parent1->dimensions != parent2->dimensions)
	{
		return GENOPER_OPFAIL;
	}

	// Builders for children are defined
	fH_Builder *child1 = new fH_Builder();
	fH_Builder *child2 = new fH_Builder();

	child1->dimensions = child2->dimensions = parent1->dimensions;

	// Children Params are prepared for incoming handles
	child1->prepareParams();
	child2->prepareParams();

	int child1count = 0;
	int child2count = 0;

	for (unsigned int i = 0; i < parent1->sticks.size(); i++)
	{
		if (rndUint(2) == 0)
		{
			child1->sticks.push_back(parent1->sticks[i]);
			child1count++;
		}
		else
		{
			child2->sticks.push_back(parent1->sticks[i]);
		}
	}

	for (unsigned int i = 0; i < parent2->sticks.size(); i++)
	{
		if (rndUint(2) == 0)
		{
			child1->sticks.push_back(parent2->sticks[i]);
		}
		else
		{
			child2->sticks.push_back(parent2->sticks[i]);
			child2count++;
		}
	}

	// if one of children does not have any sticks, then other child takes
	// everything else
	bool skip1 = false;
	bool skip2 = false;
	if (child1->sticks.size() == 0) skip1 = true;
	if (child2->sticks.size() == 0) skip2 = true;

	for (unsigned int i = 0; i < parent1->neurons.size(); i++)
	{
		if ((rndUint(2) == 0 || skip2) && !skip1)
		{
			child1->neurons.push_back(parent1->neurons[i]);
			child1count++;
		}
		else
		{
			child2->neurons.push_back(parent1->neurons[i]);
		}
	}

	for (unsigned int i = 0; i < parent2->neurons.size(); i++)
	{
		if ((rndUint(2) == 0 || skip2) && !skip1)
		{
			child1->neurons.push_back(parent2->neurons[i]);
		}
		else
		{
			child2->neurons.push_back(parent2->neurons[i]);
			child2count++;
		}
	}

	for (unsigned int i = 0; i < parent1->connections.size(); i++)
	{
		if ((rndUint(2) == 0 || skip2) && !skip1)
		{
			child1->connections.push_back(parent1->connections[i]);
			child1count++;
		}
		else
		{
			child2->connections.push_back(parent1->connections[i]);
		}
	}

	for (unsigned int i = 0; i < parent2->connections.size(); i++)
	{
		if ((rndUint(2) == 0 || skip2) && !skip1)
		{
			child1->connections.push_back(parent2->connections[i]);
		}
		else
		{
			child2->connections.push_back(parent2->connections[i]);
			child2count++;
		}
	}

	chg1 = (float)child1count / (parent1->sticks.size() + parent1->neurons.size() + parent1->connections.size());
	chg2 = (float)child2count / (parent2->sticks.size() + parent2->neurons.size() + parent2->connections.size());

	free(g1);
	free(g2);
	if (skip1 && !skip2)
	{
		g1 = strdup(child2->toString().c_str());
		g2 = strdup("");
	}
	else if (!skip1 && skip2)
	{
		g1 = strdup(child1->toString().c_str());
		g2 = strdup("");
	}
	else
	{
		g1 = strdup(child1->toString().c_str());
		g2 = strdup(child2->toString().c_str());
	}

	child1->sticks.clear();
	child1->neurons.clear();
	child1->connections.clear();

	child2->sticks.clear();
	child2->neurons.clear();
	child2->connections.clear();

	delete parent1;
	delete parent2;
	delete child1;
	delete child2;

	return GENOPER_OK;
}

int Geno_fH::mutate(char *&geno, float& chg, int &method)
{
	// method only needs to parse genotype - it won't create Model
	fH_Builder *creature = new fH_Builder();
	if (creature->parseGenotype(geno) != 0)
	{
		return GENOPER_OPFAIL;
	}

	// used for computing chg
	unsigned int sumgenes = creature->sticks.size() + creature->neurons.size() + creature->connections.size();

	// if there is only one element in genotype (stick), then deletion would end
	// up with wrong genotype. If this occurs, deletion is skipped (deletion is
	// last possible operation listed in #defines, so fH_OPCOUNT - 1 will skip
	// this mutation, and roulette method will normalize the rest of probabilities)
	int skipdelete = 0;
	if (creature->sticks.size() + creature->neurons.size() + creature->connections.size() == 1)
	{
		skipdelete = 1;
	}

	method = roulette(operations, FH_OPCOUNT - skipdelete);
	switch (method) {
	case FH_ADD:
	{
		fH_Handle *handle = NULL;
		method = FH_OPCOUNT + roulette(addoperations, FH_ADD_OPCOUNT);
		if (getActiveNeuroClassCount() == 0) method = FH_OPCOUNT + FH_ADD_STICK;
		switch (method - FH_OPCOUNT)
		{
		case FH_ADD_STICK:
		{
			handle = new fH_StickHandle(creature->dimensions, 0, 0);
			createHandleVectors(handle, creature->stickparamtab, creature->dimensions);
			break;
		}
		case FH_ADD_NEURO:
		{
			handle = new fH_NeuronHandle(creature->dimensions, 0, 0);
			createHandleVectors(handle, creature->neuronparamtab, creature->dimensions);
			break;
		}
		case FH_ADD_CONN:
		{
			handle = new fH_ConnectionHandle(creature->dimensions, 0, 0);
			createHandleVectors(handle, creature->connectionparamtab, creature->dimensions);
			break;
		}
		}
		creature->addHandle(handle);
		break;
	}
	case FH_HANDLE:
	{
		ParamEntry *tab = NULL;
		fH_Handle *handle = NULL;
		getRandomHandle(creature, handle, tab, true);
		mutateHandleValues(handle, tab, creature->dimensions, true, false);
		break;
	}
	case FH_PROP:
	{
		ParamEntry *tab = NULL;
		fH_Handle *handle = NULL;
		getRandomHandle(creature, handle, tab, true);
		if (handle->type == fHBodyType::NEURON)
		{
			mutateNeuronHandleProperties((fH_NeuronHandle *)handle, creature->neuronparamtab);
		}
		else
		{
			mutateHandleValues(handle, tab, creature->dimensions, false, true);
		}
		break;
	}
	case FH_DEL:
	{
		ParamEntry *tab = NULL;
		fH_Handle *handle = NULL;
		int todelete = getRandomHandle(creature, handle, tab, true);
		switch (handle->type)
		{
		case JOINT:
			creature->sticks.erase(creature->sticks.begin() + todelete);
			break;
		case NEURON:
			creature->neurons.erase(creature->neurons.begin() + todelete);
			break;
		case CONNECTION:
			creature->connections.erase(creature->connections.begin() + todelete);
			break;
		}
		delete handle;
		break;
	}
	}
	free(geno);
	geno = strdup(creature->toString().c_str());
	chg = (double)1.0 / sumgenes;
	delete creature;
	return GENOPER_OK;
}

void Geno_fH::mutateHandleValues(fH_Handle *handle, ParamEntry *tab,
	int dimensions, bool changedimensions, bool changeproperties)
{
	Param par(tab, handle->obj);
	handle->saveProperties(par);
	if (changedimensions)
	{
		int i = rndUint(2 * dimensions);
		changeDoubleProperty(i, par, handle->type);
	}

	if (changeproperties)
	{
		int i = 2 * dimensions + rndUint(par.getPropCount() - 2 * dimensions);
		changeDoubleProperty(i, par, handle->type);
	}
	handle->loadProperties(par);
	//ParamObject::freeObject(obj);
}

void Geno_fH::createHandleVectors(fH_Handle *handle, ParamEntry *tab, int dimensions)
{
	void *obj = ParamObject::makeObject(tab);
	Param par(tab, obj);
	par.setDefault();
	double min, max, def;
	par.getMinMaxDouble(0, min, max, def);
	for (int i = 0; i < dimensions; i++)
	{
		par.setDouble(i, min + rndDouble(max - min));
		par.setDouble(i + dimensions, min + rndDouble(max - min));
	}
	handle->loadProperties(par);
	if (handle->type != fHBodyType::NEURON)
	{
		int i = 2 * dimensions + rndUint(par.getPropCount() - 2 * dimensions);
		changeDoubleProperty(i, par, handle->type);
	}
	else
	{
		mutateNeuronHandleProperties((fH_NeuronHandle *)handle, tab, true);
	}
}

void Geno_fH::changeDoubleProperty(int id, Param &par, fHBodyType type)
{
	double min, max, def;
	if (*par.type(id) == 'f')
	{
		// need to check if property is not weight of connection
		if (type != fHBodyType::CONNECTION || *par.id(id) != 'w')
		{
			par.getMinMaxDouble(id, min, max, def);
			par.setDouble(id, mutateCreep('f', par.getDouble(id), min, max, true));
		}
		else
		{
			// if it is weight, then method needs to use mutateNeuProperty
			double current = par.getDouble(id);
			par.setDouble(id, mutateNeuProperty(current, NULL, -1));
		}
	}
	else
	{
		logMessage("Geno_fH", "changeDoubleProperty", LOG_WARN, "fH mutations are not prepared for non-double properties");
	}
}

unsigned int Geno_fH::getRandomHandle(fH_Builder *creature, fH_Handle *&handle, ParamEntry *&tab, bool skipalonestick)
{
	unsigned int allhandlescount = creature->connections.size() + creature->neurons.size();
	if (!skipalonestick || creature->sticks.size() > 1)
	{
		allhandlescount += creature->sticks.size();
	}
	unsigned int toselect = rndUint(allhandlescount);
	if (toselect < creature->connections.size())
	{
		handle = creature->connections[toselect];
		tab = creature->connectionparamtab;
		return toselect;
	}
	else if (toselect - creature->connections.size() < creature->neurons.size())
	{
		toselect -= creature->connections.size();
		handle = creature->neurons[toselect];
		tab = creature->neuronparamtab;
		return toselect;
	}
	toselect -= creature->connections.size() + creature->neurons.size();
	handle = creature->sticks[toselect];
	tab = creature->stickparamtab;
	return toselect;
}

void Geno_fH::mutateNeuronProperties(SString &det)
{
	Neuro neu;
	det = det == "" ? "N" : det;
	neu.setDetails(det == "" ? "N" : det);

	SyntParam par = neu.classProperties();

	if (par.getPropCount() > 0)
	{
		int i = rndUint(par.getPropCount());
		if (*par.type(i) == 'f')
		{
			double change = mutateNeuProperty(par.getDouble(i), &neu, 100 + i);
			par.setDouble(i, change);
		}
		SString line;
		int tmp = 0;
		par.update(&line);
		SString props;
		line.getNextToken(tmp, props, '\n'); // removal of newline character
		if (props != "")
		{
			det = neu.getClass()->name;
			det += ": ";
			det += props;
		}
	}
}

void Geno_fH::mutateNeuronHandleProperties(fH_NeuronHandle *handle, ParamEntry *tab, bool userandomclass)
{
	Neuro neu;
	Param hpar(tab, handle->obj);
	SString det = hpar.getStringById("d");
	neu.setDetails(det == "" ? "N" : det);
	NeuroClass *nc = neu.getClass();

	if (userandomclass)
	{
		nc = getRandomNeuroClass();
		// checking of neuron class availability should be checked before
	}

	det = nc->getName();

	mutateNeuronProperties(det);

	hpar.setStringById("d", det);
}

//uint32_t Geno_fH::style(const char *geno, int pos)
//{
//	char ch = geno[pos];
//	uint32_t style = GENSTYLE_CS(0, GENSTYLE_STRIKEOUT);
//	if (pos == 0 || geno[pos - 1] == '\n' || ch == ':') // single-character handle type and all colons
//	{
//		style = GENSTYLE_CS(GENCOLOR_TEXT, GENSTYLE_BOLD);
//	}
//	else if (isalpha(ch)) // properties name
//	{
//		style = GENSTYLE_RGBS(0, 200, 0, GENSTYLE_BOLD);
//	}
//	else if (isdigit(ch) || strchr(",.=", ch)) // properties values
//	{
//		style = GENSTYLE_CS(GENCOLOR_TEXT, GENSTYLE_NONE);
//	}
//	else if (ch == '\"')
//	{
//		style = GENSTYLE_RGBS(200, 0, 0, GENSTYLE_BOLD);
//	}
//
//	return style;
//}

uint32_t Geno_fH::style(const char *g, int pos)
{
   char ch=g[pos];
   uint32_t style=GENSTYLE_CS(0,GENSTYLE_NONE); //default, should be changed below

   int pp=pos; //detect comment line
   while (pp>1 && g[pp-1]!='\n') pp--;
   if (g[pp]=='#') return GENSTYLE_RGBS(0,220,0,GENSTYLE_NONE); //comment line

   if (pos==0 || g[pos-1]=='\n' || ch==':' || ch==',') style=GENSTYLE_CS(0,GENSTYLE_BOLD); else
     if (ch=='\"') style=GENSTYLE_RGBS(150,0,0,GENSTYLE_BOLD); else
     {
      int cudz=0,neuclass=1; //ile cudz. do poczatku linii; czy w nazwie neuroklasy?
      while (pos>0)
      {
         pos--;
         if (g[pos]=='\"') cudz++;
         if (cudz==0 && (g[pos]==':' || g[pos]==',')) neuclass=0;
         if (g[pos]=='\n') break;
      }
      if (cudz%2)
      {
         if (neuclass) style=GENSTYLE_RGBS(150,0,150,GENSTYLE_BOLD); else //neuroclass
           if (isalpha(ch)) style=GENSTYLE_RGBS(255,140,0,GENSTYLE_BOLD); else //property
             style=GENSTYLE_RGBS(200,0,0,GENSTYLE_NONE);
      } else
        if (isalpha(ch)) style=GENSTYLE_RGBS(0,0,200,GENSTYLE_BOLD);
     }
   return style;
}
