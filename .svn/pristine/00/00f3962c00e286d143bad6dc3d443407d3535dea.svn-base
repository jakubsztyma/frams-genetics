// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2017  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "fF_chamber3d.h"
#include "fF_conv.h"
#include <iostream>

fF_chamber3d::fF_chamber3d(double centerX, double centerY, double centerZ,
	double radius_x, double radius_y, double radius_z, double holeX, double holeY, double holeZ,
	double vectorTfX, double vectorTfY, double vectorTfZ, double beta, double phi)
{
	points = NULL;
	this->centerX = centerX;
	this->centerY = centerY;
	this->centerZ = centerZ;
	this->radius_x = radius_x;
	this->radius_y = radius_y;
	this->radius_z = radius_z;
	this->holeX = holeX;
	this->holeY = holeY;
	this->holeZ = holeZ;
	this->vectorTfX = vectorTfX;
	this->vectorTfY = vectorTfY;
	this->vectorTfZ = vectorTfZ;
	this->beta = beta;
	this->phi = phi;
}

fF_chamber3d::~fF_chamber3d()
{
	delete[] points;
}
