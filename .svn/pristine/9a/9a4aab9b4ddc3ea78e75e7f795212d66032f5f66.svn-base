// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NEUROIMPLFUZZY_H_
#define _NEUROIMPLFUZZY_H_

#include <stdlib.h>
#include <math.h>

#include <frams/neuro/neuroimpl.h>
#include <frams/util/sstring.h>

extern ParamEntry NI_FuzzyNeuro_tab[];

/** Does the fuzzyfication process (of inputs) and defuzzyfication proces (of outpurs) - represents fuzzy rules
*/
class NI_FuzzyNeuro : public NeuroImpl
{
private:

	double *fuzzySets;    /// list of four digits which represents fuzzy sets: [0]-l, [1]-m, [2]-n, [3]-r, ...  fuzzySet[4*i] = left, fuzzySet[4*i + 1] = midleft, fuzzySet[4*i + 2] = midright, fuzzySet[4*i + 3] = right

	/** Determines, which fuzzy set is connected with each input of neuron. For instance third rule:
	*   'IF input3 = fuzzy set #3 AND input5 = fuzzy set #1 then output2 = fuzzy set #6 AND output7 = fuzzy set #5'
	*   the variables shoul have values as shown below:
	*   RulesDef[4]=2; RulesDef[5]=2; //rule 3: 2 inputs, 2 outputs
	*   Rules[2][0]=3, Rules[2][1]=3, Rules[2][2]=5, Rules[2][3]=1, Rules[2][4]=2, Rules[2][5]=6, Rules[2][6]=7, Rules[2][3]=5
	*/
	int *rulesDef;    ///list of rules definitions: nr of inputs in rule 1, nr of outputs in rule 1, ... and so on for each rule
	int **rules;      ///list of rules body: input nr, fuzzy set nr, ... , output nr, fuzzy set nr, ... and so on for each rule

	/**
	*  Sets defuzzyfication parameters: determines - for each rule - cut level <0;1> (minimum membership function of current rule).
	*  In fact, defuzzParam remembers the values from 'first layer' - fuzzyfication layer (see neuron at documentation)
	*  i.e. rule 1: defuzzParam[0] = 0.3522
	*/
	double *defuzzParam; /// i.e.: defuzParam[5] = 0.455 means that rule #6 has got a minimum membership function (of given inputs set for this rule) at value 0.455 (it's cut level)

protected:

	///Fuzzy functions
	double TrapeziumFuzz(int which_fuzzy_set, double input_val);
	int Fuzzyfication();
	int Defuzzyfication();
	int GetFuzzySetParam(int set_nr, double &left, double &midleft, double &midright, double &right);

public:

	int fuzzySetsNr;      /// number of fuzzy sets
	int rulesNr;      ///number of rules
	SString fuzzySetString; /// strings containing all fuzzy sets given in f0
	SString fuzzyRulesString; /// strings containing all fuzzy rules given in f0

	NI_FuzzyNeuro() { paramentries = NI_FuzzyNeuro_tab; fuzzySets = defuzzParam = NULL; rulesDef = NULL; rules = NULL; }
	~NI_FuzzyNeuro();
	NeuroImpl* makeNew() { return new NI_FuzzyNeuro(); };
	void go();
	int lateinit();
	/** Function build model based on given genotype and conts number of neurons connected with fuzzy neuro,
	  also checks number of fuzzy neuron inputs.
	  \param genotype genotype to be scanned
	  \param inputs number of fuzzy neuron inputs
	  \param output number of fuzzy neuron outputs (= number of neurons connected to fuzzy neuron)
	  @return success or failure
	  **/
	static int countOuts(const Model *m, const Neuro *fuzzy);

};

#endif
