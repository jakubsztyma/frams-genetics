// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 2019-2020  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _PART_DISTANCE_ESTIMATOR_H
#define _PART_DISTANCE_ESTIMATOR_H

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

	static double min3(Pt3D p)
	{
		double tmp = p.x;
		if (p.y < tmp)
			tmp = p.y;
		if (p.z < tmp)
			tmp = p.z;
		return tmp;
	}

	static double max3(Pt3D p)
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
	/**
	 * Used in finding the proper distance between the parts
	 * distance between spheres / sphere radius
	 * That default value can be changed in certain cases
	 * */
	static constexpr float SPHERE_RELATIVE_DISTANCE = 0.5;
	/**
	 * Used in finding the proper distance between the parts
	 * The maximal allowed value for
	 * maximal radius of the node / sphere radius
	 */
	static const int MAX_DIAMETER_QUOTIENT = 30;
	/**
	 * The tolerance of the value of distance between parts
	 */
	static constexpr double SPHERE_DISTANCE_TOLERANCE = 0.96;

	static constexpr double SPHERE_SIZE_QUOTIENT = 0.25;


	static bool isInsidePart(Part::Shape shape, Pt3D partRadii, Pt3D center, double sphereRadius)
	{
		if(sphereRadius >= fS_Utils::min3(partRadii))
			return true; // TODO Special case
		Pt3D mostRemote = Pt3D(fabs(center.x), fabs(center.y), fabs(center.z)) + Pt3D(sphereRadius);
		bool isInEllipsis;

		bool result = false;

		switch (shape)
		{
			case Part::Shape::SHAPE_CUBOID:
				if(mostRemote.x <= mostRemote.y <= partRadii.y && mostRemote.z <= partRadii.z)
					result = true;
				break;
			case Part::Shape::SHAPE_CYLINDER:
				isInEllipsis = pow(center.y / (partRadii.y - sphereRadius), 2) + pow(center.z / (partRadii.z - sphereRadius), 2) <= 1.0;
				if (mostRemote.x <= partRadii.x && isInEllipsis)
					result = true;
				break;
			case Part::Shape::SHAPE_ELLIPSOID:
				if(pow(center.x / (partRadii.x - sphereRadius), 2) + pow(center.y / (partRadii.y - sphereRadius), 2) + pow(center.z / (partRadii.z - sphereRadius), 2) <= 1.0)
					result = true;
				break;
			default:
				logMessage("fS", "calculateVolume", LOG_ERROR, "Invalid part type");
		}
		return result;
	}

public:
	static vector<Pt3D> findSphereCenters(Part::Shape shape, double &sphereRadius, Pt3D radii, Pt3D rotations)
	{
		double sphereRelativeDistance = SPHERE_RELATIVE_DISTANCE;
		double minRadius = fS_Utils::min3(radii);
		if(minRadius <= 0)
			throw fS_Exception("Invalid part size in PartDistanceEstimator", 0);
		double maxRadius = fS_Utils::max3(radii);
		sphereRadius = SPHERE_SIZE_QUOTIENT * minRadius;
		if (MAX_DIAMETER_QUOTIENT < maxRadius / sphereRadius)
		{
			// When max radius is much bigger than sphereRadius and there are to many spheresZ
			sphereRelativeDistance = 1.0;   // Make the spheres adjacent to speed up the computation
			sphereRadius = maxRadius / MAX_DIAMETER_QUOTIENT;
		}
		else if(shape == Part::Shape::SHAPE_ELLIPSOID)
			sphereRadius = minRadius;

		double sphereDiameter = 2 * sphereRadius;

		double radiiArr[3]{radii.x, radii.y, radii.z};
		vector<double> coordinates[3];
		for (int dim = 0; dim < 3; dim++)
		{
			double spaceForSphereCenters = 2 * radiiArr[dim] - sphereDiameter;
			if (spaceForSphereCenters > 0)
			{
				int lastIndex = ceil(spaceForSphereCenters / (sphereDiameter * sphereRelativeDistance));
				for (int i = 0; i <= lastIndex; i++)
				{
					coordinates[dim].push_back(spaceForSphereCenters * (double(i) / lastIndex - 0.5));
				}
			}
			else
				coordinates[dim].push_back(0.0);
		}

		vector<Pt3D> centers;
		for (int xi = 0; xi < int(coordinates[0].size()); xi++)
		{
			for (int yi = 0; yi < int(coordinates[1].size()); yi++)
			{
				for (int zi = 0; zi < int(coordinates[2].size()); zi++)
				{
						Pt3D center = Pt3D(coordinates[0][xi], coordinates[1][yi], coordinates[2][zi]);
						if (isInsidePart(shape, radii, center, sphereRadius))
						{
							fS_Utils::rotateVector(center, rotations);
							centers.push_back(center);
						}
				}
			}
		}
		return centers;
	}

	static int isCollision(vector<Pt3D> &centersParent, vector<Pt3D> &centers, Pt3D &vectorBetweenParts,
					double distanceThreshold)
	{
		double upperThresholdSq = distanceThreshold * distanceThreshold;
		double lowerThresholdSq = pow(SPHERE_DISTANCE_TOLERANCE * distanceThreshold, 2);
		double distanceSq;
		double dx, dy, dz;
		bool existsAdjacent = false;
		Pt3D *tmpPoint;
		for (int sc = 0; sc < int(centers.size()); sc++)
		{
			Pt3D shiftedSphere = Pt3D(centers[sc]);
			shiftedSphere += vectorBetweenParts;
			for (int psc = 0; psc < int(centersParent.size()); psc++)
			{
				tmpPoint = &centersParent[psc];
				dx = shiftedSphere.x - tmpPoint->x;
				dy = shiftedSphere.y - tmpPoint->y;
				dz = shiftedSphere.z - tmpPoint->z;
				distanceSq = dx * dx + dy * dy + dz * dz;

				if (distanceSq <= upperThresholdSq)
				{
					if (distanceSq >= lowerThresholdSq)
						existsAdjacent = true;
					else
					{
						return COLLISION;
					}
				}
			}
		}
		if (existsAdjacent)
			return ADJACENT;
		else
			return DISJOINT;
	}
};

double Node::getDistance()
{
	Pt3D size = calculateSize();
	Pt3D parentSize = parent->calculateSize();	// Here we are sure that parent is not nullptr
	double parentSphereRadius, sphereRadius;
	vector<Pt3D> centersParent = PartDistanceEstimator::findSphereCenters(parent->partType, parentSphereRadius, parentSize, parent->getRotation());
	vector<Pt3D> centers = PartDistanceEstimator::findSphereCenters(partType, sphereRadius, size, getRotation());

	double distanceThreshold = sphereRadius + parentSphereRadius;
	double minDistance = 0.0;
	double maxDistance = 2 * (fS_Utils::max3(parentSize) + fS_Utils::max3(size));
	double currentDistance = fS_Utils::avg(maxDistance, minDistance);
	int result = -1;
	int iterationNo = 0;
	while (result != ADJACENT)
	{
		iterationNo++;
		Pt3D vectorBetweenParts = state->v * currentDistance;
		result = PartDistanceEstimator::isCollision(centersParent, centers, vectorBetweenParts, distanceThreshold);

		if (result == DISJOINT)
		{
			maxDistance = currentDistance;
			currentDistance = fS_Utils::avg(currentDistance, minDistance);
		} else if (result == COLLISION)
		{
			minDistance = currentDistance;
			currentDistance = fS_Utils::avg(maxDistance, currentDistance);
		}

		if(maxDistance <= 0 || iterationNo > 1000)
			throw fS_Exception("Computing of distances between parts failed", 0);
		if (currentDistance > maxDistance)
		{
			throw fS_Exception("Internal error; the maximal distance between parts exceeded.", 0);
		}
		if (currentDistance < minDistance)
			throw fS_Exception("Internal error; the minimal distance between parts exceeded.", 0);

	}

	return currentDistance;
}

#endif //_PART_DISTANCE_ESTIMATOR_H
