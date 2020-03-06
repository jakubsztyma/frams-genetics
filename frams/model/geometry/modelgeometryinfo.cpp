// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "modelgeometryinfo.h"
#include <frams/model/geometry/geometryutils.h>
#include <frams/model/geometry/meshbuilder.h>

void ModelGeometryInfo::findSizesAndAxes(Model &input_model, const double density,
	Pt3D &sizes, Orient &axes)
{
	SolidsShapeTypeModel model(input_model);
	SListTempl<Pt3D> points;
	MeshBuilder::ModelApices apices(density);
	apices.initialize(&model.getModel());
	apices.addAllPointsToList(points);
	if (points.size() < 1) //maybe 1 or 2 points are also not enough for findSizesAndAxesOfPointsGroup() to work...
	{
		logPrintf("ModelGeometryInfo", "findSizesAndAxesOfModel", LOG_ERROR, "Empty points sample for model with %d part(s)", model.getModel().getPartCount());
		sizes = Pt3D_0;
		axes = Orient_1;
		return;
	}
	GeometryUtils::findSizesAndAxesOfPointsGroup(points, sizes, axes);
}

void ModelGeometryInfo::boundingBox(const Model &model, Pt3D &lowerBoundary, Pt3D &upperBoundary)
{
	if (model.getPartCount() == 0) //should never happen. Invalid model provided?
	{
		lowerBoundary = Pt3D_0;
		upperBoundary = Pt3D_0;
		return;
	}
	
	boundingBox(model.getPart(0), lowerBoundary, upperBoundary);
	
	for (int i = 1; i < model.getPartCount(); i++)
	{
		Pt3D partLowerBoundary, partUpperBoundary;
		boundingBox(model.getPart(i), partLowerBoundary, partUpperBoundary);
		
		lowerBoundary.getMin(partLowerBoundary);
		upperBoundary.getMax(partUpperBoundary);
	}
}

void ModelGeometryInfo::boundingBox(const Part *part, Pt3D &lowerBoundary, Pt3D &upperBoundary)
{
	lowerBoundary.x = upperBoundary.x = part->p.x;
	lowerBoundary.y = upperBoundary.y = part->p.y;
	lowerBoundary.z = upperBoundary.z = part->p.z;
	
	for (Octants::Octant o = Octants::FIRST; o < Octants::NUMBER; o = Octants::Octant(o+1))
	{
		Pt3D vertex = part->scale;
		vertex.x *= Octants::isPositiveX(o) ? +1 : -1;
		vertex.y *= Octants::isPositiveY(o) ? +1 : -1;
		vertex.z *= Octants::isPositiveZ(o) ? +1 : -1;
		
		vertex = part->o.transform(vertex) + part->p;
		
		lowerBoundary.getMin(vertex);
		upperBoundary.getMax(vertex);
	}
}

double ModelGeometryInfo::volume(Model &input_model, const double density)
{
	SolidsShapeTypeModel model(input_model);
	Pt3D lowerBoundary, upperBoundary;
	boundingBox(model, lowerBoundary, upperBoundary);
	
	MeshBuilder::BoundingBoxVolume iterator(density);
	iterator.initialize(lowerBoundary, upperBoundary);
	
	Pt3D point;
	int allPoints = 0, pointsInsideModel = 0;
	
	while (iterator.tryGetNext(point))
	{
		allPoints += 1;
		pointsInsideModel += GeometryUtils::isPointInsideModel(point, model) ? 1 : 0;
	}
	
	double boundingBoxVolume
		= (upperBoundary.x - lowerBoundary.x)
		* (upperBoundary.y - lowerBoundary.y)
		* (upperBoundary.z - lowerBoundary.z);
	
	return boundingBoxVolume * (double)pointsInsideModel / (double)allPoints;
}

double ModelGeometryInfo::area(Model &input_model, const double density)
{
	SolidsShapeTypeModel model(input_model);
	double area = 0.0;
	
	for (int partIndex = 0; partIndex < model.getModel().getPartCount(); partIndex+=1)
	{
		area += externalAreaOfPart(model, partIndex, density);
	}
	
	return area;
}

double ModelGeometryInfo::externalAreaOfPart(const Model &model, const int partIndex, const double density)
{
	Part *part = model.getPart(partIndex);
	switch (part->shape)
	{
		case Part::SHAPE_ELLIPSOID:
			return externalAreaOfEllipsoid(model, partIndex, density);
			
		case Part::SHAPE_CUBOID:
			return externalAreaOfCuboid(model, partIndex, density);
			
		case Part::SHAPE_CYLINDER:
			return externalAreaOfCylinder(model, partIndex, density);
	}
	logPrintf("ModelGeometryInfo", "externalAreaOfPart", LOG_ERROR, "Part shape=%d not supported", part->shape);
	return 0;
}

double externalAreaOfSurface(const Model &model, const int partIndex, MeshBuilder::Iterator &surface, const double area)
{
	Pt3D point;
	int all = 0, sur = 0;
	
	while (surface.tryGetNext(point))
	{
		all += 1;
		sur += GeometryUtils::isPointInsideModelExcludingPart(point, &model, partIndex) ? 0 : 1;
	}
	
	return sur * area / all;
}

double ModelGeometryInfo::externalAreaOfEllipsoid(const Model &model, const int partIndex, const double density)
{
	Part *part = model.getPart(partIndex);
	
	MeshBuilder::EllipsoidSurface ellipsoid(density);
	ellipsoid.initialize(part);
	
	double area = GeometryUtils::ellipsoidArea(part->scale);
	
	return externalAreaOfSurface(model, partIndex, ellipsoid, area);
}

double ModelGeometryInfo::externalAreaOfCuboid(const Model &model, const int partIndex, const double density)
{
	Part *part = model.getPart(partIndex);
	
	double externalArea = 0.0;
	
	MeshBuilder::RectangleSurface rectangle(density);
	
	for (CuboidFaces::Face f = CuboidFaces::FIRST; f < CuboidFaces::NUMBER; f = CuboidFaces::Face(f+1))
	{
		rectangle.initialize(part, f);
		
		double area = 4.0;
		area *= !CuboidFaces::isX(f) ? part->scale.x : 1.0;
		area *= !CuboidFaces::isY(f) ? part->scale.y : 1.0;
		area *= !CuboidFaces::isZ(f) ? part->scale.z : 1.0;
		
		externalArea += externalAreaOfSurface(model, partIndex, rectangle, area);
	}
	
	return externalArea;
}

double ModelGeometryInfo::externalAreaOfCylinder(const Model &model, const int partIndex, const double density)
{
	Part *part = model.getPart(partIndex);
	
	double externalArea = 0.0;
	
	MeshBuilder::EllipseSurface ellipse(density);
	
	double area = M_PI * part->scale.y * part->scale.z;
	
	for (CylinderBases::Base b = CylinderBases::FIRST; b < CylinderBases::NUMBER; b = CylinderBases::Base(b+1))
	{
		ellipse.initialize(part, b);
		
		externalArea += externalAreaOfSurface(model, partIndex, ellipse, area);
	}
	
	MeshBuilder::CylinderWallSurface cylinderWall(density);
	cylinderWall.initialize(part);
	
	area = 2.0*part->scale.x * GeometryUtils::ellipsePerimeter(part->scale.y, part->scale.z);
	
	externalArea += externalAreaOfSurface(model, partIndex, cylinderWall, area);
	
	return externalArea;
}
