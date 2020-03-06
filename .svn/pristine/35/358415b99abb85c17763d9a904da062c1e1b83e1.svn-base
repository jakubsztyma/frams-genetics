// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "f9_conv.h"
#include <frams/model/model.h>
#include <string.h>
#include <common/nonstd_stl.h> //ARRAY_LENGTH

#define APPLY_DETERMINISTIC_BODY_NOISE //this genetic representation easily produces perfectly vertical sticks that would stay upright forever in simulation. In most cases such infinite perfection is not desired, so we make the construct less perfect by perturbing its coordinates.

GenoConv_f90::GenoConv_f90()
{
	name = "Turtle3D-ortho encoding";
	in_format = '9';
	out_format = '0';
	mapsupport = 1;
}


const char* turtle_commands_f9 = "LRBFDU";

//const char* turtle_commandsX_f9="-+0000";
//const char* turtle_commandsY_f9="00-+00";
//const char* turtle_commandsZ_f9="0000-+";

SString GenoConv_f90::convert(SString &in, MultiMap *map, bool using_checkpoints)
{
	vector<XYZ_LOC> vertices;
	XYZ_LOC current;
	Model m;
	m.open(using_checkpoints);
	int recently_added = addSegment(m, 0, vertices, current, 0xDead);
	for (int i = 0; i < in.len(); i++)
	{
		char command = in[i];
		char *ptr = strchr((char*)turtle_commands_f9, command);
		if (ptr)
		{
			int delta[] = { 0, 0, 0 };
			int pos = ptr - turtle_commands_f9;
			int axis = pos / 2;
			int dir = pos % 2;
			(*(delta + axis)) += dir * 2 - 1; //+1 or -1 in the given axis
			current.add(delta);
			recently_added = addSegment(m, i, vertices, current, recently_added);
			m.checkpoint();
		}
	}
#ifdef APPLY_DETERMINISTIC_BODY_NOISE
	perturbPartLocations(m);
#endif
	setColors(m, recently_added);
	m.close();
	if (m.getPartCount() < 2) //only one part <=> there were no valid turtle commands in the input genotype
		return ""; //so we return an invalid f0 genotype
	if (map != NULL)
		m.getCurrentToF0Map(*map);
	return m.getF0Geno().getGenes();
}

int GenoConv_f90::addSegment(Model &m, int genenr, vector<XYZ_LOC> &vertices, const XYZ_LOC &new_vertex, int recently_added)
{
	if (vertices.size() < 1) //empty model?
	{
		return addNewVertex(m, vertices, new_vertex);
	}
	else
	{
		int vertex_here = findVertexAt(vertices, new_vertex);
		if (vertex_here < 0) //need to create a new Part
		{
			vertex_here = addNewVertex(m, vertices, new_vertex);
		} //else there already exists a Part in new_vertex; new Joint may or may not be needed
		Part *p1 = m.getPart(recently_added);
		Part *p2 = m.getPart(vertex_here);
		p1->addMapping(MultiRange(genenr, genenr));
		p2->addMapping(MultiRange(genenr, genenr));

		int j12 = m.findJoint(p1, p2);
		int j21 = m.findJoint(p2, p1);
		if (j12 >= 0)
			m.getJoint(j12)->addMapping(MultiRange(genenr, genenr));
		else if (j21 >= 0)
			m.getJoint(j21)->addMapping(MultiRange(genenr, genenr));
		else //both j12<0 and j21<0. New Joint needed. Should always happen if we just created a new Part (vertex_here was <0)
			m.addNewJoint(p1, p2)->addMapping(MultiRange(genenr, genenr));
		return vertex_here;
	}
}

int GenoConv_f90::findVertexAt(vector<XYZ_LOC> &vertices, const XYZ_LOC &vertex)
{
	for (size_t i = 0; i < vertices.size(); i++)
		if (vertices[i].same_coordinates(vertex)) return i;
	return -1;
}


int GenoConv_f90::addNewVertex(Model &m, vector<XYZ_LOC> &vertices, const XYZ_LOC &new_vertex)
{
	Part *p = new Part;
	p->p.x = new_vertex.x;
	p->p.y = new_vertex.y;
	p->p.z = new_vertex.z;
	m.addPart(p);

	vertices.push_back(new_vertex);
	return vertices.size() - 1;
}

double mix(int *colortab, int maxind, double ind)
{
	int indpre = (int)ind;
	int indpost = indpre + 1;
	if (indpost > maxind) indpost = maxind;
	int v1 = colortab[indpre];
	int v2 = colortab[indpost];
	double d1 = ind - indpre;
	double d2 = indpost - ind;
	double v = indpre == indpost ? v1 : d2*v1 + d1*v2; //d1+d2==1
	return v;
}

void GenoConv_f90::setColors(Model &m, int last_added_part) //sets fixed (independent from genes) colors and widths on a model, purely for aesthetic purposes
{
	//a rainbow on Joints: from the first one red, through middle green, to blue or violet - last
	static int r[] = { 1, 1, 0, 0, 0, 1 };
	static int g[] = { 0, 1, 1, 1, 0, 0 };
	static int b[] = { 0, 0, 0, 1, 1, 1 };
	int maxind = ARRAY_LENGTH(r) - 1;

	int joints_count = m.getJointCount();
	for (int i = 0; i < joints_count; i++)
	{
		Joint *j = m.getJoint(i);
		double x = joints_count < 2 ? 0 : (double)i / (joints_count - 1); //0..1, postion in the rainbow
		double ind = x*maxind;
		j->vcolor.x = mix(r, maxind, ind);
		j->vcolor.y = mix(g, maxind, ind);
		j->vcolor.z = mix(b, maxind, ind);
	}

	int parts_count = m.getPartCount();
	SList jlist;
	for (int i = 0; i < parts_count; i++)
	{
		Part *p = m.getPart(i);
		jlist.clear();
		int count = m.findJoints(jlist, p);
		Pt3D averagecolor(0, 0, 0); //Parts will get averaged colors from all attached Joints
		FOREACH(Joint*, j, jlist)
			averagecolor += j->vcolor;
		p->vcolor = averagecolor / count;
		if (count>5) count = 5; //avoid too fat...
		p->vsize = 0.3 + count / 15.0; //the more Joints is attached to a Part, the fatter it is
	}
	//m.getPart(0)->vcolor = Pt3D(0, 0, 0); //mark first Part black - a visual aid for easier editing
	m.getPart(last_added_part)->vcolor = Pt3D(1, 1, 1); //mark last Part white - a visual aid for easier editing
}

void GenoConv_f90::perturbPartLocations(Model &m) //deterministic "body noise", see APPLY_DETERMINISTIC_BODY_NOISE
{
	for (int i = 0; i < m.getPartCount(); i++)
	{
		Part *p = m.getPart(i);
		Pt3D noise(
			((i + 1) % 10) - 4.5,
			((3 * i + 5) % 10) - 4.5,
			((7 * i + 2) % 10) - 4.5
			); //-4.5 .. 4.5 in each axis
		p->p += noise / 1000;
	}
}
