// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <common/nonstd_math.h>
#include <common/log.h>
#include "3d.h"

Pt3D operator+(const Pt3D &p1, const Pt3D &p2) { return Pt3D(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z); }
Pt3D operator-(const Pt3D &p1, const Pt3D &p2) { return Pt3D(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z); }

Pt3D Pt3D_0(0, 0, 0);

bool Pt3D::report_errors = true;

double Pt3D::operator()() const
{
	double q = x*x + y*y + z*z;
	if (q < 0) { if (report_errors) logPrintf("Pt3D", "operator()", LOG_ERROR, "sqrt(%g): domain error", q); return 0; }
	return sqrt(q);
}

bool Pt3D::normalize()
{
	double len = length();
	if (fabs(len) < 1e-50) { if (report_errors) logPrintf("Pt3D", "normalize()", LOG_WARN, "vector[%g,%g,%g] too small", x, y, z); x = 1; y = 0; z = 0; return false; }
	operator/=(len);
	return true;
}

double Pt3D::distanceTo(const Pt3D& p) const
{
	double dx = x - p.x;
	double dy = y - p.y;
	double dz = z - p.z;
	return sqrt(dx*dx + dy*dy + dz*dz);
}

double Pt3D::manhattanDistanceTo(const Pt3D& p) const
{
	return fabs(x - p.x) + fabs(y - p.y) + fabs(z - p.z);
}

Orient Orient_1(Pt3D(1, 0, 0), Pt3D(0, 1, 0), Pt3D(0, 0, 1));

// simple rotation
void rotate2D(double k, double &x, double &y)
{
	double s = sin(k), c = cos(k);
	double t = c*x - s*y;
	y = s*x + c*y;
	x = t;
}

void rotate2D(double s, double c, double &x, double &y)
{
	double t = c*x - s*y;
	y = s*x + c*y;
	x = t;
}

double Pt3D::getAngle(double dx, double dy)
{
	if (dx == 0 && dy == 0)
	{
		if (report_errors) logPrintf("Pt3D", "getAngle()", LOG_WARN, "atan2(%g,%g)", dy, dx);
		return 0; // incorrect result, but there is no correct one
	}
	return atan2(dy, dx);
}

void Pt3D::getAngles(const Pt3D& X, const Pt3D& dir)
{
	Pt3D t1(X), t2(dir);
	if (fabs(t1.x) > 1e-50 || fabs(t1.y) > 1e-50) // non-vertical
	{
		z = atan2(t1.y, t1.x);
		rotate2D(-z, t1.x, t1.y);
		rotate2D(-z, t2.x, t2.y);
		y = getAngle(t1.x, t1.z);
	}
	else // vertical
	{
		z = 0;
		if (t1.z < 0)
			y = -M_PI_2; // down
		else
			y = M_PI_2; // up
	}
	rotate2D(-y, t2.x, t2.z);
	x = getAngle(t2.z, -t2.y);
}

void Pt3D::getMin(const Pt3D& p)
{
	if (p.x < x) x = p.x;
	if (p.y < y) y = p.y;
	if (p.z < z) z = p.z;
}
void Pt3D::getMax(const Pt3D& p)
{
	if (p.x > x) x = p.x;
	if (p.y > y) y = p.y;
	if (p.z > z) z = p.z;
}

void Pt3D::vectorProduct(const Pt3D& a, const Pt3D& b)
{
	x = a.y*b.z - a.z*b.y;
	y = a.z*b.x - a.x*b.z;
	z = a.x*b.y - a.y*b.x;
}

void Orient::lookAt(const Pt3D& X, const Pt3D& dir)
{
	x = X; x.normalize();
	y.vectorProduct(dir, x);
	z.vectorProduct(x, y);
	if ((!y.normalize()) || (!z.normalize()))
		lookAt(X);// dir was (nearly?) parallel, there is no good solution, use the x-only variant
}

void Orient::lookAt(const Pt3D& X)
{
	x = X; x.normalize();
	// "invent" y vector, not parallel to x
	double ax = fabs(x.x), ay = fabs(x.y), az = fabs(x.z);
	// find the smallest component
	if ((ax <= ay) && (ax <= az)) // x
	{
		y.x = 0; y.y = -x.z; y.z = x.y; // (0,-z,y)
	}
	if ((ay <= ax) && (ay <= az)) // y
	{
		y.x = -x.z; y.y = 0; y.z = x.x; // (-z,0,x)
	}
	else // z
	{
		y.x = -x.y; y.y = x.x; y.z = 0; // (-y,x,0)
	}
	y.normalize();
	z.vectorProduct(x, y);
}

// 2D distance
double d2(double x, double y)
{
	double q = x*x + y*y;
	if (q < 0) { if (Pt3D::report_errors) logPrintf("", "d2()", LOG_ERROR, "sqrt(%g): domain error", q); return 0; }
	return sqrt(q);
}

Orient::Orient(const Matrix44& m)
{
	x.x = m[0];  x.y = m[1];  x.z = m[2];
	y.x = m[4];  y.y = m[5];  y.z = m[6];
	z.x = m[8];  z.y = m[9];  z.z = m[10];
}

void Orient::operator=(const Pt3D &rot)
{
	*this = Orient_1;
	rotate(rot);
}

void Orient::rotate(const Pt3D &v)
{
	double s, c;
	if (v.x != 0)
	{
		s = sin(v.x); c = cos(v.x);
		rotate2D(s, c, x.y, x.z);
		rotate2D(s, c, y.y, y.z);
		rotate2D(s, c, z.y, z.z);
	}
	if (v.y != 0)
	{
		s = sin(v.y); c = cos(v.y);
		rotate2D(s, c, x.x, x.z);
		rotate2D(s, c, y.x, y.z);
		rotate2D(s, c, z.x, z.z);
	}
	if (v.z != 0)
	{
		s = sin(v.z); c = cos(v.z);
		rotate2D(s, c, x.x, x.y);
		rotate2D(s, c, y.x, y.y);
		rotate2D(s, c, z.x, z.y);
	}
}

void Orient::transform(Pt3D& target, const Pt3D &s) const
{
	target.x = s.x*x.x + s.y*y.x + s.z*z.x;
	target.y = s.x*x.y + s.y*y.y + s.z*z.y;
	target.z = s.x*x.z + s.y*y.z + s.z*z.z;
}

void Orient::revTransform(Pt3D& target, const Pt3D &s) const
{
	target.x = s.x*x.x + s.y*x.y + s.z*x.z;
	target.y = s.x*y.x + s.y*y.y + s.z*y.z;
	target.z = s.x*z.x + s.y*z.y + s.z*z.z;
}

void Orient::transform(Orient& target, const Orient& src) const
{
	transform(target.x, src.x);
	transform(target.y, src.y);
	transform(target.z, src.z);
}

void Orient::revTransform(Orient& target, const Orient& src) const
{
	revTransform(target.x, src.x);
	revTransform(target.y, src.y);
	revTransform(target.z, src.z);
}

void Orient::getAngles(Pt3D &angles) const
{
	angles.getAngles(x, z);
}

bool Orient::normalize()
{
	bool ret = 1;
	y.vectorProduct(z, x);
	z.vectorProduct(x, y);
	if (!x.normalize()) ret = 0;
	if (!z.normalize()) ret = 0;
	if (!y.normalize()) ret = 0;
	return ret;
}

Matrix44::Matrix44(const Orient &rot)
{
	m[0] = rot.x.x;  m[1] = rot.x.y;  m[2] = rot.x.z;  m[3] = 0;
	m[4] = rot.y.x;  m[5] = rot.y.y;  m[6] = rot.y.z;  m[7] = 0;
	m[8] = rot.z.x;  m[9] = rot.z.y;  m[10] = rot.z.z; m[11] = 0;
	m[12] = 0;       m[13] = 0;       m[14] = 0;       m[15] = 1;
}

void Matrix44::operator+=(const Pt3D &)
{

}

void Matrix44::operator*=(const Pt3D &)
{
}

void Matrix44::operator*=(double sc)
{
}
