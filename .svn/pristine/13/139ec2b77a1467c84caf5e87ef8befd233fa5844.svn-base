// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "genotypeloader.h"
#include <common/virtfile/stdiofile.h>

/**
 @file
 Sample code: Loading genotypes from Framsticks files

 \include loader_test.cpp
 */

int main(int argc, char*argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Arguments: filename [genotype name or index (1-based) [field name]]\n"
			"If a genotype is indicated (by providing the optional genotype identifier), the program will output the raw genotype, suitable for Framsticks Theater's genotype viewer mode. If a genotype and a field name is given, the field value (instead of the raw genotype) is printed. If the second argument is not given, the genotype names from the file will be listed.\n"
			"Example: loader_test walking.gen \"Basic Quadruped\" | theater -g -\n"
			);
		return 1;
	}

	long count = 0, totalsize = 0;
	StdioFileSystem_autoselect stdiofilesys;
	GenotypeMiniLoader loader(argv[1]);
	const char* selected = (argc < 3) ? NULL : argv[2];
	const char* field_name = (argc < 4) ? NULL : argv[3];
	int selected_index = (selected&&isdigit(selected[0])) ? atol(selected) : 0;
	// using char* constructor (passing the file name to open)
	GenotypeMini *loaded;
	while (loaded = loader.loadNextGenotype())
	{ // if loaded != NULL then the "org:" object data was
		// loaded into MiniGenotype object
		count++;
		totalsize += loaded->genotype.len();
		if (selected)
		{
			if (selected_index)
			{
				if (selected_index != count)
					continue;
			}
			else
			{
				if (strcmp(loaded->name.c_str(), selected))
					continue;
			}
			if (field_name)
			{
				Param p(genotypemini_paramtab, loaded);
				int field_index = p.findId(field_name);
				if (field_index < 0)
				{
					printf("Field '%s' not found\n", field_name);
					return 3;
				}
				else
					puts(p.get(field_index).c_str());
			}
			else
				puts(loaded->genotype.c_str());
			return 0;
		}
		fprintf(stderr, "%d. %s\t(%d characters)\n", count, loaded->name.c_str(), loaded->genotype.len());
	}
	// the loop repeats until loaded==NULL, which could be beacause of error
	if (loader.getStatus() == GenotypeMiniLoader::OnError)
		fprintf(stderr, "Error: %s", loader.getError().c_str());
	// (otherwise it was the end of the file)
	if (selected)
	{
		fprintf(stderr, "genotype %s not found in %s\n", selected, argv[1]);
		return 2;
	}
	else
	{
		fprintf(stderr, "\ntotal: %d items, %d characters\n", count, totalsize);
		return 0;
	}
}
