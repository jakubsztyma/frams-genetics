// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _MATRIX_TOOLS_H_
#define	_MATRIX_TOOLS_H_

#include <vector>
#include "frams/util/3d.h"

class MatrixTools
{
public:
	static void weightedMDS(std::vector<double> &vdEigenvalues, int nSize, double *pDistances, Pt3D *&Coordinates, double *weights);
};


#endif
