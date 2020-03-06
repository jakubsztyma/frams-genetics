// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NEUROIMPL_H_
#define _NEUROIMPL_H_

#include <frams/model/model.h>
#include <frams/param/param.h>
#include <common/log.h>
#ifdef NEURO_SIGNALS
#include <frams/simul/signals.h>
#endif

class NeuroImpl;
extern ParamEntry neuroimpl_tab[];

class Creature;
class NeuroLibrary;
class NeuroFactory;

class NeuroNetConfig
{
public:
	NeuroNetConfig(NeuroFactory *fac);

	Param par;
	double randominit;
	double nnoise;
	double touchrange;

	NeuroFactory *factory;
	//static NeuroNetConfig& getGlobalConfig();
};

#ifdef NEURO_SIGNALS
class NeuroSignals : public SignalSet
{
protected:
	Creature *cr;
	NeuroImpl *owner;
	Creature *getCreature();
public:

	NeuroSignals(NeuroImpl *n) :cr(0), owner(n) {}

#define STATRICKCLASS NeuroSignals
	PARAMPROCDEF(p_add);
	PARAMPROCDEF(p_get);
	PARAMGETDEF(size);
	PARAMPROCDEF(p_receive);
	PARAMPROCDEF(p_receiveSet);
	PARAMPROCDEF(p_receiveFilter);
	PARAMPROCDEF(p_receiveSingle);
#undef STATRICKCLASS

	static Param& getStaticParam();
};
#endif

/// Neuro net implementation
class NeuroNetImpl
{
	CallbackNode *cnode;
	Model &mod;
	SList neurons[4];
	NeuroNetConfig& config;
	int isbuilt, errorcount;
	STCALLBACKDEFC(NeuroNetImpl, destroyNN);
	int minorder, maxorder;

public:
#ifdef NEURO_SIGNALS
	ChannelSpace *channels;
#endif
	static int mytags_id;
	static double getStateFromNeuro(Neuro *n);
	int getErrorCount() { return errorcount; }
	NeuroNetConfig &getConfig() { return config; }
	NeuroNetImpl(Model& model, NeuroNetConfig& conf
#ifdef NEURO_SIGNALS
		, ChannelSpace *ch = 0
#endif
		);
	~NeuroNetImpl();
	void simulateNeuroNet();
	void simulateNeuroPhysics();

	static NeuroImpl *getImpl(Neuro* n) { return (NeuroImpl*)n->userdata[mytags_id]; }
};


/**
   Neuro implementation - this object calculates the Neuron's state
   (Neuro::state) in each simulation step.

   SUBCLASSING TUTORIAL
   ====================

   1.Derive your custom neuron from NeuroImpl class. The name must be prefixed with NI_

   class NI_MyNeuron: public NeuroImpl
   { ... };

   2.Public parameters
   Create any number of public fields, they will be adjustable from the genotype level.
   3 datatypes are supported: long, double and SString

   public:
   paInt intParameter;
   double fpParameter;
   SString txtParameter;


   3.Required method: "instantiator".
   It is always the same, just create a new instance of your neuron.
   public:
   NeuroImpl* makeNew() { return new NI_MyNeuron(); };


   4.Required method: default constructor
   Set the "paramentries" variable if you need public parameters in your neuron.
   NI_..._tab is created automatically and should be declared as: extern ParamEntry NI_..._tab[];
   At this stage the parameter values are not yet available.

   public:
   NI_MyNeuron() // no parameters!
   {
   paramentries=NI_MyNeuron_tab;
   // you add here: some general initialization
   }


   5.Optional method: initialization
   This method is called once before the neuron is actually used in the simulation.
   The parameter values are already initialized (according to the genotype) and the neuron is bound to the creature (i.e. this->neuro is valid).
   Return 0 if the neuron cannot be initialized.

   int lateinit()
   {
   // you add here: initialization using full neuron context
   // example: if (!neuro->joint) return 0; //this neuron must be attached to joint
   return 1;//OK
   }


   6.Required method: simulation step
   If it has output: calculate the next neuron state and call setState()
   If it is an effector: do anything else

   void go()
   {
   // you add here: things called every simulation step
   }

   Note: You can make your neuron fire before or after "regular" neurons by changing its "simorder" property (during initialization). The default value is 1, whereas receptors have simorder=0 and effectors have simorder=2.


   7.Neuron definition
   In order to incorporate the new neuron into Framsticks you need to provide some additional information (to be added to "f0.def" file).

   NEUROCLASS(MyNeuron,MN,This is the name,`Neuron description',-1,1,0)
   NEUROPROP(int,0,0,name of the int,d,,,,intParameter)
   NEUROPROP(fp,0,0,name of the floating point,f,,,,fpParameter)
   NEUROPROP(txt,0,0,name of the text,s,,,,txtParameter)
   ENDNEUROCLASS

   NEUROCLASS:
   - MyNeuron: neuron class name (without the NI_ prefix)
   - MN: neuron symbol (used in genotypes)
   - full name and description
   - -1: preferred number of inputs (special case: -1=any)
   - 1: provides output: 1=yes/0=no
   - 0: preferred location: 0=none, 1=part, 2=joint

   NEUROPROP:
   - int/fp/txt: parameter names as visible in genotypes and scripting
   - "name of the ...": descriptive name
   - d/f/s: type (int/floating point/string)
   - intParameter/fpParameter/txtParameter: C++ field names


   */
class NeuroImpl
{
protected:
	int simorder;
	int channels;
	SListTempl<double> chstate;
	SListTempl<double> chnewstate;
	Param *fields_param;
	ExtObject *fields_object;
public:
	static const int ENDDRAWING;
	static const int MAXDRAWINGXY;

	enum NeuroImplStats { BeforeInit = 0, InitError = 1, InitOk = 2 };
	NeuroImplStats status;
	/** originating neuron object (from the model) */
	Neuro *neuro;
	NeuroClass *neuroclass;
	/** don't access directly */
	double newstate;
	NeuroNetImpl *owner;
	/** will be used by readParam() method, if not null  */
	ParamEntry *paramentries; // no extra properties if ==0

#ifdef NEURO_SIGNALS
	NeuroSignals sigs;
	ExtObject sigs_obj;
#endif

	/** "virtual constructor" - NeuroFactory uses this method to create the proper implementation object.
		subclasses must return new object here. */
	virtual NeuroImpl* makeNew() { return 0; } // 
	/** read additional properties from "moredata" field of the originating Neuro */
	void readParam();
	/** called when all other neuro objects were already created and "moredata" transferred to
		object fields.
		useful for initialization that cannot be performed in the constructor.
		@return 1=ok  0=failure
		*/
	virtual int lateinit() { return 1; }
	/** calculate 'newstate - implementation dependent */
	virtual void go(){}
	/** for neurons doing some physical actions (called each simulation step when nnspeed!=1.0) */
	virtual void goPhysics(){}

	int getSimOrder() { return simorder; }
	virtual int getNeedPhysics() { return 0; }

	void setChannelCount(int c);
	int getChannelCount() { return channels; }

	int getInputCount() { return neuro->getInputCount(); }
	int getInputChannelCount(int i);
	double getInputState(int i, int channel = 0);
	double getWeightedInputState(int i, int channel = 0);
	double getInputSum(int startwith = 0);
	double getWeightedInputSum(int startwith = 0);
	double getInputWeight(int i) { return neuro->getInputWeight(i); }
	void setState(double st, int channel);
	void setState(double st) { validateNeuroState(st); newstate = st; }
	double getState(int channel);
	double getState() { return neuro->state; }

	virtual int getDrawingCount() { return 0; }
	virtual int* getDrawing(int i) { return 0; }

	/** is this implementation current? script neurons retain their original implementation when reloading *.neuro */
	virtual bool isCurrent() { return true; }

	void commit();
	void validateNeuroState(double& st) { if (st <= -1e10) st = -1e10; else if (st > 1e10) st = 1e10; }

	NeuroImpl() :simorder(1), channels(1), fields_param(0), fields_object(0), status(BeforeInit), neuro(0), newstate(0), owner(0), paramentries(0)
#ifdef NEURO_SIGNALS
		, sigs(this), sigs_obj(&NeuroSignals::getStaticParam(), &sigs)
#endif
	{}
	virtual ~NeuroImpl();
	virtual void createFieldsObject();

	/** usually == "newstate" but will obey the "hold state" */
	double getNewState(int channel = 0);

	/** don't use! */
	void setCurrentState(double st, int channel = 0);

	bool getPosition(Pt3D &pos);
	Creature* getCreature();

#define STATRICKCLASS NeuroImpl
	PARAMGETDEF(count) { arg1->setInt(getInputCount()); }
	PARAMPROCDEF(p_get) { arg2->setDouble(getInputState(arg1->getInt())); }
	PARAMPROCDEF(p_getweight) { arg2->setDouble(getInputWeight(arg1->getInt())); }
	PARAMPROCDEF(p_getw) { arg2->setDouble(getWeightedInputState(arg1->getInt())); }
	PARAMPROCDEF(p_getsum) { arg2->setDouble(getInputSum(arg1->getInt())); }
	PARAMPROCDEF(p_getwsum) { arg2->setDouble(getWeightedInputSum(arg1->getInt())); }
	PARAMGETDEF(sum) { arg1->setDouble(getInputSum(0)); }
	PARAMGETDEF(wsum) { arg1->setDouble(getWeightedInputSum(0)); }
	PARAMPROCDEF(p_getchancount) { arg2->setInt(getInputChannelCount(arg1->getInt())); }
	PARAMPROCDEF(p_getchan) { arg2->setDouble(getInputState(arg1[1].getInt(), arg1[0].getInt())); }
	PARAMPROCDEF(p_getwchan) { arg2->setDouble(getWeightedInputState(arg1[1].getInt(), arg1[0].getInt())); }
	PARAMGETDEF(state) { arg1->setDouble(getState()); }
	PARAMSETDEF(state) { setState(arg1->getDouble()); return 0; }
	PARAMGETDEF(cstate) { arg1->setDouble(neuro->state); }
	PARAMSETDEF(cstate) { setCurrentState(arg1->getDouble()); return 0; }
	PARAMGETDEF(hold) { arg1->setInt((neuro->flags&(Neuro::HoldState)) ? 1 : 0); }
	PARAMSETDEF(hold) { neuro->flags = (neuro->flags&~Neuro::HoldState) | (arg1->getInt() ? Neuro::HoldState : 0); return 0; }
	PARAMGETDEF(channels) { arg1->setInt(getChannelCount()); }
	PARAMSETDEF(channels) { setChannelCount(arg1->getInt()); return 0; }
	PARAMPROCDEF(p_getstate) { arg2->setDouble(getState(arg1->getInt())); }
	PARAMPROCDEF(p_setstate) { setState(arg1[0].getDouble(), arg1[1].getInt()); }
	PARAMPROCDEF(p_setcstate) { setCurrentState(arg1[0].getDouble(), arg1[1].getInt()); }
	PARAMGETDEF(creature);
	PARAMGETDEF(part);
	PARAMGETDEF(joint);
	PARAMGETDEF(position_x);
	PARAMGETDEF(position_y);
	PARAMGETDEF(position_z);
	PARAMGETDEF(fields);
	PARAMGETDEF(neurodef);
	PARAMGETDEF(classObject);
#undef STATRICKCLASS

	static Param& getStaticParam();
};

#endif
