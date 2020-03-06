// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "genman.h"
#include <frams/vm/classes/genoobj.h>
#include GEN_CONFIG_FILE //configuration of active genetic operators
#include "common/log.h"
#include "common/nonstd_math.h"
#include "common/util-string.h"
#include <common/loggers/loggers.h>


#define GENMAN_REPEAT_FAILED 100 //how many times GenMan tries to repeat a mutation or crossover when the operator does not return acceptable genotype
#define STRINGIFY_1(x) #x
#define STRINGIFY(x) STRINGIFY_1(x) //this second-level macro allows the parameter to be a macro itself and to stringify its value, not its name
#define GENMAN_REPEAT_FAILED_STR STRINGIFY(GENMAN_REPEAT_FAILED)


#ifdef USE_GENMAN_f0
#include "f0/f0_oper.h"
#endif
#ifdef USE_GENMAN_f0FUZZY
#include "f0/f0Fuzzy_oper.h"
#endif
#ifdef USE_GENMAN_f1
#include "f1/f1_oper.h"
#endif
#ifdef USE_GENMAN_f2
#include "f2/f2_oper.h"
#endif
#ifdef USE_GENMAN_f2
#include "f3/f3_oper.h"
#endif
#ifdef USE_GENMAN_f4
#include "f4/f4_oper.h"
#endif
#ifdef USE_GENMAN_f5
#include "f5/f5_oper.h"
#endif
#ifdef USE_GENMAN_f6
#include "f6/f6_oper.h"
#endif
#ifdef USE_GENMAN_f7
#include "f7/f7_oper.h"
#endif
#ifdef USE_GENMAN_f8
#include "f8/f8_oper.h"
#endif
#ifdef USE_GENMAN_f9
#include "f9/f9_oper.h"
#endif
#ifdef USE_GENMAN_fF
#include "fF/fF_oper.h"
#endif
#ifdef USE_GENMAN_fn
#include "fn/fn_oper.h"
#endif
#ifdef USE_GENMAN_fT
#include "fT/fTest_oper.h"
#endif
#ifdef USE_GENMAN_fB
#include "fB/fB_oper.h"
#endif
#ifdef USE_GENMAN_fH
#include "fH/fH_oper.h"
#endif
#ifdef USE_GENMAN_fL
#include "fL/fL_oper.h"
#endif

using namespace std; //string, vector

//old code needs update:
//#include "gengroups.h"
//extern GenGroup *listaGen;
//   GENGROUP(0)->l_del.add(sim->GM.onDelGen,&sim->GM); //before delete
//   GENGROUP(0)->l_del.remove(sim->GM.onDelGen,&sim->GM); //before delete


#define FIELDSTRUCT GenMan

static ParamEntry GMparam_tab[] =
{
	{ "Genetics", 1, 10, "GenMan", },
	{ "gen_hist", 0, PARAM_DONTSAVE, "Remember history of genetic operations", "d 0 1 0", FIELD(history), "Required for phylogenetic analysis", },
	{ "gen_hilite", 0, 0, "Use syntax highlighting", "d 0 1 1", FIELD(hilite), "Use colors for genes?\n(slows down viewing/editing of huge genotypes)", },
	{ "gen_extmutinfo", 0, 0, "Extended mutation info", "d 0 2 0 ~Off~Method ID~Method description", FIELD(extmutinfo), "If active, information about employed mutation method will be stored in the 'info' field of each mutated genotype.", },
	{ "operReport", 0, PARAM_DONTSAVE, "Operators report", "p()", PROCEDURE(p_report), "Show available genetic operators", },
	{ "toHTML", 0, PARAM_DONTSAVE, "HTMLize a genotype", "p s(s)", PROCEDURE(p_htmlize), "returns genotype expressed as colored HTML", },
	{ "toHTMLshort", 0, PARAM_DONTSAVE, "HTMLize a genotype, shorten if needed", "p s(s)", PROCEDURE(p_htmlizeshort), "returns genotype (abbreviated if needed) expressed as colored HTML", },
	{ "validate", 0, PARAM_DONTSAVE | PARAM_USERHIDDEN, "Validate", "p oGeno(oGeno)", PROCEDURE(p_validate), "returns validated (if possible) Geno object from supplied Geno", },
	{ "mutate", 0, PARAM_DONTSAVE | PARAM_USERHIDDEN, "Mutate", "p oGeno(oGeno)", PROCEDURE(p_mutate), "returns mutated Geno object from supplied Geno", },
	{ "crossOver", 0, PARAM_DONTSAVE | PARAM_USERHIDDEN, "Crossover", "p oGeno(oGeno,oGeno)", PROCEDURE(p_crossover), "returns crossed over genotype", },
	{ "getSimplest", 0, PARAM_DONTSAVE | PARAM_USERHIDDEN, "Get simplest genotype", "p oGeno(s format)", PROCEDURE(p_getsimplest), "returns the simplest genotype for a given encoding (format). \"0\" means f0, \"4\" means f4, etc.", },
	{ 0, },
};

static ParamEntry GMstats_tab[] =
{
	{ "Genetics", 1, 12, "GenManStats", "Statistics for genetic operations." },
	{ "gen_count", 0, PARAM_READONLY, "Number of genetic operations so far", "d", FIELD(count), "", },
	{ "gen_mvalid", 0, PARAM_READONLY, "Mutations valid", "d", FIELD(valid_m), "", },
	{ "gen_mvalidated", 0, PARAM_READONLY, "Mutations validated", "d", FIELD(validated_m), "", },
	{ "gen_minvalid", 0, PARAM_READONLY, "Mutations invalid", "d", FIELD(invalid_m), "couldn't be repaired", },
	{ "gen_mfailed", 0, PARAM_READONLY, "Mutations failed", "d", FIELD(failed_m), "couldn't be performed", },
	{ "gen_xovalid", 0, PARAM_READONLY, "Crossovers valid", "d", FIELD(valid_xo), "", },
	{ "gen_xovalidated", 0, PARAM_READONLY, "Crossovers validated", "d", FIELD(validated_xo), "", },
	{ "gen_xoinvalid", 0, PARAM_READONLY, "Crossovers invalid", "d", FIELD(invalid_xo), "couldn't be repaired", },
	{ "gen_xofailed", 0, PARAM_READONLY, "Crossovers failed", "d", FIELD(failed_xo), "couldn't be performed", },
	{ "gen_mutimpr", 0, PARAM_READONLY, "Mutations total effect", "f", FIELD(mutchg), "total cumulative mutation change", },
	{ "gen_xoimpr", 0, PARAM_READONLY, "Crossovers total effect", "f", FIELD(xochg), "total cumulative crossover change", },
	{ "clrstats", 0, PARAM_DONTSAVE, "Clear stats and history", "p()", PROCEDURE(p_clearStats), "", },
	{ 0, },
};

#undef FIELDSTRUCT

GenMan::GenMan() : localpar(GMparam_tab, this), localstats(GMstats_tab, this),
seloperpar("GenOperators", "Genetics: Active operators"),
neuronsparam("Genetics: Neurons to add", "neuronsAdd", "neuadd_"),
par("GenMan", "Manages various genetic operations, using appropriate operators for the argument genotype format.")
{
	history = 0;
	hilite = 1;
	clearStats();

#ifdef USE_GENMAN_f0
	oper_fx_list.push_back(new Geno_f0);
#endif
#ifdef USE_GENMAN_f0FUZZY
	oper_fx_list.push_back(new Geno_f0Fuzzy);
#endif
#ifdef USE_GENMAN_f1
	oper_fx_list.push_back(new Geno_f1);
#endif
#ifdef USE_GENMAN_f2
	oper_fx_list.push_back(new Geno_f2);
#endif
#ifdef USE_GENMAN_f3
	oper_fx_list.push_back(new Geno_f3);
#endif
#ifdef USE_GENMAN_f4
	oper_fx_list.push_back(new Geno_f4);
#endif
#ifdef USE_GENMAN_f5
	oper_fx_list.push_back(new Geno_f5);
#endif
#ifdef USE_GENMAN_f6
	oper_fx_list.push_back(new Geno_f6);
#endif
#ifdef USE_GENMAN_f7
	oper_fx_list.push_back(new Geno_f7);
#endif
#ifdef USE_GENMAN_f8
	oper_fx_list.push_back(new Geno_f8);
#endif
#ifdef USE_GENMAN_f9
	oper_fx_list.push_back(new GenoOper_f9);
#endif
#ifdef USE_GENMAN_fF
	oper_fx_list.push_back(new GenoOper_fF);
#endif
#ifdef USE_GENMAN_fn
	oper_fx_list.push_back(new GenoOper_fn);
#endif
#ifdef USE_GENMAN_fT
	oper_fx_list.push_back(new GenoOper_fTest);
#endif
#ifdef USE_GENMAN_fB
	oper_fx_list.push_back(new Geno_fB);
#endif
#ifdef USE_GENMAN_fH
	oper_fx_list.push_back(new Geno_fH);
#endif
#ifdef USE_GENMAN_fL
	oper_fx_list.push_back(new Geno_fL);
#endif

	seloper = new int[oper_fx_list.size()]; //may result in a little overhead if some of the operators on the oper_fx_list concern the same genetic format
	int selopercount = 0;
	for (unsigned int i = 0; i < oper_fx_list.size(); i++)
	{
		if (operformats.find(oper_fx_list[i]->supported_format) != -1) continue;
		string type = string("~") + oper_fx_list[i]->name;
		int dup = 0;
		for (unsigned int j = i + 1; j < oper_fx_list.size(); j++)
			if (oper_fx_list[i]->supported_format == oper_fx_list[j]->supported_format)
			{
			type += "~";
			type += oper_fx_list[j]->name;
			dup++;
			}
		type = ssprintf("d 0 %d ", dup) + type;
		string id = ssprintf("genoper_f%c", oper_fx_list[i]->supported_format);
		string name = ssprintf("Operators for f%c", oper_fx_list[i]->supported_format);
		seloper[selopercount] = 0;
		operformats += oper_fx_list[i]->supported_format;
		//printf("%x %s %s %s\n",&seloper[selopercount],(const char*)id,(const char*)type,(const char*)name);
		seloperpar.addProperty(&seloper[selopercount++], id.c_str(), type.c_str(), name.c_str(), "", PARAM_READONLY*(dup == 0));
	}

	par += &localpar;
	par += &seloperpar;
	par += &neuronsparam;
	for (unsigned int i = 0; i < oper_fx_list.size(); i++)
		if (oper_fx_list[i]->par.getParamTab()) par += &oper_fx_list[i]->par;

	setDefaults(); //use Param to initialize all values of fields in the paramtab of this object and genetic operators on oper_fx_list
}

GenMan::~GenMan()
{
	for (unsigned int i = 0; i < oper_fx_list.size(); i++) delete oper_fx_list[i];
	delete[] seloper;
}

void GenMan::setDefaults()
{
	for (unsigned int i = 0; i < oper_fx_list.size(); i++)
	{
		oper_fx_list[i]->par.setDefault();
		oper_fx_list[i]->setDefaults();
	}
	localpar.setDefault();
	//...and we do not reset others that are linked to 'par',
	//because there quite a few of them, and not every of them defines defaults for each of its parameters.
}

int GenMan::testValidity(Geno &g, bool &canvalidate)
{
	SString ggs = g.getGenes();
	const char *gg = ggs.c_str();
	GenoOperators *gf = getOper_f(g.getFormat());
	int check1;
	if (!gf) { canvalidate = false; return GENOPER_NOOPER; }
	else check1 = gf->checkValidity(gg, g.getName().c_str());
	if (!canvalidate) return check1; //just checking
	if (check1 == GENOPER_OK) { canvalidate = false; return check1; }
	char *g2 = strdup(gg);
	if (gf->validate(g2, g.getName().c_str()) == GENOPER_NOOPER) { free(g2); canvalidate = false; return check1; }
	if (check1 == GENOPER_NOOPER) //disaster: cannot check because there is no check operator
	{
		g.setGenesAssumingSameFormat(g2); free(g2); canvalidate = false; return GENOPER_NOOPER;
	}
	int check2 = gf->checkValidity(g2, "validated");
	if (check2 == GENOPER_OK) g.setGenesAssumingSameFormat(g2);
	free(g2);
	if (check2 == GENOPER_OK) return check1;
	canvalidate = false;
	return check1; //could not validate.
}

int GenMan::testGenoValidity(Geno& g)
{
	bool fix = false;
	switch (testValidity(g, fix))
	{
	case GENOPER_OK: return 1;
	case GENOPER_NOOPER: return -1;
	default: return 0;
	}
}

Geno GenMan::validate(const Geno& geny)
{
	char format = geny.getFormat();
	GenoOperators *gf = getOper_f(format);
	if (gf == NULL)
		return Geno(SString::empty(), -1, SString::empty(), SString::sprintf("GENOPER_NOOPER: Validate(): don't know how to handle genetic format %c", format));
	char *g2 = strdup(geny.getGenes().c_str()); //copy for validation
	int res = gf->validate(g2, geny.getName().c_str());
	SString sg2 = g2;
	free(g2);
	if (res == GENOPER_OK)
		return Geno(sg2, format, geny.getName(), geny.getComment());
	else
		return Geno(SString::empty(), -1, SString::empty(), SString::sprintf("GENOPER_NOOPER: validate() for format %c returned invalid value", format));
}

Geno GenMan::mutate(const Geno& g)
{
	float chg; //how many changes
	int method; //mutation method
	char format = g.getFormat();
	GenoOperators *gf = getOper_f(format);
	if (gf == NULL)
		return Geno(SString::empty(), -1, SString::empty(), SString::sprintf("GENOPER_NOOPER: Mutate(): don't know how to handle genetic format %c", format));
	Geno gv = g;
	bool canvalidate = true;
	if (testValidity(gv, canvalidate) > 0 && canvalidate == false)
		return Geno("", -1, "", "GENOPER_OPFAIL: Mutate(): cannot validate invalid source genotype");
	bool ok = false;
	int pcount = count;
	while (!ok)
	{
		char *gn = strdup(gv.getGenes().c_str()); //copy for mutation
		chg = 0;
		if (gf->mutate(gn, chg, method) == GENOPER_OK)
		{
			LoggerToMemory eh(LoggerBase::Enable | LoggerToMemory::StoreFirstMessage); //mute testValidity()
			Geno G(gn, gv.getFormat(), "", "");
			canvalidate = true;
			int res = testValidity(G, canvalidate);
			if (res == GENOPER_OK && canvalidate == false) { valid_m++; ok = true; }
			else
				if (res > 0 && canvalidate == false) invalid_m++; else
				{
				validated_m++; ok = true;
				}
			if (ok) gv = G;
		}
		else failed_m++;
		free(gn);
		count++;
		if (!ok && (count - pcount > GENMAN_REPEAT_FAILED))
		{
			logPrintf("GenMan", "Mutate", 2, "Tried " GENMAN_REPEAT_FAILED_STR "x and failed: %s", g.getGenes().c_str());
			return Geno("", -1, "", "GENOPER_OPFAIL: Mutate() tried " GENMAN_REPEAT_FAILED_STR "x and failed");
		}
	}
	mutchg += chg;
	if (history) saveLink(g.getGenes().c_str(), "", gv.getGenes().c_str(), chg);
	SString mutinfo;
	if (extmutinfo == 0) mutinfo = SString::sprintf("%.2f%% mutation of '%s'", 100 * chg, g.getName().c_str()); else
		if (extmutinfo == 1) mutinfo = SString::sprintf("%.2f%% mutation(%d) of '%s'", 100 * chg, method, g.getName().c_str()); else
			mutinfo = SString::sprintf("%.2f%% mutation(%s) of '%s'", 100 * chg, gf->mutation_method_names ? gf->mutation_method_names[method] : "unspecified method name", g.getName().c_str());
	gv.setComment(mutinfo);
	return gv;
}

Geno GenMan::crossOver(const Geno& g1, const Geno& g2)
{
	char format = g1.getFormat();
	if (format != g2.getFormat()) return Geno(SString::empty(), -1, SString::empty(), SString::sprintf("GENOPER_NOOPER: CrossOver(): does not work for parents with differing genetic formats (%c and %c)", format, g2.getFormat()));
	GenoOperators *gf = getOper_f(format);
	if (gf == NULL)
		return Geno(SString::empty(), -1, SString::empty(), SString::sprintf("GENOPER_NOOPER: CrossOver(): no operators found for genetic format %c", format));
	Geno g1v = g1, g2v = g2;

	{
		LoggerToMemory eh(LoggerBase::Enable | LoggerToMemory::StoreFirstMessage); //mute testValidity()
		bool canvalidate = true;
		if (testValidity(g1v, canvalidate) > 0 && canvalidate == false)
			return Geno("", -1, "", "GENOPER_OPFAIL: CrossOver(): cannot validate invalid source genotype #1");
		canvalidate = true;
		if (testValidity(g2v, canvalidate) > 0 && canvalidate == false)
			return Geno("", -1, "", "GENOPER_OPFAIL: CrossOver(): cannot validate invalid source genotype #2");
	}

	float chg;
	bool ok = false;
	int pcount = count;

	while (!ok)
	{
		float chg1, chg2;
		char *g1n = strdup(g1.getGenes().c_str()); //copy for crossover
		char *g2n = strdup(g2.getGenes().c_str()); //copy for crossover
		chg1 = chg2 = 0;
		if (gf->crossOver(g1n, g2n, chg1, chg2) == GENOPER_OK)
		{
			char *gn;
			if (g1n[0] && g2n[0]) if (rndUint(2) == 0) g1n[0] = 0; else g2n[0] = 0; //both provided? we want only one
			if (g1n[0]) { gn = g1n; chg = chg1; }
			else { gn = g2n; chg = chg2; }
			LoggerToMemory eh(LoggerBase::Enable | LoggerToMemory::StoreFirstMessage); //mute testValidity()
			Geno G(gn, g1v.getFormat(), "", "");
			bool canvalidate = true;
			int res = testValidity(G, canvalidate);
			if (res == GENOPER_OK && canvalidate == false) { valid_xo++; ok = true; }
			else
				if (res > 0 && canvalidate == false) invalid_xo++; else
				{
				validated_xo++; ok = true;
				}
			if (ok) g1v = G;
		}
		else failed_xo++;
		free(g1n);
		free(g2n);
		count++;
		if (!ok && (count - pcount > GENMAN_REPEAT_FAILED))
		{
			logPrintf("GenMan", "CrossOver", 2, "Tried " GENMAN_REPEAT_FAILED_STR "x and failed: %s and %s", g1.getGenes().c_str(), g2.getGenes().c_str());
			return Geno("", -1, "", "GENOPER_OPFAIL: CrossOver() tried " GENMAN_REPEAT_FAILED_STR "x and failed");
		}
	}
	// result in g1v
	xochg += chg;
	if (history) saveLink(g1.getGenes().c_str(), g2.getGenes().c_str(), g1v.getGenes().c_str(), chg);
	SString xoinfo = SString::sprintf("Crossing over of '%s' (%.2f%%) and '%s' (%.2f%%)",
		g1.getName().c_str(), 100 * chg, g2.getName().c_str(), 100 * (1 - chg));
	g1v.setComment(xoinfo);
	return g1v;
}

float GenMan::similarity(const Geno& g1, const Geno& g2)
{
	char format = g1.getFormat();
	if (format != g2.getFormat()) return GENOPER_NOOPER;
	GenoOperators *gf = getOper_f(format);
	if (!gf) return GENOPER_NOOPER; else return gf->similarity(g1.getGenes().c_str(), g2.getGenes().c_str());
}

uint32_t GenMan::getStyle(const char *g, const Geno *G, int pos)
{
	char format = G->getFormat();
	if (format == Geno::INVALID_FORMAT)
		return GENSTYLE_RGBS(64, 64, 64, 0); // gray & "valid" (unknown format so we don't know what is valid and what is not)
	if ((pos = G->mapStringToGen(pos)) == -1) return GENSTYLE_COMMENT;
	GenoOperators *gf = getOper_f(format);
	if (!gf) return GENSTYLE_CS(0, 0); //black & valid
	else return gf->style(G->getGenes().c_str(), pos);
}

uint32_t GenMan::getStyle(const char *g, int pos)
{
	Geno G(g);
	return getStyle(g, &G, pos);
}

void GenMan::getFullStyle(const char *g, const Geno *G, uint32_t *styletab)
{
	char format = G->getFormat();
	if (format == Geno::INVALID_FORMAT)
	{
		for (unsigned int pos = 0; pos < strlen(g); pos++)
			styletab[pos] = GENSTYLE_RGBS(64, 64, 64, 0); // gray & "valid" (unknown format so we don't know what is valid and what is not)
		return;
	}
	GenoOperators *gf = getOper_f(format);
	SString geny = G->getGenes();
	for (unsigned int pos = 0; pos < strlen(g); pos++)
	{
		int posmapped = G->mapStringToGen(pos);
		if (posmapped == -1) styletab[pos] = GENSTYLE_COMMENT;
		else if (!gf) styletab[pos] = GENSTYLE_CS(0, 0); //black & valid
		else styletab[pos] = gf->style(geny.c_str(), posmapped);
		//logPrintf("GenMan", "getFullStyle", 0, "%d  char='%c' (%d)  format=0x%08x", pos, g[pos], g[pos], styletab[pos]);
	}
}

void GenMan::getFullStyle(const char *g, uint32_t *styletab)
{
	Geno G(g);
	getFullStyle(g, &G, styletab);
}

string GenMan::HTMLize(const char *g) { return HTMLize(g, false); }

string GenMan::HTMLizeShort(const char *g) { return HTMLize(g, true); }

string GenMan::HTMLize(const char *g, bool shorten)
{
	char buf[50];
	int len = strlen(g);
	int chars = 0, lines = 0;
	bool shortened = false;
	uint32_t *styletab = new uint32_t[len];
	getFullStyle(g, styletab);
	string html = "\n<div style=\"background:white;padding:0.2em;font-family:arial,helvetica,sans-serif;font-size:90%\">";
	uint32_t prevstyle, prevcolor, style = 0, color = 0;
	for (int i = 0; i < len; i++)
	{
		if (shorten && ((lines == 0 && chars > 160) || (lines > 5 || chars > 300))) { shortened = true; break; }
		if (g[i] == '\r') continue;
		if (g[i] == '\n') { html += "<br>\n"; lines++; continue; }
		chars++;
		prevstyle = style;
		prevcolor = color;
		style = GENGETSTYLE(styletab[i]);
		color = GENGETCOLOR(styletab[i]);
		if ((i != 0 && (color != prevcolor))) html += "</font>";
		if ((style&GENSTYLE_INVALID) != (prevstyle&GENSTYLE_INVALID))
		{
			html += "<"; if (!(style&GENSTYLE_INVALID)) html += "/"; html += "u>";
		}
		if ((style&GENSTYLE_BOLD) != (prevstyle&GENSTYLE_BOLD))
		{
			html += "<"; if (!(style&GENSTYLE_BOLD)) html += "/"; html += "b>";
		}
		if ((style&GENSTYLE_ITALIC) != (prevstyle&GENSTYLE_ITALIC))
		{
			html += "<"; if (!(style&GENSTYLE_ITALIC)) html += "/"; html += "i>";
		}
		if ((i == 0 || (color != prevcolor)))
		{
			sprintf(buf, "<font color=#%02x%02x%02x>", GENGET_R(color), GENGET_G(color), GENGET_B(color)); html += buf;
		}
		if (g[i] == '<') html += "&lt;"; else if (g[i] == '>') html += "&gt;"; else html += g[i];
		if ((i % 3) == 0 && g[i] == ' ') html += "\n"; //for readability, insert some newlines into html...
	}
	delete[] styletab;
	html += "</u></b></i></font>";
	if (shortened) html += " [etc...]";
	html += "</div>\n";
	return html;
}

void GenMan::p_htmlize(ExtValue *args, ExtValue *ret)
{
	ret->setString(HTMLize(args->getString().c_str()).c_str());
}

void GenMan::p_htmlizeshort(ExtValue *args, ExtValue *ret)
{
	ret->setString(HTMLizeShort(args->getString().c_str()).c_str());
}

Geno GenMan::getSimplest(char format)
{
	GenoOperators *gf = getOper_f(format);
	if (!gf) return Geno();
	string info = "The simplest genotype of format f"; info += format;
	info += " for operators '"; info += gf->name; info += "'.";
	return Geno(gf->getSimplest(), format, "Root", info.c_str());
}

void GenMan::p_getsimplest(ExtValue *args, ExtValue *ret)
{
	int format = GenoObj::formatFromExtValue(args[0]);
	if (!getOper_f(format))
		ret->setEmpty();
	else
		*ret = GenoObj::makeDynamicObjectAndDecRef(new Geno(getSimplest(format)));
}

const char *GenMan::getOpName(char format)
{
	GenoOperators *gf = getOper_f(format);
	if (!gf) return "n/a"; else return gf->name.c_str();
}

GenoOperators* GenMan::getOper_f(char format)
{
	int ind = operformats.find(format);
	if (ind == -1) return NULL;
	int which_oper_of_format = seloper[ind];
	for (unsigned int i = 0; i < oper_fx_list.size(); i++)
		if (oper_fx_list[i]->supported_format == format)
			if (which_oper_of_format == 0) return oper_fx_list[i]; else which_oper_of_format--;
	return NULL; //should never happen
}

void GenMan::saveLink(const string parent1, const string parent2, const string child, const float chg)
{
	GenoLink l;
	l.count = count;
	l.parent1 = parent1;
	l.parent2 = parent2;
	l.child = child;
	l.chg = chg;
	l.fit = 0; //temporarily. Will be set when the genotype dies
	//logPrintf("GenMan","saveLink",0,"#%d: [%d] '%s' + '%s' -> '%s'",GenoLinkList.size(),count,parent1.c_str(),parent2.c_str(),child.c_str());
	GenoLinkList.push_back(l);
}

void GenMan::onDelGen(void *obj, intptr_t n)
{
	//old code needs update:
	//   ((SpeciesList*)obj)->przyDodaniu(i);
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	   GenMan *gm=(GenMan*)obj;
	   Genotype *gt=(Genotype*)(*listaGen)(n); //there is no more "listaGen"
	   string g=(const char*)gt->genotype.getGene();
	   float fit=gt->getFinalFitness();
	   for(int i=0;i<gm->GenoLinkList.size();i++) //find genotype
	   if (gm->GenoLinkList[i].g1==g) {gm->GenoLinkList[i].fit=fit; break;}
	   */
}

void GenMan::clearStats()
{
	count = 0;
	valid_m = valid_xo = validated_m = validated_xo = invalid_m = invalid_xo = failed_m = failed_xo = 0;
	mutchg = xochg = 0;
	GenoLinkList.clear();
}

void GenMan::p_clearStats(ExtValue *args, ExtValue *ret) { clearStats(); }

void GenMan::p_report(ExtValue *args, ExtValue *ret)
{                      //should be updated to handle multiple operators for a single format
	char *g, *g2;
	float f1, f2;
	int m;
	logMessage("GenMan", "Report", 0, "The following genetic operators are available:");
	for (unsigned int i = 0; i < oper_fx_list.size(); i++)
	{
		string l;
		if (oper_fx_list[i]->checkValidity("", "") != GENOPER_NOOPER) l += " checkValidity";
		if (oper_fx_list[i]->getSimplest())
		{
			g = strdup(oper_fx_list[i]->getSimplest());
			g2 = strdup(g);
			if (oper_fx_list[i]->validate(g, "") != GENOPER_NOOPER) l += " validate";
			if (oper_fx_list[i]->mutate(g, f1, m) != GENOPER_NOOPER) l += " mutate";
			if (oper_fx_list[i]->crossOver(g, g2, f1, f2) != GENOPER_NOOPER) l += " crossover";
			l += " getSimplest";
			free(g); free(g2);
		}
		//      if (oper_fx_list[i]->similarity("","")!=GENOPER_NOOPER) l+=" similarity";
		logPrintf("GenMan", "Report", 0, "format f%c (%s):%s",
			oper_fx_list[i]->supported_format, oper_fx_list[i]->name.c_str(), l.c_str());
	}
}

void GenMan::p_validate(ExtValue *args, ExtValue *ret)
{
	Geno *g = GenoObj::fromObject(args[0]);
	if (g == NULL)
		ret->setEmpty();
	else
		*ret = GenoObj::makeDynamicObjectAndDecRef(new Geno(validate(*g)));
}

void GenMan::p_mutate(ExtValue *args, ExtValue *ret)
{
	Geno *g = GenoObj::fromObject(args[0]);
	if (g == NULL)
		ret->setEmpty();
	else
		*ret = GenoObj::makeDynamicObjectAndDecRef(new Geno(mutate(*g)));
}

void GenMan::p_crossover(ExtValue *args, ExtValue *ret)
{
	Geno *g1 = GenoObj::fromObject(args[1]);
	Geno *g2 = GenoObj::fromObject(args[0]);
	if (g1 == NULL || g2 == NULL)
		ret->setEmpty();
	else
		*ret = GenoObj::makeDynamicObjectAndDecRef(new Geno(crossOver(*g1, *g2)));
}

