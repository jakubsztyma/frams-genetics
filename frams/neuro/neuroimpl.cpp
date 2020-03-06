// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "neuroimpl.h"
#include "neurofactory.h"
#include <frams/util/rndutil.h>
#include <common/nonstd_math.h>
#ifndef SDK_WITHOUT_FRAMS
#include <frams/simul/creature.h>
#include <frams/mech/creatmechobj.h>
#include <frams/simul/livegroups.h>
#include <frams/simul/simul.h>
#endif

const int NeuroImpl::ENDDRAWING = -9999;
const int NeuroImpl::MAXDRAWINGXY = 0xffff;

int NeuroNetImpl::mytags_id = 0;

/////////////////////////////////////////////////////////

#define FIELDSTRUCT NeuroNetConfig
static ParamEntry nncfg_paramtab[] =
{
	{ "Creature: Neurons", 1, 3, "nnsim", },
	{ "randinit", 1, 0, "Random initialization", "f 0 10 0.01", FIELD(randominit), "Allowed range for initializing all neuron states with uniform distribution random numbers and zero mean. Set to 0 for deterministic initialization." },
	{ "nnoise", 1, 0, "Noise", "f 0 1 0", FIELD(nnoise), "Gaussian neural noise: a random value is added to each neural output in each simulation step. Set standard deviation here to add random noise, or 0 for deterministic simulation." },
	{ "touchrange", 1, 0, "T receptor range", "f 0 100 1", FIELD(touchrange), },
	{ 0, 0, 0, },
};
#undef FIELDSTRUCT

NeuroNetConfig::NeuroNetConfig(NeuroFactory *fac)
	:par(nncfg_paramtab, this),
	randominit(0.01),
	nnoise(0),
	touchrange(1),
	factory(fac)
{}

/////////////////////////////////////////////////////////////////

NeuroNetImpl::NeuroNetImpl(Model& model, NeuroNetConfig& conf
#ifdef NEURO_SIGNALS
	, ChannelSpace *ch
#endif
	)
	:mod(model), config(conf),
	isbuilt(1), errorcount(0)
#ifdef NEURO_SIGNALS
	, channels(ch)
#endif
{
	if (!mytags_id) mytags_id = mod.userdata.newID();

	Neuro *n;
	NeuroImpl *ni;
	Joint *j;
	int i;
	DB(printf("makeNeuroNet(%p)\n", &mod));

	minorder = 3; maxorder = 0;
	errorcount = 0;

	for (i = 0; j = mod.getJoint(i); i++)
		j->flags &= ~(4 + 8); // todo: !!!neuroitems shouldn't use model fields!!!

	for (i = 0; n = mod.getNeuro(i); i++)
	{
		ni = conf.factory->createNeuroImpl(n);
		n->userdata[mytags_id] = ni;
		if (!ni)
		{
			errorcount++;
			logPrintf("NeuroNetImpl", "create", LOG_WARN, "neuron #%d (%s) implementation not available",
				i, n->getClassName().c_str());
			continue;
		} // implementation not available?!
		ni->owner = this;
		ni->neuro = n;
		ni->readParam();
	}

	for (i = 0; n = mod.getNeuro(i); i++)
	{
		n->state += (rndDouble(1) - 0.5)*config.randominit;
		ni = (NeuroImpl*)n->userdata[mytags_id];
		if (!ni) continue;
		if (!ni->lateinit())
		{
			ni->status = NeuroImpl::InitError;
			errorcount++;
			logPrintf("NeuroNetImpl", "create", LOG_WARN, "neuron #%d (%s) initialization failed",
				i, n->getClassName().c_str());
			continue;
		}
		ni->status = NeuroImpl::InitOk;
		int order = ni->getSimOrder();
		if (order < 0) order = 0; else if (order>2) order = 2;
		if (order < minorder) minorder = order;
		if (order > maxorder) maxorder = order;
		neurons[order] += ni;
		if (ni->getNeedPhysics())
			neurons[3] += ni;
	}
	cnode = mod.delmodel_list.add(STATRICKCALLBACK(this, &NeuroNetImpl::destroyNN, 0));
}

void NeuroNetImpl::destroyNN(CALLBACKARGS)
{
	if (!isbuilt) return;
	DB(printf("destroyNeuroNet(%p)\n", &mod));
	NeuroImpl *ni;
	Neuro *n;
	for (int i = 0; n = mod.getNeuro(i); i++)
	{
		ni = (NeuroImpl*)n->userdata[mytags_id];
		delete ni;
		n->userdata[mytags_id] = 0;
	}
	mod.delmodel_list.remove(cnode);
	isbuilt = 0; errorcount = 0;
	delete this;
}

NeuroNetImpl::~NeuroNetImpl()
{
	destroyNN(0, 0);
}

void NeuroNetImpl::simulateNeuroNet()
{
	NeuroImpl *ni;
	for (int order = minorder; order <= maxorder; order++)
	{
		int i;
		SList &nlist = neurons[order];
		for (i = 0; ni = (NeuroImpl*)nlist(i); i++)
			ni->go();
		for (i = 0; ni = (NeuroImpl*)nlist(i); i++)
			ni->commit();
	}
}

void NeuroNetImpl::simulateNeuroPhysics()
{
	NeuroImpl *ni;
	int i;
	SList &nlist = neurons[3];
	for (i = 0; ni = (NeuroImpl*)nlist(i); i++)
		ni->goPhysics();
}

///////////////////////////////////////////////

void NeuroImpl::setChannelCount(int c)
{
	if (c < 1) c = 1;
	if (c == channels) return;
	if (c < channels) { channels = c; chstate.trim(c - 1); chnewstate.trim(c - 1); return; }
	double s = getState(channels - 1);
	chnewstate.setSize(c - 1);
	chstate.setSize(c - 1);
	for (int i = channels; i < c; i++)
	{
		chstate(i - 1) = s;
		chnewstate(i - 1) = s;
	}
	channels = c;
}

void NeuroImpl::setState(double st, int channel)
{
	validateNeuroState(st);
	if (channel >= channels) channel = channels - 1;
	if (channel <= 0) { newstate = st; return; }
	chnewstate(channel - 1) = st;
}

void NeuroImpl::setCurrentState(double st, int channel)
{
	validateNeuroState(st);
	if (channel >= channels) channel = channels - 1;
	if (channel <= 0) { neuro->state = st; return; }
	chstate(channel - 1) = st;
}

double NeuroImpl::getNewState(int channel)
{
	if (neuro->flags&Neuro::HoldState) return getState(channel);
	if (channel >= channels) channel = channels - 1;
	if (channel <= 0) { return newstate; }
	return chnewstate(channel - 1);
}

double NeuroImpl::getState(int channel)
{
	if (channel >= channels) channel = channels - 1;
	if (channel <= 0) return neuro->state;
	return chstate(channel - 1);
}

void NeuroImpl::commit()
{
	if (!(neuro->flags&Neuro::HoldState))
	{
		if (channels > 1)
			chstate = chnewstate;
		neuro->state = newstate;
		if (owner->getConfig().nnoise > 0.0)
		{
			neuro->state += RndGen.GaussStd()*owner->getConfig().nnoise;
			if (channels > 1)
				for (int i = 0; i < chstate.size(); i++)
					chstate(0) += RndGen.GaussStd()*owner->getConfig().nnoise;
		}
	}
}

int NeuroImpl::getInputChannelCount(int i)
{
	if ((i < 0) || (i >= neuro->getInputCount())) return 1;
	Neuro *nu = neuro->getInput(i);
	NeuroImpl *ni = NeuroNetImpl::getImpl(nu);
	if (!ni) return 1;
	return ni->channels;
}

double NeuroImpl::getInputState(int i, int channel)
{
	if ((i < 0) || (i >= neuro->getInputCount())) return 0;
	Neuro *nu = neuro->getInput(i);
	if (channel <= 0) return nu->state;
	NeuroImpl *ni = NeuroNetImpl::getImpl(nu);
	if (!ni) return nu->state;
	if (channel >= ni->channels) channel = ni->channels - 1;
	if (!channel) return nu->state;
	return ni->chstate(channel - 1);
}

double NeuroImpl::getWeightedInputState(int i, int channel)
{
	if ((i < 0) || (i >= neuro->getInputCount())) return 0;
	double w;
	Neuro *nu = neuro->getInput(i, w);
	if (channel <= 0) return nu->state * w;
	NeuroImpl *ni = NeuroNetImpl::getImpl(nu);
	if (!ni) return nu->state * w;
	if (channel >= ni->channels) channel = ni->channels - 1;
	if (!channel) return w * nu->state;
	return w * ni->chstate(channel - 1);
}

double NeuroImpl::getInputSum(int startwith)
{
	if (startwith < 0) return 0;
	Neuro *inp;
	double sum = 0.0;
	while (inp = neuro->getInput(startwith++))
		sum += inp->state;
	return sum;
}

double NeuroImpl::getWeightedInputSum(int startwith)
{
	if (startwith < 0) return 0;
	Neuro *inp;
	double sum = 0.0;
	double w;
	while (inp = neuro->getInput(startwith++, w))
		sum += inp->state*w;
	return sum;
}

void NeuroImpl::readParam()
{
	Param par;
	if (!paramentries) return;
	par.setParamTab(paramentries);
	par.select(this);
	par.setDefault();
	par.load(ParamInterface::FormatSingleLine, neuro->getClassParams());
}

Param& NeuroImpl::getStaticParam()
{
	static Param p(neuroimpl_tab, 0, "Neuro");
	return p;
}

/////////////////////////////

#ifdef NEURO_SIGNALS
#define NEUROIMPL_SIGNAL_PROPS 1
#else
#define NEUROIMPL_SIGNAL_PROPS 0
#endif

#define FIELDSTRUCT NeuroImpl
ParamEntry neuroimpl_tab[] =
{
	{ "Neuro", 1, 27 + NEUROIMPL_SIGNAL_PROPS, "Neuro", "Live Neuron object." },

	{ "getInputState", 0, 0, "Get input signal", "p f(d input)", PROCEDURE(p_get), },
	{ "getInputWeight", 0, 0, "Get input weight", "p f(d input)", PROCEDURE(p_getweight), },
	{ "getWeightedInputState", 0, 0, "Get weighted input signal", "p f(d input)", PROCEDURE(p_getw), },
	{ "getInputSum", 0, 0, "Get signal sum", "p f(d input)", PROCEDURE(p_getsum), },
	{ "getWeightedInputSum", 0, 0, "Get weighted signal sum", "p f(d input)", PROCEDURE(p_getwsum), "Uses any number of inputs starting with the specified input. getWeightedInputSum(0)=weightedInputSum" },
	{ "getInputCount", 0, PARAM_READONLY, "Get input count", "d", GETONLY(count), },
	{ "inputSum", 0, PARAM_READONLY, "Full signal sum", "f", GETONLY(sum), },
	{ "weightedInputSum", 0, PARAM_READONLY, "Full weighted signal sum", "f", GETONLY(wsum), },
	{ "getInputChannelCount", 0, 0, "Get channel count for input", "p d(d input)", PROCEDURE(p_getchancount), },
	{ "getInputStateChannel", 0, 0, "Get input signal from channel", "p f(d input,d channel)", PROCEDURE(p_getchan), },
	{ "getWeightedInputStateChannel", 0, 0, "Get weighted input signal from channel", "p f(d input,d channel)", PROCEDURE(p_getwchan), },
	{ "state", 0, 0, "Neuron state (channel 0)", "f", GETSET(state), "When read, returns the current neuron state.\nWhen written, sets the 'internal' neuron state that will become current in the next step.\nTypically you should use this field, and not currState." },
	{ "channelCount", 0, 0, "Number of output channels", "d", GETSET(channels), },
	{ "getStateChannel", 0, 0, "Get state for channel", "p f(d channel)", PROCEDURE(p_getstate), },
	{ "setStateChannel", 0, 0, "Set state for channel", "p(d channel,f value)", PROCEDURE(p_setstate), },
	{ "hold", 0, 0, "Hold state", "d 0 1", GETSET(hold), "\"Holding\" means keeping the neuron state as is, blocking the regular neuron operation. This is useful when your script needs to inject some control signals into the NN. Without \"holding\", live neurons would be constantly overwriting your changes, and the rest of the NN could see inconsistent states, depending on the connections. Setting hold=1 ensures the neuron state will be only set by you, and not by the neuron. The enforced signal value can be set using Neuro.currState before or after setting hold=1. Set hold=0 to resume normal operation.", },
	{ "currState", 0, 0, "Current neuron state (channel 0)", "f", GETSET(cstate), "When read, it behaves just like the 'state' field.\nWhen written, changes the current neuron state immediately, which disturbs the regular synchronous NN operation.\nThis feature should only be used while controlling the neuron 'from outside' (like a neuro probe) and not in the neuron definition. See also: Neuro.hold", },
	{ "setCurrStateChannel", 0, 0, "Set current neuron state for channel", "p(d channel,f value)", PROCEDURE(p_setcstate), "Analogous to \"currState\"." },
	{ "position_x", 0, PARAM_READONLY, "Position x", "f", GETONLY(position_x), },
	{ "position_y", 0, PARAM_READONLY, "Position y", "f", GETONLY(position_y), },
	{ "position_z", 0, PARAM_READONLY, "Position z", "f", GETONLY(position_z), },
	{ "creature", 0, PARAM_READONLY, "Gets owner creature", "oCreature", GETONLY(creature), },
	{ "part", 0, PARAM_READONLY, "The Part object where this neuron is located", "oMechPart", GETONLY(part), },
	{ "joint", 0, PARAM_READONLY, "The Joint object where this neuron is located", "oMechJoint", GETONLY(joint), },
	{ "neuroproperties", 0, PARAM_READONLY, "Custom neuron fields", "oNeuroProperties", GETONLY(fields),
	"Neurons can have different fields depending on their class. Script neurons have their fields defined using the \"property:\" syntax. If you develop a custom neuron script you should use the NeuroProperties object for accessing your own neuron fields. The Neuro.neuroproperties property is meant for accessing the neuron fields from the outside script.\n"
	"Examples:\n"
	"var c=Populations.createFromString(\"X[N]\");\n"
	"Simulator.print(\"standard neuron inertia=\"+c.getNeuro(0).neuroproperties.in);\n"
	"c=Populations.createFromString(\"X[Nn,e:0.1]\");\n"
	"Simulator.print(\"noisy neuron error rate=\"+c.getNeuro(0).neuroproperties.e);\n"
	"\n"
	"The Interface object can be used to discover which fields are available for a certain neuron object:\n"
	"c=Populations.createFromString(\"X[N]\");\n"
	"var iobj=Interface.makeFrom(c.getNeuro(0).neuroproperties);\n"
	"var i;\n"
	"for(i=0;i<iobj.size;i++)\n"
	" Simulator.print(iobj.getId(i)+\" (\"+iobj.getName(i)+\")\");", },
	{ "def", 0, PARAM_READONLY, "Neuron definition from which this live neuron was built", "oNeuroDef", GETONLY(neurodef), },
	{ "classObject", 0, PARAM_READONLY, "Neuron class for this neuron", "oNeuroClass", GETONLY(classObject), },
#ifdef NEURO_SIGNALS
	{ "signals", 0, PARAM_READONLY, "Signals", "oNeuroSignals", FIELD(sigs_obj), },
#endif

	{ 0, 0, 0, },
};
#undef FIELDSTRUCT

#ifdef NEURO_SIGNALS
ParamEntry neurosignals_paramtab[] =
{
	{ "NeuroSignals", 1, 8, "NeuroSignals", "Signals attached to a neuron.\nSee also: Signal, WorldSignals, CreatureSignals.\nscripts/light.neuro and scripts/seelight.neuro are simple custom neuron examples demonstrating how to send/receive signals between creatures.", },

#define FIELDSTRUCT NeuroSignals
	SIGNPAR_ADD(""),
	SIGNPAR_RECEIVE(""),
	SIGNPAR_RECEIVESET(""),
	SIGNPAR_RECEIVEFILTER(""),
	SIGNPAR_RECEIVESINGLE(""),
#undef FIELDSTRUCT

#define FIELDSTRUCT SignalSet
	SIGNSETPAR_GET,
	SIGNSETPAR_SIZE,
	SIGNSETPAR_CLEAR,
#undef FIELDSTRUCT
	{ 0, 0, 0, },
};

Param& NeuroSignals::getStaticParam()
{
	static Param p(neurosignals_paramtab, 0);
	return p;
}
#endif

#ifdef NEURO_SIGNALS
class NeuroSigSource : public SigSource
{
protected:
	NeuroImpl* owner;
public:
	NeuroSigSource(NeuroImpl *n, Creature *c) :SigSource(0, c), owner(n) {}
	bool update();
};

bool NeuroSigSource::update()
{
	Pt3D p;
	if (owner->getPosition(p))
	{
		setLocation(p);
		return true;
	}
	return false;
}

Creature *NeuroSignals::getCreature()
{
	if (!cr)
	{
		cr = owner->getCreature();
	}
	return cr;
}

void NeuroSignals::p_add(PARAMPROCARGS)
{
	SigSource *s = new NeuroSigSource(owner, getCreature());
	if (owner->owner->channels)
	{
		SigChannel *ch = owner->owner->channels->getChannel(args->getString(), true);
		ch->addSource(s);
	}
	else
		SigChannel::dummy_channel.addSource(s);
	sigs += s;
	s->setupObject(ret);
}

void NeuroSignals::p_receive(PARAMPROCARGS)
{
	SigChannel *ch; Pt3D p;
	if (owner->owner->channels && (ch = owner->owner->channels->getChannel(args->getString(), false)) && owner->getPosition(p))
		ret->setDouble(ch->receive(&p, getCreature()));
	else
		ret->setDouble(0);
}

void NeuroSignals::p_receiveFilter(PARAMPROCARGS)
{
	SigChannel *ch; Pt3D p;
	if (owner->owner->channels && (ch = owner->owner->channels->getChannel(args[3].getString(), false)) && owner->getPosition(p))
		ret->setDouble(ch->receive(&p, getCreature(), args[2].getDouble(), args[1].getDouble(), args[0].getDouble()));
	else
		ret->setDouble(0);
}

void NeuroSignals::p_receiveSet(PARAMPROCARGS)
{
	SigChannel *ch; Pt3D p;
	SigVector *vec = new SigVector();
	if (owner->owner->channels && (ch = owner->owner->channels->getChannel(args[1].getString(), false)) && owner->getPosition(p))
		ch->receiveSet(vec, &p, getCreature(), args[0].getDouble());
	ret->setObject(vec->makeObject());
}

void NeuroSignals::p_receiveSingle(PARAMPROCARGS)
{
	SigChannel *ch; Pt3D p;
	if (owner->owner->channels && (ch = owner->owner->channels->getChannel(args[1].getString(), false)) && owner->getPosition(p))
	{
		SigSource *src = ch->receiveSingle(&p, getCreature(), args[0].getDouble(), 0, 1e99);
		if (src)
		{
			src->setupObject(ret);
			return;
		}
	}
	ret->setEmpty();
}
#endif

#ifndef SDK_WITHOUT_FRAMS
extern ParamEntry creature_paramtab[];
static Param creature_param(creature_paramtab, 0);
#endif

Creature* NeuroImpl::getCreature()
{
#ifndef SDK_WITHOUT_FRAMS
	CreatMechObject *cmo = (CreatMechObject *)neuro->owner->userdata[CreatMechObject::modeltags_id];
	return cmo->creature;
#else
	return 0;
#endif
}

void NeuroImpl::get_creature(ExtValue *ret)
{
#ifndef SDK_WITHOUT_FRAMS
	ret->setObject(ExtObject(&creature_param, getCreature()));
#endif
}

void NeuroImpl::get_part(ExtValue *ret)
{
#ifndef SDK_WITHOUT_FRAMS
	Part *pa;
	if (pa = neuro->getPart())
		ret->setObject(ExtObject(&MechPart::getStaticParam(), ((MechPart *)pa->userdata[CreatMechObject::modeltags_id])));
	else
		ret->setEmpty();
#endif
}

void NeuroImpl::get_joint(ExtValue *ret)
{
#ifndef SDK_WITHOUT_FRAMS
	Joint *jo;
	if (jo = neuro->getJoint())
		ret->setObject(ExtObject(&MechJoint::getStaticParam(), ((MechJoint*)jo->userdata[CreatMechObject::modeltags_id])));
	else
		ret->setEmpty();
#endif
}

bool NeuroImpl::getPosition(Pt3D &pos)
{
#ifndef SDK_WITHOUT_FRAMS
	Part *pa; Joint *jo;
	if (pa = neuro->getPart())
	{
		pos = ((MechPart *)pa->userdata[CreatMechObject::modeltags_id])->p;
		return true;
	}
	if (jo = neuro->getJoint())
	{
		if (neuro->getClass()->getVisualHints() & NeuroClass::AtFirstPart)
			pos = ((MechPart*)jo->part1->userdata[CreatMechObject::modeltags_id])->p;
		else if (neuro->getClass()->getVisualHints() & NeuroClass::AtSecondPart)
			pos = ((MechPart*)jo->part2->userdata[CreatMechObject::modeltags_id])->p;
		else pos = (((MechPart*)jo->part1->userdata[CreatMechObject::modeltags_id])->p
			+ ((MechPart*)jo->part2->userdata[CreatMechObject::modeltags_id])->p) / 2;
		return true;
	}
#endif
	return false;
}

void NeuroImpl::get_position_x(ExtValue *ret)
{
	Pt3D pos;
	if (getPosition(pos)) ret->setDouble(pos.x); else ret->setEmpty();
}
void NeuroImpl::get_position_y(ExtValue *ret)
{
	Pt3D pos;
	if (getPosition(pos)) ret->setDouble(pos.y); else ret->setEmpty();
}
void NeuroImpl::get_position_z(ExtValue *ret)
{
	Pt3D pos;
	if (getPosition(pos)) ret->setDouble(pos.z); else ret->setEmpty();
}


void NeuroImpl::createFieldsObject()
{
	fields_param = new Param(paramentries ? paramentries : (ParamEntry*)&empty_paramtab, this, "NeuroProperties");
	fields_object = new ExtObject(fields_param);
}

void NeuroImpl::get_fields(ExtValue *ret)
{
	if (!fields_object)
		createFieldsObject();
	ret->setObject(*fields_object);
}

void NeuroImpl::get_neurodef(ExtValue *ret)
{
	ret->setObject(ExtObject(&Neuro::getStaticParam(), neuro));
}

void NeuroImpl::get_classObject(ExtValue *ret)
{
#ifndef SDK_WITHOUT_FRAMS
	NeuroClassExt::makeStaticObject(ret, neuroclass);
#endif
}

NeuroImpl::~NeuroImpl()
{
	if (fields_param)
	{
		delete fields_param;
		delete fields_object;
	}
}
