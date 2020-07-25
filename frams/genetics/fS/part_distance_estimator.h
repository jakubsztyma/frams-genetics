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
		printf("buildTemporaryPart %d\n", shape);
		Part *tmpPart = new Part(shape);
		tmpPart->scale = scale;
		tmpPart->setRot(rotations);
		printf("buildTemporaryPart2 %d\n", tmpPart->shape);
		return tmpPart;
	}

	/// Get some of the points from the surface of the part
	static vector <Pt3D> findSurfacePoints(Part *part, double  relativeDensity)
	{
		printf("findSurfacePoints %d\n", part->shape);
//		// Divide by maximal radius to avoid long computations
//		MeshBuilder::PartSurface surface(relativeDensity / fS_Utils::max3(part->scale));
//		surface.initialize(part);
//
		vector <Pt3D> centers;
//		Pt3D point;
//		while (surface.tryGetNext(point))
//		{
//			centers.push_back(point);
//		}
		centers.push_back(Pt3D(0));
		printf("findSurfacePoints2 %c\n", part->shape);
		return centers;
	}

	/// Check if there is a collision between the parts
	static bool isCollision(Part *parentPart, vector <Pt3D> &centers, Pt3D &vectorBetweenParts)
	{
		printf("isCollision %d\n", parentPart->shape);
		static double CBRT_3 = std::cbrt(3);
		double maxParentReachSq = pow(CBRT_3 * fS_Utils::max3(parentPart->scale), 2);
		for (int i = 0; i < int(centers.size()); i++)
		{
			Pt3D shifted = centers[i] + vectorBetweenParts;
			double distanceToCenterSq = shifted.x * shifted.x + shifted.y * shifted.y + shifted.z * shifted.z;
			if (distanceToCenterSq <= maxParentReachSq)
			{
				printf("isCollision2 %c\n", parentPart->shape);
				return true;
			}
		}
		printf("isCollision2 %d\n", parentPart->shape);
		return false;
	}


	static double calculateDistance(Part *tmpPart, Part *parentTmpPart, Pt3D &directionVersor, double distanceTolerance, double relativeDensity)
	{
		printf("calculateDistance %d %d\n", tmpPart->shape, parentTmpPart->shape);
		static double CBRT_3 = std::cbrt(3);
		vector <Pt3D> centers = PartDistanceEstimator::findSurfacePoints(tmpPart, relativeDensity);

		double minDistance = fS_Utils::min3(parentTmpPart->scale) + fS_Utils::min3(tmpPart->scale);
		double maxDistance = CBRT_3 * (fS_Utils::max3(parentTmpPart->scale) + fS_Utils::max3(tmpPart->scale));
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
		printf("calculateDistance2\n");
		return currentDistance;
	}
};


#endif //_PART_DISTANCE_ESTIMATOR_H_
