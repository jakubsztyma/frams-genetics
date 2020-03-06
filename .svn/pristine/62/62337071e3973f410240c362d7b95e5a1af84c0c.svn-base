// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2016  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _GENOTYPELOADER_H_
#define _GENOTYPELOADER_H_

#include <frams/util/sstring.h>
#include <frams/param/multiparamload.h>
#include "genotypemini.h"

/** In most simple cases this is the class you would use to load a series of genotypes from
	the Framsticks genotype file.

	Typical usage: (see loader_test_geno.cpp for the working code)
	while(genotype=loader.loadNextGenotype()) doSomethingWith(genotype);

	MiniGenotypeLoader is simply the MultiParamLoader configured to load one kind of data: "org:" objects.
	The instance of this class can also be reconfigured to recognize more objects by using MultiParamLoader
	methods, or you can use it as a guide for creating your own specialized class.
	*/
class GenotypeMiniLoader : public MultiParamLoader
{
	GenotypeMini genotype_object;
	Param genotype_param;
	void init();
public:
	GenotypeMiniLoader();
	GenotypeMiniLoader(VirtFILE *f);
	GenotypeMiniLoader(const char* filename);

	/** @returns genotype object if one was loaded or NULL otherwise.

		Returned GenotypeMini pointer always references the the same object (GenotypeMiniLoader::genotype_object)
		which means you may need to copy the data from it before calling loadNextGenotype() again.
		In the default configuration (simple GenotypeMiniLoader) NULL is always final and should be used
		to finish processing.

		If the loader is configured to load other objects or stop on other conditions, NULL will also mean
		every condition other than "GenotypeLoaded". In such cases you need MultiParamLoader::getStatus(),
		MultiParamLoader::finished() and other methods to determine the real cause of NULL.
		*/
	GenotypeMini* loadNextGenotype();
};

#endif
