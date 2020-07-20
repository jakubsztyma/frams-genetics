// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 2019-2020  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _PART_DISTANCE_ESTIMATOR_H_
#define _PART_DISTANCE_ESTIMATOR_H_

#include "frams/model/geometry/meshbuilder.h"

class fS_Utils
{
public:
	static void rotateVector(Pt3D &vector, const Pt3D &rotation)
	{
		Orient rotmatrix = Orient_1;
		rotmatrix.rotate(rotation);
		vector = rotmatrix.transform(vector);
	}

	static double avg(double a, double b)
	{
		return 0.5 * (a + b);
	}

	static double min3(const Pt3D &p)
	{
		double tmp = p.x;
		if (p.y < tmp)
			tmp = p.y;
		if (p.z < tmp)
			tmp = p.z;
		return tmp;
	}

	static double max3(const Pt3D &p)
	{
		double tmp = p.x;
		if (p.y > tmp)
			tmp = p.y;
		if (p.z > tmp)
			tmp = p.z;
		return tmp;
	}
};

class PartDistanceEstimator
{

public:

	static Part *buildTemporaryPart(Part::Shape shape, const Pt3D &scale, const Pt3D &rotations)
	{
		Part *tmpPart = new Part(shape);
		tmpPart->scale = scale;
		tmpPart->setRot(rotations);
		return tmpPart;
	}

	/// Get some of the points from the surface of the part
	static vector <Pt3D> findSurfacePoints(Part *part, double  relativeDensity)
	{
		// Divide by maximal radius to avoid long computations
		MeshBuilder::PartSurface surface(relativeDensity / fS_Utils::max3(part->scale));
		surface.initialize(part);

		vector <Pt3D> centers;
		Pt3D point;
		while (surface.tryGetNext(point))
		{
			centers.push_back(point);
		}
		return centers;
	}

	/// Check if there is a collision between the parts
	static bool isCollision(Part *parentPart, vector <Pt3D> &centers, Pt3D &vectorBetweenParts)
	{
		static double CBRT_3 = std::cbrt(3);
		double maxParentReachSq = pow(CBRT_3 * fS_Utils::max3(parentPart->scale), 2);
		for (int i = 0; i < int(centers.size()); i++)
		{
			Pt3D shifted = centers[i] + vectorBetweenParts;
			double distanceToCenterSq = shifted.x * shifted.x + shifted.y * shifted.y + shifted.z * shifted.z;
			if (distanceToCenterSq <= maxParentReachSq && GeometryUtils::isPointInsidePart(shifted, parentPart))
				return true;
		}
		return false;
	}


	static double calculateDistance(Part *tmpPart, Part *parentTmpPart, Pt3D &directionVersor, double distanceTolerance, double relativeDensity)
	{
		vector <Pt3D> centers = PartDistanceEstimator::findSurfacePoints(tmpPart, relativeDensity);

		double minDistance = 0.0;
		double maxDistance = 2 * (fS_Utils::max3(parentTmpPart->scale) + fS_Utils::max3(tmpPart->scale));
		double currentDistance = fS_Utils::avg(maxDistance, minDistance);
		int collisionDetected = false;
		while (maxDistance - minDistance > distanceTolerance)
		{
			Pt3D vectorBetweenParts = directionVersor * currentDistance;
			collisionDetected = PartDistanceEstimator::isCollision(parentTmpPart, centers, vectorBetweenParts);

			if (collisionDetected)
			{
				minDistance = currentDistance;
				currentDistance = fS_Utils::avg(maxDistance, currentDistance);
			} else
			{
				maxDistance = currentDistance;
				currentDistance = fS_Utils::avg(currentDistance, minDistance);
			}
		}
		return currentDistance;
	}
};


#endif //_PART_DISTANCE_ESTIMATOR_H_
