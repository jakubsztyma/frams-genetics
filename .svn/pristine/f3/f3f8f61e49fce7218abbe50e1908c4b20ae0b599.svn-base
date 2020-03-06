// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _CONVERT_H_
#define _CONVERT_H_

#include "nonstd.h"
#include "nonstd_math.h"
#include "nonstd_stl.h"
#include "2d.h"
#include <stdint.h>


typedef XY<double> Pt2D;

#ifdef LINUX
#define UINT64_FORMAT "%llu" //we want to avoid this and ambiguous "long long", but gcc does not seem to support I64u (yet?)
#else
#define UINT64_FORMAT "%I64u"
#endif

struct Convert
{
public:
	static int toInt(string s);
	static float toFloat(string s);
	static string toLowerCase(string s);
	static string toUpperCase(string s);
	static char toLowerCase(char c);
	static char toUpperCase(char c);
	template<class T> static string _toString(const T& value);
	static string toString(unsigned int v);
	static string toString(int v);
	static string toString(short v);
	static string toString(float v);
	static string toString(double v);
	static string zeroPad(string s, int l) { while ((int)s.length() < l) s = string("0") + s; return s; }
	static uint32_t hexToInt(const string& col);

	static double toRadians(double angle) { return angle*M_PI / 180; }
	static double toDegrees(double angle) { return angle / M_PI * 180; }
	static double atan_2(double y, double x) { if (x == 0 && y == 0) return 0; else return atan2(y, x); } //needed by borland 5/6 only?

	static double odleglosc_sq(double x1, double y1, double x2, double y2) //odleglosc do kwadratu, wystarczy do porownywania
	{
		double dx = x2 - x1, dy = y2 - y1; return dx*dx + dy*dy;
	}
	static double odleglosc_sq(const Pt2D& p1, const Pt2D& p2) //odleglosc do kwadratu
	{
		return odleglosc_sq(p1.x, p1.y, p2.x, p2.y);
	}

	static double odleglosc(double x1, double y1, double x2, double y2)	{ return sqrt(odleglosc_sq(x1, y1, x2, y2)); }
	static double odleglosc(const Pt2D& p1, const Pt2D& p2)
	{
		return sqrt(odleglosc_sq(p1, p2));
	}

	//static float odleglosc(int x1,int y1,int x2,int y2) {float dx=x1-x2; float dy=y1-y2; return sqrt(dx*dx+dy*dy);}
	//static float odleglosc(float x1,float y1,float x2,float y2) {return sqrt(odleglosc_sq(x1,y1,x2,y2));}
	//static float odleglosc_sq(float x1,float y1,float x2,float y2) {float dx=x1-x2; float dy=y1-y2; return dx*dx+dy*dy;}

	static struct tm localtime(const time_t &timep);//jak ::localtime ale zwraca strukture zamiast wskaznika, ref w parametrze dla wygodnego wywolywania
	static string asctime(const struct tm &tm);//jak ::asctime ale thread safe i bez glupiego \n na koncu, ref w parametrze dla wygodnego wywolywania

	static std::wstring strTOwstr(const char *s)
	{
		string str(s);
		return std::wstring(str.begin(), str.end());
	}

	static string wstrTOstr(const wchar_t *s)
	{
		wstring str(s);
		return string(str.begin(), str.end());
	}

	static string wstrToUtf8(const wchar_t *str);
#ifdef _WIN32
	static wstring utf8ToUtf16(const char *str);
#endif
};



struct Angle //normalized angle in radians [0,2pi) and degrees [0,360) with pre-computed sine and cosine and degree as integer [0,359]
{
private:
	double angle; //in radians, read-only
public:
	double angle_deg; //read-only
	int angle_deg_int; //read-only

	Angle() { set(0); }
	Angle(double k) { set(k); }
	Angle(Angle &kt) { set(kt.get()); }
	Angle(double dy, double dx) { set(dy, dx); }
	void set(double k) { k = fmod(k, M_PI * 2); if (k < 0) k += M_PI * 2; angle = k; sine = sin(k); cosine = cos(k); angle_deg = Convert::toDegrees(angle); angle_deg_int = roundToInt(angle_deg); angle_deg_int %= 360; }
	void set(double dy, double dx) { set(Convert::atan_2(dy, dx)); }
	void add(double dk) { set(angle + dk); }
	void add(Angle &kt) { set(angle + kt.get()); }
	double get() { return angle; }
	double sine, cosine;
};


#endif
