// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "genotypeloader.h"
#include <common/virtfile/stdiofile.h>

/**
 @file
 Sample code: Saving genotypes

 \include saver_test_geno.cpp
 */

int main(int argc, char*argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "Arguments: filename number_of_genotypes\n"
			"Example: saver_test_geno file.gen 3\n"
			);
		return 1;
	}

	StdioFileSystem_autoselect stdiofilesys;
	VirtFILE *f = Vfopen(argv[1], "w");
	if (f)
	{
		int N = atoi(argv[2]);
		GenotypeMini g;
		Param p(genotypemini_paramtab, &g);
		g.clear();
		printf("Saving %d genotypes to %s\n", N, argv[1]);
		for (int i = 1; i <= N; i++)
		{
			g.name = SString::sprintf("Genotype#%d", i);
			g.genotype = ""; for (int x = 0; x < i; x++) g.genotype += "X";
			g.velocity = 0.1*i;
			g.energy0 = 1;
			g.info = "Saved by saver_test_geno.cpp";
			g.is_valid = 1;
			p.saveMultiLine(f, "org");
		}
		delete f;
		return 0;
	}
	else
	{
		printf("Could not write to %s\n", argv[1]);
		return 1;
	}
}
