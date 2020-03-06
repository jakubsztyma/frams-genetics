// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "neuroimpl-fuzzy.h"
#include "neuroimpl-fuzzy-f0.h"
#include <common/nonstd_stl.h> //min,max

int NI_FuzzyNeuro::countOuts(const Model *m, const Neuro *fuzzy)
{
	int outputs = 0;
	for (int i = 0; i < m->getNeuroCount(); i++)
		for (int in = 0; in < m->getNeuro(i)->getInputCount(); in++)
			if (m->getNeuro(i)->getInput(in) == fuzzy) outputs++;
	return outputs;
}

int NI_FuzzyNeuro::lateinit()
{
	int i, maxOutputNr;

	//check correctness of given parameters: string must not be null, sets&rules number > 0
	if ((fuzzySetsNr < 1) || (rulesNr < 1) || (fuzzySetString.len() == 0) || (fuzzyRulesString.len() == 0))
		return 0; //error

	// this part contains transformation of fuzzy sets
	fuzzySets = new double[4 * fuzzySetsNr]; //because every fuzzy set consist of 4 numbers
	// converts fuzzy string from f0 to table of fuzzy numbers type 'double'
	// (fill created space with numbers taken from string)
	// also checks whether number of fuzzy sets in the string equals declared in the definition
	if (FuzzyF0String::convertStrToSets(fuzzySetString, fuzzySets, fuzzySetsNr) != 0)
		return 0; //error

	// this part contains transformation of fuzzy rules and defuzzyfication parameters
	rulesDef = new int[2 * rulesNr];    //for each rule remembers number of inputs and outputs
	//check correctness of string and fill in the rulesDef
	if (FuzzyF0String::countInputsOutputs(fuzzyRulesString.c_str(), rulesDef, rulesNr) == 0)
	{
		defuzzParam = new double[rulesNr]; // parameters used in defuzyfication process
		// create space for rules according to rulesDef
		rules = new int*[rulesNr];   //list of rules...
		for (i = 0; i < rulesNr; i++)    //...that contains rules body
		{
			rules[i] = new int[2 * (rulesDef[2 * i] + rulesDef[2 * i + 1])];  //each rule can have different number of inputs and outputs
			defuzzParam[i] = 0; //should be done a little bit earlier, but why do not use this loop?
		}
		// fill created space with numbers taken from string
		if (FuzzyF0String::convertStrToRules(fuzzyRulesString, rulesDef, rules, fuzzySetsNr, rulesNr, maxOutputNr) != 0)
			return 0; //error
	}
	else
		return 0; //error

	setChannelCount(countOuts(neuro->owner, neuro));
	return 1; //success
}

NI_FuzzyNeuro::~NI_FuzzyNeuro()
{
	if (rules) //delete rows and columns of **rules
	{
		for (int i = 0; i < rulesNr; i++) SAFEDELETEARRAY(rules[i])
			SAFEDELETEARRAY(rules)
	}
	SAFEDELETEARRAY(defuzzParam)
		SAFEDELETEARRAY(rulesDef)
		SAFEDELETEARRAY(fuzzySets)
}

int NI_FuzzyNeuro::GetFuzzySetParam(int set_nr, double &left, double &midleft, double &midright, double &right)
{
	if ((set_nr >= 0) && (set_nr < fuzzySetsNr))
	{
		left = fuzzySets[4 * set_nr];
		midleft = fuzzySets[4 * set_nr + 1];
		midright = fuzzySets[4 * set_nr + 2];
		right = fuzzySets[4 * set_nr + 3];
		return 0;
	}
	else
		return 1;
}

/** Function conduct fuzzyfication of inputs and calculates - according to rules - crisp multi-channel output */
void NI_FuzzyNeuro::go()
{
	if (Fuzzyfication() != 0)
		return;
	if (Defuzzyfication() != 0)
		return;
}

/**
* Function conduct fuzzyfication process - calculates minimum membership function (of every input) for each rule,
* and writes results into defuzzParam - variable that contains data necessary for defuzzyfication
*/
int NI_FuzzyNeuro::Fuzzyfication()
{
	int i, j, nrIn, inputNr, nrFuzzySet;
	double minimumCut; // actual minimal level of cut (= min. membership function)

	// sets defuzzyfication parameters for each rule:
	for (i = 0; i < rulesNr; i++)
	{
		nrIn = rulesDef[2 * i]; // nr of inputs in rule #i
		minimumCut = 2; // the highest value of membership function is 1.0, so this value will definitely change
		for (j = 0; (j < nrIn) && (minimumCut > 0); j++) //minimumCut can not be <0, so if =0 then stop calculations
		{
			nrFuzzySet = rules[i][j * 2 + 1]; // j*2 moves pointer through each output, +1 moves to nr of fuzzy set
			inputNr = rules[i][j * 2]; // as above but gives input number
			minimumCut = min(minimumCut, TrapeziumFuzz(nrFuzzySet, getWeightedInputState(inputNr))); // value of membership function for this input and given fuzzy set
		}
		if ((minimumCut > 1) || (minimumCut < 0))
			return 1;
		defuzzParam[i] = minimumCut;
	}
	return 0;
}

/**
* Function calculates value of the membership function of the set given by wchich_fuzzy_set for given crisp value input_val
* In other words, this function fuzzyficates given crisp value with given fuzzy set, returning it's membership function
* @param which_fuzzy_set - 0 < number of set < fuzzySetsNr
* @param input_val - crisp value of input in range <-1; 1>
* @return value of membership function (of given input for given set) in range <0;1> or, if error occur, negative value
*/
double NI_FuzzyNeuro::TrapeziumFuzz(int which_fuzzy_set, double input_val)
{
	double range = 0, left = 0, midleft = 0, midright = 0, right = 0;

	if ((which_fuzzy_set < 0) || (which_fuzzy_set > fuzzySetsNr))
		return -2;
	if ((input_val < -1) || (input_val > 1))
		return -3;

	if (GetFuzzySetParam(which_fuzzy_set, left, midleft, midright, right) != 0)
		return -4;

	if ((input_val < left) || (input_val > right)) // greather than right value
		return 0;
	else if ((input_val >= midleft) && (input_val <= midright)) // in the core of fuzzy set
		return 1;
	else if ((input_val >= left) && (input_val < midleft)) // at the left side of trapezium
	{
		range = fabs(midleft - left);
		return fabs(input_val - left) / ((range > 0) ? range : 1); // quotient of distance between input and extreme left point of trapezium and range of rising side, or 1
	}
	else if ((input_val > midright) && (input_val <= right)) // at the right side of trapezium
	{
		range = fabs(right - midright);
		return fabs(right - input_val) / ((range > 0) ? range : 1); // quotient of distance between input and extreme right point of trapezium and range of falling side, or 1
	};

	// should not occur
	return 0;

}

/**
* Function conducts defuzzyfication process: multi-channel output values are calculates with singleton method (method of high).
* For each rules, all outputs fuzzy sets are taken and cut at 'cut-level', that is at minumum membership function (of current rule).
* For all neuro pseudo-outputs, answer is calculated according to prior computations.
* In fact, there is one output with multi-channel answer and appropriate values are given to right channels.
*/
int NI_FuzzyNeuro::Defuzzyfication()
{
	int i, j, nrIn, nrOut, out, set, outputsNr;
	double *numerators, *denominators, midleft, midright, unimp;

	outputsNr = getChannelCount();

	numerators = new double[outputsNr];
	denominators = new double[outputsNr];

	for (i = 0; i < outputsNr; i++) numerators[i] = denominators[i] = 0;

	// for each rule...
	for (i = 0; i < rulesNr; i++)
	{
		nrIn = rulesDef[2 * i]; // number of inputs in rule #i
		nrOut = rulesDef[2 * i + 1]; // number of outputs in rule #i
		// ...calculate each output's product of middle fuzzy set value and minimum membership function (numerator) and sum of minimum membership function (denominator)
		for (j = 0; j < nrOut; j++)
		{
			out = rules[i][2 * nrIn + 2 * j]; //number of j-output
			set = rules[i][2 * nrIn + 2 * j + 1]; //number of fuzzy set attributed to j-output
			if (GetFuzzySetParam(set, unimp, midleft, midright, unimp) != 0) // gets range of core of given fuzzy set
			{
				SAFEDELETEARRAY(denominators) SAFEDELETEARRAY(numerators) return 1;
			}
			//defuzzParam[i] = minimum membership function for rule #i - calculated in fuzzyfication block
			// defuzzyfication method of singletons (high): (fuzzy set modal value) * (minimum membership value)
			numerators[out] += ((midleft + midright) / 2.0) * defuzzParam[i];
			denominators[out] += defuzzParam[i];
		}
	}

	for (i = 0; i < outputsNr; i++)
	{
		if (denominators[i] == 0)
			setState(0, i);
		else
			setState(numerators[i] / denominators[i], i);
	}

	SAFEDELETEARRAY(denominators)
		SAFEDELETEARRAY(numerators)

		return 0;
}
