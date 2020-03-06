// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2017  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FF_CHAMBER3D_H_
#define	_FF_CHAMBER3D_H_

struct fF_point;

//Chamber parameters; see http://www.framsticks.com/foraminifera
class fF_chamber3d
{
public:
	double centerX;
	double centerY;
	double centerZ;
	double radius_x;
	double radius_y;
	double radius_z;
	double holeX;
	double holeY;
	double holeZ;
	double vectorTfX;
	double vectorTfY;
	double vectorTfZ;
	double beta;
	double phi;

	fF_point *points;

	fF_chamber3d(double centerX, double centerY, double centerZ,
		double radius_x, double radius_y, double radius_z, double holeX, double holeY, double holeZ,
		double vectorTfX, double vectorTfY, double vectorTfZ, double beta, double phi);
	~fF_chamber3d();
};

#endif	/* CHAMBER3D_H */
