// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "f1_conv.h"
#include <common/nonstd_stl.h>
#include <common/log.h>
#include <frams/util/multirange.h>
#include <frams/util/multimap.h>
#include <frams/genetics/geneprops.h>
#include <ctype.h>
#include <assert.h>

//#define v1f1COMPATIBLE //as in ancient Framsticks 1.x

class Builder
{
public:
	Builder(const char*g, int mapping = 0) :invalid(0), genbegin(g), usemapping(mapping), first_part_mapping(NULL), own_first_part_mapping(true), model_energy(0), model_energy_count(0) {}
	~Builder() { if (own_first_part_mapping) SAFEDELETE(first_part_mapping); }
	char tmp[222];
	bool invalid;
	Model model;
	const char *genbegin;
	SList neuro_f1_to_f0; // neuro_f1_to_f0(f1_refno) = actual neuro pointer
	Neuro *last_f1_neuro;
	SyntParam *neuro_cls_param;

	struct Connection
	{
		int n1, n2; double w;
		Connection(int _n1, int _n2, double _w) :n1(_n1), n2(_n2), w(_w) {}
	};

	SListTempl<Connection> connections;
	int usemapping;
	MultiRange range;
	MultiRange *first_part_mapping;
	bool own_first_part_mapping;
	double lastjoint_muscle_power;
	double model_energy;
	int model_energy_count;
	void grow(int part1, const char*g, Pt3D k, GeneProps c, int branching_part);
	void setPartMapping(int p, const char* g);
	int growJoint(int part1, int part2, Pt3D &angle, GeneProps &c, const char *g);
	int growPart(GeneProps &c, const char *g);
	const char *skipNeuro(const char *z);
	const char* growNeuro(const char* t, GeneProps &c, int&);
	void growConnection(const char* begin, const char* colon, const char* end, GeneProps& props);
	int countBranches(const char*g, SList &out);
	SyntParam* lastNeuroClassParam();
	void addClassParam(const char* name, double value);
	void addClassParam(const char* name, const char* value);

	const MultiRange* makeRange(const char*g) { return makeRange(g, g); }
	const MultiRange* makeRange(const char*g, const char*g2);
	Part *getLastPart() { return getLastJoint()->part2; }
	Neuro *getLastNeuro() { return model.getNeuro(model.getNeuroCount() - 1); }
	Joint *getLastJoint() { return model.getJoint(model.getJointCount() - 1); }
	void addOrRememberInput(int n1, int n2, double w)
	{
		//if (!addInput(n1,n2,w,false))
		connections += Connection(n1, n2, w);
	}
	bool addInput(int n1, int n2, double w, bool final)
	{
		if ((n1 < 0) || (n2 < 0) || (n1 >= neuro_f1_to_f0.size()) || (n2 >= neuro_f1_to_f0.size()))
		{
			if (final) logPrintf("GenoConvF1", "addInput", LOG_WARN,
				"illegal neuron connection %d <- %d (ignored)", n1, n2);
			return 0;
		}
		Neuro *neuro = (Neuro*)neuro_f1_to_f0(n1);
		Neuro *input = (Neuro*)neuro_f1_to_f0(n2);
		neuro->addInput(input, w);
		return 1;
	}
	void addPendingInputs()
	{
		for (int i = 0; i < connections.size(); i++)
		{
			Connection *c = &connections(i);
			addInput(c->n1, c->n2, c->w, true);
		}
	}
};

const MultiRange* Builder::makeRange(const char*g, const char*g2)
{
	if (!usemapping) return 0;
	range.clear();
	range.add(g - genbegin, g2 - genbegin);
	return &range;
}

/** main conversion function - with conversion map support */
SString GenoConv_f1::convert(SString &i, MultiMap *map, bool using_checkpoints)
{
	const char* g = i.c_str();
	Builder builder(g, map ? 1 : 0);
	builder.model.open(using_checkpoints);
	builder.grow(-1, g, Pt3D_0, GeneProps::standard_values, -1); // uses Model::addFromString() to create model elements
	if (builder.invalid) return SString();
	builder.addPendingInputs();
	builder.model.startenergy = (builder.model_energy_count > 0) ? (builder.model_energy / builder.model_energy_count) : 1.0;
	builder.model.close(); // model is ready to use now
	if (map) builder.model.getCurrentToF0Map(*map); // generate f1-to-f0 conversion map
	return builder.model.getF0Geno().getGenes();
}

void Builder::setPartMapping(int p, const char* g)
{
	if (!usemapping) return;
	const MultiRange *r = makeRange(g);
	if (p < 0)
	{ //special case: mapping the part which is not yet created
		if (first_part_mapping) first_part_mapping->add(*r);
		else { first_part_mapping = new MultiRange(*r); own_first_part_mapping = true; }
	}
	else
		model.getPart(p)->addMapping(*r);
}

void Builder::grow(int part1, const char*g, Pt3D k, GeneProps c, int branching_part)
{
	int hasmuscles = 0;
	k += Pt3D(c.twist, 0, c.curvedness);
	while (1)
	{
		if (c.executeModifier(*g) == 0)
		{
			setPartMapping(part1, g);
		}
		else
		{
			switch (*g)
			{
			case 0: return;
			case ',': case ')': setPartMapping(branching_part, g); return;
			case 'R': k.x += 0.7853; setPartMapping(part1, g); break;
			case 'r': k.x -= 0.7853;	setPartMapping(part1, g); break;
			case '[': //neuron
				//		setdebug(g-(char*)geny,DEBUGNEURO | !l_neu);
				if (model.getJointCount())
					g = growNeuro(g + 1, c, hasmuscles);
				else
				{
					logMessage("GenoConv_F1", "grow", 1, "Illegal neuron position (ignored)");
					g = skipNeuro(g + 1);
				}
				break;
			case 'X':
			{
				int freshpart = 0;
				//setdebug(g-(char*)geny,DEBUGEST | !l_est);
				if (part1 < 0) //initial grow
				{
					if (model.getPartCount() > 0)
						part1 = 0;
					else
					{
						part1 = growPart(c, g);
						freshpart = 1;
						if (first_part_mapping)
						{
							//mapping was defined before creating this initial Part -> put it into the Part
							assert(own_first_part_mapping);
							model.getPart(part1)->setMapping(*first_part_mapping);
							delete first_part_mapping;
							//first_part_mapping can be still used later but from now on it references the internal Part mapping
							first_part_mapping = model.getPart(part1)->getMapping();
							own_first_part_mapping = false;
						}
					}
				}
				if (!freshpart)
				{
					Part *part = model.getPart(part1);
					part->density = ((part->mass*part->density) + 1.0 / c.weight) / (part->mass + 1.0); // v=m*d
					//			part->volume+=1.0/c.weight;
					part->mass += 1.0;
				}
				model_energy += 0.9*c.energy + 0.1;
				model_energy_count++;

				int part2 = growPart(c, g);
				growJoint(part1, part2, k, c, g);
				//		est* e = new est(*s,*s2,k,c,zz,this);

				// attenuate properties as they are propagated along the structure
				c.propagateAlong(true);

				model.checkpoint();
				grow(part2, g + 1, Pt3D_0, c, branching_part);
				return;
			}
			case '(':
			{
				setPartMapping(part1, g);
				SList ga;
				int i, count;
				count = countBranches(g + 1, ga);
				c.muscle_reset_range = false;
				c.muscle_bend_range = 1.0 / count;
				for (i = 0; i < count; i++)
					grow(part1, (char*)ga(i), k + Pt3D(0, 0, -M_PI + (i + 1)*(2 * M_PI / (count + 1))), c, part1);
				return;
			}
			case ' ': case '\t': case '\n': case '\r': break;
			default: invalid = 1; return;
			}
		}
		g++;
	}
}

SyntParam* Builder::lastNeuroClassParam()
{
	if (!neuro_cls_param)
	{
		NeuroClass *cls = last_f1_neuro->getClass();
		if (cls)
		{
			neuro_cls_param = new SyntParam(last_f1_neuro->classProperties());
			// this is equivalent to:
			//		SyntParam tmp=last_f1_neuro->classProperties();
			//		neuro_cls_param=new SyntParam(tmp);
			// interestingly, some compilers eliminate the call to new SyntParam,
			// realizing that a copy constructor is redundant when the original object is
			// temporary. there are no side effect of such optimization, as long as the
			// copy-constructed object is exact equivalent of the original.
		}
	}
	return neuro_cls_param;
}

void Builder::addClassParam(const char* name, double value)
{
	lastNeuroClassParam();
	if (neuro_cls_param)
		neuro_cls_param->setDoubleById(name, value);
}

void Builder::addClassParam(const char* name, const char* value)
{
	lastNeuroClassParam();
	if (neuro_cls_param)
	{
		ExtValue e(value);
		const ExtValue &re(e);
		neuro_cls_param->setById(name, re);
	}
}

int Builder::countBranches(const char*g, SList &out)
{
	int gl = 0;
	out += (void*)g;
	while (gl >= 0)
	{
		switch (*g)
		{
		case 0: gl = -1; break;
		case '(': case '[': ++gl; break;
		case ')': case ']': --gl; break;
		case ',': if (!gl) out += (void*)(g + 1);
		}
		g++;
	}
	return out.size();
}

int Builder::growJoint(int part1, int part2, Pt3D &angle, GeneProps &c, const char *g)
{
	double len = min(2.0, c.length);
	sprintf(tmp, "p1=%d,p2=%d,dx=%lg,rx=%lg,ry=%lg,rz=%lg,stam=%lg,vr=%g,vg=%g,vb=%g",
		part1, part2, len, angle.x, angle.y, angle.z, c.stamina, c.cred, c.cgreen, c.cblue);
	lastjoint_muscle_power = c.muscle_power;
	return model.addFromString(Model::JointType, tmp, makeRange(g));
}

int Builder::growPart(GeneProps &c, const char *g)
{
	sprintf(tmp, "dn=%lg,fr=%lg,ing=%lg,as=%lg,vs=%g,vr=%g,vg=%g,vb=%g",
		1.0 / c.weight, c.friction, c.ingestion, c.assimilation, c.visual_size, c.cred, c.cgreen, c.cblue);
	return model.addFromString(Model::PartType, tmp, makeRange(g));
}

const char *Builder::skipNeuro(const char *z)
{
	for (; *z; z++) if ((*z == ']') || (*z == ')')) break;
	return z - 1;
}

const char* Builder::growNeuro(const char* t, GeneProps& props, int &hasmuscles)
{
	const char*neuroend = skipNeuro(t);
	last_f1_neuro = model.addNewNeuro();
	neuro_cls_param = NULL;
	last_f1_neuro->attachToPart(getLastPart());
	const MultiRange *mr = makeRange(t - 1, neuroend + 1);
	if (mr) last_f1_neuro->addMapping(*mr);
	neuro_f1_to_f0 += last_f1_neuro;

	SString clsname;
	bool haveclass = 0;
	while (*t && *t <= ' ') t++;
	const char* next = (*t) ? (t + 1) : t;
	while (*next && *next <= ' ') next++;
	if (*t && *next != ',' && *next != ']') // old style muscles [|rest] or [@rest]
		switch (*t)
	{
		case '@': if (t[1] == ':') break;
			haveclass = 1;
			//		if (!(hasmuscles&1))
			{
				hasmuscles |= 1;
				Neuro *muscle = model.addNewNeuro();
				sprintf(tmp, "@:p=%lg", lastjoint_muscle_power);
				muscle->addInput(last_f1_neuro);
				muscle->setDetails(tmp);
				muscle->attachToJoint(getLastJoint());
				if (usemapping) muscle->addMapping(*makeRange(t));
			}
			t++;
			break;
		case '|': if (t[1] == ':') break;
			haveclass = 1;
			//		if (!(hasmuscles&2))
			{
				hasmuscles |= 2;
				Neuro *muscle = model.addNewNeuro();
				sprintf(tmp, "|:p=%lg,r=%lg", lastjoint_muscle_power, props.muscle_bend_range);
				muscle->addInput(last_f1_neuro);
				muscle->setDetails(tmp);
				muscle->attachToJoint(getLastJoint());
				if (usemapping) muscle->addMapping(*makeRange(t));
			}
			t++;
			break;
	}
	while (*t && *t <= ' ') t++;
	bool finished = 0;
	const char *begin = t;
	const char* colon = 0;
	SString classparams;
	while (!finished)
	{
		switch (*t)
		{
		case ':': colon = t; break;
		case 0: case ']': case ')': finished = 1;
			// NO break!
		case ',':
			if (!haveclass && !colon && t > begin)
			{
				haveclass = 1;
				SString clsname(begin, t - begin);
				clsname = trim(clsname);
				last_f1_neuro->setClassName(clsname);
				NeuroClass *cls = last_f1_neuro->getClass();
				if (cls)
				{
					if (cls->getPreferredLocation() == 2)
						last_f1_neuro->attachToJoint(getLastJoint());
					else if (cls->getPreferredLocation() == 1)
						last_f1_neuro->attachToPart(getLastPart());

					lastNeuroClassParam();
					//special handling: muscle properties (can be overwritten by subsequent property assignments)
					if (!strcmp(cls->getName().c_str(), "|"))
					{
						neuro_cls_param->setDoubleById("p", lastjoint_muscle_power);
						neuro_cls_param->setDoubleById("r", props.muscle_bend_range);
					}
					else if (!strcmp(cls->getName().c_str(), "@"))
					{
						neuro_cls_param->setDoubleById("p", lastjoint_muscle_power);
					}
				}
			}
			else if (colon && (colon > begin) && (t > colon))
				growConnection(begin, colon, t, props);
			if (t[0] != ',') t--;
			begin = t + 1; colon = 0;
			break;
		}
		t++;
	}
	SAFEDELETE(neuro_cls_param);
	return t;
}
void Builder::growConnection(const char* begin, const char* colon, const char* end, GeneProps& props)
{
	while (*begin && *begin <= ' ') begin++;
	int i;
	if (isdigit(begin[0]) || (begin[0] == '-'))
	{
		double conn_weight = ExtValue::getDouble(trim(SString(colon + 1, end - (colon + 1))).c_str());
		paInt relative = ExtValue::getInt(trim(SString(begin, colon - begin)).c_str(), false);
		int this_refno = neuro_f1_to_f0.size() - 1;
		addOrRememberInput(this_refno, this_refno + relative, conn_weight);
	}
	else if ((i = last_f1_neuro->extraProperties().findIdn(begin, colon - begin)) >= 0)
	{
		last_f1_neuro->extraProperties().setFromString(i, colon + 1);
	}
	else if (isupper(begin[0]) || strchr("*|@", begin[0]))
	{
		SString clsname(begin, colon - begin);
		trim(clsname);
		Neuro *receptor = model.addNewNeuro();
		receptor->setClassName(clsname);
		NeuroClass *cls = receptor->getClass();
		if (cls)
		{
			if (cls->getPreferredLocation() == 2) receptor->attachToJoint(getLastJoint());
			else if (cls->getPreferredLocation() == 1) receptor->attachToPart(getLastPart());
		}
		last_f1_neuro->addInput(receptor, ExtValue::getDouble(trim(SString(colon + 1, end - (colon + 1))).c_str()));
		if (usemapping) receptor->addMapping(*makeRange(begin, end - 1));
	}
	else if ((begin[0] == '>') && (begin[1]))
	{
		Neuro *out = model.addNewNeuro();
		out->addInput(last_f1_neuro, ExtValue::getDouble(trim(SString(colon + 1, end - (colon + 1))).c_str()));
		out->setClassName(SString(begin + 1, end - colon - 1));
		if (begin[1] == '@')
		{
			sprintf(tmp, "p=%lg", lastjoint_muscle_power);
			out->setClassParams(tmp);
		}
		else if (begin[1] == '|')
		{
			sprintf(tmp, "p=%lg,r=%lg", lastjoint_muscle_power, props.muscle_bend_range);
			out->setClassParams(tmp);
		}
		NeuroClass *cls = out->getClass();
		if (cls)
		{
			if (cls->getPreferredLocation() == 2) out->attachToJoint(getLastJoint());
			else if (cls->getPreferredLocation() == 1) out->attachToPart(getLastPart());
		}
		if (usemapping) out->addMapping(*makeRange(begin, end - 1));
	}
	else if (*begin == '!') addClassParam("fo", ExtValue::getDouble(trim(SString(colon + 1, end - (colon + 1))).c_str()));
	else if (*begin == '=') addClassParam("in", ExtValue::getDouble(trim(SString(colon + 1, end - (colon + 1))).c_str()));
	else if (*begin == '/') addClassParam("si", ExtValue::getDouble(trim(SString(colon + 1, end - (colon + 1))).c_str()));
	else if (islower(begin[0]))
	{
		SString name(begin, colon - begin);
		SString value(colon + 1, end - (colon + 1));
		addClassParam(name.c_str(), value.c_str());
	}
}
