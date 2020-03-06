// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FF_CONV_H_
#define _FF_CONV_H_



#define fF_TOO_MUCH 0.75
#define fF_TOO_LITTLE 0.10

#define fF_HOLE_RADIUS 0.05
#define fF_LONGITUDE_NUM 69

#define fF_LATITUDE_NUM ((fF_LONGITUDE_NUM - 1)*2)
#define fF_AMOUNT ((fF_LATITUDE_NUM)*(fF_LONGITUDE_NUM))

#define fF_THICK_RATIO 0.95

#define fF_SIZE fF_LONGITUDE_NUM * fF_LATITUDE_NUM + fF_LATITUDE_NUM

#include <frams/util/multimap.h>
#include <frams/util/sstring.h>
#include <frams/genetics/genoconv.h>
#include <frams/model/model.h>
#include "fF_chamber3d.h"


//A point on the surface of a chamber
struct fF_point
{
	double x, y, z;
	bool inside; //helper field used when computing whether this point is inside some chamber
};


// The f9->f0 converter
class GenoConv_fF0 : public GenoConverter {
public:
	GenoConv_fF0();
	~GenoConv_fF0();
	//implementation of the GenoConverter method
	SString convert(SString &in, MultiMap *map, bool using_checkpoints);

protected:
	void createSphere(int ktora, fF_chamber3d **chambers, double radius0x, double radius0y, double radius0z, double translation, double alpha, double gamma, double kx, double ky, double kz);
	fF_point* generate_points(fF_chamber3d *chamber);
	double dist(double x1, double y1, double z1, double x2, double y2, double z2);
	void search_hid(int nr, fF_chamber3d **spheres);
	int find_hole(int which, double x, double y, double z, fF_chamber3d **chambers);
	double get_radius(double prev_radius, double scale, double radius0);
private:
	double* cosines;
	double* sines;
	void precompute_cos_and_sin();
	Part *addNewPart(Model *m, const fF_chamber3d* c);
};


#endif
