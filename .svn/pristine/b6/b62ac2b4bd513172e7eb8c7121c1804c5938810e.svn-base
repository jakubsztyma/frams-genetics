// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "geno.h"
#include "genoconv.h"
#include <common/loggers/loggers.h>
#include <common/util-string.h>
#include <frams/model/model.h>

THREAD_LOCAL_DEF_PTR(Geno::Validators, geno_validators);
THREAD_LOCAL_DEF_PTR(GenoConvManager, geno_converters);

Geno::Validators* Geno::getValidators() { return tlsGetPtr(geno_validators); }
GenoConvManager* Geno::getConverters() { return tlsGetPtr(geno_converters); }

Geno::Validators* Geno::useValidators(Validators* val)
{
	return tlsSetPtr(geno_validators, val);
}
GenoConvManager* Geno::useConverters(GenoConvManager* gcm)
{
	return tlsSetPtr(geno_converters, gcm);
}

void Geno::init(const SString& genstring, char genformat, const SString& genname, const SString& comment)
{
	refcount = 1;
	owner = 0;
	f0gen = 0;
	isvalid = -1;
	name = genname;
	txt = comment;
	setGenesAndFormat(genstring, genformat);
}

void Geno::setGenesAndFormat(const SString& genstring, char genformat)
{
	mapinshift = 0;
	mapoutshift = 0;
	SString gencopy(genstring);
	if (genformat == -1)
	{ // unknown format
		genformat = '1';
		if (genstring.charAt(0) == '/')
		{
			int end, error_end = -1;
			switch (genstring.charAt(1))
			{
			case '/':
				genformat = genstring.charAt(2);
				if ((end = genstring.indexOf('\n')) >= 0)
				{
					mapinshift = end + 1;
					gencopy = genstring.substr(end + 1);
					if ((end > 0) && (genstring[end - 1] == '\r')) end--;
					error_end = end;
					if (end != 3) genformat = INVALID_FORMAT;
				}
				else
				{
					if (genstring.len() != 3) genformat = INVALID_FORMAT;
					gencopy = 0;
					mapinshift = genstring.len();
				}
				break;
			case '*':
				genformat = genstring.charAt(2);
				if ((end = genstring.indexOf("*/")) >= 0)
				{
					error_end = end + 2;
					if (end != 3) genformat = INVALID_FORMAT;
					gencopy = genstring.substr(end + 2);
					mapinshift = end + 2;
				}
				else
				{
					if (genstring.len() != 5) genformat = INVALID_FORMAT;
					gencopy = 0;
					mapinshift = genstring.len();
				}
				break;
			}
			if (!isalnum(genformat)) genformat = INVALID_FORMAT;
			if (genformat == INVALID_FORMAT)
			{
				SString cut;
				if (error_end<0) error_end = genstring.len();
				static const int MAX_ERROR = 20;
				if (error_end>MAX_ERROR)
					cut = genstring.substr(0, MAX_ERROR) + "...";
				else
					cut = genstring.substr(0, error_end);
				int lf = cut.indexOf('\n');
				if (lf >= 0) { if ((lf > 0) && (cut[lf - 1] == '\r')) lf--; cut = cut.substr(0, lf); }
				sstringQuote(cut);
				logPrintf("Geno", "init", LOG_ERROR, "Invalid genotype format declaration: '%s'%s", cut.c_str(), name.len() ? SString::sprintf(" in '%s'", name.c_str()).c_str() : "");
			}

		}
	}
	gen = gencopy;
	multiline = (strchr(gen.c_str(), '\n') != 0);
	format = genformat;
	freeF0();
	isvalid = -1;
	// mapoutshift...?
}

void Geno::freeF0()
{
	if (f0gen) { delete f0gen; f0gen = 0; }
}

Geno::Geno(const char *genstring, char genformat, const char *genname, const char *comment)
{
	init(SString(genstring), genformat, SString(genname), SString(comment));
}

Geno::Geno(const SString& genstring, char genformat, const SString& genname, const SString& comment)
{
	init(genstring, genformat, genname, comment);
}

Geno::Geno(const Geno& src)
	:gen(src.gen), name(src.name), format(src.format), txt(src.txt), isvalid(src.isvalid),
	f0gen(0), mapinshift(src.mapinshift), mapoutshift(src.mapinshift),
	multiline(src.multiline), owner(0)
{
	f0gen = src.f0gen ? new Geno(*src.f0gen) : 0; refcount = 1;
}

void Geno::operator=(const Geno& src)
{
	freeF0();
	gen = src.gen;
	name = src.name;
	format = src.format;
	txt = src.txt;
	isvalid = src.isvalid;
	mapinshift = src.mapinshift;
	mapoutshift = src.mapinshift;
	multiline = src.multiline;
	f0gen = src.f0gen ? new Geno(*src.f0gen) : 0;
	owner = 0;
}

Geno::Geno(const SString& src)
{
	init(src, -1, SString::empty(), SString::empty());
}

void Geno::setGenesAssumingSameFormat(const SString& g)
{
	gen = g;
	isvalid = -1;
	freeF0();
}

void Geno::setString(const SString& g)
{
	freeF0();
	init(g, -1, SString::empty(), SString::empty());
}

void Geno::setName(const SString& n)
{
	name = n;
}

void Geno::setComment(const SString& c)
{
	txt = c;
}

SString Geno::getGenesAndFormat(void) const
{
	SString out;
	if (format != '1')
	{
		if (multiline)
			out += "//";
		else
			out += "/*";
		if (format == 0)
			out += "invalid";
		else
			out += format;
		if (multiline)
			out += "\n";
		else
			out += "*/";
	}
	out += gen;
	return out;
}

int Geno::mapGenToString(int genpos) const
{
	if (genpos > gen.len()) return -2;
	if (genpos < 0) return -1;
	return mapinshift + genpos;
}

int Geno::mapStringToGen(int stringpos) const
{
	stringpos -= mapinshift;
	if (stringpos > gen.len()) return -2;
	if (stringpos < 0) return -1;
	return stringpos;
}

SString Geno::getGenes(void) const { return gen; }
SString Geno::getName(void) const { return name; }
char Geno::getFormat(void) const { return format; }
SString Geno::getComment(void) const { return txt; }

int ModelGenoValidator::testGenoValidity(Geno& g)
{
	if (g.getFormat() == '0')
	{
		Model mod(g);
		return mod.isValid();
	}
	else
	{
		bool converter_missing;
		Geno f0geno = g.getConverted('0', NULL, false, &converter_missing);
		if (converter_missing)
			return -1;//no result
		return f0geno.isValid();
	}
}

void Geno::validate()
{
	if (isvalid >= 0) return;
	if (gen.len() == 0) { isvalid = 0; return; }
	if (format == INVALID_FORMAT) { isvalid = 0; return; }
	Validators* vals = getValidators();
	if (vals != NULL)
	{
#ifdef WARN_VALIDATION_INCONSISTENCY
		vector<int> results;
		int first_result=-1;
		FOREACH(GenoValidator*, v, (*vals))
		{
			int r=v->testGenoValidity(*this);
			if (first_result<0) first_result=r;
			results.push_back(r);
		}
		int N=vals->size();
		for(int i=1;i<N;i++)
			if (results[i]!=results[0])
			{
			SString txt="Inconsistent validation results";
			for(int i=0;i<N;i++)
				txt+=SString::sprintf(" %d",results[i]);
			txt+=" for genotype '";
			txt+=getGene();
			txt+="'";
			logPrintf("Geno","validate",LOG_WARN,txt.c_str());
			break;
			}
		isvalid=first_result;
		if (isvalid>=0)
			return;
#else
		FOREACH(GenoValidator*, v, (*vals))
			if ((isvalid = v->testGenoValidity(*this)) >= 0)
				return;
#endif
	}
	isvalid = 0;
	logPrintf("Geno", "validate", LOG_WARN, "Wrong configuration? No genotype validators defined for genetic format 'f%c'.", format);
}

bool Geno::isValid(void)
{
	if (isvalid < 0)
	{
		LoggerToMemory err(LoggerBase::Enable | LoggerToMemory::StoreAllMessages, LOG_INFO);
		validate();
		err.disable();
		string msg = err.getCountSummary();
		if (msg.size() > 0)
		{
			msg += ssprintf(" while checking validity of '%s'", getName().c_str());
			msg += "\n";
			msg += err.getMessages();
			logMessage("Geno", "isValid", err.getErrorLevel(), msg.c_str());
		}
	}
	return isvalid > 0;
}

Geno Geno::getConverted(char otherformat, MultiMap *m, bool using_checkpoints, bool *converter_missing)
{
	if (otherformat == getFormat()) { if (converter_missing) *converter_missing = false; return *this; }
#ifndef NO_GENOCONVMANAGER
	GenoConvManager *converters = getConverters();
	if (converters)
	{
		if ((otherformat == '0') && (!m) && (!using_checkpoints))
		{
			if (!f0gen)
				f0gen = new Geno(converters->convert(*this, otherformat, NULL, using_checkpoints, converter_missing));
			else
			{
				if (converter_missing) *converter_missing = false;
			}
			return *f0gen;
		}
		else
			return converters->convert(*this, otherformat, m, using_checkpoints, converter_missing);
	}
#endif
	if (converter_missing) *converter_missing = true;
	return (otherformat == getFormat()) ? *this : Geno(0, 0, 0, "GenConvManager not available");
}

Geno::~Geno()
{
	if (f0gen) delete f0gen;
}
