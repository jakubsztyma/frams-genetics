// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _MODELGEOCLASS_H_
#define _MODELGEOCLASS_H_

#include <frams/model/modelobj.h>

class ModelGeometry : public DestrBase
{
public:
	ModelObj *model;
	double density;

	//"cached" fields let avoid redundant computations when asking for the same properties of the same model at the same density
	double cached_for_density;
	double cached_volume, cached_area;
	Pt3D cached_sizes; Orient cached_axes;

	Param par;

	ModelGeometry(ModelObj *mo = NULL);
	~ModelGeometry();

	void invalidateAllCached();
	void onDensityChanged();

#define STATRICKCLASS ModelGeometry
	PARAMPROCDEF(p_formodel);
	PARAMPROCDEF(p_volume);
	PARAMPROCDEF(p_area);
	PARAMPROCDEF(p_sizesandaxes);
#undef STATRICKCLASS

	static ExtObject makeDynamicObject(ModelGeometry* mg);
};

#endif
