// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2016  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "genotypeloader.h"


GenotypeMiniLoader::GenotypeMiniLoader() :genotype_param(genotypemini_paramtab, &genotype_object) { init(); }
GenotypeMiniLoader::GenotypeMiniLoader(VirtFILE *f) : MultiParamLoader(f), genotype_param(genotypemini_paramtab, &genotype_object) { init(); }
GenotypeMiniLoader::GenotypeMiniLoader(const char* filename) : MultiParamLoader(filename), genotype_param(genotypemini_paramtab, &genotype_object) { init(); }

void GenotypeMiniLoader::init()
{
	addObject(&genotype_param);
	breakOn(MultiParamLoader::OnError + MultiParamLoader::AfterObject);
}

GenotypeMini* GenotypeMiniLoader::loadNextGenotype()
{
	genotype_object.clear();
	if ((go() == AfterObject) && (getObject().matchesInterfaceName(&genotype_param)))
		return &genotype_object;
	else
		return 0;
}
