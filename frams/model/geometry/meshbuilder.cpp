// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "meshbuilder.h"

#include <frams/model/geometry/modelgeometryinfo.h>
#include <stdio.h>
#include <math.h>

MeshBuilder::Iterator::Iterator(const double _density):
	density(_density)
{}

double MeshBuilder::Iterator::getDensity() const
{
	return density;
}

void MeshBuilder::Iterator::setDensity(const double _density)
{
	density = _density;
}

void MeshBuilder::Iterator::forEach(Callback &callback)
{
	Pt3D point;
	
	while (tryGetNext(point))
	{
		callback(point);
	}
}

void MeshBuilder::Iterator::addAllPointsToList(SListTempl<Pt3D> &list)
{
	Pt3D point;
	
	while (tryGetNext(point))
	{
		list += point;
	}
}



MeshBuilder::Segment::Segment(const double _density):
	Iterator(_density), point1(0), point2(0), i(0), I(0), numberOfPoints(0)
{}

void MeshBuilder::Segment::initialize(const Pt3D &point1, const Pt3D &point2, const bool skipFirst, const bool skipLast, const bool forceOddNumberOfPoints)
{
	this->point1 = point1;
	this->point2 = point2;
	numberOfPoints = (int)ceil(density * point1.distanceTo(point2)) + 1;
	numberOfPoints += forceOddNumberOfPoints && (numberOfPoints%2 == 0) ? 1 : 0;
	i = skipFirst ? 1 : 0;
	I = numberOfPoints - (skipLast ? 1 : 0);
}

bool MeshBuilder::Segment::tryGetNext(Pt3D &point)
{
	if (i >= I)
	{
		return false;
	}
	
	double position = GeometryUtils::pointPosition(i, numberOfPoints);
	point.x = GeometryUtils::combination(point1.x, point2.x, position);
	point.y = GeometryUtils::combination(point1.y, point2.y, position);
	point.z = GeometryUtils::combination(point1.z, point2.z, position);
	
	i += 1;
	return true;
}



MeshBuilder::RectangleSurface::RectangleSurface(const double _density):
	Iterator(_density), edge1(_density), edge2(_density), area(_density), skipVerticalEdges(false), forceOddNumberOfPoints(false)
{}

void MeshBuilder::RectangleSurface::initialize(const Part *part, const CuboidFaces::Face face, const bool skipVerticalEdges, const bool skipHorizontalEdges, const bool forceOddNumberOfPoints)
{
	Pt3D apex1, apex2, apex3, apex4;
	GeometryUtils::getRectangleApicesFromCuboid(part, face, apex1, apex2, apex3, apex4);
	initialize(apex1, apex2, apex3, apex4, skipVerticalEdges, skipHorizontalEdges, forceOddNumberOfPoints);
}

void MeshBuilder::RectangleSurface::initialize(const double width, const double height, const Pt3D &position, const Orient &orient, const bool skipVerticalEdges, const bool skipHorizontalEdges, const bool forceOddNumberOfPoints)
{
	Pt3D apex1, apex2, apex3, apex4;
	GeometryUtils::getRectangleApices(width, height, position, orient, apex1, apex2, apex3, apex4);
	initialize(apex1, apex2, apex3, apex4, skipVerticalEdges, skipHorizontalEdges, forceOddNumberOfPoints);
}

void MeshBuilder::RectangleSurface::initialize(const Pt3D &apex1, const Pt3D &apex2, const Pt3D &apex3, const Pt3D &apex4, const bool skipVerticalEdges, const bool skipHorizontalEdges, const bool forceOddNumberOfPoints)
{
	this->skipVerticalEdges = skipVerticalEdges;
	this->forceOddNumberOfPoints = forceOddNumberOfPoints;
	
	edge1.setDensity(density);
	edge1.initialize(apex1, apex2, skipHorizontalEdges, skipHorizontalEdges, forceOddNumberOfPoints);
	
	edge2.setDensity(density);
	edge2.initialize(apex3, apex4, skipHorizontalEdges, skipHorizontalEdges, forceOddNumberOfPoints);
	
	area = Segment(density);
}

bool MeshBuilder::RectangleSurface::tryGetNext(Pt3D &point)
{
	while (!area.tryGetNext(point))
	{
		Pt3D point1, point2;
		if (edge1.tryGetNext(point1) && edge2.tryGetNext(point2))
		{
			area.initialize(point1, point2, skipVerticalEdges, skipVerticalEdges, forceOddNumberOfPoints);
		}
		else
		{
			return false;
		}
	}
	
	return true;
}



MeshBuilder::CuboidApices::CuboidApices():
	Iterator(0), scale(0), position(0), orient(Orient_1), octant(Octants::NUMBER)
{}

void MeshBuilder::CuboidApices::initialize(const Part *part)
{
	initialize(part->scale, part->p, part->o);
}

void MeshBuilder::CuboidApices::initialize(const Pt3D &scale, const Pt3D &position, const Orient &orient)
{
	this->scale = scale;
	this->position = position;
	this->orient = orient;
	octant = Octants::FIRST;
}

bool MeshBuilder::CuboidApices::tryGetNext(Pt3D &point)
{
	if (octant == Octants::NUMBER)
	{
		return false;
	}
	
	Pt3D temp(scale);
	
	temp.x *= Octants::isPositiveX(octant) ? 1 : -1;
	temp.y *= Octants::isPositiveY(octant) ? 1 : -1;
	temp.z *= Octants::isPositiveZ(octant) ? 1 : -1;
	
	orient.transform(point, temp);
	point += position;
	
	octant = Octants::Octant(octant+1);
	
	return true;
}



MeshBuilder::CuboidSurface::CuboidSurface(const double _density):
	Iterator(_density), apices(), rectangle(_density), iterator(NULL), part(NULL), face(CuboidFaces::NUMBER)
{}

void MeshBuilder::CuboidSurface::initialize(const Part *part)
{
	this->part = part;
	face = CuboidFaces::FIRST;
	apices.setDensity(density);
	apices.initialize(part);
	iterator = &apices;
	rectangle.setDensity(density);
}

bool MeshBuilder::CuboidSurface::tryGetNext(Pt3D &point)
{
	if (iterator == NULL)
	{
		return false;
	}
	
	while(!iterator->tryGetNext(point))
	{
		if (face < CuboidFaces::NUMBER)
		{
			iterator = &rectangle;
			rectangle.initialize(part, face, true, false);
			face = CuboidFaces::Face(face+1);
		}
		else
		{
			return false;
		}
	}
	
	return true;
}



MeshBuilder::EllipseSurface::EllipseSurface(const double _density):
	Iterator(_density), rectangle(_density), position(0), orient(Orient_1), inversedSquaredWidth(0.0), inversedSquaredHeight(0.0)
{}

void MeshBuilder::EllipseSurface::initialize(const Part *part, const CylinderBases::Base base)
{
	double relativePositionX = part->scale.x;
	relativePositionX *= CylinderBases::isPositive(base) ? +1 : -1;
	Pt3D relativePosition;
	part->o.transform(relativePosition, Pt3D(relativePositionX, 0.0, 0.0));
	initialize(part->scale.y, part->scale.z, part->p + relativePosition, part->o);
}

void MeshBuilder::EllipseSurface::initialize(const double width, const double height, const Pt3D &position, const Orient &orient)
{
	this->position = position;
	this->orient = orient;
	rectangle.setDensity(density);
	rectangle.initialize(width, height, Pt3D(0), Orient_1, false, false, true);
	inversedSquaredWidth = 1.0 / pow(width, 2.0);
	inversedSquaredHeight = 1.0 / pow(height, 2.0);
}

bool MeshBuilder::EllipseSurface::tryGetNext(Pt3D &point)
{
	Pt3D temp;
	
	while (rectangle.tryGetNext(temp))
	{
		double r
			= (pow(temp.y, 2.0) * inversedSquaredWidth)
			+ (pow(temp.z, 2.0) * inversedSquaredHeight);
		if (r <= 1.0)
		{
			orient.transform(point, temp);
			point += position;
			return true;
		}
	}
	
	return false;
}



MeshBuilder::Ellipse::Ellipse(const double _density):
	Iterator(_density), phase(Done), position(0), orient(Orient_1), width(0.0), height(0.0), d(0.0), p(0.0), q(0.0), P(0.0), Q(0.0), D(0.0), a(0.0), b(0.0), quadrant(QuadrantsYZ::FIRST)
{}

void MeshBuilder::Ellipse::initialize(const Part *part, const CylinderBases::Base base)
{
	double relativePositionX = part->scale.x;
	relativePositionX *= CylinderBases::isPositive(base) ? +1 : -1;
	Pt3D relativePosition;
	part->o.transform(relativePosition, Pt3D(relativePositionX, 0.0, 0.0));
	initialize(part->scale.y, part->scale.z, part->p + relativePosition, part->o);
}

void MeshBuilder::Ellipse::initialize(const double width, const double height, const Pt3D &position, const Orient &orient)
{
	this->width = width;
	this->height = height;
	this->position = position;
	this->orient = orient;
	calculateAndSetSegmentationDistance();
	initializePhase(yToZ);
	D = 0;
}

bool MeshBuilder::Ellipse::tryGetNext(Pt3D &point)
{
	if (phase == Done)
	{
		return false;
	}
	
	setPoint(point);
	
	quadrant = QuadrantsYZ::QuadrantYZ(quadrant+1);
	
	if (quadrant == QuadrantsYZ::NUMBER)
	{
		quadrant = QuadrantsYZ::FIRST;
		findNextPQAndPhase();
	}
	
	return true;
}

void MeshBuilder::Ellipse::calculateAndSetSegmentationDistance()
{
	d = 1.0 / density;
	int i = 0;
	
	Pt3D p1(0);
	i += findLastPQOfPhase(yToZ, p1.z, p1.y);
	
	Pt3D p2(0);
	i += findLastPQOfPhase(zToY, p2.y, p2.z);
		
	double ld = p1.distanceTo(p2);
	double td = i * d + ld;
	int n = (int)ceil(td * density);
	d = td / n;
}

void MeshBuilder::Ellipse::initializePhase(Phase ph)
{
	phase = ph;
	
	if (phase != Done)
	{
		a = phase == yToZ ? height : width;
		b = phase == yToZ ? width : height;
		
		p = 0.0;
		q = b;
		P = a*a / sqrt(a*a + b*b);
		Q = b*b / sqrt(a*a + b*b);
		
		quadrant = QuadrantsYZ::POSITIVE_Y_NEGATIVE_Z;
	}
}

void MeshBuilder::Ellipse::setPoint(Pt3D &point)
{
	double np = p * (QuadrantsYZ::isPositiveY(quadrant) ? +1 : -1);
	double nq = q * (QuadrantsYZ::isPositiveZ(quadrant) ? +1 : -1);
	
	double y = phase == yToZ ? nq : np;
	double z = phase == yToZ ? np : nq;
	
	Pt3D temp(0.0, y, z);
	orient.transform(point, temp);
	point += position;
}

void MeshBuilder::Ellipse::findNextPQAndPhase()
{
	double _p = p;
	double _q = q;
	
	GeometryUtils::getNextEllipseSegmentationPoint(d, a, b, p, q);
	
	if (p > P)
	{
		D += sqrt(pow(P-_p, 2.0) + pow(Q-_q, 2.0));
		
		if ((phase == zToY) && (D > 1.5 * d))
		{
			p = P;
			q = Q;
			D = 0;
		}
		else
		{
			initializePhase(Phase(phase+1));
		}
	}
}

int MeshBuilder::Ellipse::findLastPQOfPhase(Phase ph, double &lp, double &lq)
{
	initializePhase(ph);
	int i = 0;
	
	do
	{
		lp = p;
		lq = q;
		
		GeometryUtils::getNextEllipseSegmentationPoint(d, a, b, p, q);
		
		i += p <= P ? 1 : 0;
	} while (p <= P);
	
	return i;
}

MeshBuilder::CylinderEdges::CylinderEdges(const double _density):
	Iterator(_density), ellipse(_density), edge(0.0), length(0.0), base(CylinderBases::NUMBER)
{}

void MeshBuilder::CylinderEdges::initialize(const Part *part)
{
	initialize(part->scale, part->p, part->o);
}

void MeshBuilder::CylinderEdges::initialize(const Pt3D &scale, const Pt3D &position, const Orient &orient)
{
	initialize(scale.x, scale.y, scale.z, position, orient);
}

void MeshBuilder::CylinderEdges::initialize(const double length, const double width, const double height, const Pt3D &position, const Orient &orient)
{
	ellipse.setDensity(density);
	ellipse.initialize(width, height, position, orient);
	
	orient.transform(this->length, Pt3D(length, 0.0, 0.0));
	
	base = CylinderBases::NUMBER;
}

bool MeshBuilder::CylinderEdges::tryGetNext(Pt3D &point)
{
	if (base == CylinderBases::NUMBER)
	{
		if (!ellipse.tryGetNext(edge))
		{
			return false;
		}
		
		base = CylinderBases::FIRST;
	}
	
	point.x = edge.x + length.x * (CylinderBases::isPositive(base) ? +1 : -1);
	point.y = edge.y + length.y * (CylinderBases::isPositive(base) ? +1 : -1);
	point.z = edge.z + length.z * (CylinderBases::isPositive(base) ? +1 : -1);
	
	base = CylinderBases::Base(base+1);
	
	return true;
}



MeshBuilder::CylinderWallSurface::CylinderWallSurface(const double _density):
	Iterator(_density), edge(_density), length(0.0), area(_density)
{}

void MeshBuilder::CylinderWallSurface::initialize(const Part *part)
{
	initialize(part->scale, part->p, part->o);
}

void MeshBuilder::CylinderWallSurface::initialize(const Pt3D &scale, const Pt3D &position, const Orient &orient)
{
	initialize(scale.x, scale.y, scale.z, position, orient);
}

void MeshBuilder::CylinderWallSurface::initialize(const double length, const double width, const double height, const Pt3D &position, const Orient &orient)
{
	edge.setDensity(density);
	edge.initialize(width, height, position, orient);
	
	orient.transform(this->length, Pt3D(length, 0.0, 0.0));
	
	area.setDensity(density);
}

bool MeshBuilder::CylinderWallSurface::tryGetNext(Pt3D &point)
{
	while (!area.tryGetNext(point))
	{
		if (edge.tryGetNext(point))
		{
			area.initialize(point + length, point - length);
		}
		else
		{
			return false;
		}
	}
	
	return true;
}



MeshBuilder::CylinderSurface::CylinderSurface(const double _density):
	Iterator(_density), wall(_density), ellipse(_density), iterator(NULL), part(NULL), base(CylinderBases::NUMBER)
{}

void MeshBuilder::CylinderSurface::initialize(const Part *part)
{
	this->part = part;
	base = CylinderBases::FIRST;
	wall.setDensity(density);
	wall.initialize(part);
	iterator = &wall;
	ellipse.setDensity(density);
}

bool MeshBuilder::CylinderSurface::tryGetNext(Pt3D &point)
{
	if (iterator == NULL)
	{
		return false;
	}
	
	while(!iterator->tryGetNext(point))
	{
		if (base < CylinderBases::NUMBER)
		{
			iterator = &ellipse;
			ellipse.initialize(part, base);
			base = CylinderBases::Base(base+1);
		}
		else
		{
			return false;
		}
	}
	
	return true;
}



MeshBuilder::EllipsoidSurface::EllipsoidSurface(const double _density):
	Iterator(_density), phase(Done), edge(0.0), area(0.0), scale(0.0), limit(0.0), d(0.0), part(NULL), octant(Octants::NUMBER)
{}

void MeshBuilder::EllipsoidSurface::initialize(const Part *part)
{
	this->part = part;
	d = 1.0 / density;
	initializePhase(X);
}

bool MeshBuilder::EllipsoidSurface::tryGetNext(Pt3D &point)
{
	if (phase == Done)
	{
		return false;
	}
	
	setPoint(point);
	proceedToNextOctant();
	
	if (octant == Octants::NUMBER)
	{
		octant = Octants::FIRST;
		findNextAreaEdgeAndPhase();
	}
	
	return true;
}

void MeshBuilder::EllipsoidSurface::initializePhase(Phase ph)
{
	phase = ph;
	
	if (phase != Done)
	{
		scale.x = phase == X ? part->scale.x : phase == Y ? part->scale.y : part->scale.z;
		scale.y = phase == X ? part->scale.y : phase == Y ? part->scale.z : part->scale.x;
		scale.z = phase == X ? part->scale.z : phase == Y ? part->scale.x : part->scale.y;
		
		edge = Pt3D(scale.x, 0.0, scale.z);
		limit.y = scale.y*scale.y / sqrt(scale.x*scale.x + scale.y*scale.y);
		limit.z = edge.z*edge.z / sqrt(edge.x*edge.x + edge.z*edge.z);
		area = Pt3D(edge.x, edge.y, 0.0);
		
		octant = Octants::FIRST;
	}
}

void MeshBuilder::EllipsoidSurface::setPoint(Pt3D &point)
{
	Pt3D temp1(area);
	temp1.x *= Octants::isPositiveX(octant) ? +1 : -1;
	temp1.y *= Octants::isPositiveY(octant) ? +1 : -1;
	temp1.z *= Octants::isPositiveZ(octant) ? +1 : -1;
	
	Pt3D temp2;
	temp2.x = phase == X ? temp1.x : phase == Y ? temp1.z : temp1.y;
	temp2.y = phase == X ? temp1.y : phase == Y ? temp1.x : temp1.z;
	temp2.z = phase == X ? temp1.z : phase == Y ? temp1.y : temp1.x;
	
	part->o.transform(point, temp2);
	point += part->p;
}

void MeshBuilder::EllipsoidSurface::proceedToNextOctant()
{
	int step = 1;
	step += (Octants::isNegativeZ(octant) && (area.z == 0.0)) ? 1 : 0;
	step += (Octants::isNegativeY(octant) && (area.y == 0.0)) ? 2 : 0;
	octant = Octants::Octant(octant + step);
}

void MeshBuilder::EllipsoidSurface::findNextAreaEdgeAndPhase()
{
	GeometryUtils::getNextEllipseSegmentationPoint(d, edge.z, edge.x, area.z, area.x);

	if ((area.z > limit.z) || (area.y > limit.y * sqrt(1.0 - (area.z*area.z) / (scale.z*scale.z))))
	{
		GeometryUtils::getNextEllipseSegmentationPoint(d, scale.y, scale.x, edge.y, edge.x);
		if (edge.y > limit.y)
		{
			initializePhase(Phase(phase+1));
		}

		else
		{
			edge.z = scale.z * sqrt(1.0 - (edge.y * edge.y) / (scale.y * scale.y));
			limit.z = edge.z * edge.z / sqrt(edge.x * edge.x + edge.z * edge.z);
			area = Pt3D(edge.x, edge.y, 0.0);
		}
	}
}



MeshBuilder::PartSurface::PartSurface(const double _density):
	Iterator(_density), cuboid(_density), cylinder(_density), ellipsoid(_density), iterator(NULL)
{}

void MeshBuilder::PartSurface::initialize(const Part *part)
{
	switch (part->shape)
	{
		case Part::SHAPE_ELLIPSOID:
			ellipsoid.setDensity(density);
			ellipsoid.initialize(part);
			iterator = &ellipsoid;
			break;
			
		case Part::SHAPE_CUBOID:
			cuboid.setDensity(density);
			cuboid.initialize(part);
			iterator = &cuboid;
			break;
			
		case Part::SHAPE_CYLINDER:
			cylinder.setDensity(density);
			cylinder.initialize(part);
			iterator = &cylinder;
			break;

		default:
			logPrintf("MeshBuilder::PartSurface", "initialize", LOG_WARN, "Part shape=%d not supported, skipping...", part->shape);
	}
}

bool MeshBuilder::PartSurface::tryGetNext(Pt3D &point)
{
	if (iterator == NULL)
	{
		return false;
	}

	return iterator->tryGetNext(point);
}



MeshBuilder::ModelSurface::ModelSurface(const double _density):
	Iterator(_density), surface(_density), model(NULL), index(0)
{}

void MeshBuilder::ModelSurface::initialize(const Model *model)
{
	this->model = model;
	surface.setDensity(density);
	
	index = 0;
	if (model->getPartCount() > index)
	{
		surface.initialize(model->getPart(index));
	}
}

bool MeshBuilder::ModelSurface::tryGetNext(Pt3D &point)
{
	if (model == NULL)
	{
		return false;
	}
	
	do
	{
		while (!surface.tryGetNext(point))
		{
			index += 1;
			if (model->getPartCount() > index)
			{
				surface.initialize(model->getPart(index));
			}
			else
			{
				return false;
			}
		}
	} while (GeometryUtils::isPointInsideModelExcludingPart(point, model, index));
	
	return true;
}



MeshBuilder::PartApices::PartApices(const double _density):
	Iterator(_density), cuboid(), cylinder(_density), ellipsoid(_density), iterator(NULL)
{}

void MeshBuilder::PartApices::initialize(const Part *part)
{
	switch (part->shape)
	{
		case Part::SHAPE_ELLIPSOID:
			ellipsoid.setDensity(density);
			ellipsoid.initialize(part);
			iterator = &ellipsoid;
			break;
			
		case Part::SHAPE_CUBOID:
			cuboid.initialize(part);
			iterator = &cuboid;
			break;
			
		case Part::SHAPE_CYLINDER:
			cylinder.setDensity(density);
			cylinder.initialize(part);
			iterator = &cylinder;
			break;

		default:
			logPrintf("MeshBuilder::PartApices", "initialize", LOG_WARN, "Part shape=%d not supported, skipping...", part->shape);
	}
}

bool MeshBuilder::PartApices::tryGetNext(Pt3D &point)
{
	if (iterator == NULL)
	{
		return false;
	}

	return iterator->tryGetNext(point);
}



MeshBuilder::ModelApices::ModelApices(const double _density):
	Iterator(_density), surface(_density), model(NULL), index(0)
{}

void MeshBuilder::ModelApices::initialize(const Model *model)
{
	this->model = model;
	surface.setDensity(density);
	
	index = 0;
	if (model->getPartCount() > index)
	{
		surface.initialize(model->getPart(index));
	}
}

bool MeshBuilder::ModelApices::tryGetNext(Pt3D &point)
{
	if (model == NULL)
	{
		return false;
	}
	
	do
	{
		while (!surface.tryGetNext(point))
		{
			index += 1;
			if (model->getPartCount() > index)
			{
				surface.initialize(model->getPart(index));
			}
			else
			{
				return false;
			}
		}
	} while (GeometryUtils::isPointInsideModelExcludingPart(point, model, index));
	
	return true;
}



MeshBuilder::BoundingBoxVolume::BoundingBoxVolume(const double _density):
	Iterator(_density), edge(_density), area(_density), volume(_density), length(0.0), width(0.0), height(0.0)
{}

void MeshBuilder::BoundingBoxVolume::initialize(const Model &model)
{
	Pt3D lowerBoundary, upperBoundary;
	ModelGeometryInfo::boundingBox(model, lowerBoundary, upperBoundary);
	initialize(lowerBoundary, upperBoundary);
}

void MeshBuilder::BoundingBoxVolume::initialize(const Pt3D &lowerBoundary, const Pt3D &upperBoundary)
{
	length = Pt3D(upperBoundary.x - lowerBoundary.x, 0.0, 0.0);
	width = Pt3D(0.0, upperBoundary.y - lowerBoundary.y, 0.0);
	height = Pt3D(0.0, 0.0, upperBoundary.z - lowerBoundary.z);

	edge.setDensity(density);
	edge.initialize(lowerBoundary, lowerBoundary + length);
        
	Pt3D dummy_point;
	edge.tryGetNext(dummy_point); //increment iterator to start from the second point

	area.setDensity(density);
	area.initialize(lowerBoundary, lowerBoundary + width);
	
	volume = Segment(density);
}

bool MeshBuilder::BoundingBoxVolume::tryGetNext(Pt3D &point)
{
	while (!volume.tryGetNext(point))
	{
		while (!area.tryGetNext(point))
		{
			if (edge.tryGetNext(point))
			{
				area.initialize(point, point + width);
			}
			else
			{
				return false;
			}
		}
		
		volume.initialize(point, point + height);
	}
	
	return true;
}
