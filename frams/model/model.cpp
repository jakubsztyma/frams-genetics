// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <common/nonstd_math.h>
#include "model.h"
#include <common/log.h>
#include <frams/util/multimap.h>
#include <common/loggers/loggers.h>

#define F0_CHECKPOINT_LINE "checkpoint:"

Model::Model()
{
	autobuildmaps = false;
	init();
}

void Model::init()
{
	partmappingchanged = 0;
	using_checkpoints = false;
	is_checkpoint = false;
	buildstatus = empty;
	modelfromgenotype = 0;
	startenergy = 1.0;
	checklevel = 1;
	map = 0;
	f0map = 0;
	f0genoknown = 1;
	shape = SHAPE_UNKNOWN;
}

void Model::moveElementsFrom(Model &source)
{
	int i;
	open();
	for (i = 0; i < source.getPartCount(); i++)
		addPart(source.getPart(i));
	for (i = 0; i < source.getJointCount(); i++)
		addJoint(source.getJoint(i));
	for (i = 0; i < source.getNeuroCount(); i++)
		addNeuro(source.getNeuro(i));
	source.parts.clear(); source.joints.clear(); source.neurons.clear();
	source.clear();
}

void Model::internalCopy(const Model &mod)
{
	geno = mod.geno;
	f0genoknown = 0;
	startenergy = mod.startenergy;
	modelfromgenotype = mod.modelfromgenotype;
	for (int i = 0; i < mod.getPartCount(); i++)
		addPart(new Part(*mod.getPart(i)));
	for (int i = 0; i < mod.getJointCount(); i++)
	{
		Joint *oldj = mod.getJoint(i);
		Joint *j = new Joint(*oldj);
		addJoint(j);
		j->attachToParts(oldj->part1->refno, oldj->part2->refno);
	}
	for (int i = 0; i < mod.getNeuroCount(); i++)
	{
		Neuro *oldn = mod.getNeuro(i);
		Neuro *n = new Neuro(*oldn);
		addNeuro(n);
		if (oldn->part_refno >= 0) n->attachToPart(oldn->part_refno);
		else n->attachToJoint(oldn->joint_refno);
	}
	for (int i = 0; i < mod.getNeuroCount(); i++)
	{
		Neuro *oldn = mod.getNeuro(i);
		Neuro *n = getNeuro(i);
		for (int ni = 0; ni < oldn->getInputCount(); ni++)
		{
			double w;
			Neuro *oldinput = oldn->getInput(ni, w);
			SString info = n->getInputInfo(ni);
			n->addInput(getNeuro(oldinput->refno), w, &info);
		}
	}
	updateRefno();
	if (using_checkpoints)
		for (vector<Model *>::const_iterator it = mod.checkpoints.begin(); it != mod.checkpoints.end(); it++)
		{
			Model *m = *it;
			Model *n = new Model(*m, m->autobuildmaps, false, true);
			checkpoints.push_back(n);
		}
}


Model::Model(const Geno &src, bool buildmaps, bool _using_checkpoints, bool _is_checkpoint)
	:autobuildmaps(buildmaps)
{
	init(src, _using_checkpoints, _is_checkpoint);
}

void Model::operator=(const Model &mod)
{
	clear();
	open(mod.isUsingCheckpoints(), mod.isCheckpoint());
	internalCopy(mod);
	buildstatus = mod.buildstatus;
}

Model::Model(const Model &mod, bool buildmaps, bool _using_checkpoints, bool _is_checkpoint)
	:autobuildmaps(buildmaps)
{
	init();
	open(_using_checkpoints, _is_checkpoint);
	internalCopy(mod);
	if (is_checkpoint)
		close();
	else
		buildstatus = mod.buildstatus;
	if (mod.map)
		map = new MultiMap(*mod.map);
	if (mod.f0map)
		f0map = new MultiMap(*mod.f0map);
}

void Model::init(const Geno &src, bool _using_checkpoints, bool _is_checkpoint)
{
	init();
	using_checkpoints = _using_checkpoints;
	is_checkpoint = _is_checkpoint;
	modelfromgenotype = 1;
	geno = src;
	build();
}

void Model::resetAllDelta()
{
	for (int i = 0; i < getJointCount(); i++)
		getJoint(i)->resetDelta();
}

void Model::useAllDelta(bool yesno)
{
	for (int i = 0; i < getJointCount(); i++)
		getJoint(i)->useDelta(yesno);
}

Model::~Model()
{
	delmodel_list.action((intptr_t)this);
	clear();
}

void Model::clear()
{
	FOREACH(Part *, p, parts)
		delete p;
	FOREACH(Joint *, j, joints)
		delete j;
	FOREACH(Neuro *, n, neurons)
		delete n;
	parts.clear(); joints.clear(); neurons.clear();
	delMap();
	delF0Map();
	init();
	geno = Geno();
	f0geno = Geno();
	for (vector<Model *>::iterator it = checkpoints.begin(); it != checkpoints.end(); it++)
		delete *it;
	checkpoints.clear();
}

Part *Model::addPart(Part *p)
{
	p->owner = this;
	p->refno = parts.size();
	parts += p;
	return p;
}

Joint *Model::addJoint(Joint *j)
{
	j->owner = this;
	j->refno = joints.size();
	joints += j;
	return j;
}

Neuro *Model::addNeuro(Neuro *n)
{
	n->owner = this;
	n->refno = neurons.size();
	neurons += n;
	return n;
}

void Model::removeNeuros(SList &nlist)
{
	FOREACH(Neuro *, nu, nlist)
	{
		int i = findNeuro(nu);
		if (i >= 0) removeNeuro(i);
	}
}

void Model::removePart(int partindex, int removeattachedjoints, int removeattachedneurons)
{
	Part *p = getPart(partindex);
	if (removeattachedjoints)
	{
		SList jlist;
		findJoints(jlist, p);
		FOREACH(Joint *, j, jlist)
		{
			int i = findJoint(j);
			if (i >= 0) removeJoint(i, removeattachedneurons);
		}
	}
	if (removeattachedneurons)
	{
		SList nlist;
		findNeuros(nlist, 0, p);
		removeNeuros(nlist);
	}
	parts -= partindex;
	delete p;
}

void Model::removeJoint(int jointindex, int removeattachedneurons)
{
	Joint *j = getJoint(jointindex);
	if (removeattachedneurons)
	{
		SList nlist;
		findNeuros(nlist, 0, 0, j);
		removeNeuros(nlist);
	}
	joints -= jointindex;
	delete j;
}

void Model::removeNeuro(int neuroindex, bool removereferences)
{
	Neuro *thisN = getNeuro(neuroindex);

	if (removereferences)
	{
		Neuro *n;
		// remove all references to thisN
		for (int i = 0; n = (Neuro *)neurons(i); i++)
		{
			Neuro *inp;
			for (int j = 0; inp = n->getInput(j); j++)
				if (inp == thisN)
				{
					n->removeInput(j);
					j--;
				}
		}
	}

	neurons -= neuroindex;
	delete thisN;
}

MultiMap &Model::getMap()
{
	if (!map) map = new MultiMap();
	return *map;
}

void Model::delMap()
{
	if (map) { delete map; map = 0; }
}
void Model::delF0Map()
{
	if (f0map) { delete f0map; f0map = 0; }
}

void Model::makeGenToGenMap(MultiMap &result, const MultiMap &gen1tomodel, const MultiMap &gen2tomodel)
{
	result.clear();
	MultiMap m;
	m.addReversed(gen2tomodel);
	result.addCombined(gen1tomodel, m);
}

void Model::getCurrentToF0Map(MultiMap &result)
{
	result.clear();
	if (!map) return;
	const MultiMap &f0m = getF0Map();
	makeGenToGenMap(result, *map, f0m);
}

void Model::rebuild(bool buildm)
{
	autobuildmaps = buildm;
	clear();
	build();
}

void Model::initMap()
{
	if (!map) map = new MultiMap();
	else map->clear();
}

void Model::initF0Map()
{
	if (!f0map) f0map = new MultiMap();
	else f0map->clear();
}

Model::ItemType Model::itemTypeFromLinePrefix(const char *line)
{
	struct PrefixAndItem { const char *prefix; ItemType type; };
	static const PrefixAndItem types[] = { { "m:", ModelType }, { "p:", PartType }, { "j:", JointType }, { "n:", NeuronType }, { "c:", NeuronConnectionType }, { F0_CHECKPOINT_LINE, CheckpointType }, { NULL } };
	for (const PrefixAndItem *t = types; t->prefix != NULL; t++)
	{
		const char *in = line;
		const char *pattern = t->prefix;
		for (; *in == *pattern; in++, pattern++)
			if (*pattern == ':')
				return t->type;
	}
	return UnknownType;
}

void Model::build()
{
	f0errorposition = -1;
	f0warnposition = -1;
	MultiMap *convmap = autobuildmaps ? new MultiMap() : NULL;
	f0geno = (geno.getFormat() == '0') ? geno : geno.getConverted('0', convmap, using_checkpoints);
	f0genoknown = 1;
	if (f0geno.isInvalid())
	{
		buildstatus = invalid;
		if (convmap) delete convmap;
		return;
	}
	SString f0txt = f0geno.getGenes();
	buildstatus = building; // was: open();
	if (autobuildmaps)
	{
		partmappingchanged = 0;
		initMap();
		initF0Map();
	}
	int pos = 0, lnum = 1, lastpos = 0;
	SString line;
	MultiRange frommap;
	LoggerToMemory mh(LoggerBase::Enable | LoggerBase::DontBlock);
	Model *current_model = this;
	for (; f0txt.getNextToken(pos, line, '\n'); lnum++)
	{
		const char *line_ptr = line.c_str();
		for (; *line_ptr; line_ptr++)
			if (!strchr(" \r\t", *line_ptr)) break;
		if (*line_ptr == '#') continue;
		if (!*line_ptr) continue;

		const char *colon = strchr(line_ptr, ':');
		ItemType type = UnknownType;
		SString excluding_prefix;
		if (colon != NULL)
		{
			colon++;
			type = itemTypeFromLinePrefix(line_ptr);
			for (; *colon; colon++)
				if (!strchr(" \r\t", *colon)) break;
			excluding_prefix = colon;
		}

		if (autobuildmaps)
		{
			frommap.clear();
			frommap.add(lastpos, pos - 1);
		}
		mh.reset();
		if (type == CheckpointType)
		{
			current_model->close();
			current_model = new Model;
			current_model->open(false, true);
			checkpoints.push_back(current_model);
		}
		else if (current_model->addFromString(type, excluding_prefix, lnum, autobuildmaps ? (&frommap) : 0) == -1)
		{
			buildstatus = invalid;
			f0errorposition = lastpos;
			if (convmap) delete convmap;
			return;
		}
		if (mh.getWarningCount())
		{
			if (f0warnposition < 0) f0warnposition = lastpos;
		}
		lastpos = pos;
	}
	mh.disable();
	current_model->close();
	if (convmap)
	{
		*f0map = *map;
		if (geno.getFormat() != '0')
		{
			MultiMap tmp;
			tmp.addCombined(*convmap, getMap());
			*map = tmp;
		}
		delete convmap;
	}
}

const MultiMap &Model::getF0Map()
{
	if (!f0map)
	{
		f0map = new MultiMap();
		makeGeno(f0geno, f0map);
		f0genoknown = 1;
	}
	return *f0map;
}

Geno Model::rawGeno()
{
	Geno tmpgen;
	makeGeno(tmpgen);
	return tmpgen;
}

#include <iostream>
using  namespace std;
void Model::makeGeno(Geno &g, MultiMap *map, bool handle_defaults)
{
	if ((buildstatus != valid) && (buildstatus != building))
	{
		g = Geno(0, 0, 0, "invalid model");
		return;
	}

	SString gen;

	Param modelparam(f0_model_paramtab);
	Param partparam(f0_part_paramtab);
	Param jointparam(f0_joint_paramtab);
	Param neuroparam(f0_neuro_paramtab);
	Param connparam(f0_neuroconn_paramtab);

	static Part defaultpart;
	static Joint defaultjoint;
	static Neuro defaultneuro;
	static Model defaultmodel;
	static NeuroConn defaultconn;
	//static NeuroItem defaultneuroitem;

	Part *p;
	Joint *j;
	Neuro *n;
	int i;
	int len;
	int a, b;
	//NeuroItem *ni;

	SString mod_props;
	modelparam.select(this);
	modelparam.saveSingleLine(mod_props, handle_defaults ? &defaultmodel : NULL, true, !handle_defaults);
	if (mod_props.len() > 1) //are there any non-default values? ("\n" is empty)
	{
		gen += "m:";
		gen += mod_props;
	}
	for (i = 0; p = (Part *)parts(i); i++)
	{
		partparam.select(p);
		len = gen.len();
		gen += "p:";
		partparam.saveSingleLine(gen, handle_defaults ? &defaultpart : NULL, true, !handle_defaults);
		if (map)
			map->add(len, gen.len() - 1, partToMap(i));
	}
	for (i = 0; j = (Joint *)joints(i); i++)
	{
		jointparam.select(j);
		len = gen.len();
		jointparam.setParamTab(j->usedelta ? f0_joint_paramtab : f0_nodeltajoint_paramtab);
		gen += "j:";
		jointparam.saveSingleLine(gen, handle_defaults ? &defaultjoint : NULL, true, !handle_defaults);
		if (map)
			map->add(len, gen.len() - 1, jointToMap(i));
	}
	for (i = 0; n = (Neuro *)neurons(i); i++)
	{
		neuroparam.select(n);
		len = gen.len();
		gen += "n:";
		neuroparam.saveSingleLine(gen, handle_defaults ? &defaultneuro : NULL, true, !handle_defaults);
		if (map)
			map->add(len, gen.len() - 1, neuroToMap(i));
	}
	for (a = 0; a < neurons.size(); a++)
	{ // inputs
		n = (Neuro *)neurons(a);
		//	if ((n->getInputCount()==1)&&(n->getInput(0).refno <= n->refno))
		//		continue; // already done with Neuro::conn_refno

		for (b = 0; b < n->getInputCount(); b++)
		{
			double w;
			NeuroConn nc;
			Neuro *n2 = n->getInput(b, w);
			//		if (((n2.parentcount==1)&&(n2.parent)&&(n2.parent->refno < n2.refno)) ^ 
			//		    (n2.neuro_refno>=0))
			//			printf("!!!! bad Neuro::neuro_refno ?!\n");

			//		if ((n2.parentcount==1)&&(n2.parent)&&(n2.parent->refno < n2.refno))
			//		if (n2.neuro_refno>=0)
			//			continue; // already done with Neuro::neuro_refno

			nc.n1_refno = n->refno; nc.n2_refno = n2->refno;
			nc.weight = w;
			SString **s = n->inputInfo(b);
			if ((s) && (*s))
				nc.info = **s;
			connparam.select(&nc);
			len = gen.len();
			gen += "c:";
			connparam.saveSingleLine(gen, handle_defaults ? &defaultconn : NULL, true, !handle_defaults);
			if (map)
				map->add(len, gen.len() - 1, neuroToMap(n->refno));
		}
	}

	for (vector<Model *>::const_iterator it = checkpoints.begin(); it != checkpoints.end(); it++)
	{
		Geno g = (*it)->getF0Geno();
		gen += F0_CHECKPOINT_LINE "\n";
		gen += g.getGenes();
	}

	g = Geno(gen.c_str(), '0');
}

//////////////

void Model::open(bool _using_checkpoints, bool _is_checkpoint)
{
	if (buildstatus == building) return;
	using_checkpoints = _using_checkpoints;
	is_checkpoint = _is_checkpoint;
	buildstatus = building;
	modelfromgenotype = 0;
	partmappingchanged = 0;
	f0genoknown = 0;
	delMap();
}

int Model::getCheckpointCount()
{
	return checkpoints.size();
}

Model *Model::getCheckpoint(int i)
{
	return checkpoints[i];
}

void Model::checkpoint()
{
	if (!using_checkpoints) return;
	updateRefno();
	Model *m = new Model(*this, false, false, true);
	checkpoints.push_back(m);
}

void Model::setGeno(const Geno &newgeno)
{
	geno = newgeno;
}

void Model::clearMap()
{
	Part *p; Joint *j; Neuro *n;
	int i;
	delMap();
	delF0Map();
	for (i = 0; p = (Part *)parts(i); i++)
		p->clearMapping();
	for (i = 0; j = (Joint *)joints(i); i++)
		j->clearMapping();
	for (i = 0; n = (Neuro *)neurons(i); i++)
		n->clearMapping();
}

int Model::close(bool building_live_model)
{
	if (buildstatus != building)
		logPrintf("Model", "close", LOG_WARN, "Unexpected close() - no open()");
    if (internalcheck(is_checkpoint ? CHECKPOINT_CHECK : (building_live_model ? LIVE_CHECK : FINAL_CHECK)) > 0)
	{
        buildstatus = valid;

		if (partmappingchanged)
		{
			getMap();
			Part *p; Joint *j; Neuro *n;
			int i;
			for (i = 0; p = (Part *)parts(i); i++)
				if (p->getMapping())
					map->add(*p->getMapping(), partToMap(i));
			for (i = 0; j = (Joint *)joints(i); i++)
				if (j->getMapping())
					map->add(*j->getMapping(), jointToMap(i));
			for (i = 0; n = (Neuro *)neurons(i); i++)
				if (n->getMapping())
					map->add(*n->getMapping(), neuroToMap(i));
		}
	}
	else
		buildstatus = invalid;

	return (buildstatus == valid);
}

int Model::validate()
{
	return internalcheck(EDITING_CHECK);
}

Pt3D Model::whereDelta(const Part &start, const Pt3D &rot, const Pt3D &delta)
{
	Orient roto;
	roto = rot;
	Orient o;
	roto.transform(o, start.o);
	//o.x=start.o/roto.x;
	//o.y=start.o/roto.y;
	//o.z=start.o/roto.z;
	return o.transform(delta) + start.p;
}

int Model::addFromString(ItemType item_type, const SString &singleline, const MultiRange *srcrange)
{
	return addFromString(item_type, singleline, 0, srcrange);
}

int Model::addFromString(ItemType item_type, const SString &singleline, int line_num, const MultiRange *srcrange)
{
	SString error_message;
	int result = addFromStringNoLog(item_type, singleline, error_message, srcrange);
	if (result < 0)
	{
		if (error_message.len() == 0) // generic error when no detailed message is available
			error_message = "Invalid f0 code";
		if (line_num > 0)
			error_message += SString::sprintf(", line #%d", line_num);
		error_message += nameForErrors();
		logPrintf("Model", "build", LOG_ERROR, "%s", error_message.c_str());
	}
	return result;
}

int Model::addFromStringNoLog(ItemType item_type, const SString &line, SString &error_message, const MultiRange *srcrange)
{
	error_message = SString::empty();
	ParamInterface::LoadOptions opts;
	switch (item_type)
	{
	case PartType:
	{
		Param partparam(f0_part_paramtab);
		Part *p = new Part();
		partparam.select(p);
		partparam.load(ParamInterface::FormatSingleLine, line, &opts);
		if (opts.parse_failed) { delete p; error_message = "Invalid 'p:'"; return -1; }
		p->o.rotate(p->rot);
		parts += p;
		p->owner = this;
		if (srcrange) p->setMapping(*srcrange);
		return getPartCount() - 1;
	}

	case ModelType:
	{
		Param modelparam(f0_model_paramtab);
		modelparam.select(this);
		modelparam.load(ParamInterface::FormatSingleLine, line, &opts);
		if (opts.parse_failed) { error_message = "Invalid 'm:'"; return -1; }
		return 0;
	}

	case JointType:
	{
		Param jointparam(f0_joint_paramtab);
		Joint *j = new Joint();
		jointparam.select(j);
		j->owner = this;
		jointparam.load(ParamInterface::FormatSingleLine, line, &opts);
		if (opts.parse_failed) { delete j; error_message = "Invalid 'j:'"; return -1; }
		bool p1_ok = false, p2_ok = false;
		if ((p1_ok = ((j->p1_refno >= 0) && (j->p1_refno < getPartCount()))) &&
			(p2_ok = ((j->p2_refno >= 0) && (j->p2_refno < getPartCount()))))
		{
			addJoint(j);
			if ((j->d.x != JOINT_DELTA_MARKER) || (j->d.y != JOINT_DELTA_MARKER) || (j->d.z != JOINT_DELTA_MARKER))
			{
				j->useDelta(1);
				j->resetDeltaMarkers();
			}
			j->attachToParts(j->p1_refno, j->p2_refno);
			if (srcrange) j->setMapping(*srcrange);
			return j->refno;
		}
		else
		{
			error_message = SString::sprintf("Invalid reference to Part #%d", p1_ok ? j->p1_refno : j->p2_refno);
			delete j;
			return -1;
		}
	}

	case NeuronType:
	{
		Param neuroparam(f0_neuro_paramtab);
		Neuro *nu = new Neuro();
		neuroparam.select(nu);
		neuroparam.load(ParamInterface::FormatSingleLine, line, &opts);
		if (opts.parse_failed) { delete nu; error_message = "Invalid 'n:'"; return -1; }
		{
			// default class for unparented units: standard neuron
			if (nu->getClassName().len() == 0) nu->setClassName("N");
		}
		/*
			if (nu->conn_refno>=0) // input specified...
			{
			if (nu->conn_refno >= getNeuroCount()) // and it's illegal
			{
			delete nu;
			return -1;
			}
			Neuro *inputNU=getNeuro(nu->conn_refno);
			nu->addInput(inputNU,nu->weight);
			}
			*/
		nu->owner = this;
		// attach to part/joint
		if (nu->part_refno >= 0)
		{
			nu->attachToPart(nu->part_refno);
			if (nu->part == NULL)
			{
				error_message = SString::sprintf("Invalid reference to Part #%d", nu->part_refno); delete nu; return -1;
			}
		}
		if (nu->joint_refno >= 0)
		{
			nu->attachToJoint(nu->joint_refno);
			if (nu->joint == NULL)
			{
				error_message = SString::sprintf("Invalid reference to Joint #%d", nu->joint_refno); delete nu; return -1;
			}
		}
		if (srcrange) nu->setMapping(*srcrange);
		// todo: check part/joint ref# 
		{
			neurons += nu;
			return neurons.size() - 1;
		}
	}

	case NeuronConnectionType:
	{
		Param ncparam(f0_neuroconn_paramtab);
		NeuroConn c;
		ncparam.select(&c);
		ncparam.load(ParamInterface::FormatSingleLine, line, &opts);
		if (opts.parse_failed) { error_message = "Invalid 'c:'"; return -1; }
		bool n1_ok = false, n2_ok = false;
		if ((n1_ok = ((c.n1_refno >= 0) && (c.n1_refno < getNeuroCount())))
			&& (n2_ok = ((c.n2_refno >= 0) && (c.n2_refno < getNeuroCount()))))
		{
			Neuro *na = getNeuro(c.n1_refno);
			Neuro *nb = getNeuro(c.n2_refno);
			na->addInput(nb, c.weight, &c.info);
			if (srcrange)
				na->addMapping(*srcrange);
			return 0;
		}
		error_message = SString::sprintf("Invalid reference to Neuro #%d", n1_ok ? c.n2_refno : c.n1_refno);
		return -1;
	}

	case CheckpointType: case UnknownType: //handled by addFromString for uniform error handling
		return -1;
	}
	return -1;
}


/////////////

void Model::updateRefno()
{
	for (int i = 0; i < parts.size(); i++)
		getPart(i)->refno = i;
	for (int i = 0; i < joints.size(); i++)
	{
		Joint *j = getJoint(i);
		j->refno = i;
		if (j->part1 && j->part2 && (j->part1 != j->part2))
		{
			j->p1_refno = j->part1->refno;
			j->p2_refno = j->part2->refno;
		}
	}
	for (int i = 0; i < neurons.size(); i++)
		getNeuro(i)->refno = i;
}

#define VALIDMINMAX(var,template,field) \
if (var -> field < getMin ## template () . field) \
	{ var->field= getMin ## template () . field; \
	logPrintf("Model","internalCheck",LOG_WARN,# field " too small in " # template " #%d (adjusted)",i);} \
else if (var -> field > getMax ## template () . field) \
	{ var->field= getMax ## template ()  . field; \
	logPrintf("Model","internalCheck",LOG_WARN,# field " too big in " # template " #%d (adjusted)",i);}

#define LINKFLAG 0x8000000

SString Model::nameForErrors() const
{
	if (geno.getName().len() > 0)
		return SString::sprintf(" in '%s'", geno.getName().c_str());
	return SString::empty();
}

#include <iostream>
using  namespace std;
int Model::internalcheck(CheckType check)
{
	Part *p;
	Joint *j;
	Neuro *n;
	int i, k;
	int ret = 1;
	shape = SHAPE_UNKNOWN;
	updateRefno();
	if ((parts.size() == 0) && (neurons.size() == 0)) return 0;
	if (parts.size() == 0)
		size = Pt3D_0;
	else
	{
		Pt3D bbmin = ((Part *)parts(0))->p, bbmax = bbmin;
		for (i = 0; i < parts.size(); i++)
		{
			p = (Part *)parts(i);
			p->owner = this;
			if (checklevel > 0)
				p->mass = 0.0;
			//VALIDMINMAX(p,part,mass);//mass is very special
			// VALIDMINMAX are managed manually when adding part properties in f0-def!
			// (could be made dynamic but not really worth the effort)
			VALIDMINMAX(p, Part, size);
			VALIDMINMAX(p, Part, scale.x);
			VALIDMINMAX(p, Part, scale.y);
			VALIDMINMAX(p, Part, scale.z);
			VALIDMINMAX(p, Part, hollow);
			VALIDMINMAX(p, Part, density);
			VALIDMINMAX(p, Part, friction);
			VALIDMINMAX(p, Part, ingest);
			VALIDMINMAX(p, Part, assim);
			VALIDMINMAX(p, Part, vsize);
			VALIDMINMAX(p, Part, vcolor.x);
			VALIDMINMAX(p, Part, vcolor.y);
			VALIDMINMAX(p, Part, vcolor.z);
			p->flags &= ~LINKFLAG; // for delta joint cycle detection
			if (p->p.x - p->size < bbmin.x) bbmin.x = p->p.x - p->size;
			if (p->p.y - p->size < bbmin.y) bbmin.y = p->p.y - p->size;
			if (p->p.z - p->size < bbmin.z) bbmin.z = p->p.z - p->size;
			if (p->p.x + p->size > bbmax.x) bbmax.x = p->p.x + p->size;
			if (p->p.y + p->size > bbmax.y) bbmax.y = p->p.y + p->size;
			if (p->p.z + p->size > bbmax.z) bbmax.z = p->p.z + p->size;
			if (shape == SHAPE_UNKNOWN)
				shape = (p->shape == Part::SHAPE_BALL_AND_STICK) ? SHAPE_BALL_AND_STICK : SHAPE_SOLIDS;
			else if (shape != SHAPE_ILLEGAL)
			{
				if ((p->shape == Part::SHAPE_BALL_AND_STICK) ^ (shape == SHAPE_BALL_AND_STICK))
				{
					shape = SHAPE_ILLEGAL;
					logPrintf("Model", "internalCheck", LOG_WARN, "Inconsistent part shapes (mixed ball-and-stick and solids shape types)%s", nameForErrors().c_str());
				}
			}
		}
		size = bbmax - bbmin;
		for (i = 0; i < joints.size(); i++)
		{
			j = (Joint *)joints(i);
			// VALIDMINMAX are managed manually when adding joint properties in f0-def!
			// (could be made dynamic but not really worth the effort)
			VALIDMINMAX(j, Joint, stamina);
			VALIDMINMAX(j, Joint, stif);
			VALIDMINMAX(j, Joint, rotstif);
			VALIDMINMAX(p, Part, vcolor.x);
			VALIDMINMAX(p, Part, vcolor.y);
			VALIDMINMAX(p, Part, vcolor.z);
			j->refno = i;
			j->owner = this;
			if (j->part1 && j->part2 && (j->part1 != j->part2))
			{
				j->p1_refno = j->part1->refno;
				j->p2_refno = j->part2->refno;
				if (checklevel > 0)
				{
					j->part1->mass += 1.0;
					j->part2->mass += 1.0;
				}
				if ((j->usedelta) && ((j->d.x != JOINT_DELTA_MARKER) || (j->d.y != JOINT_DELTA_MARKER) || (j->d.z != JOINT_DELTA_MARKER)))
				{ // delta positioning -> calc. orient.
					if (j->part2->flags & LINKFLAG)
					{
						ret = 0;
						logPrintf("Model", "internalCheck", LOG_ERROR,
							"Delta joint cycle detected at Joint #%d%s",
							i, nameForErrors().c_str());
					}
					j->resetDeltaMarkers();
					j->o = j->rot;
					j->part1->o.transform(j->part2->o, j->o);
					//			j->part2->o.x=j->part1->o/j->o.x;
					//			j->part2->o.y=j->part1->o/j->o.y;
					//			j->part2->o.z=j->part1->o/j->o.z;
					j->part2->p = j->part2->o.transform(j->d) + j->part1->p;
					j->part2->flags |= LINKFLAG; j->part1->flags |= LINKFLAG; // for delta joint cycle detection
				}
				else
				{ // abs.positioning -> calc. delta
					if (check != EDITING_CHECK)
					{
						// calc orient delta
						//			Orient tmpo(j->part2->o);
						//			tmpo*=j->part1->o;
						Orient tmpo;
						j->part1->o.revTransform(tmpo, j->part2->o);
						tmpo.getAngles(j->rot);
						j->o = j->rot;
						// calc position delta
						Pt3D tmpp(j->part2->p);
						tmpp -= j->part1->p;
						j->d = j->part2->o.revTransform(tmpp);
					}
				}
				if ((check != LIVE_CHECK) && (check != CHECKPOINT_CHECK))
				{
					if (j->shape != Joint::SHAPE_FIXED)
					{
						if (j->d() > getMaxJoint().d.x)
						{
							ret = 0;
							logPrintf("Model", "internalCheck", LOG_ERROR, "Joint #%d too long (its length %g exceeds allowed %g)%s", i, j->d(), getMaxJoint().d.x, nameForErrors().c_str());
						}
					}
				}
			}
			else
			{
				logPrintf("Model", "internalCheck", LOG_ERROR, "Illegal part references in Joint #%d%s", i, nameForErrors().c_str());
				ret = 0;
			}
			if (shape != SHAPE_ILLEGAL)
			{
				if ((j->shape == Joint::SHAPE_BALL_AND_STICK) ^ (shape == SHAPE_BALL_AND_STICK))
				{
					shape = SHAPE_ILLEGAL;
					logPrintf("Model", "internalCheck", LOG_WARN, "Inconsistent joint shapes (mixed old and new shapes)%s", nameForErrors().c_str());
				}
			}
		}
	}

	for (i = 0; i < neurons.size(); i++)
	{
		n = (Neuro *)neurons(i);
		n->part_refno = (n->part) ? n->part->refno : -1;
		n->joint_refno = (n->joint) ? n->joint->refno : -1;
	}

	if (check != CHECKPOINT_CHECK)
	{

		if (parts.size() && (checklevel > 0))
		{
			for (i = 0; i < parts.size(); i++)
			{
				p = (Part *)parts(i);
				if (p->mass <= 0.001)
					p->mass = 1.0;
				p->flags &= ~LINKFLAG;
			}
			getPart(0)->flags |= LINKFLAG;
			int change = 1;
			while (change)
			{
				change = 0;
				for (i = 0; i < joints.size(); i++)
				{
					j = (Joint *)joints(i);
					if (j->part1->flags & LINKFLAG)
					{
						if (!(j->part2->flags & LINKFLAG))
						{
							change = 1;
							j->part2->flags |= LINKFLAG;
						}
					}
					else
						if (j->part2->flags & LINKFLAG)
						{
							if (!(j->part1->flags & LINKFLAG))
							{
								change = 1;
								j->part1->flags |= LINKFLAG;
							}
						}
				}
			}
			for (i = 0; i < parts.size(); i++)
			{
				p = (Part *)parts(i);
				if (!(p->flags & LINKFLAG))
				{
					logPrintf("Model", "internalCheck", LOG_ERROR, "Not all parts connected (eg. Part #0 and Part #%d)%s", i, nameForErrors().c_str());
					ret = 0;
					break;
				}
			}
		}

		for (i = 0; i < joints.size(); i++)
		{
			j = (Joint *)joints(i);
			if (j->p1_refno == j->p2_refno)
			{
				logPrintf("Model", "internalCheck", LOG_ERROR, "Illegal self connection, Joint #%d%s", i, nameForErrors().c_str());
				ret = 0;
				break;
			}
			for (k = i + 1; k < joints.size(); k++)
			{
				Joint *j2 = (Joint *)joints(k);
				if (((j->p1_refno == j2->p1_refno) && (j->p2_refno == j2->p2_refno))
					|| ((j->p1_refno == j2->p2_refno) && (j->p2_refno == j2->p1_refno)))
				{
					logPrintf("Model", "internalCheck", LOG_ERROR, "Illegal duplicate Joint #%d and Joint #%d%s", i, k, nameForErrors().c_str());
// TODO make sure this is fixed
//					ret = 0;
					break;
				}
			}
		}
	}

	if (shape == SHAPE_ILLEGAL)
		ret = 0;
	return ret;
}

/////////////

int Model::getErrorPosition(bool includingwarnings)
{
	return includingwarnings ?
		((f0errorposition >= 0) ? f0errorposition : f0warnposition)
		:
		f0errorposition;
}

const Geno &Model::getGeno() const
{
	return geno;
}

const Geno Model::getF0Geno()
{
	if (buildstatus == building)
		logPrintf("Model", "getGeno", LOG_WARN, "Model was not completed - missing close()");
	if (buildstatus != valid)
		return Geno("", '0', "", "invalid");
	if (!f0genoknown)
	{
		if (autobuildmaps)
		{
			initF0Map();
			makeGeno(f0geno, f0map);
		}
		else
		{
			delF0Map();
			makeGeno(f0geno);
		}
		f0genoknown = 1;
	}
	return f0geno;
}

int Model::getPartCount() const
{
	return parts.size();
}

Part *Model::getPart(int i) const
{
	return ((Part *)parts(i));
}

int Model::getJointCount() const
{
	return joints.size();
}

Joint *Model::getJoint(int i) const
{
	return ((Joint *)joints(i));
}

int Model::findJoints(SList &result, const Part *part)
{
	Joint *j;
	int n0 = result.size();
	if (part)
		for (int i = 0; j = (Joint *)joints(i); i++)
			if ((j->part1 == part) || (j->part2 == part)) result += (void *)j;
	return result.size() - n0;
}

int Model::findNeuro(Neuro *n)
{
	return neurons.find(n);
}

int Model::findPart(Part *p)
{
	return parts.find(p);
}

int Model::findJoint(Joint *j)
{
	return joints.find(j);
}

int Model::findJoint(Part *p1, Part *p2)
{
	Joint *j;
	for (int i = 0; j = getJoint(i); i++)
		if ((j->part1 == p1) && (j->part2 == p2)) return i;
	return -1;
}

///////////////////////

int Model::getNeuroCount() const
{
	return neurons.size();
}

Neuro *Model::getNeuro(int i) const
{
	return (Neuro *)neurons(i);
}

int Model::getConnectionCount() const
{
	int n = 0;
	for (int i = 0; i < getNeuroCount(); i++)
		n += getNeuro(i)->getInputCount();
	return n;
}

int Model::findNeuros(SList &result,
	const char *classname, const Part *part, const Joint *joint)
{
	Neuro *nu;
	SString cn(classname);
	int n0 = result.size();
	for (int i = 0; nu = (Neuro *)neurons(i); i++)
	{
		if (part)
			if (nu->part != part) continue;
		if (joint)
			if (nu->joint != joint) continue;
		if (classname)
			if (nu->getClassName() != cn) continue;
		result += (void *)nu;
	}
	return result.size() - n0;
}

///////////////////

void Model::disturb(double amount)
{
	int i;
	if (amount <= 0) return;
	for (i = 0; i < parts.size(); i++)
	{
		Part *p = getPart(i);
		p->p.x += (rndDouble(1) - 0.5) * amount;
		p->p.y += (rndDouble(1) - 0.5) * amount;
		p->p.z += (rndDouble(1) - 0.5) * amount;
	}
	for (i = 0; i < joints.size(); i++)
	{
		Joint *j = getJoint(i);
		Pt3D tmpp(j->part2->p);
		tmpp -= j->part1->p;
		j->d = j->part2->o.revTransform(tmpp);
	}
}

void Model::move(const Pt3D &shift)
{
	FOREACH(Part *, p, parts)
		p->p += shift;
}

void Model::rotate(const Orient &rotation)
{
	FOREACH(Part *, p, parts)
	{
		p->p = rotation.transform(p->p);
		p->setOrient(rotation.transform(p->o));
	}
}

void Model::buildUsingSolidShapeTypes(const Model &src_ballandstick_shapes, Part::Shape use_shape, double thickness)
{
	for (int i = 0; i < src_ballandstick_shapes.getJointCount(); i++)
	{
		Joint *oj = src_ballandstick_shapes.getJoint(i);
		Part *p = addNewPart(use_shape);
		p->p = (oj->part1->p + oj->part2->p) / 2;
		Orient o;
		o.lookAt(oj->part1->p - oj->part2->p);
		p->setRot(o.getAngles());
		p->scale.x = oj->part1->p.distanceTo(oj->part2->p) / 2;
		p->scale.y = thickness;
		p->scale.z = thickness;
	}
	if (src_ballandstick_shapes.getJointCount() == 0) //single part "ball-and-stick" models are valid so let's make a valid solid shape model
		for (int i = 0; i < src_ballandstick_shapes.getPartCount(); i++)
		{
			Part *op = src_ballandstick_shapes.getPart(i);
			Part *p = addNewPart(Part::SHAPE_ELLIPSOID); //always using spherical shape regardless of the 'use_shape' parameter - 'use shape' is meant for sticks!
			p->p = op->p;
			p->rot = op->rot;
			p->scale.x = p->scale.y = p->scale.z = thickness;
		}
	for (int i = 0; i < src_ballandstick_shapes.getPartCount(); i++)
	{
		Part *op = src_ballandstick_shapes.getPart(i);
		for (int j = 0; j < src_ballandstick_shapes.getJointCount(); j++)
		{
			Joint *oj = src_ballandstick_shapes.getJoint(j);
			if ((oj->part1 == op) || (oj->part2 == op))
			{
				for (int j2 = j + 1; j2 < src_ballandstick_shapes.getJointCount(); j2++)
				{
					Joint *oj2 = src_ballandstick_shapes.getJoint(j2);
					if ((oj2->part1 == op) || (oj2->part2 == op))
					{
						addNewJoint(getPart(j), getPart(j2), Joint::SHAPE_FIXED);
					}
				}
				break;
			}
		}
	}
}

SolidsShapeTypeModel::SolidsShapeTypeModel(Model &m, Part::Shape use_shape, double thickness)
{
	using_model = converted_model = NULL;
	if (m.getShapeType() == Model::SHAPE_BALL_AND_STICK)
	{
		converted_model = new Model;
		converted_model->open();
		converted_model->buildUsingSolidShapeTypes(m, use_shape, thickness);
		converted_model->close();
		using_model = converted_model;
	}
	else
	{
		converted_model = NULL;
		using_model = &m;
	}
}

//////////////////////

int Model::elementToMap(ItemType type, int index)
{
	switch (type)
	{
	case PartType: return partToMap(index);
	case JointType: return jointToMap(index);
	case NeuronType: return neuroToMap(index);
	default: return -1;
	}
}

Model::TypeAndIndex Model::mapToElement(int map_index)
{
	if ((map_index >= 0) && (map_index < MODEL_MAPPING_OFFSET))
		return TypeAndIndex(PartType, mapToPart(map_index));
	if ((map_index >= MODEL_MAPPING_OFFSET) && (map_index < 2 * MODEL_MAPPING_OFFSET))
		return TypeAndIndex(JointType, mapToJoint(map_index));
	if ((map_index >= 2 * MODEL_MAPPING_OFFSET) && (map_index < 3 * MODEL_MAPPING_OFFSET))
		return TypeAndIndex(NeuronType, mapToNeuro(map_index));
	return TypeAndIndex();
}

int Model::partToMap(int i) { return MODEL_MAPPING_OFFSET + i; }
int Model::jointToMap(int i) { return 2 * MODEL_MAPPING_OFFSET + i; }
int Model::neuroToMap(int i) { return 3 * MODEL_MAPPING_OFFSET + i; }
int Model::mapToPart(int i) { return i - MODEL_MAPPING_OFFSET; }
int Model::mapToJoint(int i) { return i - 2 * MODEL_MAPPING_OFFSET; }
int Model::mapToNeuro(int i) { return i - 3 * MODEL_MAPPING_OFFSET; }


//////////////////////

class MinPart : public Part { public: MinPart() { Param par(f0_part_paramtab, this); par.setMin(); } };
class MaxPart : public Part { public: MaxPart() { Param par(f0_part_paramtab, this); par.setMax(); } };
class MinJoint : public Joint { public: MinJoint() { Param par(f0_joint_paramtab, this); par.setMin(); } };
class MaxJoint : public Joint { public: MaxJoint() { Param par(f0_joint_paramtab, this); par.setMax(); } };
class MinNeuro : public Neuro { public: MinNeuro() { Param par(f0_neuro_paramtab, this); par.setMin(); } };
class MaxNeuro : public Neuro { public: MaxNeuro() { Param par(f0_neuro_paramtab, this); par.setMax(); } };

Part &Model::getMinPart() { static MinPart part; return part; }
Part &Model::getMaxPart() { static MaxPart part; return part; }
Part &Model::getDefPart() { static Part part; return part; }
Joint &Model::getMinJoint() { static MinJoint joint; return joint; }
Joint &Model::getMaxJoint() { static MaxJoint joint; return joint; }
Joint &Model::getDefJoint() { static Joint joint; return joint; }
Neuro &Model::getMinNeuro() { static MinNeuro neuro; return neuro; }
Neuro &Model::getMaxNeuro() { static MaxNeuro neuro; return neuro; }
Neuro &Model::getDefNeuro() { static Neuro neuro; return neuro; }
