// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2016  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "geometrytestutils.h"
#include <frams/model/geometry/geometryutils.h>
#include <frams/model/geometry/meshbuilder.h>
#include <frams/model/model.h>
#include <stdio.h>

void test(Model &model, const double density)
{
	// Creating and preparing result Model object.
	Model resultModel;
	resultModel.open();
	GeometryTestUtils::addAnchorToModel(resultModel);
	
	// Creating instance of Iterator class (MeshBuilder::ModelApices in this case). Object is
	// placed in stack memory, thus there is no heap memory allocation and creation is fast.
	MeshBuilder::BoundingBoxVolume iterator(density);
	
	// Once created, iterator can be used many times, but must be initialized before every time.
	iterator.initialize(model);
	
	// Creating output variable.
	Pt3D point;
	
	// Method tryGetNext checks if next point exists and optionally updates fields of Pt3D object.
	while (iterator.tryGetNext(point))
	{
		// Processing points created by iterator. In this case, they are added to result model as
		// small spheres only if they are placed inside or on surface of input model.
		if (GeometryUtils::isPointInsideModel(point, model))
		{
			GeometryTestUtils::addPointToModel(point, resultModel);
		}
	}
	
	// Finishing result Model and printing its genotype.
	resultModel.close();
	puts(resultModel.getF0Geno().getGenesAndFormat().c_str());
}

int main(int argc, char *argv[])
{
	return GeometryTestUtils::execute(
		"Creates new model built of small spheres filling volume of input model and prints its "
		"genotype.", argc, argv, test);
}
