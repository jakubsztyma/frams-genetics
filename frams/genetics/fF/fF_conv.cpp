// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "fF_conv.h"
#include "fF_genotype.h"
#include <common/nonstd_stl.h>
#include <common/Convert.h>

GenoConv_fF0::GenoConv_fF0()
{
	name = "10-parameter Foraminifera encoding";
	in_format = 'F';
	out_format = '0';
	mapsupport = 0;
	cosines = new double[fF_LATITUDE_NUM];
	sines = new double[fF_LATITUDE_NUM];
	precompute_cos_and_sin();
}

GenoConv_fF0::~GenoConv_fF0()
{
	delete[] cosines;
	delete[] sines;
}

Part *GenoConv_fF0::addNewPart(Model *m, const fF_chamber3d* c)
{
	Part *part = m->addNewPart(Part::SHAPE_ELLIPSOID);
	part->p = Pt3D(c->centerX, c->centerY, c->centerZ);
	Pt3D hole = Pt3D(c->holeX, c->holeY, c->holeZ);
	Orient o;
	o.lookAt(part->p - hole);
	part->setOrient(o);
	return part;
}

SString GenoConv_fF0::convert(SString &in, MultiMap *map, bool using_checkpoints)
{
	fF_growth_params gp;
	if (!gp.load(in.c_str())) //invalid input genotype?
		return ""; //so we return an invalid f0 genotype

	Model m;
	m.open(using_checkpoints);

	m.vis_style = "foram"; //dedicated visual look for Foraminifera

	fF_chamber3d **chambers = new fF_chamber3d*[gp.number_of_chambers];
	for (int i = 0; i < gp.number_of_chambers; i++)
		createSphere(i, chambers, gp.radius0x, gp.radius0y, gp.radius0z, gp.translation, gp.angle1, gp.angle2, gp.scalex, gp.scaley, gp.scalez);

	Part *p1 = addNewPart(&m, chambers[0]);
	p1->scale = Pt3D(gp.radius0x, gp.radius0y, gp.radius0z); //size of the initial chamber
	m.checkpoint();
	for (int i = 1; i < gp.number_of_chambers; i++)
	{
		Part *p2 = addNewPart(&m, chambers[i]);
		p2->scale = p1->scale.entrywiseProduct(Pt3D(gp.scalex, gp.scaley, gp.scalez)); //each part's scale is its predecessor's scale * scaling
		m.addNewJoint(p1, p2, Joint::SHAPE_FIXED); //all parts must be connected
		m.checkpoint();
		p1 = p2;
	}

	for (int i = 0; i < gp.number_of_chambers; i++)
		delete chambers[i];
	delete[] chambers;

	m.close();
	return m.getF0Geno().getGenes();
}

void GenoConv_fF0::createSphere(int which, fF_chamber3d **chambers, double radius0x, double radius0y, double radius0z, double translation, double alpha_, double gamma_, double kx_, double ky_, double kz_)
{
	chambers[which] = new fF_chamber3d(0.0, 0.0, 0.0,
		radius0x, radius0y, radius0z, radius0x * kx_, 0.0, 0.0,
		radius0x * translation, 0.0, 0.0, 0.0, 0.0);
	if (which == 0)
		chambers[which]->points = generate_points(chambers[which]);
	if (which > 0)
	{
		chambers[which]->radius_x = get_radius(chambers[which - 1]->radius_x, kx_, chambers[0]->radius_x);
		chambers[which]->radius_y = get_radius(chambers[which - 1]->radius_y, ky_, chambers[0]->radius_y);
		chambers[which]->radius_z = get_radius(chambers[which - 1]->radius_z, kz_, chambers[0]->radius_z);

		/* new growth vector length */
		double len = chambers[which]->radius_y * translation;
		double max_radius = fF_TOO_MUCH * chambers[which]->radius_y;
		if (fabs(len) > (max_radius))
			len = ((len < 0) ? (-1) : 1) * max_radius;
		if (len == 0)
			len = -0.0000001;

		/* aperture of the previous chamber */
		double pzx = chambers[which - 1]->holeX;
		double pzy = chambers[which - 1]->holeY;
		double pzz = chambers[which - 1]->holeZ;

		//center of the previous chamber
		double pcx = chambers[which - 1]->centerX;
		double pcy = chambers[which - 1]->centerY;
		//double pcz = chambers[which - 1]->centerZ; //not used

		/* aperture of the next to last chamber */
		double ppx;
		double ppy;
		//double ppz; //not used

		if (which == 1)
		{
			ppx = pcx;
			ppy = pcy;
			//ppz = pcz;
		}
		else
		{
			ppx = chambers[which - 2]->holeX;
			ppy = chambers[which - 2]->holeY;
			//ppz = chambers[which - 2]->holeZ;
		}

		double pzxprim = pzx - ppx;
		double pzyprim = pzy - ppy;
		double angle;

		angle = Convert::atan_2(pzyprim, pzxprim);
		double alpha = angle - alpha_;

		double gamma = chambers[which - 1]->phi + gamma_;

		double wx = len * cos(alpha);
		double wy = len * sin(alpha);
		double wz = len * sin(alpha) * sin(gamma);

		/*center of the new sphere*/
		double x = pzx + wx;
		double y = pzy + wy;
		double z = pzz + wz;

		chambers[which]->centerX = x;
		chambers[which]->centerY = y;
		chambers[which]->centerZ = z;
		chambers[which]->vectorTfX = wx;
		chambers[which]->vectorTfY = wy;
		chambers[which]->vectorTfZ = wz;
		chambers[which]->beta = alpha;
		chambers[which]->phi = gamma;

		chambers[which]->points = generate_points(chambers[which]);
		search_hid(which, chambers);
		int pun;
		pun = find_hole(which, pzx, pzy, pzz, chambers);
		if (pun < 0) //should never happen
		{
			logPrintf("GenoConv_fF0", "createSphere", LOG_ERROR, "find_hole(%d) returned %d", which, pun);
			pun = 0;
		}
		chambers[which]->holeX = chambers[which]->points[pun].x;
		chambers[which]->holeY = chambers[which]->points[pun].y;
		chambers[which]->holeZ = chambers[which]->points[pun].z;
	}
}

double GenoConv_fF0::get_radius(double prev_radius, double scale, double start_radius)
{
	double radius = prev_radius * scale;
	double min_radius = fF_TOO_LITTLE*start_radius;
	if (radius < min_radius) {
		radius = min_radius;
	}

	return radius;
}

void GenoConv_fF0::precompute_cos_and_sin()
{
	double angle = M_PI * 2 / fF_LATITUDE_NUM;
	for (int i = 0; i < fF_LATITUDE_NUM; i++)
	{
		cosines[i] = cos(i * angle);
		sines[i] = sin(i * angle);
	}
}

fF_point* GenoConv_fF0::generate_points(fF_chamber3d *chamber)
{
	double cenx = chamber->centerX;
	double ceny = chamber->centerY;
	double cenz = chamber->centerZ;

	double rx = chamber->radius_x;
	double ry = chamber->radius_y;
	double rz = chamber->radius_z;

	fF_point *points = new fF_point[fF_SIZE];

	for (int i = 0; i < fF_LONGITUDE_NUM; i++)
	{
		double y = ceny + ry * cosines[i];

		for (int j = 0; j < fF_LATITUDE_NUM; j++)
		{
			double x = cenx + rx * cosines[j] * sines[i];
			double z = cenz + rz * sines[j] * sines[i];
			fF_point &p = points[(i * fF_LATITUDE_NUM) + j];
			p.x = x;
			p.y = y;
			p.z = z;
			p.inside = false;
		}
	}
	return points;

}

template<typename T> T Square(T x) { return x * x; }

double GenoConv_fF0::dist(double x1, double y1, double z1, double x2, double y2, double z2)
{
	return sqrt(Square(x2 - x1) + Square(y2 - y1) + Square(z2 - z1));
}

void GenoConv_fF0::search_hid(int nr, fF_chamber3d **chambers)
{
	for (int i = 0; i < nr; i++)
	{
		fF_chamber3d *chamber = chambers[i];

		double rx_sq = Square(chamber->radius_x);
		double ry_sq = Square(chamber->radius_y);
		double rz_sq = Square(chamber->radius_z);

		for (int j = 0; j < fF_AMOUNT; j++)
		{
			fF_point &p = chambers[nr]->points[j];

			double upx = Square(p.x - chamber->centerX);
			double upy = Square(p.y - chamber->centerY);
			double upz = Square(p.z - chamber->centerZ);

			double expx = upx / rx_sq;
			double expy = upy / ry_sq;
			double expz = upz / rz_sq;

			double result = expx + expy + expz;

			if (result < fF_THICK_RATIO)
			{
				p.inside = true;
			}
		}
	}
}

int GenoConv_fF0::find_hole(int which, double x, double y, double z, fF_chamber3d **chambers)
{
	int found = -1;
	double distsq_found;

	for (int i = 0; i < fF_AMOUNT; i++)
	{
		fF_point &p = chambers[which]->points[i];
		if (!p.inside) //it is not inside another chamber
		{
			double distancesq = Square(p.x - x) + Square(p.y - y) + Square(p.z - z);
			if (found < 0)
			{
				found = i;
				distsq_found = distancesq;
			}
			if (distancesq < distsq_found)
			{
				if (which != 0)
				{
					bool good = true;
					for (int j = 0; j < which && good; j++)
					{
						fF_chamber3d *chamber = chambers[j];

						double rx_sq = Square(chamber->radius_x);
						double ry_sq = Square(chamber->radius_y);
						double rz_sq = Square(chamber->radius_z);

						double upx = Square(p.x - chamber->centerX);
						double upy = Square(p.y - chamber->centerY);
						double upz = Square(p.z - chamber->centerZ);

						double expx = upx / rx_sq;
						double expy = upy / ry_sq;
						double expz = upz / rz_sq;

						double result = expx + expy + expz;
						if (result < 1.0)
						{
							good = false;
						}
					}
					if (good)
					{
						found = i;
						distsq_found = distancesq;
					}
				}
			}
		}
	}

	return found;
}
