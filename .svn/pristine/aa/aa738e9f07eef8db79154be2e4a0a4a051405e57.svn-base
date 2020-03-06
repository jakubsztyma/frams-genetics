// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2016  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <common/virtfile/stdiofile.h>

#include <frams/model/model.h>
#include <common/loggers/loggertostdout.h>
#include <frams/genetics/preconfigured.h>

/**
 @file
 Sample code: Convert old-style shapes (sticks are Joints) to new style (sticks are Parts)
 Usage: shapeconvert [-sSHAPE] [-tTHICKNESS] [genotype_or_stdin]

 Calling examples:

 # convert any genotype to f0 using new-style shapes:
 shapeconvert "X"

 //0
 m:
 p:0.5, sh=3, sx=0.5, sy=0.200000002980232, sz=0.200000002980232, rx=-1.5707963267949, rz=3.14159265358979

 # load a genotype from file, convert to new shapes, display:
 loader_test data/other.gen "Bird" | shapeconvert -s1 - | theater.exe -g -

 */

int main(int argc, char*argv[])
{
	StdioFILE::setStdio();//setup VirtFILE::Vstdin/out/err
	LoggerToStdout messages_to_stderr(LoggerBase::Enable | LoggerBase::DontBlock, VirtFILE::Vstderr); //errors -> stderr, don't interfere with stdout

	PreconfiguredGenetics genetics;
	Part::Shape shape = Part::SHAPE_CYLINDER;
	float thickness = 0.2;

	char* gen_arg = 0;
	for (int i = 1; i < argc; i++)
	{
		char* ar = argv[i];
		if (ar[0] == '-')
			switch (ar[1])
		{
			case 's': shape = (Part::Shape)atol(ar + 2);
				if ((shape != Part::SHAPE_ELLIPSOID) && (shape != Part::SHAPE_CUBOID) && (shape != Part::SHAPE_CYLINDER))
				{
					logPrintf("", "shapeconvert", LOG_ERROR, "Invalid shape");
					return 4;
				}
				break;
			case 't': thickness = atof(ar + 2); break;
			case 'h': puts("Usage: shapeconvert [-sSHAPE] [-tTHICKNESS] [genotype_or_stdin]\n\tSHAPE: 1=ellipsoid, 2=cuboid, 3(default)=cylinder\n\tTHICKNESS: used for Part.sy/sz (default=0.2)"); break;
		}
		else
			if (!gen_arg)
				gen_arg = ar;
	}
	SString gen;
	if (gen_arg)
		gen = gen_arg;
	else
		loadSString(VirtFILE::Vstdin, gen);
	Geno g(gen);
	Model m(g);

	if (!m.isValid())
	{
		logPrintf("", "shapeconvert", LOG_ERROR, "Cannot build Model from the supplied genotype");
		return 2;
	}

	if (m.getShapeType() != Model::SHAPE_BALL_AND_STICK)
	{
		logPrintf("", "shapeconvert", LOG_ERROR, "Only ball-and-stick models can be converted");
		return 3;
	}

	Model newmodel;
	newmodel.open();
	newmodel.buildUsingSolidShapeTypes(m, shape, thickness);
	newmodel.close();

	Geno f0_g = newmodel.getF0Geno();
	puts(f0_g.getGenesAndFormat().c_str());

	return 0;
}
