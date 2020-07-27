// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 2019-2020  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _PART_DISTANCE_ESTIMATOR_H_
#define _PART_DISTANCE_ESTIMATOR_H_

#include "frams/model/geometry/meshbuilder.h"

class PartDistanceEstimator
{
public:

	static Part *buildTemporaryPart(Part::Shape shape, const Pt3D &scale, const Pt3D &rotation)
	{
		Part *tmpPart1 = new Part(shape);
		tmpPart1->scale = scale;
		tmpPart1->setRot(rotation);
		return tmpPart1;
	}

	/// Get some of the points from the surface of the part
	static vector <Pt3D> findSurfacePoints(Part *part, double  relativeDensity)
	{
		// Divide by maximal radius to avoid long computations
		MeshBuilder::PartSurface surface(relativeDensity / part->scale.maxComponent());
		surface.initialize(part);

		vector <Pt3D> points;
		Pt3D point;
		while (surface.tryGetNext(point))
		{
			points.push_back(point);
		}
		return points;
	}

	/// Check if there is a collision between the parts
	static bool isCollision(Part *part, vector <Pt3D> &points, Pt3D &vectorBetweenParts)
	{
		static double CBRT_3 = std::cbrt(3);
		double maxPartReachSq = pow(CBRT_3 * part->scale.maxComponent(), 2);
		for (int i = 0; i < int(points.size()); i++)
		{
			Pt3D shifted = points[i] + vectorBetweenParts;
			double distanceToPointSq = shifted.x * shifted.x + shifted.y * shifted.y + shifted.z * shifted.z;
			if (distanceToPointSq <= maxPartReachSq && GeometryUtils::isPointInsidePart(shifted, part))
				return true;
		}
		return false;
	}


	static double calculateDistance(Part tmpPart1, Part tmpPart2, double distanceTolerance, double relativeDensity)
	{
		/// tmpPart1 and tmpPart2 are copied for purpose and should not be passed as reference
		/// This function can change some of the properties of those parts
		Pt3D directionVersor = tmpPart1.p - tmpPart2.p;
		directionVersor.normalize();

		tmpPart1.p = Pt3D(0);
		tmpPart2.p = Pt3D(0);

		static double CBRT_3 = std::cbrt(3);
		vector <Pt3D> points = PartDistanceEstimator::findSurfacePoints(&tmpPart1, relativeDensity);

		double minDistance = tmpPart2.scale.minComponent() + tmpPart1.scale.minComponent();
		double maxDistance = CBRT_3 * (tmpPart2.scale.maxComponent() + tmpPart1.scale.maxComponent());
		double currentDistance = 0.5 * (maxDistance + minDistance);
		int collisionDetected = false;
		while (maxDistance - minDistance > distanceTolerance)
		{
			Pt3D vectorBetweenParts = directionVersor * currentDistance;
			collisionDetected = PartDistanceEstimator::isCollision(&tmpPart2, points, vectorBetweenParts);

			if (collisionDetected)
			{
				minDistance = currentDistance;
				currentDistance = 0.5 * (maxDistance + currentDistance);
			} else
			{
				maxDistance = currentDistance;
				currentDistance = 0.5 * (currentDistance + minDistance);
			}
		}
		return currentDistance;
	}
};


#endif //_PART_DISTANCE_ESTIMATOR_H_
