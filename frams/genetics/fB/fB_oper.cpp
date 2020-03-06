// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <frams/util/sstring.h>
#include <vector>
#include <frams/param/param.h>
#include "fB_conv.h"
#include "fB_general.h"
#include "fB_oper.h"
#include "../fH/fH_oper.h"

#define FIELDSTRUCT Geno_fB

static ParamEntry GENOfBparam_tab[] =
{
	{ "Genetics: fB", 3, FB_MUT_COUNT + FB_XOVER_COUNT, },
	{ "Genetics: fB: Mutation", },
	{ "Genetics: fB: Crossover", },
	{ "fB_mut_substitution", 1, 0, "Substitution", "f 0 1 0.6", FIELD(mutationprobs[FB_SUBSTITUTION]), "Probability of mutation by changing single random letter in genotype", },
	{ "fB_mut_insertion", 1, 0, "Insertion", "f 0 1 0.095", FIELD(mutationprobs[FB_INSERTION]), "Probability of mutation by inserting characters in random place of genotype", },
	{ "fB_mut_nclassins", 1, 0, "Insertion of neuron class definition", "f 0 1 0.005", FIELD(mutationprobs[FB_NCLASSINS]), "Probability of mutation by inserting neuron class definition in random place of genotype", },
	{ "fB_mut_deletion", 1, 0, "Deletion", "f 0 1 0.1", FIELD(mutationprobs[FB_DELETION]), "Probability of mutation by deleting random characters in genotype", },
	{ "fB_mut_duplication", 1, 0, "Duplication", "f 0 1 0.0", FIELD(mutationprobs[FB_DUPLICATION]), "Probability of mutation by copying single *gene* of genotype and appending it to the beginning of this genotype", },
	{ "fB_mut_translocation", 1, 0, "Translocation", "f 0 1 0.15", FIELD(mutationprobs[FB_TRANSLOCATION]), "Probability of mutation by replacing two substrings in genotype", },
	{ "fB_cross_gene_transfer", 2, 0, "Horizontal gene transfer", "f 0 1 0.0", FIELD(crossoverprobs[FB_GENE_TRANSFER]), "Probability of crossing over by transferring single genes from both parents to beginning of each other", },
	{ "fB_cross_crossover", 2, 0, "Crossing over", "f 0 1 1.0", FIELD(crossoverprobs[FB_CROSSING_OVER]), "Probability of crossing over by random distribution of genes from both parents to both children", },
	{ 0, },
};

#undef FIELDSTRUCT

Geno_fB::Geno_fB()
{
	par.setParamTab(GENOfBparam_tab);
	par.select(this);
	par.setDefault();
	supported_format = 'B';
}

bool Geno_fB::hasStick(const SString &genotype)
{
	for (int i = 0; i < fB_GenoHelpers::geneCount(genotype); i++)
	{
		int start, end;
		SString gene = fB_GenoHelpers::getGene(i, genotype, start, end);
		int endoffset = 0;
		if (gene.indexOf("zz", 0) != -1) endoffset = 2;
		if (gene.len() - endoffset < 3)
		{
			return true; // genes with length < 3 are always sticks
		}
		else if (gene[2] >= 'a' && gene[2] <= 'i')
		{
			return true; // gene within this range is stick
		}
	}
	return false;
}

int Geno_fB::checkValidity(const char *geno, const char *genoname)
{
	// load genotype
	SString genotype(geno);
	SString line;
	int pos = 0;
	// if there is no genotype to load, then return error
	if (!genotype.getNextToken(pos, line, '\n'))
	{
		return pos + 1;
	}
	// extract dimensions
	int dims = 0;
	if (!ExtValue::parseInt(line.c_str(), dims, true, false))
	{
		return 1;
	}
	// extract next token in order to check if next line starts with "aa"
	int genstart = genotype.indexOf("aa", 0);
	if (genstart != pos)
	{
		return pos + 1;
	}
	// check if rest of characters are lowercase
	for (int i = genstart; i < genotype.len(); i++)
	{
		if (!islower(genotype[i]))
		{
			if (genotype[i] == '"')
			{
				SString neuclassdef;
				int nextid = i + 1;
				if (!genotype.getNextToken(nextid, neuclassdef, '"'))
				{
					return i + 1;
				}
				Neuro *neu = new Neuro();
				neu->setDetails(neuclassdef);

				bool isclass = neu->getClass() ? true : false;
				delete neu;
				if (!isclass)
				{
					return i + 1;
				}
				i = nextid;
			}
			else
			{
				return i + 1;
			}
		}
	}
	if (!hasStick(genotype))
	{
		return 1;
	}
	return GENOPER_OK;
}

int Geno_fB::validate(char *&geno, const char *genoname)
{
	// load genotype
	SString genotype(geno);
	SString strdims;
	int pos = 0;
	if (!genotype.getNextToken(pos, strdims, '\n'))
	{
		return GENOPER_OPFAIL;
	}
	// parse dimension
	int dims = 0;
	if (!ExtValue::parseInt(strdims.c_str(), dims, true, false))
	{
		return GENOPER_OPFAIL;
	}
	SString line;
	bool fix = false;
	int genstart = genotype.indexOf("aa", 0);
	// if there is no "aa" codon in the beginning of a genotype, then add it
	if (genstart != pos)
	{
		genotype = strdims + "\naa" + genotype.substr(pos);
		fix = true;
	}
	for (int i = pos; i < genotype.len(); i++)
	{
		// if character is not alphabetic - error
		if (!isalpha(genotype[i]))
		{
			if (genotype[i] == '"')
			{
				SString neuclassdef;
				int nextid = i + 1;
				if (!genotype.getNextToken(nextid, neuclassdef, '"'))
				{
					return i + 1;
				}
				Neuro *neu = new Neuro();
				neu->setDetails(neuclassdef);

				bool isclass = neu->getClass() ? true : false;
				delete neu;
				if (!isclass)
				{
					return i + 1;
				}
				i = nextid;
			}
			else
			{
				return GENOPER_OPFAIL;
			}
		}
		// if character is uppercase, then convert it to lowercase
		else if (isupper(genotype[i]))
		{
			genotype.directWrite()[i] = tolower(genotype[i]);
			fix = true;
		}
	}
	// if the genotype does not contain any stick - add it
	if (!hasStick(genotype))
	{
		genotype = SString("aaazz") + genotype;
	}
	// if there were any changes - save them
	if (fix)
	{
		free(geno);
		geno = strdup(genotype.c_str());
	}
	return GENOPER_OK;
}

SString Geno_fB::detokenizeSequence(std::list<SString> *tokenlist)
{
	SString res = "";
	for (std::list<SString>::iterator it = tokenlist->begin(); it != tokenlist->end(); it++)
	{
		res += (*it);
	}
	return res;
}

std::list<SString> Geno_fB::tokenizeSequence(const SString &genotype)
{
	std::list<SString> res;
	int i = 0;
	while (i < genotype.len())
	{
		// if character is not alphabetic - error
		if (isalpha(genotype[i]))
		{
			SString el = "";
			el += genotype[i];
			res.push_back(el);
			i++;
		}
		else
		{
			SString neuclassdef;
			i++;
			genotype.getNextToken(i, neuclassdef, '"');
			SString ndef = "\"";
			ndef += neuclassdef;
			ndef += "\"";
			res.push_back(ndef);
		}
	}
	return res;
}

int Geno_fB::mutate(char *&geno, float &chg, int &method)
{
	SString genotype(geno);
	SString strdims;
	int pos = 0;
	genotype.getNextToken(pos, strdims, '\n');
	SString line;
	genotype.getNextToken(pos, line, '\n');
	method = roulette(mutationprobs, FB_MUT_COUNT);
	switch (method)
	{
	case FB_SUBSTITUTION:
	{
		std::list<SString> tokenized = tokenizeSequence(line);
		int rndid = rndUint(tokenized.size()); // select random letter from genotype
		// increment/decrement character - when overflow happens, this method
		// uses reflect method
		std::list<SString>::iterator it = tokenized.begin();
		std::advance(it, rndid);
		SString t = (*it);
		if ((*it).len() == 1)
		{
			if (rndUint(2) == 0)
			{
				if ((*it)[0] == 'a') (*it).directWrite()[0] = 'b';
				else (*it).directWrite()[0] = (*it)[0] - 1;
			}
			else
			{
				if ((*it)[0] == 'z') (*it).directWrite()[0] = 'y';
				else (*it).directWrite()[0] = (*it)[0] + 1;
			}
			chg = 1.0 / line.len();
		}
		else
		{
			// first method needs to extract quotes
			SString def = (*it);
			def = def.substr(1, def.len() - 2);
			Geno_fH::mutateNeuronProperties(def);
			SString res = "\"";
			res += def;
			res += "\"";
			(*it) = res;
			chg = (double)def.len() / line.len();
		}
		line = detokenizeSequence(&tokenized);
		break;
	}
	case FB_NCLASSINS:
	{
		std::list<SString> tokenized = tokenizeSequence(line);
		std::list<SString>::iterator it = tokenized.begin();
		int rndid = rndUint(tokenized.size()); // select random insertion point
		std::advance(it, rndid);
		NeuroClass *cls = getRandomNeuroClass();
		if (cls)
		{
			SString classdef = cls->getName();
			Geno_fH::mutateNeuronProperties(classdef);
			SString res = "\"";
			res += classdef;
			res += "\"";
			tokenized.insert(it, res);
			chg = (double)classdef.len() / line.len();
			line = detokenizeSequence(&tokenized);
			break;
		}
	}
	[[fallthrough]];
	case FB_INSERTION:
	{
		chg = 1.0 / line.len();
		std::list<SString> tokenized = tokenizeSequence(line);
		int rndid = rndUint(tokenized.size()); // select random insertion point
		std::list<SString>::iterator it = tokenized.begin();
		std::advance(it, rndid);
		SString letter = "a";
		letter.directWrite()[0] = 'a' + rndUint(26);
		tokenized.insert(it, letter);
		line = detokenizeSequence(&tokenized);
		break;
	}
	case FB_DELETION:
	{
		chg = 1.0 / line.len();
		std::list<SString> tokenized = tokenizeSequence(line);
		std::list<SString>::iterator it = tokenized.begin();
		int rndid = rndUint(tokenized.size()); // select random deletion point
		std::advance(it, rndid);
		tokenized.erase(it);
		line = detokenizeSequence(&tokenized);
		break;
	}
	case FB_DUPLICATION:
	{
		int rndgene = rndUint(fB_GenoHelpers::geneCount(line));
		int start, end;
		SString gene = fB_GenoHelpers::getGene(rndgene, line, start, end);
		if (gene.indexOf("zz", 0) == -1) gene += "zz";
		chg = (float)gene.len() / line.len();
		line = gene + line;
		break;
	}
	case FB_TRANSLOCATION:
	{
		std::list<SString> tokenized = tokenizeSequence(line);
		std::vector<unsigned int> cuts(4);
		for (int i = 0; i < 4; i++)
		{
			cuts[i] = rndUint(tokenized.size());
		}
		std::sort(cuts.begin(), cuts.end());
		std::vector<std::list<SString>::iterator> iters(4);
		for (int i = 0; i < 4; i++)
		{
			iters[i] = tokenized.begin();
			std::advance(iters[i], cuts[i]);
		}

		std::list<SString> res;
		res.insert(res.end(), tokenized.begin(), iters[0]);
		res.insert(res.end(), iters[2], iters[3]);
		res.insert(res.end(), iters[1], iters[2]);
		res.insert(res.end(), iters[0], iters[1]);
		res.insert(res.end(), iters[3], tokenized.end());

		//		SString first = line.substr(cuts[0], cuts[1] - cuts[0]);
		//		SString second = line.substr(cuts[2], cuts[3] - cuts[2]);
		//		SString result = line.substr(0, cuts[0]) + second +
		//			line.substr(cuts[1], cuts[2] - cuts[1]) + first + line.substr(cuts[3]);
		line = detokenizeSequence(&res);
		chg = (float)(cuts[3] - cuts[2] + cuts[1] - cuts[0]) / line.len();
		break;
	}
	}
	SString result = strdims + "\n" + line;
	free(geno);
	geno = strdup(result.c_str());
	return GENOPER_OK;
}

int Geno_fB::crossOver(char *&g1, char *&g2, float& chg1, float& chg2)
{
	SString p1(g1);
	SString p2(g2);

	int dims1 = 0, dims2 = 0;
	int pos = 0;
	SString strdims;
	p1.getNextToken(pos, strdims, '\n');
	ExtValue::parseInt(strdims.c_str(), dims1, true, false);
	SString parent1;
	p1.getNextToken(pos, parent1, '\n');

	pos = 0;
	p2.getNextToken(pos, strdims, '\n');
	ExtValue::parseInt(strdims.c_str(), dims2, true, false);

	if (dims1 != dims2)
	{
		return GENOPER_OPFAIL;
	}

	SString parent2;
	p2.getNextToken(pos, parent2, '\n');

	SString child1 = "";
	SString child2 = "";

	switch (roulette(crossoverprobs, FB_XOVER_COUNT))
	{
	case FB_GENE_TRANSFER:
	{
		// get random gene from first parent
		int choice = rndUint(fB_GenoHelpers::geneCount(parent1));
		int start, end;
		SString gene = fB_GenoHelpers::getGene(choice, parent1, start, end);
		// add this gene to the beginning of the second parent genotype
		child2 = gene + parent2;
		chg2 = (float)parent2.len() / (float)child2.len();
		// do the same for second parent
		choice = rndUint(fB_GenoHelpers::geneCount(parent2));
		gene = fB_GenoHelpers::getGene(choice, parent2, start, end);
		child1 = gene + parent1;
		chg1 = (float)parent1.len() / (float)child1.len();
		break;
	}
	//	case FB_CROSSING_OVER:
	//	{
	//		// iterate through all genes of the first parent and assign them
	//		// randomly to children
	//		for (int i = 0; i < fB_GenoHelpers::geneCount(parent1); i++)
	//		{
	//			int start, end;
	//			SString gene = fB_GenoHelpers::getGene(i, parent1, start, end);
	//			if (rndUint(2) == 0)
	//			{
	//				child1 += gene;
	//				chg1 += 1.0f;
	//			}
	//			else
	//			{
	//				child2 += gene;
	//			}
	//		}
	//		chg1 /= fB_GenoHelpers::geneCount(parent1);
	//
	//		// do the same with second parent
	//		for (int i = 0; i < fB_GenoHelpers::geneCount(parent2); i++)
	//		{
	//			int start, end;
	//			SString gene = fB_GenoHelpers::getGene(i, parent2, start, end);
	//			if (rndUint(2) == 0)
	//			{
	//				child1 += gene;
	//			}
	//			else
	//			{
	//				child2 += gene;
	//				chg2 += 1.0f;
	//			}
	//		}
	//		chg2 /= fB_GenoHelpers::geneCount(parent2);
	//		break;
	//	}
	case FB_CROSSING_OVER:
	{
		// get maximal count of genes from both parents
		int maxgenecount = max(fB_GenoHelpers::geneCountNoNested(parent1),
			fB_GenoHelpers::geneCountNoNested(parent2));

		// while there are genes in at least one genotype
		for (int i = 0; i < maxgenecount; i++)
		{
			SString to1 = "", to2 = "";
			int start = 0, end = 0;

			// if both parents have genes available, then distribute them
			if (i < fB_GenoHelpers::geneCountNoNested(parent1) &&
				i < fB_GenoHelpers::geneCountNoNested(parent2))
			{
				if (rndUint(2) == 0)
				{
					to1 = fB_GenoHelpers::getNonNestedGene(i, parent1, start, end);
					to2 = fB_GenoHelpers::getNonNestedGene(i, parent2, start, end);
					chg1 += 1.0f;
					chg2 += 1.0f;
				}
				else
				{
					to1 = fB_GenoHelpers::getNonNestedGene(i, parent2, start, end);
					to2 = fB_GenoHelpers::getNonNestedGene(i, parent1, start, end);
				}
			}
			else if (i < fB_GenoHelpers::geneCountNoNested(parent1))
			{
				if (rndUint(2) == 0)
				{
					to1 = fB_GenoHelpers::getNonNestedGene(i, parent1, start, end);
					chg1 += 1.0f;
				}
				else
				{
					to2 = fB_GenoHelpers::getNonNestedGene(i, parent1, start, end);
				}
			}
			else // if (i < fB_GenoHelpers::geneCountNoNested(parent2))
			{
				if (rndUint(2) == 0)
				{
					to1 = fB_GenoHelpers::getNonNestedGene(i, parent2, start, end);
				}
				else
				{
					to2 = fB_GenoHelpers::getNonNestedGene(i, parent2, start, end);
					chg2 += 1.0f;
				}
			}
			child1 += to1;
			child2 += to2;
		}

		chg1 /= fB_GenoHelpers::geneCountNoNested(parent1);
		chg2 /= fB_GenoHelpers::geneCountNoNested(parent2);
		break;
	}
	}

	free(g1);
	free(g2);
	if (child1.len() > 0 && child2.len() == 0)
	{
		child1 = strdims + "\n" + child1;
		g1 = strdup(child1.c_str());
		g2 = strdup("");
	}
	else if (child2.len() > 0 && child1.len() == 0)
	{
		child2 = strdims + "\n" + child2;
		g1 = strdup(child2.c_str());
		g2 = strdup("");
	}
	else
	{
		child1 = strdims + "\n" + child1;
		child2 = strdims + "\n" + child2;
		g1 = strdup(child1.c_str());
		g2 = strdup(child2.c_str());
	}
	return GENOPER_OK;
}

uint32_t Geno_fB::style(const char *geno, int pos)
{
	char ch = geno[pos];
	if (isdigit(ch))
	{
		while (pos > 0)
		{
			pos--;
			if (isdigit(geno[pos]) == 0)
			{
				return GENSTYLE_CS(0, GENSTYLE_INVALID);
			}
		}
		return GENSTYLE_RGBS(0, 0, 200, GENSTYLE_BOLD);
	}
	if (islower(ch) == 0)
	{
		return GENSTYLE_CS(0, GENSTYLE_INVALID);
	}
	uint32_t style = GENSTYLE_CS(GENCOLOR_TEXT, GENSTYLE_NONE);
	if (ch == 'a' && pos > 0 && (geno[pos - 1] == 'a' || geno[pos - 1] == '\n'))
	{
		style = GENSTYLE_RGBS(0, 200, 0, GENSTYLE_BOLD);
	}
	else if (ch == 'z' && pos > 0 && geno[pos - 1] == 'z')
	{
		style = GENSTYLE_RGBS(200, 0, 0, GENSTYLE_BOLD);
	}
	return style;
}
