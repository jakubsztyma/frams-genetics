// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2020  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _2D_H_
#define _2D_H_

#include "nonstd_stl.h"
#include <math.h>

//unification of old GUIXY and Pt2D
template <typename T> class XY
{
public:
	T x, y;
	XY() {}
	XY(T _x, T _y) :x(_x), y(_y) {}
	template <typename Q> XY(const Q &other) : x(other.x), y(other.y) {}
	template <typename Q> const XY &operator=(const Q &other) { x = other.x; y = other.y; return *this; }
	template <typename Q> const XY operator()(const Q &other) { return XY(other.x, other.y); }
	XY operator+(const XY &p) const { return XY(x + p.x, y + p.y); }
	XY operator-(const XY &p) const { return XY(x - p.x, y - p.y); }
	XY operator+=(const XY &p) { x += p.x; y += p.y; return *this; }
	XY operator-=(const XY &p) { x -= p.x; y -= p.y; return *this; }
	XY operator-() const { return XY(-x, -y); }
	// allows float operations on ints
	template <typename Q> XY operator*=(Q q) { x *= q; y *= q; return *this; }
	template <typename Q> XY operator/=(Q q) { x /= q; y /= q; return *this; }
	template <typename Q> XY operator/(Q q) const { return XY(x / q, y / q); }
	template <typename Q> XY operator*(Q q) const { return XY(q * x, q * y); }
	XY operator*=(const XY &q) { x *= q.x; y *= q.y; return *this; }
	XY operator/=(const XY &q) { x /= q.x; y /= q.y; return *this; }
	XY operator*(const XY &q) const { return XY(x * q.x, y * q.y); }
	XY operator/(const XY &q) const { return XY(x / q.x, y / q.y); }
	void set(T _x, T _y) { x = _x; y = _y; }
	void add(T _x, T _y) { x += _x; y += _y; }
	void sub(T _x, T _y) { x -= _x; y -= _y; }
	bool operator==(const XY &p) const { return (fabs(double(x - p.x)) < 1e-20) && (fabs(double(y - p.y)) < 1e-20); }
	bool operator!=(const XY &p) const { return !operator==(p); }
	T distanceTo(const XY &p) const { return sqrt(double((p.x - x) * (p.x - x) + (p.y - y) * (p.y - y))); }
	T magnitude() const { return sqrt(x * x + y * y); }
	T length() const { return sqrt(x * x + y * y); }
	T lengthSq() const { return x * x + y * y; }
	T dotProduct(const XY &v) const { return x * v.x + y * v.y; }
	T crossProduct(const XY &v) const { return x * v.y - y * v.x; }
	void normalize() { operator/=(length()); } // length becomes 1
	static XY average(const XY &v1, const XY &v2) { return XY((v1.x + v2.x) * 0.5, (v1.y + v2.y) * 0.5); }
	double getDirection() const { return atan2(y, x); }
	static XY interpolate(const XY &v1, const XY &v2, double t) { return universal_lerp(v1, v2, t); }
	XY toInt() const { return XY(int(x), int(y)); }
	XY transpose() const { return XY(y, x); }
	static const XY &zero() { static XY t(0, 0); return t; }
	static const XY &one() { static XY t(1, 1); return t; }
};

//specialized: int equality not using fabs()
template<> inline bool XY<int>::operator==(const XY<int> &p) const { return (x == p.x) && (y == p.y); }

template <typename T> XY<T> xymin(const XY<T> &a, const XY<T> &b) { return XY<T>(min(a.x, b.x), min(a.y, b.y)); }
template <typename T> XY<T> xymax(const XY<T> &a, const XY<T> &b) { return XY<T>(max(a.x, b.x), max(a.y, b.y)); }

template <typename T>
class XYMargin
{
public:
	XYMargin(T x = 0) :left(x), top(x), right(x), bottom(x) {}
	XYMargin(T l, T t, T r, T b) :left(l), top(t), right(r), bottom(b) {}
	T left, top, right, bottom;
	void operator=(T x) { left = top = right = bottom = x; }
	XYMargin operator-() const { return XYMargin(-left, -top, -right, -bottom); }
	void operator=(const XYMargin<T> &other) { left = other.left; top = other.top; right = other.right; bottom = other.bottom; }
	T horizontal() const { return left + right; }
	T vertical() const { return top + bottom; }
	bool operator==(const XYMargin &other) const { return left == other.left && top == other.top && right == other.right && bottom == other.bottom; }
	XYMargin normalized() const { return XYMargin(max(left, T(0)), max(top, T(0)), max(right, T(0)), max(bottom, T(0))); }
};

template <typename T>
class XYRect
{
public:
	XY<T> p, size;
	XYRect() {}
	XYRect(const XY<T> &p1, const XY<T> &s) :p(p1), size(s) {}
	template <typename Q> XYRect(const Q &other) : p(other.p), size(other.size) {}
	XYRect(T _x, T _y, T _w, T _h) :p(_x, _y), size(_w, _h) {}
	static XYRect<T> centeredAt(const XY<T> &p, XY<T> s) { return XYRect<T>(p - s * 0.5, s); }

	bool isEmpty() const { return (size.x < 0) || (size.y < 0); }
	XYRect toInt() const { return XYRect(int(p.x), int(p.y), int(p.x + size.x) - int(p.x), int(p.y + size.y) - int(p.y)); }
	bool operator==(const XYRect &r) const { return (p == r.p) && (size == r.size); }
	template <typename Q> const XYRect &operator=(const Q &other) { p = other.p; size = other.size; return *this; }

	T right() const { return p.x + size.x; }
	T bottom() const { return p.y + size.y; }
	T top() const { return p.y; }
	T left() const { return p.x; }
	XY<T> center() const { return p + size / 2; }
	const XY<T> &topLeft() const { return p; }
	XY<T> bottomRight() const { return p + size; }
	XY<T> topRight() const { return XY<T>(p.x + size.x, p.y); }
	XY<T> bottomLeft() const { return XY<T>(p.x, p.y + size.y); }

	T area() const { return size.x * size.y; }

	bool intersects(const XYRect &r) const
	{
		if (r.p.x >= (p.x + size.x)) return false;
		if (r.p.y >= (p.y + size.y)) return false;
		if ((r.p.x + r.size.x) <= p.x) return false;
		if ((r.p.y + r.size.y) <= p.y) return false;
		return true;
	}

	bool contains(const XY<T> &n) const
	{
		if (n.x < p.x) return false;
		if (n.x > (p.x + size.x)) return false;
		if (n.y < p.y) return false;
		if (n.y > (p.y + size.y)) return false;
		return true;
	}

	bool contains(const XYRect &r) const
	{
		return contains(r.p) && contains(r.p + r.size);
	}

	void add(const XY<T> &n)
	{
		if (n.x < p.x) { size.x += p.x - n.x; p.x = n.x; }
		else if (n.x > (p.x + size.x)) size.x = n.x - p.x;
		if (n.y < p.y) { size.y += p.y - n.y; p.y = n.y; }
		else if (n.y > (p.y + size.y)) size.y = n.y - p.y;
	}

	XYRect extendBy(const XY<T> &border_size) const
	{
		return XYRect(p - border_size, size + border_size * 2);
	}

	XYRect shrinkBy(const XY<T> &border_size) const
	{
		return XYRect(p + border_size, size - border_size * 2);
	}

	XYRect extendBy(const XYMargin<T> &m) const
	{
		return XYRect(p.x - m.left, p.y - m.top, size.x + m.horizontal(), size.y + m.vertical());
	}

	XYRect shrinkBy(const XYMargin<T> &m) const
	{
		return XYRect(p.x + m.left, p.y + m.top, size.x - m.horizontal(), size.y - m.vertical());
	}

	XYMargin<T> marginTowards(const XYRect &r) const
	{
		return XYMargin<T>(r.p.x - p.x, r.p.y - p.y,
			(p.x + size.x) - (r.p.x + r.size.x), (p.y + size.y) - (r.p.y + r.size.y));
	}

	XYRect fitAspect(float aspect) ///< place a new rectangle having 'aspect' inside the rectangle
	{
		XYRect r;
		r.size = size;
		if (size.x < size.y * aspect)
			r.size.y = r.size.x / aspect;
		else
			r.size.x = r.size.y * aspect;
		r.p = p + (size - r.size) * 0.5;
		return r;
	}

	XYRect intersection(const XYRect &r) const
	{
		XYRect i;
		XY<T> p2 = p + size;
		XY<T> rp2 = r.p + r.size;
		i.p.x = max(p.x, r.p.x);
		i.p.y = max(p.y, r.p.y);
		i.size.x = min(p2.x, rp2.x) - i.p.x;
		i.size.y = min(p2.y, rp2.y) - i.p.y;
		return i;
	}

	XYRect extensionContaining(const XY<T> &p) const
	{
		XY<T> p1 = xymin(topLeft(), p);
		XY<T> p2 = xymax(bottomRight(), p);
		return XYRect(p1, p2 - p1);
	}

	XYRect extensionContaining(const XYRect &r) const
	{
		XY<T> p1 = xymin(topLeft(), r.topLeft());
		XY<T> p2 = xymax(bottomRight(), r.bottomRight());
		return XYRect(p1, p2 - p1);
	}

	XYRect translation(const XY<T> &t) const
	{
		return XYRect(p + t, size);
	}

	T distanceTo(const XY<T> &n) const
	{
		XY<T> tp = n;
		if (n.x < p.x) tp.x = p.x; else if (n.x >= (p.x + size.x)) tp.x = p.x + size.x;
		if (n.y < p.y) tp.y = p.y; else if (n.y >= (p.y + size.y)) tp.y = p.y + size.y;
		return tp.distanceTo(n);
	}

	T distanceTo(const XYRect<T> &r) const
	{
		bool r_above = (r.bottom() <= top());
		bool r_below = (r.top() >= bottom());
		bool r_left = (r.right() <= left());
		bool r_right = (r.left() >= right());

		if (r_above)
		{
			if (r_left) return r.bottomRight().distanceTo(topLeft());
			else if (r_right) return r.bottomLeft().distanceTo(topRight());
			else return top() - r.bottom();
		}
		else if (r_below)
		{
			if (r_left) return r.topRight().distanceTo(bottomLeft());
			else if (r_right) return r.topLeft().distanceTo(bottomRight());
			else return r.top() - bottom();
		}
		else if (r_left)
		{
			return left() - r.right();
		}
		else if (r_right)
		{
			return r.left() - right();
		}
		else
			return 0; //intersection
	}

	static const XYRect &zero() { static XYRect t(0, 0, 0, 0); return t; }
	static const XYRect &one() { static XYRect t(0, 0, 1, 1); return t; }
};

typedef XY<int> IntXY;
typedef XYRect<int> IntRect;

typedef XY<float> FloatXY;
typedef XYRect<float> FloatRect;

#endif
