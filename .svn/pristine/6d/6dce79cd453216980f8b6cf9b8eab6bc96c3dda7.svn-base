// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _F9_CONV_H_
#define _F9_CONV_H_

#include <ctype.h>
#include <common/nonstd_math.h>
#include <frams/model/modelparts.h>
#include <frams/util/multimap.h>
#include <frams/util/sstring.h>
#include <frams/genetics/genoconv.h>
#include <vector>
using std::vector;


extern const char* turtle_commands_f9;


struct XYZ_LOC
{
	int x, y, z; //coordinates xyz of a vertex - represented as int's so that it is easy and safe to check identity. Could also be done using lists of Model's Parts, but that would involve comparing floats
	XYZ_LOC() { x = y = z = 0; }
	void add(int delta[3]) { x += delta[0]; y += delta[1]; z += delta[2]; }
	bool same_coordinates(const XYZ_LOC &loc) { return x == loc.x && y == loc.y && z == loc.z; }
};


// The f9->f0 converter
class GenoConv_f90 : public GenoConverter
{
public:
	GenoConv_f90();

	//implementation of the GenoConverter method
	SString convert(SString &in, MultiMap *map, bool using_checkpoints);

protected:
	//auxiliary methods
	int addSegment(Model &m, int genenr, vector<XYZ_LOC> &vertices, const XYZ_LOC &new_vertex, int recently_added);
	int findVertexAt(vector<XYZ_LOC> &vertices, const XYZ_LOC &new_vertex);
	int addNewVertex(Model &m, vector<XYZ_LOC> &punkty, const XYZ_LOC &nowypunkt);
	void setColors(Model &m, int last_added_part); //sets fixed (independent from genes) colors and widths on a model, purely for aesthetic purposes
	void perturbPartLocations(Model &m); //deterministic "body noise", see APPLY_DETERMINISTIC_BODY_NOISE
};

#endif
