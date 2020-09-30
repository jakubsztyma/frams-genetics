// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "geometryutils.h"

#include <math.h>

double GeometryUtils::pointPosition(const int pointIndex, const int numberOfPoints)
{
	if (numberOfPoints == 1)
		return 0;
	else
		return pointIndex / (numberOfPoints-1.0);
}

double GeometryUtils::pointOnAxis(const double scale, const double position)
{
	return (position-0.5) * scale;
}

double GeometryUtils::pointOnAxis(const double scale, const int pointIndex, const int numberOfPoints)
{
	return pointOnAxis(scale, pointPosition(pointIndex, numberOfPoints));
}

double GeometryUtils::combination(const double value1, const double value2, const double position)
{
	return value1 + position * (value2-value1);
}

double GeometryUtils::combination(const double value1, const double value2, const int pointIndex, const int numberOfPoints)
{
	return combination(value1, value2, pointPosition(pointIndex, numberOfPoints));
}

bool GeometryUtils::isPointInsideModelExcludingPart(const Pt3D &point, const Model *model, const int excludedPartIndex)
{
	for (int i = 0; i < excludedPartIndex; i++)
	{
		if (isPointInsidePart(point, model->getPart(i)))
		{
			return true;
		}
	}
	
	for (int i = excludedPartIndex+1; i < model->getPartCount(); i++)
	{
		if (isPointStrictlyInsidePart(point, model->getPart(i)))
		{
			return true;
		}
	}
	
	return false;
}

bool GeometryUtils::isPointInsideModel(const Pt3D &point, const Model &model)
{
	for (int i = 0; i < model.getPartCount(); i++)
	{
		if (isPointInsidePart(point, model.getPart(i)))
		{
			return true;
		}
	}
	
	return false;
}

bool GeometryUtils::isPointInsidePart(const Pt3D &point, const Part *part)
{
	switch (part->shape)
	{
		case Part::SHAPE_ELLIPSOID:
			return isPointInsideEllipsoid(point, part);
			break;
			
		case Part::SHAPE_CUBOID:
			return isPointInsideCuboid(point, part);
			break;
			
		case Part::SHAPE_CYLINDER:
			return isPointInsideCylinder(point, part);
			break;
	}
	logPrintf("GeometryUtils", "isPointInsidePart", LOG_ERROR, "Part shape=%d not supported", part->shape);
	return false;
}

bool GeometryUtils::isPointStrictlyInsidePart(const Pt3D &point, const Part *part)
{
	switch (part->shape)
	{
		case Part::SHAPE_ELLIPSOID:
			return isPointStrictlyInsideEllipsoid(point, part);
			break;
			
		case Part::SHAPE_CUBOID:
			return isPointStrictlyInsideCuboid(point, part);
			break;
			
		case Part::SHAPE_CYLINDER:
			return isPointStrictlyInsideCylinder(point, part);
			break;
	}
	logPrintf("GeometryUtils", "isPointStrictlyInsidePart", LOG_ERROR, "Part shape=%d not supported", part->shape);
	return false;
}

bool GeometryUtils::isPointInsideEllipsoid(const Pt3D &point, const Part *part)
{
	Pt3D moved = point - part->p;
	Pt3D rotated;
	part->o.revTransform(rotated, moved);
	
	double r
		= (pow(rotated.x, 2.0) / pow(part->scale.x, 2.0))
		+ (pow(rotated.y, 2.0) / pow(part->scale.y, 2.0))
		+ (pow(rotated.z, 2.0) / pow(part->scale.z, 2.0));
	
	return r <= 1.0;
}

bool GeometryUtils::isPointStrictlyInsideEllipsoid(const Pt3D &point, const Part *part)
{
	Pt3D moved = point - part->p;
	Pt3D rotated;
	part->o.revTransform(rotated, moved);
	
	double r
		= (pow(rotated.x, 2.0) / pow(part->scale.x, 2.0))
		+ (pow(rotated.y, 2.0) / pow(part->scale.y, 2.0))
		+ (pow(rotated.z, 2.0) / pow(part->scale.z, 2.0));
	
	return r < 1.0;
}

bool GeometryUtils::isPointInsideCuboid(const Pt3D &point, const Part *part)
{
	Pt3D moved = point - part->p;
	Pt3D rotated;
	part->o.revTransform(rotated, moved);
	
	return (fabs(rotated.x) <= part->scale.x)
		&& (fabs(rotated.y) <= part->scale.y)
		&& (fabs(rotated.z) <= part->scale.z);
}

bool GeometryUtils::isPointStrictlyInsideCuboid(const Pt3D &point, const Part *part)
{
	Pt3D moved = point - part->p;
	Pt3D rotated;
	part->o.revTransform(rotated, moved);
	
	return (fabs(rotated.x) < part->scale.x)
		&& (fabs(rotated.y) < part->scale.y)
		&& (fabs(rotated.z) < part->scale.z);
}

bool GeometryUtils::isPointInsideCylinder(const Pt3D &point, const Part *part)
{
	Pt3D moved = point - part->p;
	Pt3D rotated;
	part->o.revTransform(rotated, moved);
	
	double r
		= (pow(rotated.y, 2.0) / pow(part->scale.y, 2.0))
		+ (pow(rotated.z, 2.0) / pow(part->scale.z, 2.0));
	
	return (fabs(rotated.x) <= part->scale.x) && (r <= 1.0);
}

bool GeometryUtils::isPointStrictlyInsideCylinder(const Pt3D &point, const Part *part)
{
	Pt3D moved = point - part->p;
	Pt3D rotated;
	part->o.revTransform(rotated, moved);
	
	double r
		= (pow(rotated.y, 2.0) / pow(part->scale.y, 2.0))
		+ (pow(rotated.z, 2.0) / pow(part->scale.z, 2.0));
	
	return (fabs(rotated.x) < part->scale.x) && (r < 1.0);
}

void GeometryUtils::findSizesAndAxesOfPointsGroup(SListTempl<Pt3D> &points, Pt3D &sizes,
	Orient &axes)
{
	findSizeAndAxisOfPointsGroup(points, sizes.x, axes.x);
	orthographicProjectionToPlane(points, axes.x);
	findSizeAndAxisOfPointsGroup(points, sizes.y, axes.y);
	orthographicProjectionToPlane(points, axes.y);
	
	Pt3D minimal(points.get(0)), maximal(points.get(0));
	
	for (int i = 1; i < points.size(); i++)
	{
		minimal.getMin(points.get(i));
		maximal.getMax(points.get(i));
	}
	
	sizes.z = minimal.distanceTo(maximal);
	axes.z.vectorProduct(axes.x, axes.y);
}

void GeometryUtils::findSizeAndAxisOfPointsGroup(const SListTempl<Pt3D> &points, double &size,
	Pt3D &axis)
{
	int index1, index2;
	size = findTwoFurthestPoints(points, index1, index2);
	createAxisFromTwoPoints(axis, points.get(index1), points.get(index2));
}

double GeometryUtils::findTwoFurthestPoints(const SListTempl<Pt3D> &points, int &index1,
	int &index2)
{
	double distance = 0;
	index1 = index2 = 0;
	
	for (int i = 0; i < points.size()-1; i++)
	{
		Pt3D p1 = points.get(i);
		
		for (int j = i+1; j < points.size(); j++)
		{
			Pt3D p2 = points.get(j);
			double d = p1.distanceTo(p2);
			
			if (d > distance)
			{
				distance = d;
				index1 = i;
				index2 = j;
			}
		}
	}
	
	return distance;
}

void GeometryUtils::createAxisFromTwoPoints(Pt3D &axis, const Pt3D &point1, const Pt3D &point2)
{
	Pt3D vector = point2 - point1;
	vector.normalize();
	
	axis.x = vector.x;
	axis.y = vector.y;
	axis.z = vector.z;
}

void GeometryUtils::orthographicProjectionToPlane(SListTempl<Pt3D> &points,
	const Pt3D &planeNormalVector)
{
	for (int i = 0; i < points.size(); i++)
	{
		Pt3D &point = points.get(i);
		
		double distance = pointDistanceToPlane(point, planeNormalVector);
		
		point.x -= planeNormalVector.x * distance;
		point.y -= planeNormalVector.y * distance;
		point.z -= planeNormalVector.z * distance;
	}
}

double GeometryUtils::pointDistanceToPlane(const Pt3D &point, const Pt3D &planeNormalVector)
{
	return planeNormalVector.x*point.x + planeNormalVector.y*point.y + planeNormalVector.z*point.z;
}

void GeometryUtils::getRectangleApicesFromCuboid(const Part *part, const CuboidFaces::Face face, Pt3D &apex1, Pt3D &apex2, Pt3D &apex3, Pt3D &apex4)
{
	Pt3D temp1(part->scale), temp2(part->scale), temp3(part->scale), temp4(part->scale);
	
	if (CuboidFaces::isX(face))
	{
		temp2.z *= -1;
		temp3.y *= -1;
		temp4.z *= -1;
		temp4.y *= -1;
	}
	else if (CuboidFaces::isY(face))
	{
		temp2.x *= -1;
		temp3.z *= -1;
		temp4.x *= -1;
		temp4.z *= -1;
	}
	else if (CuboidFaces::isZ(face))
	{
		temp2.y *= -1;
		temp3.x *= -1;
		temp4.y *= -1;
		temp4.x *= -1;
	}
	
	if (CuboidFaces::isNegative(face))
	{
		temp1 *= -1;
		temp2 *= -1;
		temp3 *= -1;
		temp4 *= -1;
	}

	part->o.transform(apex1, temp1);
	part->o.transform(apex2, temp2);
	part->o.transform(apex3, temp3);
	part->o.transform(apex4, temp4);

	apex1 += part->p;
	apex2 += part->p;
	apex3 += part->p;
	apex4 += part->p;
}

void GeometryUtils::getRectangleApices(const double width, const double height, const Pt3D &position, const Orient &orient, Pt3D &apex1, Pt3D &apex2, Pt3D &apex3, Pt3D &apex4)
{
	Pt3D temp1(0.0, +width, +height);
	Pt3D temp2(0.0, +width, -height);
	Pt3D temp3(0.0, -width, +height);
	Pt3D temp4(0.0, -width, -height);

	orient.transform(apex1, temp1);
	orient.transform(apex2, temp2);
	orient.transform(apex3, temp3);
	orient.transform(apex4, temp4);

	apex1 += position;
	apex2 += position;
	apex3 += position;
	apex4 += position;
}

void GeometryUtils::getNextEllipseSegmentationPoint(const double d, const double a, const double b, double &x, double &y)
{
	x += d / sqrt(1.0 + (b * b * x * x) / (a * a * (a * a - x * x)));
	double sqrt_arg = 1.0 - (x * x) / (a * a);
	if (sqrt_arg >= 0)
		y = b * sqrt(sqrt_arg);
	else
		y = std::numeric_limits<double>::signaling_NaN();
	//This function is called from MeshBuilder::EllipsoidSurface::findNextAreaEdgeAndPhase().
	//y=NaN set above co-occurs with the value of x that doesn't meet the condition tested in findNextAreaEdgeAndPhase().
	//If the condition is true (i.e., x exceeds the allowed range), entirely new values of x and y are set in the next step anyway.
	//An impossible-to-calculate y should never be used for invalid x, hence y=NaN is set here to indicate this specific situation and signal just in case anyone would try to use such y.
}

double GeometryUtils::ellipsoidArea(const Pt3D &sizes)
{
	return ellipsoidArea(sizes.x, sizes.y, sizes.z);
}

double GeometryUtils::ellipsoidArea(const double a, const double b, const double c)
{
	double p = 1.6075;
	double ap = pow(a, p);
	double bp = pow(b, p);
	double cp = pow(c, p);
	return 4*M_PI * pow((ap*bp + bp*cp + cp*ap) / 3.0, 1.0 / p);
}

double GeometryUtils::ellipsePerimeter(const double a, const double b)
{
	return M_PI * ((3 * (a+b)) - sqrt((3*a + b) * (a + 3*b)));
}


double GeometryUtils::calculateSolidVolume(Part * part)
{
	double radiiProduct = part->scale.x * part->scale.y * part->scale.z;
	switch (part->shape)
	{
		case Part::Shape::SHAPE_CUBOID:
			return 8.0 * radiiProduct;
		case Part::Shape::SHAPE_CYLINDER:
			return  2.0 * M_PI * radiiProduct;
		case Part::Shape::SHAPE_ELLIPSOID:
			return  (4.0 / 3.0) * M_PI * radiiProduct;
		default:
			logMessage("GenoOperators", "calculateSolidVolume", LOG_ERROR, "Unsupported part shape");
			return -1;
	}
}

bool GeometryUtils::isSolidPartScaleValid(Part::Shape &partShape, Pt3D &scale)
{
	Part *tmpPart = new Part(partShape);
	tmpPart->scale = scale;
	double volume = GeometryUtils::calculateSolidVolume(tmpPart);

	Part_MinMaxDef minP = Model::getMinPart();
	Part_MinMaxDef maxP = Model::getMaxPart();

	if (volume > maxP.volume || minP.volume > volume)
		return false;
	if (scale.x < minP.scale.x || scale.y < minP.scale.y || scale.z < minP.scale.z)
		return false;
	if (scale.x > maxP.scale.x || scale.y > maxP.scale.y || scale.z > maxP.scale.z)
		return false;

	if (partShape == Part::Shape::SHAPE_ELLIPSOID && scale.maxComponentValue() != scale.minComponentValue())
		// When any radius has a different value than the others
		return false;
	if (partShape == Part::Shape::SHAPE_CYLINDER && scale.y != scale.z)
		// If base radii have different values
		return false;
	return true;
}
