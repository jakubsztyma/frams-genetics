// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _3D_H_
#define _3D_H_

#if defined SHP || defined __ANDROID__
#include <string.h> //memcpy
#else
#include <memory.h> //memcpy
#endif

/**********************************
\file 3d.h 3d.cpp

basic 3D classes and operators
*********************************/

/// point in 3D space
class Pt3D
{
public:
	double x, y, z;
	static bool report_errors;

	Pt3D(double _x, double _y, double _z) :x(_x), y(_y), z(_z) {} ///< constructor initializing all coords
	Pt3D(double xyz) :x(xyz), y(xyz), z(xyz) {} ///< all coords equal 
	Pt3D() {} ///< coords will be not initialized!
	Pt3D(const Pt3D &p) :x(p.x), y(p.y), z(p.z) {} ///< copy from another point
	bool 	operator==(const Pt3D& p)	{ return (x == p.x) && (y == p.y) && (z == p.z); }
	void 	operator+=(const Pt3D& p)	{ x += p.x; y += p.y; z += p.z; }
	void 	operator-=(const Pt3D& p)	{ x -= p.x; y -= p.y; z -= p.z; }
	void  	operator*=(double d)	{ x *= d; y *= d; z *= d; }
	Pt3D  	operator*(const Pt3D &p) const { return Pt3D(y*p.z - z*p.y, z*p.x - x*p.z, x*p.y - y*p.x); }
	void 	operator/=(double d)	{ x /= d; y /= d; z /= d; }
	//Pt3D 	operator+(const Pt3D& p) const {return Pt3D(x+p.x,y+p.y,z+p.z);}
	//Pt3D 	operator-(const Pt3D& p) const {return Pt3D(x-p.x,y-p.y,z-p.z);}
	Pt3D 	operator-() const { return Pt3D(-x, -y, -z); }
	Pt3D 	operator*(double d) const { return Pt3D(x*d, y*d, z*d); }
	Pt3D 	operator/(double d) const { return Pt3D(x / d, y / d, z / d); }
	bool    allCoordsLowerThan(const Pt3D& p) const { return (x < p.x) && (y < p.y) && (z<p.z); }
	bool    allCoordsHigherThan(const Pt3D& p) const { return (x>p.x) && (y > p.y) && (z > p.z); }
	void getMin(const Pt3D& p);
	void getMax(const Pt3D& p);
	/** vector length = \f$\sqrt{x^2+y^2+z^2}\f$  */
	double operator()() const;
	/** vector length = \f$\sqrt{x^2+y^2+z^2}\f$  */
	double length() const { return operator()(); }
	double length2() const { return x*x + y*y + z*z; }
	double distanceTo(const Pt3D& p) const;
	double manhattanDistanceTo(const Pt3D& p) const;
	/** calculate angle between (0,0)-(dx,dy), @return 1=ok, 0=can't calculate */
	static double getAngle(double dx, double dy);
	/** calculate 3 rotation angles translating (1,0,0) into 'X' and (0,0,1) into 'dir' */
	void getAngles(const Pt3D& X, const Pt3D& dir);
	void vectorProduct(const Pt3D& a, const Pt3D& b);
	Pt3D vectorProduct(const Pt3D& p) const { return (*this)*p; }
	Pt3D entrywiseProduct(const Pt3D &p) const { return Pt3D(x*p.x, y*p.y, z*p.z); } ///< also known as Hadamard product or Schur product
	double dotProduct(const Pt3D& p) const { return x*p.x + y*p.y + z*p.z; }
	bool normalize();
};

Pt3D operator+(const Pt3D &p1, const Pt3D &p2);
Pt3D operator-(const Pt3D &p1, const Pt3D &p2);

class Pt3D_DontReportErrors
{
	bool state;
public:
	Pt3D_DontReportErrors() { state = Pt3D::report_errors; Pt3D::report_errors = false; }
	~Pt3D_DontReportErrors() { Pt3D::report_errors = state; }
};

///  orientation in 3D space = rotation matrix
class Matrix44;

class Orient
{
public:
	Pt3D x, y, z; ///< 3 vectors (= 3x3 matrix)

	Orient() {}
	Orient(const Orient& src) { x = src.x; y = src.y; z = src.z; }
	Orient(const Pt3D& a, const Pt3D& b, const Pt3D& c) :x(a), y(b), z(c) {}
	//	Orient(const Pt3D& rot) {*this=rot;}
	Orient(const Matrix44& m);
	void operator=(const Pt3D &rot);
	void rotate(const Pt3D &); ///< rotate matrix around 3 axes

	void transform(Pt3D &target, const Pt3D &src) const;	///< transform a vector
	void revTransform(Pt3D &target, const Pt3D &src) const;	///< reverse transform
	Pt3D transform(const Pt3D &src) const { Pt3D t; transform(t, src); return t; }
	Pt3D revTransform(const Pt3D &src) const { Pt3D t; revTransform(t, src); return t; }

	void transform(Orient& target, const Orient& src) const;    ///< transform other orient
	void revTransform(Orient& target, const Orient& src) const; ///< reverse transform other orient
	Orient transform(const Orient& src) const { Orient o; transform(o, src); return o; }    ///< transform other orient
	Orient revTransform(const Orient& src) const { Orient o; revTransform(o, src); return o; } ///< reverse transform other orient

	void transformSelf(const Orient &rot) { Orient tmp; rot.transform(tmp, *this); *this = tmp; }
	void revTransformSelf(const Orient &rot) { Orient tmp; rot.revTransform(tmp, *this); *this = tmp; }

	void getAngles(Pt3D &) const; ///< calculate rotation from current matrix
	Pt3D getAngles() const { Pt3D ret; getAngles(ret); return ret; }; ///< calculate rotation from current matrix
	void lookAt(const Pt3D &X, const Pt3D &dir); ///< calculate orientation matrix from 2 vectors: X becomes (normalized) Orient.x, dir is the preferred "up" direction (Orient.z). Use lookAt(Pt3D) if only X is relevant.
	void lookAt(const Pt3D &X); ///< calculate orientation matrix from 1 vector, X becomes (normalized) Orient.x, the other coordinates are deterministic but not continuous. Use lookAt(Pt3D,Pt3D) if you need more control.

	bool normalize();
};

class Matrix44
{
public:
	double m[16];
	Matrix44() {}
	Matrix44(const Matrix44& src) { memcpy(m, src.m, sizeof(m)); }
	Matrix44(double *srcm) { memcpy(m, srcm, sizeof(m)); }
	Matrix44(const Orient &rot);

	const double& operator()(int i, int j) const { return m[i + 16 * j]; }
	const double& operator[](int i) const { return m[i]; }
	double& operator()(int i, int j) { return m[i + 16 * j]; }
	double& operator[](int i) { return m[i]; }

	void operator+=(const Pt3D &); ///< translate matrix
	void operator*=(const Pt3D &); ///< scale matrix
	void operator*=(double sc); ///< scale matrix
};

extern Pt3D Pt3D_0; ///< zero vector
extern Orient Orient_1; ///< standard unit matrix: 100 010 001
extern Matrix44 Matrix44_1; ///< standard unit matrix: 1000 0100 0010 0001

void rotate2D(double, double &, double &); ///< rotate 2d vector, given angle
void rotate2D(double, double, double &, double &); ///< rotate 2d vector, given sin and cos
double d2(double, double); ///< distance in 2D

#endif
