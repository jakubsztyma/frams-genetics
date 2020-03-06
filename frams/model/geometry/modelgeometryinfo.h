// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _MODELGEOMETRYINFO_H_
#define _MODELGEOMETRYINFO_H_

#include <frams/model/model.h>
#include <frams/util/3d.h>
#include <frams/util/list.h>


/**
Currently, the three functions area(), volume() and findSizesAndAxes() are independent and self-sufficient.
Each of them converts the input model to SolidsShapeTypeModel if necessary.
Were these functions to be called multiple times for the same model, this would be inefficient
(the potential conversion of the same model would take place multiple times).

To improve efficiency, these functions should not have the Model argument.
Instead, another function should be introduced to set the model, the conversion to SolidsShapeTypeModel
should take place in this function once, and these three functions would then always use the converted model.
A similar optimization is already implemented in the ModelGeometry class that provides FramScript access to ModelGeometryInfo.
*/
namespace ModelGeometryInfo
{
	double area(Model &model, const double density);
	double volume(Model &model, const double density);
	void findSizesAndAxes(Model &model, const double density, Pt3D &sizes, Orient &axes);

	void boundingBox(const Model &model, Pt3D &lowerBoundary, Pt3D &upperBoundary);
	void boundingBox(const Part *part, Pt3D &lowerBoundary, Pt3D &upperBoundary);
	double externalAreaOfPart(const Model &model, const int partIndex, const double density);
	double externalAreaOfEllipsoid(const Model &model, const int partIndex, const double density);
	double externalAreaOfCuboid(const Model &model, const int partIndex, const double density);
	double externalAreaOfCylinder(const Model &model, const int partIndex, const double density);
}

#endif
