//
// Created by jakub on 07.07.2020.
//

#ifndef CPP_DISTANCE_H
#define CPP_DISTANCE_H

/**
 * Used in finding the proper distance between the parts
 * distance between spheres / sphere radius
 * That default value can be changed in certain cases
 * */
const float SPHERE_RELATIVE_DISTANCE = 0.5;
/**
 * Used in finding the proper distance between the parts
 * The maximal allowed value for
 * maximal radius of the node / sphere radius
 */
const int MAX_DIAMETER_QUOTIENT = 30;
/**
 * The tolerance of the value of distance between parts
 */
const double SPHERE_DISTANCE_TOLERANCE = 0.96;

const double SPHERE_SIZE_QUOTIENT = 0.25;

void rotateVector(Pt3D &vector, const Pt3D &rotation)
{
	Orient rotmatrix = Orient_1;
	rotmatrix.rotate(rotation);
	vector = rotmatrix.transform(vector);
}

double avg(double a, double b)
{
	return 0.5 * (a + b);
}

double min3(Pt3D p)
{
	double tmp = p.x;
	if (p.y < tmp)
		tmp = p.y;
	if (p.z < tmp)
		tmp = p.z;
	return tmp;
}

double max3(Pt3D p)
{
	double tmp = p.x;
	if (p.y > tmp)
		tmp = p.y;
	if (p.z > tmp)
		tmp = p.z;
	return tmp;
}

bool isInsidePart(Part::Shape shape, Pt3D partRadii, Pt3D center, double sphereRadius)
{
	if(sphereRadius >= min3(partRadii))
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

vector<Pt3D> findSphereCenters(Part::Shape shape, double &sphereRadius, Pt3D radii, Pt3D rotations)
{
	double sphereRelativeDistance = SPHERE_RELATIVE_DISTANCE;
	double minRadius = min3(radii);
	if(minRadius <= 0)
		throw fS_Exception("Invalid part size", 0);
	double maxRadius = max3(radii);
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
						rotateVector(center, rotations);
						centers.push_back(center);
					}
			}
		}
	}
	return centers;
}

int isCollision(vector<Pt3D> centersParent, vector<Pt3D> centers, Pt3D &vector,
				double distanceThreshold)
{
	double upperThreshold = distanceThreshold * distanceThreshold;
	double lowerThreshold = pow(SPHERE_DISTANCE_TOLERANCE * distanceThreshold, 2);
	double distance;
	double dx, dy, dz;
	bool existsAdjacent = false;
	Pt3D *tmpPoint;
	for (int sc = 0; sc < int(centers.size()); sc++)
	{
		Pt3D shiftedSphere = Pt3D(centers[sc]);
		shiftedSphere += vector;
		for (int psc = 0; psc < int(centersParent.size()); psc++)
		{
			tmpPoint = &centersParent[psc];
			dx = shiftedSphere.x - tmpPoint->x;
			dy = shiftedSphere.y - tmpPoint->y;
			dz = shiftedSphere.z - tmpPoint->z;
			distance = dx * dx + dy * dy + dz * dz;

			if (distance <= upperThreshold)
			{
				if (distance >= lowerThreshold)
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

double Node::getDistance()
{
	Pt3D size = calculateSize();
	Pt3D parentSize = parent->calculateSize();	// Here we are sure that parent is not nullptr
	double parentSphereRadius, sphereRadius;
	vector<Pt3D> centersParent = findSphereCenters(parent->partType, parentSphereRadius, parentSize, parent->getRotation());
	vector<Pt3D> centers = findSphereCenters(partType, sphereRadius, size, getRotation());

	double distanceThreshold = sphereRadius + parentSphereRadius;
	double minDistance = 0.0;
	double maxDistance = 2 * (max3(parentSize) + max3(size));
	double currentDistance = avg(maxDistance, minDistance);
	int result = -1;
	int iterationNo = 0;
	while (result != ADJACENT)
	{
		iterationNo++;
		Pt3D currentVector = state->v * currentDistance;
		result = isCollision(centersParent, centers, currentVector, distanceThreshold);

		if (result == DISJOINT)
		{
			maxDistance = currentDistance;
			currentDistance = avg(currentDistance, minDistance);
		} else if (result == COLLISION)
		{
			minDistance = currentDistance;
			currentDistance = avg(maxDistance, currentDistance);
		}

		if(maxDistance <= 0 || iterationNo > 1000)
			throw fS_Exception("Computing of distances between parts failed", 0);
		if (currentDistance > maxDistance)
		{
			throw fS_Exception("Internal error; then maximal distance between parts exceeded.", 0);
		}
		if (currentDistance < minDistance)
			throw fS_Exception("Internal error; the minimal distance between parts exceeded.", 0);

	}

	return currentDistance;
}

#endif //CPP_DISTANCE_H
