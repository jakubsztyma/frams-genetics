// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <ctype.h>  //isupper()
#include "genooperators.h"
#include <common/log.h>
#include <common/nonstd_math.h>
#include <frams/util/rndutil.h>

static double distrib_force[] =   // for '!'
{
	3,             // distribution 0 -__/ +1
	0.001, 0.2,    // "slow" neurons
	0.001, 1,
	1, 1,          // "fast" neurons
};
static double distrib_inertia[] =  // for '='
{
	2,             // distribution 0 |..- +1
	0, 0,          // "fast" neurons
	0.7, 0.98,
};
static double distrib_sigmo[] =  // for '/'
{
	5,             // distribution -999 -..-^-..- +999
	-999, -999,    //"perceptron"
	999, 999,
	-5, -1,        // nonlinear
	1, 5,
	-1, 1,         // ~linear
};


int GenoOperators::roulette(const double *probtab, const int count)
{
	double sum = 0;
	int i;
	for (i = 0; i < count; i++) sum += probtab[i];
	double sel = rndDouble(sum);
	for (sum = 0, i = 0; i < count; i++) { sum += probtab[i]; if (sel < sum) return i; }
	return -1;
}

bool GenoOperators::getMinMaxDef(ParamInterface *p, int i, double &mn, double &mx, double &def)
{
	mn = mx = def = 0;
	int defined = 0;
	if (p->type(i)[0] == 'f')
	{
		double _mn = 0, _mx = 1, _def = 0.5;
		defined = p->getMinMaxDouble(i, _mn, _mx, _def);
		if (defined == 1) _mx = _mn + 1000.0; //only min was defined, so let's set some arbitrary range, just to have some freedom. Assumes _mn is not close to maxdouble...
		if (_mx < _mn && defined == 3) //only default was defined, so let's assume some arbitrary range. Again, no check for min/maxdouble...
		{
			_mn = _def - 500.0;
			_mx = _def + 500.0;
		}
		if (defined < 3) _def = (_mn + _mx) / 2.0;
		mn = _mn; mx = _mx; def = _def;
	}
	if (p->type(i)[0] == 'd')
	{
		paInt _mn = 0, _mx = 1, _def = 0;
		defined = p->getMinMaxInt(i, _mn, _mx, _def);
		if (defined == 1) _mx = _mn + 1000; //only min was defined, so let's set some arbitrary range, just to have some freedom. Assumes _mn is not close to maxint...
		if (_mx < _mn && defined == 3) //only default was defined, so let's assume some arbitrary range. Again, no check for min/maxint...
		{
			_mn = _def - 500;
			_mx = _def + 500;
		}
		if (defined < 3) _def = (_mn + _mx) / 2;
		mn = _mn; mx = _mx; def = _def;
	}
	return defined == 3;
}

int GenoOperators::selectRandomProperty(Neuro *n)
{
	int neuext = n->extraProperties().getPropCount(),
		neucls = n->getClass() == NULL ? 0 : n->getClass()->getProperties().getPropCount();
	if (neuext + neucls == 0) return -1; //no properties in this neuron
	int index = rndUint(neuext + neucls);
	if (index >= neuext) index = index - neuext + 100;
	return index;
}

double GenoOperators::mutateNeuProperty(double current, Neuro *n, int i)
{
	if (i == -1) return mutateCreepNoLimit('f', current, 2, true); //i==-1: mutating weight of neural connection
	Param p;
	if (i >= 100) { i -= 100; p = n->getClass()->getProperties(); }
	else p = n->extraProperties();
	double newval = current;
	/*bool ok=*/getMutatedProperty(p, i, current, newval);
	return newval;
}

bool GenoOperators::mutatePropertyNaive(ParamInterface &p, int i)
{
	double mn, mx, df;
	if (p.type(i)[0] != 'f' && p.type(i)[0] != 'd') return false; //don't know how to mutate
	getMinMaxDef(&p, i, mn, mx, df);

	ExtValue ev;
	p.get(i, ev);
	ev.setDouble(mutateCreep(p.type(i)[0], ev.getDouble(), mn, mx, true));
	p.set(i, ev);
	return true;
}

bool GenoOperators::mutateProperty(ParamInterface &p, int i)
{
	double newval;
	ExtValue ev;
	p.get(i, ev);
	bool ok = getMutatedProperty(p, i, ev.getDouble(), newval);
	if (ok) { ev.setDouble(newval); p.set(i, ev); }
	return ok;
}

bool GenoOperators::getMutatedProperty(ParamInterface &p, int i, double oldval, double &newval)
{
	newval = 0;
	if (p.type(i)[0] != 'f' && p.type(i)[0] != 'd') return false; //don't know how to mutate
	const char *n = p.id(i), *na = p.name(i);
	if (strcmp(n, "si") == 0 && strcmp(na, "Sigmoid") == 0) newval = CustomRnd(distrib_sigmo); else
		if (strcmp(n, "in") == 0 && strcmp(na, "Inertia") == 0) newval = CustomRnd(distrib_inertia); else
			if (strcmp(n, "fo") == 0 && strcmp(na, "Force") == 0) newval = CustomRnd(distrib_force); else
			{
				double mn, mx, df;
				getMinMaxDef(&p, i, mn, mx, df);
				newval = mutateCreep(p.type(i)[0], oldval, mn, mx, true);
			}
	return true;
}

double GenoOperators::mutateCreepNoLimit(char type, double current, double stddev, bool limit_precision_3digits)
{
	double result = RndGen.Gauss(current, stddev);
	if (type == 'd')
	{
		result = int(result + 0.5);
		if (result == current) result += rndUint(2) * 2 - 1; //force some change
	}
	else
	{
		if (limit_precision_3digits)
			result = floor(result * 1000 + 0.5) / 1000.0; //round
	}
	return result;
}

double GenoOperators::mutateCreep(char type, double current, double mn, double mx, double stddev, bool limit_precision_3digits)
{
	double result = mutateCreepNoLimit(type, current, stddev, limit_precision_3digits);
	if (result<mn || result>mx) //exceeds boundary, so bring to the allowed range
	{
		//reflect:
		if (result > mx) result = mx - (result - mx); else
			if (result < mn) result = mn + (mn - result);
		//wrap (just in case 'result' exceeded the allowed range so much that after reflection above it exceeded the other boundary):
		if (result > mx) result = mn + fmod(result - mx, mx - mn); else
			if (result < mn) result = mn + fmod(mn - result, mx - mn);
		if (limit_precision_3digits)
		{
			//reflect and wrap above may have changed the (limited) precision, so try to round again (maybe unnecessarily, because we don't know if reflect+wrap above were triggered)
			double result_try = floor(result * 1000 + 0.5) / 1000.0; //round
			if (mn <= result_try && result_try <= mx) result = result_try; //after rounding still witin allowed range, so keep rounded value
		}
	}
	return result;
}

double GenoOperators::mutateCreep(char type, double current, double mn, double mx, bool limit_precision_3digits)
{
	double stddev = (mx - mn) / 2 / 5; // magic arbitrary formula for stddev, which becomes /halfinterval, 5 times narrower
	return mutateCreep(type, current, mn, mx, stddev, limit_precision_3digits);
}

void GenoOperators::setIntFromDoubleWithProbabilisticDithering(ParamInterface &p, int index, double value) //TODO
{
	p.setInt(index, (paInt)(value + 0.5)); //TODO value=2.499 will result in 2 and 2.5 will result in 3, but we want these cases to be 2 or 3 with almost equal probability. value=2.1 should be mostly 2, rarely 3. Careful with negative values (test it!)
}

void GenoOperators::linearMix(vector<double> &p1, vector<double> &p2, double proportion)
{
	if (p1.size() != p2.size())
	{
		logPrintf("GenoOperators", "linearMix", LOG_ERROR, "Cannot mix vectors of different length (%d and %d)", p1.size(), p2.size());
		return;
	}
	for (unsigned int i = 0; i < p1.size(); i++)
	{
		double v1 = p1[i];
		double v2 = p2[i];
		p1[i] = v1 * proportion + v2 * (1 - proportion);
		p2[i] = v2 * proportion + v1 * (1 - proportion);
	}
}

void GenoOperators::linearMix(ParamInterface &p1, int i1, ParamInterface &p2, int i2, double proportion)
{
	char type1 = p1.type(i1)[0];
	char type2 = p2.type(i2)[0];
	if (type1 == 'f' && type2 == 'f')
	{
		double v1 = p1.getDouble(i1);
		double v2 = p2.getDouble(i2);
		p1.setDouble(i1, v1 * proportion + v2 * (1 - proportion));
		p2.setDouble(i2, v2 * proportion + v1 * (1 - proportion));
	}
	else
		if (type1 == 'd' && type2 == 'd')
		{
			int v1 = p1.getInt(i1);
			int v2 = p2.getInt(i2);
			setIntFromDoubleWithProbabilisticDithering(p1, i1, v1 * proportion + v2 * (1 - proportion));
			setIntFromDoubleWithProbabilisticDithering(p2, i2, v2 * proportion + v1 * (1 - proportion));
		}
		else
			logPrintf("GenoOperators", "linearMix", LOG_WARN, "Cannot mix values of types '%c' and '%c'", type1, type2);
}

int GenoOperators::getActiveNeuroClassCount()
{
	int count = 0;
	for (int i = 0; i < Neuro::getClassCount(); i++)
		if (Neuro::getClass(i)->genactive)
			count++;
	return count;
}

NeuroClass *GenoOperators::getRandomNeuroClass()
{
	vector<NeuroClass *> active;
	for (int i = 0; i < Neuro::getClassCount(); i++)
		if (Neuro::getClass(i)->genactive)
			active.push_back(Neuro::getClass(i));
	if (active.size() == 0) return NULL; else return active[rndUint(active.size())];
}

NeuroClass *GenoOperators::getRandomNeuroClassWithOutput()
{
	vector<NeuroClass *> active;
	for (int i = 0; i < Neuro::getClassCount(); i++)
		if (Neuro::getClass(i)->genactive && Neuro::getClass(i)->getPreferredOutput() != 0)
			active.push_back(Neuro::getClass(i));
	if (active.size() == 0) return NULL; else return active[rndUint(active.size())];
}

NeuroClass *GenoOperators::getRandomNeuroClassWithInput()
{
	vector<NeuroClass *> active;
	for (int i = 0; i < Neuro::getClassCount(); i++)
		if (Neuro::getClass(i)->genactive && Neuro::getClass(i)->getPreferredInputs() != 0)
			active.push_back(Neuro::getClass(i));
	if (active.size() == 0) return NULL; else return active[rndUint(active.size())];
}

NeuroClass *GenoOperators::getRandomNeuroClassWithOutputAndNoInputs()
{
	vector<NeuroClass *> active;
	for (int i = 0; i < Neuro::getClassCount(); i++)
		if (Neuro::getClass(i)->genactive && Neuro::getClass(i)->getPreferredOutput() != 0 && Neuro::getClass(i)->getPreferredInputs() == 0)
			active.push_back(Neuro::getClass(i));
	if (active.size() == 0) return NULL; else return active[rndUint(active.size())];
}

int GenoOperators::getRandomNeuroClassWithOutput(const vector<NeuroClass *> &NClist)
{
	vector<int> allowed;
	for (size_t i = 0; i < NClist.size(); i++)
		if (NClist[i]->getPreferredOutput() != 0) //this NeuroClass provides output
			allowed.push_back(i);
	if (allowed.size() == 0) return -1; else return allowed[rndUint(allowed.size())];
}

int GenoOperators::getRandomNeuroClassWithInput(const vector<NeuroClass *> &NClist)
{
	vector<int> allowed;
	for (size_t i = 0; i < NClist.size(); i++)
		if (NClist[i]->getPreferredInputs() != 0) //this NeuroClass wants one input connection or more			
			allowed.push_back(i);
	if (allowed.size() == 0) return -1; else return allowed[rndUint(allowed.size())];
}

int GenoOperators::getRandomChar(const char *choices, const char *excluded)
{
	int allowed_count = 0;
	for (size_t i = 0; i < strlen(choices); i++) if (!strchrn0(excluded, choices[i])) allowed_count++;
	if (allowed_count == 0) return -1; //no char is allowed
	int rnd_index = rndUint(allowed_count) + 1;
	allowed_count = 0;
	for (size_t i = 0; i < strlen(choices); i++)
	{
		if (!strchrn0(excluded, choices[i])) allowed_count++;
		if (allowed_count == rnd_index) return i;
	}
	return -1; //never happens
}

NeuroClass *GenoOperators::parseNeuroClass(char *&s)
{
	int maxlen = (int)strlen(s);
	int NClen = 0;
	NeuroClass *NC = NULL;
	for (int i = 0; i < Neuro::getClassCount(); i++)
	{
		const char *ncname = Neuro::getClass(i)->name.c_str();
		int ncnamelen = (int)strlen(ncname);
		if (maxlen >= ncnamelen && ncnamelen > NClen && (strncmp(s, ncname, ncnamelen) == 0))
		{
			NC = Neuro::getClass(i);
			NClen = ncnamelen;
		}
	}
	s += NClen;
	return NC;
}

Neuro *GenoOperators::findNeuro(const Model *m, const NeuroClass *nc)
{
	if (!m) return NULL;
	for (int i = 0; i < m->getNeuroCount(); i++)
		if (m->getNeuro(i)->getClass() == nc) return m->getNeuro(i);
	return NULL; //neuron of class 'nc' was not found
}

int GenoOperators::neuroClassProp(char *&s, NeuroClass *nc, bool also_v1_N_props)
{
	int len = (int)strlen(s);
	int Len = 0, I = -1;
	if (nc)
	{
		Param p = nc->getProperties();
		for (int i = 0; i < p.getPropCount(); i++)
		{
			const char *n = p.id(i);
			int l = (int)strlen(n);
			if (len >= l && l > Len && (strncmp(s, n, l) == 0)) { I = 100 + i; Len = l; }
			if (also_v1_N_props) //recognize old properties symbols /=!
			{
				if (strcmp(n, "si") == 0) n = "/"; else
					if (strcmp(n, "in") == 0) n = "="; else
						if (strcmp(n, "fo") == 0) n = "!";
				l = (int)strlen(n);
				if (len >= l && l > Len && (strncmp(s, n, l) == 0)) { I = 100 + i; Len = l; }
			}
		}
	}
	Neuro n;
	Param p = n.extraProperties();
	for (int i = 0; i < p.getPropCount(); i++)
	{
		const char *n = p.id(i);
		int l = (int)strlen(n);
		if (len >= l && l > Len && (strncmp(s, n, l) == 0)) { I = i; Len = l; }
	}
	s += Len;
	return I;
}

bool GenoOperators::isWS(const char c)
{
	return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

void GenoOperators::skipWS(char *&s)
{
	if (s == NULL)
		logMessage("GenoOperators", "skipWS", LOG_WARN, "NULL reference!");
	else
		while (isWS(*s)) s++;
}

bool GenoOperators::areAlike(char *g1, char *g2)
{
	while (*g1 || *g2)
	{
		skipWS(g1);
		skipWS(g2);
		if (*g1 != *g2) return false; //when difference
		if (!*g1 && !*g2) break; //both end
		g1++;
		g2++;
	}
	return true; //equal
}

char *GenoOperators::strchrn0(const char *str, char ch)
{
	return ch == 0 ? NULL : strchr((char *)str, ch);
}

bool GenoOperators::canStartNeuroClassName(const char firstchar)
{
	return isupper(firstchar) || firstchar == '|' || firstchar == '@' || firstchar == '*';
}
