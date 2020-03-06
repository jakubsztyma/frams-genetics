// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2016  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "geometrytestutils.h"
#include <frams/model/geometry/geometryutils.h>
#include <frams/model/geometry/modelgeometryinfo.h>
#include <frams/model/model.h>
#include <frams/util/3d.h>
#include <stdio.h>

void test(Model &model, const double density)
{
	// EXAMPLES ------------------------------------------------------------------------------------
	
	// Calculating approximate volume of model. Parameter density describes minimal number of points
	// created for each unit of length. It means, that for each cubic unit of bounding box of model,
	// at least density^3 points will be created.
	double volume = ModelGeometryInfo::volume(model, density);
	
	
	// Calculating approximate surface area of model. Parameter density describes minimal number of
	// points created for each unit of length. It means, that for each square unit of surface area
	// of parts of model, at least density^2 points will be created.
	double area = ModelGeometryInfo::area(model, density);
	
	
	// Creating output variables.
	Pt3D sizes;
	Orient axes;
	// Calculating approximate sizes and axes of model. These values are calculated together because
	// they depend on each other. Sizes are distances between furthest points on model surface
	// measured along corresponding axes. Fields sizes.x and axes.x describes the 'length' of 3D
	// model. Fields sizes.y and axes.y desctibes 'width' of 2D model created by projecting original
	// 3D model onto plane defined by axes.x vector. Fields sizes.z and axes.z destribes 'height' of
	// model. Axes are perpendicular to each other. Parameter density describes minimal number of
	// points created for each unit of length. It means, that for each square unit of surface area
	// of parts of model, at least density^2 points will be created.
	ModelGeometryInfo::findSizesAndAxes(model, density, sizes, axes);
	
	
	// Creating output variables.
	Pt3D lowerBoundary, upperBoundary;
	// Calculating bounding box of model. Result is stored in variables lowerBoundary and
	// upperBoundary.
	ModelGeometryInfo::boundingBox(model, lowerBoundary, upperBoundary);
	
	// PRINTING RESULT -----------------------------------------------------------------------------
	
	// Creating and preparing result Model object.
	Model resultModel;
	resultModel.open();
	GeometryTestUtils::addAnchorToModel(resultModel);
	
	// Adding bounding markers of bounding box apices.
	for (Octants::Octant o = Octants::FIRST; o < Octants::NUMBER; o = Octants::Octant(o+1))
	{
		Pt3D apex;
		apex.x = Octants::isPositiveX(o) ? upperBoundary.x : lowerBoundary.x;
		apex.y = Octants::isPositiveY(o) ? upperBoundary.y : lowerBoundary.y;
		apex.z = Octants::isPositiveZ(o) ? upperBoundary.z : lowerBoundary.z;
		GeometryTestUtils::addPointToModel(apex, resultModel);
	}
	
	// Adding markers of axes (intersection of axes is in the center of bounding box).
	Pt3D intersection = (lowerBoundary + upperBoundary) * 0.5;
	GeometryTestUtils::addAxesToModel(sizes, axes, intersection, resultModel);
	
	// Merging with original model.
	GeometryTestUtils::mergeModels(resultModel, model);
	
	// Finishing result Model and printing its genotype.
	resultModel.close();
	puts(resultModel.getF0Geno().getGenesAndFormat().c_str());
	
	// Printing calculated values.
	printf("# volume=%f\n", volume);
	printf("# area=%f\n", area);
	printf("# sizes.x=%f sizes.y=%f sizes.z=%f\n", sizes.x, sizes.y, sizes.z);
	printf("# axes.x=(%f, %f, %f)\n", axes.x.x, axes.x.y, axes.x.z);
	printf("# axes.y=(%f, %f, %f)\n", axes.y.x, axes.y.y, axes.y.z);
	printf("# axes.z=(%f, %f, %f)\n", axes.z.x, axes.z.y, axes.z.z);
	printf("# box.x=[%f, %f]\n", lowerBoundary.x, upperBoundary.x);
	printf("# box.y=[%f, %f]\n", lowerBoundary.y, upperBoundary.y);
	printf("# box.z=[%f, %f]\n", lowerBoundary.z, upperBoundary.z);
}

int main(int argc, char *argv[])
{
	return GeometryTestUtils::execute(
		"Calculates approximate values of surface area, volume, sizes and axes of model. Adds to "
		"model markers of bounding box apices and axes.", argc, argv, test);
}
