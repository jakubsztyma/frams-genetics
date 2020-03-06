// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "fB_conv.h"

#include <frams/param/paramobj.h>
#include <vector>
#include <frams/util/multimap.h>
#include "fB_general.h"

double GenoConv_fBH::convertCharacterTo01(char c)
{
	return (double)(c - 'a') / 25.0;
}

double GenoConv_fBH::convertCharacterToWeight(char c)
{
	if (c <= 'm')
	{
		return -0.001 * pow(2.0, (double)('m' - c));
	}
	else
	{
		return 0.001 * pow(2.0, (double)(c - 'n'));
	}
}

fH_Handle* GenoConv_fBH::convertCharacterToHandle(char c, int dims, int start, int end, std::vector<IRange> ranges[3])
{
	fH_Handle *handle = NULL;
	if (c >= 'a' && c <= 'i')
	{
		handle = new fH_StickHandle(dims, start, end);
		ranges[0].push_back(IRange(start, end));
	}
	else if (c >= 'j' && c <= 'p')
	{
		handle = new fH_NeuronHandle(dims, start, end);
		ranges[1].push_back(IRange(start, end));
	}
	else
	{
		handle = new fH_ConnectionHandle(dims, start, end);
		ranges[2].push_back(IRange(start, end));
	}
	return handle;
}

SString GenoConv_fBH::convert(SString &i, MultiMap *map, bool using_checkpoints)
{
	// if there is no genotype to load, then return error

	std::vector<IRange> ranges[3];

	int pos = 0;
	SString line;
	i.getNextToken(pos, line, '\n');
	int dims = 0;
	// extract dimensions
	if (!ExtValue::parseInt(line.c_str(), dims, true, false))
	{
		logMessage("f2::Builder", "parseGenotype", LOG_ERROR, "Could not parse number of dimensions");
		return "";
	}

	if (dims < 1)
	{
		logMessage("f2::Builder", "parseGenotype", LOG_ERROR, "Number of dimensions cannot be lower than 1");
		return 1;
	}

	fH_Builder creature(dims, false);

	for (int q = 0; q < fB_GenoHelpers::geneCount(i); q++)
	{
		int start, end;
		SString gene = fB_GenoHelpers::getGene(q, i, start, end);
		end -= 1; // last character is included in range, so decrementation is required
		int endoffset = 0;
		if (gene.indexOf("zz", 0) != -1) endoffset = 2;
		if (gene.len() - endoffset < 3)
		{
			fH_StickHandle *handle = new fH_StickHandle(dims, start, end);
			ParamEntry *tab = creature.getParamTab(handle->type);
			void *obj = ParamObject::makeObject(tab);
			Param par(tab, NULL);
			par.select(obj);
			par.setDefault();
			handle->loadProperties(par);
			ranges[0].push_back(IRange(start, end));
			creature.addHandle(handle);
			continue;
		}
		int hclasspos = 2;
		if (gene[2] == '"')
		{
			hclasspos--;
			if (!getNextCharId(gene, hclasspos))
			{
				return "";
			}
		}
		fH_Handle *handle = convertCharacterToHandle(gene[hclasspos], dims, start, end, ranges);
		ParamEntry *tab = creature.getParamTab(handle->type);
		void *obj = ParamObject::makeObject(tab);
		Param par(tab, NULL);
		par.select(obj);
		par.setDefault();

		int propindex = 0;
		int z = hclasspos;
		if (gene[z] == '"')
		{
			z--;
			if (!getNextCharId(gene, z))
			{
				delete handle;
				ParamObject::freeObject(obj);
				return "";
			}
		}
		endoffset = 0;
		if (gene.indexOf("zz", 0) != -1) endoffset = 2;
		int nclassdefcount = 1;
		while (z < gene.len() - endoffset)
		{
			if (processNextLetter(creature, handle, par, gene, propindex, z, ranges, nclassdefcount) == -1)
			{
				logMessage("GenoConv_fBH", "convert", LOG_ERROR, "Property of fH could not be parsed");
				delete handle;
				ParamObject::freeObject(obj);
				return "";
			}
		}
		if (handle->type == fHBodyType::NEURON && propindex < par.getPropCount())
		{
			SString nclass;
			if (!getNeuroClass(gene, nclass, nclassdefcount))
			{
				delete handle;
				ParamObject::freeObject(obj);
				return "";
			}
			par.setStringById(FH_PE_NEURO_DET, nclass);
		}
		handle->loadProperties(par);
		creature.addHandle(handle);
	}

	SString fHgenotype = creature.toString();

	if (NULL != map)
	{
		int fHpos = 0;
		SString line;
		fHgenotype.getNextToken(fHpos, line, '\n');
		int lastpos = fHpos;
		for (int t = 0; t < 3; t++)
		{
			for (unsigned int q = 0; q < ranges[t].size(); q++)
			{
				fHgenotype.getNextToken(fHpos, line, '\n');
				map->add(ranges[t][q].begin, ranges[t][q].end, lastpos, fHpos - 1);
				lastpos = fHpos;
			}
		}
	}

	return fHgenotype;
}

bool GenoConv_fBH::getNextCharId(const SString& genotype, int &i)
{
	i++;
	if (genotype[i] == '"')
	{
		int nextid = i + 1;
		do
		{
			nextid = genotype.indexOf('"', nextid);
			if (nextid == -1)
			{
				return false;
			}
			nextid++;
		}
		while (genotype[nextid] == '"');
		i = nextid;
	}
	return true;
}

bool GenoConv_fBH::getNeuroClass(const SString& gene, SString &def, int nclassdefcount)
{
	SString lastdef = "N";
	int nclass = 0;
	int pos = 0;
	while (nclass < nclassdefcount)
	{
		pos = gene.indexOf('\"', pos);
		if (pos == -1)
		{
			def = lastdef;
			return true;
		}
		pos++;
		SString currdef;
		if (gene.indexOf('\"', pos) == -1 || !gene.getNextToken(pos, currdef, '\"'))
		{
			def = lastdef;
			return false;
		}
		lastdef = currdef;
		nclass++;
	}
	def = lastdef;
	return true;
}

int GenoConv_fBH::processNextLetter(fH_Builder &creature, fH_Handle *&currhandle, Param &par, const SString& gene, int &propindex, int &i, std::vector<IRange> ranges[3], int &nclassdefcount)
{
	if (propindex >= par.getPropCount())
	{
		int tmpend = currhandle->end;
		currhandle->end = i - 1;
		currhandle->loadProperties(par);
		creature.addHandle(currhandle);
		currhandle = convertCharacterToHandle(gene[i], currhandle->getDimensions(), currhandle->begin + i, tmpend, ranges);
		ParamEntry *tab = creature.getParamTab(currhandle->type);
		par.setParamTab(tab);
		void *obj = ParamObject::makeObject(tab);
		par.select(obj);
		par.setDefault();
		propindex = 0;
		if (!getNextCharId(gene, i))
			return -1;
		return 0;
	}
	else
	{
		if (*par.type(propindex) == 'f')
		{
			if (currhandle->type == fHBodyType::CONNECTION && *par.id(propindex) == 'w')
			{
				par.setDouble(propindex, convertCharacterToWeight(gene[i]));
			}
			else
			{
				double val = convertCharacterTo01(gene[i]);
				double mn, mx, def;
				par.getMinMaxDouble(propindex, mn, mx, def);
				par.setDouble(propindex, min(mx, max(mn, (mx - mn) * val + mn)));
			}
			propindex++;
			if (!getNextCharId(gene, i))
				return -1;
			return 0;
		}
		else if (currhandle->type == fHBodyType::NEURON && *par.id(propindex) == 'd')
		{
			//When 'd' property appears for i-th element in gene, the method
			//looks for i-th neuron definition
			SString nclass;
			if (!getNeuroClass(gene, nclass, nclassdefcount)) return -1;
			par.setString(propindex, nclass);
			propindex++;
			nclassdefcount++;
			if (!getNextCharId(gene, i))
				return -1;
			return 0;
		}
		// other property types are not available in this encoding
		return -1;
	}
}
