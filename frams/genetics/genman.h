// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2016  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _GENMAN_H_
#define _GENMAN_H_

#include <common/nonstd.h>
#include <common/nonstd_stl.h>
#include <frams/param/mutableparam.h>
#include <frams/param/mutparamlist.h>
#include <frams/neuro/geneticneuroparam.h>
#include "geno.h"
#include "genooperators.h"

#define GENSTYLE_COMMENT GENSTYLE_RGBS(0,150,0,GENSTYLE_BOLD)

struct GenoLink
{
	int count;
	string parent1, parent2, child; //parent2 only used in crossover, it is "" in mutation
	float chg;
	float fit;
};

class GenMan : public GenoValidator
{
public:
	GenMan();
	~GenMan();
	void setDefaults();
	/**
	if canvalidate==false, returns GENOPER_NOOPER (cannot test), GENOPER_OK (\e g is valid), or 1-based error position.
	if canvalidate==true, returns
	- GENOPER_OK and canvalidate==false if \e g was already valid
	- GENOPER_NOOPER and canvalidate==false if \e g was (probably) validated but couldn't checkValidity
	- 1-based error position and canvalidate==false if \e g was invalid and couldn't validate
	- 1-based error position and canvalidate==true if \e g was invalid and was validated successfully

	Note: 1-based error position is always related to pure genotype (g.GetGene()), without the leading comment
	*/
	int testValidity(Geno &g, bool &canvalidate);
	int testGenoValidity(Geno& g); //class GenoValidator (geno.h)
	Geno validate(const Geno&); ///<returns validated (if possible) genotype
	Geno mutate(const Geno&); //returns mutated genotype or empty if errors
	Geno crossOver(const Geno&, const Geno&); //returns xover genotype ("child") or empty if errors
	float similarity(const Geno&, const Geno&); //returns GENOPER_NOOPER or normalized similarity (1: identical, 0: different)
	uint32_t getStyle(const char* g, int pos); //returns style (and validity) of a genotype char.
	uint32_t getStyle(const char *g, const Geno *G, int pos); //returns style (and validity) of a genotype char. Assumes G is created from g.
	void getFullStyle(const char *g, uint32_t *styletab); //optimized. Fills styletab with styles for all genotype chars. sizeof(*styletab) must be at least strlen(g).
	void getFullStyle(const char *g, const Geno *G, uint32_t *styletab); //optimized. Assumes G is created from g. Fills styletab with styles for all genotype chars. sizeof(*styletab) must be at least strlen(g).
	string HTMLize(const char *g); //returns colored genotype in HTML.
	string HTMLizeShort(const char *g); //returns colored genotype (abbreviated if needed) in HTML.
	Geno getSimplest(char format); ///<returns pointer to the simplest genotype of \e format or empty Geno()
	const char *getOpName(char format); ///<returns pointer to the active operator set for \e format
	const vector<GenoOperators*>& GetOperators() const { return oper_fx_list; } ///<returns the list of available genetic operators
private:
	vector<GenoOperators*> oper_fx_list;
	void saveLink(const string parent1, const string parent2, const string child, const float chg);
	GenoOperators* getOper_f(char format);
	string HTMLize(const char *g, bool shorten);
public:
	vector<GenoLink> GenoLinkList;
	int history; //remember history?
	int hilite; //syntax highlighting (Style)
	int extmutinfo; //extended info: the info field of the genotype will contain the name of the mutation method
	int count; //# of the last genetic operation
	int valid_m, valid_xo, validated_m, validated_xo, invalid_m, invalid_xo, failed_m, failed_xo;
	double mutchg, xochg;
	SListTempl<char> operformats; //the list of supported_format, in the same order as in seloperpar
	int* seloper; //fields for seloperpar
	Param localpar, localstats;
	MutableParam seloperpar;
	GeneticNeuroParam neuronsparam;
	MutableParamList par;
#define STATRICKCLASS GenMan
	PARAMPROCDEF(p_clearStats);
	PARAMPROCDEF(p_report);
	PARAMPROCDEF(p_htmlize);
	PARAMPROCDEF(p_htmlizeshort);
	PARAMPROCDEF(p_validate);
	PARAMPROCDEF(p_mutate);
	PARAMPROCDEF(p_crossover);
	PARAMPROCDEF(p_getsimplest);
#undef STATRICKCLASS
	void clearStats();
	static void onDelGen(void*, intptr_t);
};


#endif
