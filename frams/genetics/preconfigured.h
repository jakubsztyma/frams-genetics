// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2017  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _PRECONFIGURED_GENETICS_H_
#define _PRECONFIGURED_GENETICS_H_

#include "genman.h"
#include "defgenoconv.h"

/** This class handles a typical initialization procedure and configuration of genetics:
- adds converters between genetic formats as configured by gen-config.h,
- validation of genotypes by dedicated genetic operators, or by conversion to f0 if no genetic operator is found that can validate a genotype.
*/
class PreconfiguredGenetics
{
public:
	DefaultGenoConvManager gcm;
	GenMan genman;
	Geno::Validators validators;
	ModelGenoValidator model_validator; //validation through conversion

	PreconfiguredGenetics()
	{
		gcm.addDefaultConverters(); //without converters, the application would only handle "format 0" genotypes
		if (Geno::useConverters(&gcm) != NULL)
			logPrintf("PreconfiguredGenetics", "init", LOG_WARN, "Geno converters already configured"); //someone is using multiple PreconfiguredGenetics objects? (or other potentially unsafe configuration)
		if (Geno::useValidators(&validators) != NULL)
			logPrintf("PreconfiguredGenetics", "init", LOG_WARN, "Geno validators already configured");
		static const char* genactive_classes[] = { "N", "G", "T", "S", "*", "|", "@", NULL };
		NeuroClass::resetActive();
		NeuroClass::setGenActive(genactive_classes);
		validators += &genman; //primary validation: use the extended validity checking (through dedicated genetic operators)
		validators += &model_validator; //secondary validation: this simple validator handles all cases when there is no dedicated genetic validation operator, but a converter for a particular format is available. Converters may be less strict in detecting invalid genotypes but using them and checking whether they produced a valid f0 genotype is also some way to tell whether the initial genotype was valid. Otherwise, without dedicated genetic validation operator, we would have no validity check at all.
	}

	~PreconfiguredGenetics()
	{
		Geno::useConverters(NULL);
		Geno::useValidators(NULL);
	}
};

#endif
