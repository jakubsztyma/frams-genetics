// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _GEOMETRYUTILS_H_
#define _GEOMETRYUTILS_H_

#include <frams/model/model.h>
#include <frams/model/modelparts.h>
#include <frams/util/3d.h>
#include <frams/util/list.h>


/*Binary literals like 0b010 are standardized only in C++14. We use macros as they are compatible with older compilers too.

3-bit numbers are used when iterating through octants in a 3D space. Example: when creating points that cover the surface
of an ellipsoid, the points are only created for the positive octant (x, y, and z coordinates are positive). Points in
the remaining 7 octants are created by reflecting points from the positive octant through the appropriate planes defined by
pairs of axes.

2-bit numbers are used for 2D. Example: cylinders are aligned along the x axis so that both bases are parallel to
the yz plane. When points are created along the edge of the base (these will be used later to create points along the side
of the cylinder), only y and z axes are important, so quadrants of the 2D are sufficient. Just as in the 3D example above,
only points for the positive quadrant, QuadrantYZ, are created, and points of the remaining quadrants are created by reflection.
QuadrantXY and QuadrantZX enumerations are never used and are provided only for completeness.
*/
#define b000 0
#define b01 1
#define b001 1
#define b10 2
#define b010 2
#define b100 4
#define b110 6



namespace CuboidFaces
{
	enum Face
	{
		NEGATIVE_X = 0,
		POSITIVE_X = 1,
		NEGATIVE_Y = 2,
		POSITIVE_Y = 3,
		NEGATIVE_Z = 4,
		POSITIVE_Z = 5,
		FIRST = 0,
		NUMBER = 6
	};
	
	inline bool isPositive(Face f) { return f & b001; }
	inline bool isNegative(Face f) { return !isPositive(f); }
	inline bool isX(Face f) { return (f & b110) == b000; }
	inline bool isY(Face f) { return (f & b110) == b010; }
	inline bool isZ(Face f) { return (f & b110) == b100; }
}

namespace CylinderBases
{
	enum Base
	{
		NEGATIVE_X = 0,
		POSITIVE_X = 1,
		FIRST = 0,
		NUMBER = 2
	};
	
	inline bool isPositive(Base b) { return b & b001; }
	inline bool isNegative(Base b) { return !isPositive(b); }
}

namespace QuadrantsXY
{
	enum QuadrantXY
	{
		NEGATIVE_X_NEGATIVE_Y = 0,
		NEGATIVE_X_POSITIVE_Y = 1,
		POSITIVE_X_NEGATIVE_Y = 2,
		POSITIVE_X_POSITIVE_Y = 3,
		FIRST = 0,
		NUMBER = 4
	};
	
	inline bool isPositiveX(QuadrantXY q) { return (q & b10) != 0; }
	inline bool isNegativeX(QuadrantXY q) { return !isPositiveX(q); }
	inline bool isPositiveY(QuadrantXY q) { return q & b01; }
	inline bool isNegativeY(QuadrantXY q) { return !isPositiveY(q); }
}

namespace QuadrantsYZ
{
	enum QuadrantYZ
	{
		NEGATIVE_Y_NEGATIVE_Z = 0,
		NEGATIVE_Y_POSITIVE_Z = 1,
		POSITIVE_Y_NEGATIVE_Z = 2,
		POSITIVE_Y_POSITIVE_Z = 3,
		FIRST = 0,
		NUMBER = 4
	};
	
	inline bool isPositiveY(QuadrantYZ q) { return (q & b10) != 0; }
	inline bool isNegativeY(QuadrantYZ q) { return !isPositiveY(q); }
	inline bool isPositiveZ(QuadrantYZ q) { return q & b01; }
	inline bool isNegativeZ(QuadrantYZ q) { return !isPositiveZ(q); }
}

namespace QuadrantsZX
{
	enum QuadrantZX
	{
		NEGATIVE_Z_NEGATIVE_X = 0,
		NEGATIVE_Z_POSITIVE_X = 1,
		POSITIVE_Z_NEGATIVE_X = 2,
		POSITIVE_Z_POSITIVE_X = 3,
		FIRST = 0,
		NUMBER = 4
	};
	
	inline bool isPositiveZ(QuadrantZX q) { return (q & b10) != 0; }
	inline bool isNegativeZ(QuadrantZX q) { return !isPositiveZ(q); }
	inline bool isPositiveX(QuadrantZX q) { return (q & b01) != 0; }
	inline bool isNegativeX(QuadrantZX q) { return !isPositiveX(q); }
}

namespace Octants
{
	enum Octant
	{
		NEGATIVE_X_NEGATIVE_Y_NEGATIVE_Z = 0,
		NEGATIVE_X_NEGATIVE_Y_POSITIVE_Z = 1,
		NEGATIVE_X_POSITIVE_Y_NEGATIVE_Z = 2,
		NEGATIVE_X_POSITIVE_Y_POSITIVE_Z = 3,
		POSITIVE_X_NEGATIVE_Y_NEGATIVE_Z = 4,
		POSITIVE_X_NEGATIVE_Y_POSITIVE_Z = 5,
		POSITIVE_X_POSITIVE_Y_NEGATIVE_Z = 6,
		POSITIVE_X_POSITIVE_Y_POSITIVE_Z = 7,
		FIRST = 0,
		NUMBER = 8
	};
	
	inline bool isPositiveX(Octant o) { return (o & b100) != 0; }
	inline bool isNegativeX(Octant o) { return !isPositiveX(o); }
	inline bool isPositiveY(Octant o) { return (o & b010) != 0; }
	inline bool isNegativeY(Octant o) { return !isPositiveY(o); }
	inline bool isPositiveZ(Octant o) { return o & b001; }
	inline bool isNegativeZ(Octant o) { return !isPositiveZ(o); }
}

namespace GeometryUtils
{
	double pointPosition(const int pointIndex, const int numberOfPoints);
	double pointOnAxis(const double scale, const double position);
	double pointOnAxis(const double scale, const int pointIndex, const int numberOfPoints);
	double combination(const double value1, const double value2, const double position);
	double combination(const double value1, const double value2, const int pointIndex, const int numberOfPoints);
	bool isPointInsideModelExcludingPart(const Pt3D &point, const Model *model, const int excludedPartIndex);
	bool isPointInsideModel(const Pt3D &point, const Model &model);
	bool isPointInsidePart(const Pt3D &point, const Part *part);
	bool isPointStrictlyInsidePart(const Pt3D &point, const Part *part);
	bool isPointInsideEllipsoid(const Pt3D &point, const Part *part);
	bool isPointStrictlyInsideEllipsoid(const Pt3D &point, const Part *part);
	bool isPointInsideCuboid(const Pt3D &point, const Part *part);
	bool isPointStrictlyInsideCuboid(const Pt3D &point, const Part *part);
	bool isPointInsideCylinder(const Pt3D &point, const Part *part);
	bool isPointStrictlyInsideCylinder(const Pt3D &point, const Part *part);
	void findSizesAndAxesOfPointsGroup(SListTempl<Pt3D> &points, Pt3D &sizes, Orient &axes);
	void findSizeAndAxisOfPointsGroup(const SListTempl<Pt3D> &points, double &size, Pt3D &axis);
	double findTwoFurthestPoints(const SListTempl<Pt3D> &points, int &index1, int &index2);
	void createAxisFromTwoPoints(Pt3D &axis, const Pt3D &point1, const Pt3D &point2);
	void orthographicProjectionToPlane(SListTempl<Pt3D> &points, const Pt3D &planeNormalVector);
	double pointDistanceToPlane(const Pt3D &point, const Pt3D &planeNormalVector);
	void getRectangleApicesFromCuboid(const Part *part, const CuboidFaces::Face face, Pt3D &apex1, Pt3D &apex2, Pt3D &apex3, Pt3D &apex4);
	void getRectangleApices(const double width, const double height, const Pt3D &position, const Orient &orient, Pt3D &apex1, Pt3D &apex2, Pt3D &apex3, Pt3D &apex4);
	void getNextEllipseSegmentationPoint(const double d, const double a, const double b, double &x, double &y);
	double ellipsoidArea(const Pt3D &sizes);
	double ellipsoidArea(const double a, const double b, const double c);
	double ellipsePerimeter(const double a, const double b);
	double calculateSolidVolume(Part *part);
	bool isSolidPartScaleValid(Part::Shape &partShape, Pt3D &scale);
}

#endif
