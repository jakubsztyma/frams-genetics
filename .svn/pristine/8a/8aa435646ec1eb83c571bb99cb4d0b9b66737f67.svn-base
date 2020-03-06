// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2017  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "geometrytestutils.h"

#include "../genotypeloader.h"
#include "frams/genetics/preconfigured.h"
#include <common/virtfile/stdiofile.h>
#include <common/loggers/loggertostdout.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int printGenotypesList(const char *file)
{
	long count = 0;
	long totalSize = 0;
	GenotypeMiniLoader loader(file);
	GenotypeMini *genotype;

	while (genotype = loader.loadNextGenotype())
	{
		count++;
		totalSize += genotype->genotype.len();

		fprintf(stderr, "%d. (%6d chars) %s\n", count, genotype->genotype.len(),
			genotype->name.c_str());
	}

	if (loader.getStatus() == GenotypeMiniLoader::OnError)
	{
		fprintf(stderr, "Error: %s\n", loader.getError().c_str());
		return 2;
	}
	else
	{
		fprintf(stderr, "\ntotal: %d items, %d chars\n", count, totalSize);
		return 0;
	}
}

class TestInvoker
{
public:
	virtual void operator()(Model &model) = 0;
	virtual ~TestInvoker() {}
};

int executeTestUsingLoadedModel(const char *file, const char *genoId, TestInvoker &test)
{
	const char* genoName = genoId;
	const int genoIndex = isdigit(genoId[0]) ? atol(genoId) : 0;
	long count = 0;
	GenotypeMiniLoader loader(file);
	GenotypeMini *genotype;

	while (genotype = loader.loadNextGenotype())
	{
		count++;

		if ((genoIndex == count) || (strcmp(genotype->name.c_str(), genoName) == 0))
		{
			Model model(genotype->genotype);

			if (!model.isValid())
			{
				fprintf(stderr, "Cannot build a valid Model from this genotype!\n");
				return 4;
			}
			SolidsShapeTypeModel sst_model(model);
			test(sst_model);
			return 0;
		}
	}

	if (loader.getStatus() == GenotypeMiniLoader::OnError)
	{
		fprintf(stderr, "Error: %s\n", loader.getError().c_str());
		return 2;
	}
	else
	{
		fprintf(stderr, "Genotype %s not found in %s\n", genoId, file);
		return 3;
	}
}

int executeTestUsingRandomModel(int shape, TestInvoker &test)
{
	Model model;
	model.open();

	if ((shape < 1) || (shape > 3))
	{
		shape = (rand() % 3) + 1;
	}

	Part *part = model.addNewPart(Part::Shape(shape));
	GeometryTestUtils::randomizePositionScaleAndOrient(part);

	model.close();
	test(model);
	GeometryTestUtils::describePart(part, stdout);
	return 0;
}

class ModelBasedTestInvoker : public TestInvoker
{
private:
	void(*test)(Model &);
public:
	ModelBasedTestInvoker(void(*_test)(Model &)) :
		test(_test)
	{}
	void operator()(Model &model)
	{
		test(model);
	}
};

int GeometryTestUtils::execute(const SString header, int argc, char *argv[], void(*test)(Model &))
{
	LoggerToStdout messages_to_stdout(LoggerBase::Enable); //comment this object out to mute error/warning messages
	StdioFileSystem_autoselect stdiofilesys;
	PreconfiguredGenetics genetics;

	srand(time(NULL));

	if ((argc == 3) && (strcmp("-l", argv[1]) == 0))
	{
		return printGenotypesList(argv[2]);
	}

	if ((argc == 4) && (strcmp("-l", argv[1]) == 0))
	{
		ModelBasedTestInvoker invoker(test);
		return executeTestUsingLoadedModel(argv[2], argv[3], invoker);
	}

	if ((argc == 2) && (strcmp("-c", argv[1]) == 0))
	{
		ModelBasedTestInvoker invoker(test);
		return executeTestUsingRandomModel(-1, invoker);
	}

	if ((argc == 3) && (strcmp("-c", argv[1]) == 0) && isdigit(argv[2][0]))
	{
		int shape = atol(argv[2]);
		ModelBasedTestInvoker invoker(test);
		return executeTestUsingRandomModel(shape, invoker);
	}

	fprintf(stderr,
		"%s\n\n"
		"argument lists:\n"
		"-l FILENAME            - to print list of models in file\n"
		"-l FILENAME GENO_ID    - to load model from file and run test\n"
		"-c [SHAPE]             - to create simple random model and run test\n\n"
		"FILENAME - name of file containing named f0 genotypes\n"
		"GENO_ID - either genotype name or index (1-based)\n"
		"SHAPE - 1=ellipsoid, 2=cuboid, 3=cylinder, others or none=random\n",
		header.c_str());
	return 1;
}

class ModelAndDensityBasedTestInvoker : public TestInvoker
{
private:
	void(*test)(Model &, const double);
	double density;
public:
	ModelAndDensityBasedTestInvoker(void(*_test)(Model &, const double), double _density) :
		test(_test),
		density(_density)
	{}

	void operator()(Model &model)
	{
		test(model, density);
	}
};

int GeometryTestUtils::execute(const SString header, int argc, char *argv[],
	void(*test)(Model &, const double))
{
	LoggerToStdout messages_to_stdout(LoggerBase::Enable); //comment this object out to mute error/warning messages
	StdioFileSystem_autoselect stdiofilesys;
	PreconfiguredGenetics genetics;

	srand(time(NULL));

	if ((argc == 3) && (strcmp("-l", argv[1]) == 0))
	{
		return printGenotypesList(argv[2]);
	}

	if ((argc == 5) && (strcmp("-l", argv[1]) == 0) && isdigit(argv[4][0]))
	{
		double density = atol(argv[4]);
		ModelAndDensityBasedTestInvoker invoker(test, density);
		return executeTestUsingLoadedModel(argv[2], argv[3], invoker);
	}

	if ((argc == 3) && (strcmp("-c", argv[1]) == 0) && isdigit(argv[2][0]))
	{
		double density = atol(argv[2]);
		ModelAndDensityBasedTestInvoker invoker(test, density);
		return executeTestUsingRandomModel(-1, invoker);
	}

	if ((argc == 4) && (strcmp("-c", argv[1]) == 0) && isdigit(argv[2][0]) && isdigit(argv[3][0]))
	{
		double density = atol(argv[2]);
		int shape = atol(argv[3]);
		ModelAndDensityBasedTestInvoker invoker(test, density);
		return executeTestUsingRandomModel(shape, invoker);
	}

	fprintf(stderr,
		"%s\n\n"
		"argument list:\n"
		"-l FILENAME                    - to print the list of models in file\n"
		"-l FILENAME GENO_ID DENSITY    - to load the model from the file and run test\n"
		"-c DENSITY [SHAPE]             - to create a simple random model and run test\n\n"
		"FILENAME - name of the file containing named f0 genotypes\n"
		"GENO_ID - either genotype name or index (1-based)\n"
		"DENSITY - minimal number of samples per unit\n"
		"SHAPE - 1=ellipsoid, 2=cuboid, 3=cylinder, others or none=random\n",
		header.c_str());
	return 1;
}

void GeometryTestUtils::addAnchorToModel(Model &model)
{
	Part *part = model.addNewPart(Part::SHAPE_ELLIPSOID);

	part->p = Pt3D(0);
	part->scale = Pt3D(0.1);
	part->vcolor = Pt3D(1.0, 0.0, 1.0);

	addAxesToModel(Pt3D(0.5), Orient(Orient_1), Pt3D(0.0), model);
}

void GeometryTestUtils::addPointToModel(const Pt3D &markerLocation, Model &model)
{
	Part *anchor = model.getPart(0);
	Part *part = model.addNewPart(Part::SHAPE_ELLIPSOID);

	part->p = Pt3D(markerLocation);
	part->scale = Pt3D(0.05);
	part->vcolor = Pt3D(1.0, 1.0, 0.0);

	model.addNewJoint(anchor, part, Joint::SHAPE_FIXED);
}

void GeometryTestUtils::addAxesToModel(const Pt3D &sizes, const Orient &axes, const Pt3D &center,
	Model &model)
{
	Part *anchor = model.getPart(0);
	Part *part;

	part = model.addNewPart(Part::SHAPE_CUBOID);
	part->scale = Pt3D(sizes.x, 0.05, 0.05);
	part->setOrient(axes);
	part->p = center;
	part->vcolor = Pt3D(1.0, 0.0, 0.0);
	model.addNewJoint(anchor, part, Joint::SHAPE_FIXED);

	part = model.addNewPart(Part::SHAPE_CUBOID);
	part->scale = Pt3D(0.05, sizes.y, 0.05);
	part->setOrient(axes);
	part->p = center;
	part->vcolor = Pt3D(0.0, 1.0, 0.0);
	model.addNewJoint(anchor, part, Joint::SHAPE_FIXED);

	part = model.addNewPart(Part::SHAPE_CUBOID);
	part->scale = Pt3D(0.05, 0.05, sizes.z);
	part->setOrient(axes);
	part->p = center;
	part->vcolor = Pt3D(0.0, 0.0, 1.0);
	model.addNewJoint(anchor, part, Joint::SHAPE_FIXED);
}

void GeometryTestUtils::mergeModels(Model &target, Model &source)
{
	Part *targetAnchor = target.getPart(0);
	Part *sourceAnchor = source.getPart(0);

	target.moveElementsFrom(source);

	target.addNewJoint(targetAnchor, sourceAnchor, Joint::SHAPE_FIXED);
}

double frand(double from, double width)
{
	return from + width * ((rand() % 10000) / 10000.0);
}

void GeometryTestUtils::randomizePositionScaleAndOrient(Part *part)
{
	part->p = Pt3D(frand(1.5, 1.0), frand(1.5, 1.0), frand(1.5, 1.0));
	part->scale = Pt3D(frand(0.1, 0.9), frand(0.1, 0.9), frand(0.1, 0.9));
	part->setRot(Pt3D(frand(0.0, M_PI), frand(0.0, M_PI), frand(0.0, M_PI)));
}

void GeometryTestUtils::describePart(const Part *part, FILE *output)
{
	fprintf(output, "# shape=%d\n", part->shape);
	fprintf(output, "# x=%f\n", part->p.x);
	fprintf(output, "# y=%f\n", part->p.y);
	fprintf(output, "# z=%f\n", part->p.z);
	fprintf(output, "# sx=%f\n", part->scale.x);
	fprintf(output, "# sy=%f\n", part->scale.y);
	fprintf(output, "# sz=%f\n", part->scale.z);
	fprintf(output, "# rx=%f\n", part->rot.x);
	fprintf(output, "# ry=%f\n", part->rot.y);
	fprintf(output, "# rz=%f\n", part->rot.z);
}
