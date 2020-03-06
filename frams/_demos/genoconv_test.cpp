// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <ctype.h>
#include <frams/genetics/defgenoconv.h>
#include <frams/model/model.h>
#include <frams/util/multimap.h>
#include <common/virtfile/stdiofile.h>
#include "printconvmap.h"
#include <common/loggers/loggertostdout.h>

/**
 @file
 Sample code: Genotype converter class
 */

/// Sample Geno converter not using Model class.
/// (This converter generates the same output for each input).
/// Such a converter is responsible for doing valid f0 (or other format) output and storing temporary data.
class GenoConv_Test : public GenoConverter
{
public:
	GenoConv_Test()
	{
		name = "Test Converter";
		in_format = 'x';
	}
	SString convert(SString &i, MultiMap *map, bool using_checkpoints) { return SString("after conversion..."); }
	~GenoConv_Test() {}
};

/// Sample Geno converter using Model class.
/// (This converter generates the same output for each input).
class GenoConv_Test2 : public GenoConverter
{
public:
	GenoConv_Test2()
	{
		name = "Test Converter #2";
		in_format = 'y';
	}

	SString convert(SString &i, MultiMap *map, bool using_checkpoints)
	{
		Model mod;
		mod.open();
		mod.addFromString(Model::PartType, "0,0,0");
		mod.addFromString(Model::PartType, "0,0,-1");
		mod.addFromString(Model::JointType, "0,1");
		mod.getPart(1)->p.y += 0.2; //as an example, directly modify position of part #1
		mod.close();
		return mod.getF0Geno().getGenes();
	}

	~GenoConv_Test2() {}
};

/// Sample Geno converter supporting conversion mapping.
/// The conversion is very simple: any sequence of <digit><character>
/// (but not inside neurons) is replaced by the repeated sequence of the character.
class GenoConv_Test3 : public GenoConverter
{
public:
	GenoConv_Test3()
	{
		name = "Test Converter #3";
		in_format = 'z';
		out_format = '1';
		mapsupport = 1;
	}
	SString convert(SString &in, MultiMap *map, bool using_checkpoints);
	~GenoConv_Test3() {}
};

/** main converting routine - most important: direct conversion map example */
SString GenoConv_Test3::convert(SString &in, MultiMap *map, bool using_checkpoints)
{
	SString dst;
	const char* src = in.c_str();
	const char* t;
	int insideneuron = 0;
	int n;
	for (t = src; *t; t++)
	{
		if (insideneuron&&*t == ']') insideneuron = 0;
		if (*t == '[') insideneuron = 1;
		if ((!insideneuron) && isdigit(*t) && t[1])
		{ // special sequence detected!
			n = *t - '0';
			t++; // *t will be repeated 'n' times
			for (int i = 0; i < n; i++)
				dst += *t;
			if (map) // fill in the map only if requested
				map->add(t - src, t - src, dst.len() - n, dst.len() - 1);
			// meaning: source character (t-src) becomes (dst.len()-n ... dst.len()-1)
		}
		else
		{
			dst += *t;
			if (map)
				map->add(t - src, t - src, dst.len() - 1, dst.len() - 1);
			// meaning: map single to single character: (t-src) into (dst.len()-1)
		}
	}
	return dst;
}


///////////////////////////////////////////////

void printGen(Geno &g)
{
	printf("Genotype:\n%s\nFormat: %c\nValid: %s\nComment: %s\n",
		g.getGenes().c_str(), g.getFormat(), g.isValid() ? "yes" : "no", g.getComment().c_str());
}

static int goodWidthForFormat(int genotype_format)
{
	return genotype_format == '0' ? 45 : 15; // more space for long f0 lines
}

// arguments:
//     genotype (or - meaning "read from stdin") [default: X]
//     target format [default: 0]
//     "checkpoints" (just write this exact word) [default: not using checkpoints]
int main(int argc, char *argv[])
{
	LoggerToStdout messages_to_stdout(LoggerBase::Enable);

	DefaultGenoConvManager gcm;
	gcm.addDefaultConverters();
	gcm.addConverter(new GenoConv_Test());
	gcm.addConverter(new GenoConv_Test2());
	gcm.addConverter(new GenoConv_Test3());
	Geno::useConverters(&gcm);

	Geno::Validators validators;
	ModelGenoValidator model_validator;
	validators += &model_validator;
	Geno::useValidators(&validators);

	SString src;
	if (argc > 1)
	{
		src = argv[1];
		if (src == "-")
		{
			StdioFILEDontClose in(stdin);
			src = "";
			loadSString(&in, src, false);
		}
	}
	else
		src = "X";
	char dst = (argc > 2) ? *argv[2] : '0';
	bool using_checkpoints = (argc > 3) ? (strcmp(argv[3], "checkpoints") == 0) : false;

	printf("*** Source genotype:\n");
	Geno g1(src);
	printGen(g1);
	MultiMap m;
	Geno g2 = g1.getConverted(dst, &m, using_checkpoints);
	printf("*** Converted to f%c:\n", dst);
	printGen(g2);

	if (using_checkpoints)
	{ // using Model with checkpoints
		Model m1(g2, false, true);//true=using_checkpoints
		printf("\nModel built from the converted f%c genotype has %d checkpoints\n", g2.getFormat(), m1.getCheckpointCount());
		Model m2(g1, false, true);//true=using_checkpoints
		printf("Model built from the source f%c genotype has %d checkpoints\n", g1.getFormat(), m2.getCheckpointCount());
		// accessing individual checkpoint models (if available)
		if (m1.getCheckpointCount() > 0)
		{
			int c = m1.getCheckpointCount() / 2;
			Model *cm = m1.getCheckpoint(c);
			printf("Checkpoint #%d (%d parts, %d joint, %d neurons)\n%s", c, cm->getPartCount(), cm->getJointCount(), cm->getNeuroCount(), cm->getF0Geno().getGenesAndFormat().c_str());
		}
	}
	else
	{ // there is no mapping for checkpoints so it's nothing interesting to see here in the checkpoints mode
		if (m.isEmpty())
			printf("(conversion map not available)\n");
		else
		{
			printf("Conversion map:\n");
			m.print();
			printConvMap(g1.getGenes(), g2.getGenes(), m, goodWidthForFormat(g1.getFormat()));
			printf("Reverse conversion map:\n");
			MultiMap rm;
			rm.addReversed(m);
			rm.print();
			printConvMap(g2.getGenes(), g1.getGenes(), rm, goodWidthForFormat(g2.getFormat()));
		}

		Model mod1(g1, 1);
		printf("\nModel map for f%c genotype:\n", g1.getFormat());
		ModelDisplayMap dm1(mod1);
		dm1.print(goodWidthForFormat(g1.getFormat()));
		MultiMap mod1combined;
		mod1combined.addCombined(mod1.getMap(), dm1.getMap());
		mod1combined.print();
		Model mod2(g2, 1);
		printf("\nModel map for f%c genotype:\n", g2.getFormat());
		ModelDisplayMap dm2(mod2);
		dm2.print(goodWidthForFormat(g2.getFormat()));
		MultiMap mod2combined;
		mod2combined.addCombined(mod2.getMap(), dm2.getMap());
		mod2combined.print();
	}
	return 0;
}
