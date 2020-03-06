// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <common/nonstd.h>

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

#include "genoconv.h"
#include <frams/util/multimap.h>
#include <common/util-string.h>

///////////////////////////////////////////////////////////////////////////

GenoConvParam::GenoConvParam(GenoConvManager *g) :Param(0), gcm(g)
{
	updatetab();
}

void GenoConvParam::freetab()
{
	if (tab) free(tab);
	tab = 0;
}

const char *GenoConvParam::id(int i)
{
	if (i >= gcm->converters.size()) return 0;
	sprintf(tmp_id, "genkonw%d", i);
	return tmp_id;
}

void GenoConvParam::updatetab()
{
	int i;
	GenoConverter *gk;
	ParamEntry *pe;
	int ile = gcm->converters.size();
	freetab();
	tab = (ParamEntry*)calloc(2 + ile, sizeof(ParamEntry));
	tab[0].id = "Genetics: Conversions";
	tab[0].group = 1;
	tab[0].flags = (paInt)ile;
	tab[0].name = "gkparam:";
	gcnames.clear();
	gcnames.reserve(gcm->converters.size()); //avoid reallocations in the loop below, since we externally store pointers to objects saved in this vector
	for (i = 0, pe = tab + 1; gk = (GenoConverter *)gcm->converters(i); pe++, i++)
	{
		pe->id = "?";
		pe->group = 0;
		pe->flags = 0;
		std::string descr = "f";
		descr += gk->in_format;
		descr += " --> f";
		descr += gk->out_format;
		descr += "  :  ";
		descr += gk->name;
		gcnames.push_back(descr);
		pe->name = gcnames.back().c_str(); //externally store a pointer to the object just saved in the vector
		pe->type = "d 0 1";
	}
	pe->id = 0;
}

GenoConvParam::~GenoConvParam()
{
	freetab();
}

void *GenoConvParam::getTarget(int i)
{
	GenoConverter *gk = (GenoConverter *)gcm->converters(i);
	return &gk->enabled;
}

GenoConvManager::GenoConvManager()
	:param(this)
{
}

GenoConvManager::~GenoConvManager()
{
	FOREACH(GenoConverter*, gc, converters) delete gc;
}

void GenoConvManager::addConverter(GenoConverter *gc)
{
	converters += gc;
	param.updatetab();
}
void GenoConvManager::removeConverter(GenoConverter *gc)
{
	converters -= gc;
	param.updatetab();
}

GenoConverter *GenoConvManager::findConverters(SListTempl<GenoConverter*>* result, char in, char out, int enabled, char* name)
{
	GenoConverter *gk, *retval = 0;
	int i = 0;
	for (; gk = (GenoConverter*)converters(i); i++)
	{
		if ((in != -1) && (in != gk->in_format)) continue;
		if ((out != -1) && (out != gk->out_format)) continue;
		if ((enabled != -1) && (enabled != gk->enabled)) continue;
		if ((name) && (strcmp(name, gk->name))) continue;
		if (!retval) { retval = gk; if (!result) break; }
		if (result) result->append(gk);
	}
	return retval;
}

/// Writes path into 'path'.
/// return the last path element (return >= path)
/// null -> path not found
/// @param mapavailable will receive 1 if conversion map is supported by all converters in path
/// (can be NULL if you don't need this information)

char *GenoConvManager::getPath(char in, char out, char *path, int maxlen, int *mapavailable)
{
	if (!maxlen) return 0;
	GenoConverter *gk;
	int i = 0;
	for (; gk = (GenoConverter*)converters(i); i++)
	{
		if ((gk->enabled) && (gk->in_format == in))
		{
			*path = (char)i;
			if (gk->out_format == out)
			{
				if (mapavailable)
					*mapavailable = gk->mapsupport;
				return path;
			}
			else
			{
				int mapavail;
				char *ret = getPath(gk->out_format, out, path + 1, maxlen - 1, &mapavail);
				if (ret)
				{
					if (mapavailable)
						*mapavailable = gk->mapsupport && mapavail;
					return ret;
				}
			}
		}
	}
	return 0;
}

char *GenoConvManager::getFormatPath(char in, char out, char *path, int maxlen, int *mapavailable)
{
	char *ret = getPath(in, out, path, maxlen, mapavailable);
	if (ret)
	{
		for (char*t = path; t <= ret; t++)
			*t = ((GenoConverter*)converters(*t))->out_format;
	}
	return ret;
}

Geno GenoConvManager::convert(Geno &in, char format, MultiMap *map, bool using_checkpoints, bool *converter_missing)
{
	if (in.getFormat() == format) { if (converter_missing) *converter_missing = false; return in; }
	char path[10];
	int dep;
	char *ret;
	if (in.isInvalid()) { if (converter_missing) *converter_missing = false; return Geno("", 0, "", "invalid genotype cannot be converted"); }
	int mapavail;
	for (dep = 1; dep < (int)sizeof(path); dep++) //iterative deepening
		if (ret = getPath(in.getFormat(), format, path, dep, &mapavail)) break;
	if (!ret) { if (converter_missing) *converter_missing = true; return Geno("", 0, "", "converter not found"); }
	if (converter_missing) *converter_missing = false;
	if (!map) mapavail = 0;
	char *t = path;
	SString tmp;
	tmp = in.getGenes();
	MultiMap lastmap, tmpmap;
	int firstmap = 1;
	for (; t <= ret; t++)
	{
		GenoConverter *gk = (GenoConverter*)converters(*t);
		tmp = gk->convert(tmp, mapavail ? &tmpmap : 0, using_checkpoints);
		if (!tmp.len())
		{
			string t = ssprintf("f%c->f%c conversion failed (%s)", gk->in_format, gk->out_format, gk->name);
			return Geno(0, 0, 0, t.c_str());
		}
		if (mapavail)
		{
			if (firstmap)
			{
				lastmap = tmpmap;
				firstmap = 0;
			}
			else
			{
				MultiMap m;
				m.addCombined(lastmap, tmpmap);
				lastmap = m;
			}
			tmpmap.clear();
		}
	}
	if (map)
		*map = lastmap;
	return Geno(tmp, format, in.getName(), in.getComment());
}
