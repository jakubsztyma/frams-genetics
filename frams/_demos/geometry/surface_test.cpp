// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2016  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "geometrytestutils.h"
#include <frams/model/geometry/meshbuilder.h>
#include <frams/model/model.h>
#include <frams/util/3d.h>
#include <stdio.h>

void test(Model &model, const double density)
{
	// Creating and preparing result Model object.
	Model resultModel;
	resultModel.open();
	GeometryTestUtils::addAnchorToModel(resultModel);
	
	// Creating instance of Iterator class (MeshBuilder::ModelSurface in this case). Object is
	// placed in stack memory, thus there is no heap memory allocation and creation is fast.
	MeshBuilder::ModelSurface iterator(density);
	
	// Once created, iterator can be used many times, but must be initialized before every time.
	iterator.initialize(&model);
	
	// Creating iterator output object in stack memory.
	Pt3D point;
	
	// Method tryGetNext checks if next point exists and optionally updates fields of Pt3D object.
	while (iterator.tryGetNext(point))
	{
		// Processing points created by iterator. In this case, they are added to result model as
		// small spheres.
		GeometryTestUtils::addPointToModel(point, resultModel);
	}
	
	// After Iterator initialization there are two alternative ways of using it.
	//
	// First adds all points to the specified list:
	// SListTempl<Pt3D> list;
	// iterator.addAllPointsToList(list);
	//
	// Second executes specified operation for each point:
	// Operation func;
	// iterator.forEach(func);
	// In this case, Operation is derived class of MeshBuilder::Callback class.
	
	// Finishing result Model and printing its genotype.
	resultModel.close();
	puts(resultModel.getF0Geno().getGenesAndFormat().c_str());
}

int main(int argc, char *argv[])
{
	return GeometryTestUtils::execute(
		"Creates new model built of small spheres covering surface of input model and prints its "
		"genotype.", argc, argv, test);
}
