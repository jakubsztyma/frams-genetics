// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FB_GENERAL_H_
#define _FB_GENERAL_H_

#include <frams/util/sstring.h>

class fB_GenoHelpers
{
public:
	static int geneCount(const SString& geno)
	{
		int start = 0;
		int prev = 0;
		int count = -1;
		do {
			count++;
			start = geno.indexOf("aa", start) + 1; // +1 is for progress, starting codons can overlap
			int quotecount = 0;
			for (int q = prev; q < start; q++) if (geno[q] == '\"') quotecount++;
			prev = start;
			if (quotecount % 2 != 0) count--; // 'aa' sequence is within quotes
		} while (start - 1 != -1);
		return count;
	}

	static SString getNonNestedGene(int i, const SString& genotype, int &start, int &end)
	{
		int count = -1;
		start = 0;
		int tmp = 0;
		SString result = "";
		do {
			count++;
			if (start < genotype.len())
				result = getNextGene(start, genotype, tmp, start);
			else
				start = -1;
		} while (start != -1 && count < i);
		start = tmp;
		end = start;
		return result;
	}

	static int geneCountNoNested(const SString& geno)
	{
		int start = 0;
		int count = -1;
		int tmp = 0;
		do {
			count++;
			if (start < geno.len())
				getNextGene(start, geno, tmp, start);
			else
				start = -1;
		} while (start != -1);
		return count;
	}

	static SString getGene(int i, const SString& genotype, int &start, int &end)
	{
		start = 0;
		int count = -1;
		do {
			count++;
			start = genotype.indexOf("aa", start) + 1;
			int quotecount = 0;
			for (int q = 0; q < start; q++) if (genotype[q] == '\"') quotecount++;
			if (quotecount % 2 != 0) count--; // 'aa' sequence is within quotes
		} while (start - 1 != -1 && count != i);
		if (start - 1 == -1) // there is no gene with a given "i"
		{
			start = -1;
			end = -1;
			return "";
		}
		end = start;
		int quotecount = 0;
		do {
			quotecount = 0;
			end = genotype.indexOf("zz", end);
			if (end != -1)
			{
				for (int q = start; q < end; q++) if (genotype[q] == '\"') quotecount++;
				if (quotecount % 2 != 0) end++;
			}
		} while (quotecount % 2 != 0 && end != -1);

		if (end == -1) end = genotype.len();
		else end += 2;
		start -= 1;
		return genotype.substr(start, end - start);
	}

	static SString getNextGene(int searchbegin, const SString& genotype, int &start, int &end)
	{
		start = searchbegin;
		int count = -1;
		do {
			count++;
			start = genotype.indexOf("aa", start) + 1;
			int quotecount = 0;
			for (int q = 0; q < start; q++) if (genotype[q] == '\"') quotecount++;
			if (quotecount % 2 != 0) count--; // 'aa' sequence is within quotes
		} while (start - 1 != -1 && count != 0);
		if (start - 1 == -1) // there is no gene with a given "i"
		{
			start = -1;
			end = -1;
			return "";
		}
		end = start;
		int quotecount = 0;
		do {
			quotecount = 0;
			end = genotype.indexOf("zz", end);
			if (end != -1)
			{
				for (int q = start; q < end; q++) if (genotype[q] == '\"') quotecount++;
				if (quotecount % 2 != 0) end++;
			}
		} while (quotecount % 2 != 0 && end != -1);

		if (end == -1) end = genotype.len();
		else end += 2;
		start -= 1;
		return genotype.substr(start, end - start);
	}
private:
	fB_GenoHelpers() {}
};

#endif // _FB_GENERAL_H_
