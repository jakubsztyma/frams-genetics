// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

////////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//
/*
 * AutoBuffer from https://github.com/Itseez/opencv/blob/master/modules/core/include/opencv2/core/utility.hpp
 * VBLAS, JacobiSVDImpl_ from https://github.com/Itseez/opencv/blob/master/modules/core/src/lapack.cpp
 * changes MK, May 2015:
 * - "RNG rng(0x12345678)" and "rng.next()" replaced with a local PRBS-7 implementation so that this file does not depend on external random number generators.
 */
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <limits>
#include <cfloat>
//#include <assert.h>
#include <math.h> //hypot(), embarcadero
#include <stdint.h> //uint8_t
#include "lapack.h"


#if defined _M_IX86 && defined _MSC_VER && _MSC_VER < 1700
#pragma float_control(precise, on)
#endif

template<typename _Tp, size_t fixed_size = 1024 / sizeof(_Tp) + 8 > class AutoBuffer
{
public:
	typedef _Tp value_type;

	//! the default constructor
	AutoBuffer();
	//! constructor taking the real buffer size
	AutoBuffer(size_t _size);

	//! the copy constructor
	AutoBuffer(const AutoBuffer<_Tp, fixed_size>& buf);
	//! the assignment operator
	AutoBuffer<_Tp, fixed_size>& operator=(const AutoBuffer<_Tp, fixed_size>& buf);

	//! destructor. calls deallocate()
	~AutoBuffer();

	//! allocates the new buffer of size _size. if the _size is small enough, stack-allocated buffer is used
	void allocate(size_t _size);
	//! deallocates the buffer if it was dynamically allocated
	void deallocate();
	//! resizes the buffer and preserves the content
	void resize(size_t _size);
	//! returns the current buffer size
	size_t size() const;
	//! returns pointer to the real buffer, stack-allocated or head-allocated
	operator _Tp* ();
	//! returns read-only pointer to the real buffer, stack-allocated or head-allocated
	operator const _Tp* () const;

protected:
	//! pointer to the real buffer, can point to buf if the buffer is small enough
	_Tp* ptr;
	//! size of the real buffer
	size_t sz;
	//! pre-allocated buffer. At least 1 element to confirm C++ standard reqirements
	_Tp buf[(fixed_size > 0) ? fixed_size : 1];
};

/////////////////////////////// AutoBuffer implementation ////////////////////////////////////////

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::AutoBuffer()
{
	ptr = buf;
	sz = fixed_size;
}

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::AutoBuffer(size_t _size)
{
	ptr = buf;
	sz = fixed_size;
	allocate(_size);
}

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::AutoBuffer(const AutoBuffer<_Tp, fixed_size>& abuf)
{
	ptr = buf;
	sz = fixed_size;
	allocate(abuf.size());
	for (size_t i = 0; i < sz; i++)
		ptr[i] = abuf.ptr[i];
}

template<typename _Tp, size_t fixed_size> inline AutoBuffer<_Tp, fixed_size>&
AutoBuffer<_Tp, fixed_size>::operator=(const AutoBuffer<_Tp, fixed_size>& abuf)
{
	if (this != &abuf)
	{
		deallocate();
		allocate(abuf.size());
		for (size_t i = 0; i < sz; i++)
			ptr[i] = abuf.ptr[i];
	}
	return *this;
}

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::~AutoBuffer()
{
	deallocate();
}

template<typename _Tp, size_t fixed_size> inline void
AutoBuffer<_Tp, fixed_size>::allocate(size_t _size)
{
	if (_size <= sz)
	{
		sz = _size;
		return;
	}
	deallocate();
	if (_size > fixed_size)
	{
		ptr = new _Tp[_size];
		sz = _size;
	}
}

template<typename _Tp, size_t fixed_size> inline void
AutoBuffer<_Tp, fixed_size>::deallocate()
{
	if (ptr != buf)
	{
		delete[] ptr;
		ptr = buf;
		sz = fixed_size;
	}
}

template<typename _Tp, size_t fixed_size> inline void
AutoBuffer<_Tp, fixed_size>::resize(size_t _size)
{
	if (_size <= sz)
	{
		sz = _size;
		return;
	}
	//size_t i, prevsize = sz, minsize = MIN(prevsize, _size);
	size_t i, prevsize = sz, minsize = prevsize < _size ? prevsize : _size;
	_Tp* prevptr = ptr;

	ptr = _size > fixed_size ? new _Tp[_size] : buf;
	sz = _size;

	if (ptr != prevptr)
		for (i = 0; i < minsize; i++)
			ptr[i] = prevptr[i];
	for (i = prevsize; i < _size; i++)
		ptr[i] = _Tp();

	if (prevptr != buf)
		delete[] prevptr;
}

template<typename _Tp, size_t fixed_size> inline size_t
AutoBuffer<_Tp, fixed_size>::size() const
{
	return sz;
}

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::operator _Tp* ()
{
	return ptr;
}

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::operator const _Tp* () const
{
	return ptr;
}

template<typename T> struct VBLAS
{

	int dot(const T*, const T*, int, T*) const
	{
		return 0;
	}

	int givens(T*, T*, int, T, T) const
	{
		return 0;
	}

	int givensx(T*, T*, int, T, T, T*, T*) const
	{
		return 0;
	}
};

template<typename _Tp> void
JacobiSVDImpl_(_Tp* At, size_t astep, _Tp* _W, _Tp* Vt, size_t vstep,
int m, int n, int n1, double minval, _Tp eps)
{
	VBLAS<_Tp> vblas;
	AutoBuffer<double> Wbuf(n);
	double* W = Wbuf;
	int i, j, k, iter, max_iter = std::max(m, 30);
	_Tp c, s;
	double sd;
	astep /= sizeof(At[0]);
	vstep /= sizeof(Vt[0]);

	for (i = 0; i < n; i++)
	{
		for (k = 0, sd = 0; k < m; k++)
		{
			_Tp t = At[i * astep + k];
			sd += (double)t*t;
		}
		W[i] = sd;

		if (Vt)
		{
			for (k = 0; k < n; k++)
				Vt[i * vstep + k] = 0;
			Vt[i * vstep + i] = 1;
		}
	}

	for (iter = 0; iter < max_iter; iter++)
	{
		bool changed = false;

		for (i = 0; i < n - 1; i++)
			for (j = i + 1; j < n; j++)
			{
			_Tp *Ai = At + i*astep, *Aj = At + j*astep;
			double a = W[i], p = 0, b = W[j];

			for (k = 0; k < m; k++)
				p += (double)Ai[k] * Aj[k];

			if (std::abs(p) <= eps * std::sqrt((double)a * b))
				continue;

			p *= 2;
			double beta = a - b, gamma = hypot((double)p, beta);
			if (beta < 0)
			{
				double delta = (gamma - beta)*0.5;
				s = (_Tp)std::sqrt(delta / gamma);
				c = (_Tp)(p / (gamma * s * 2));
			}
			else
			{
				c = (_Tp)std::sqrt((gamma + beta) / (gamma * 2));
				s = (_Tp)(p / (gamma * c * 2));
			}

			a = b = 0;
			for (k = 0; k < m; k++)
			{
				_Tp t0 = c * Ai[k] + s * Aj[k];
				_Tp t1 = -s * Ai[k] + c * Aj[k];
				Ai[k] = t0;
				Aj[k] = t1;

				a += (double)t0*t0;
				b += (double)t1*t1;
			}
			W[i] = a;
			W[j] = b;

			changed = true;

			if (Vt)
			{
				_Tp *Vi = Vt + i*vstep, *Vj = Vt + j*vstep;
				k = vblas.givens(Vi, Vj, n, c, s);

				for (; k < n; k++)
				{
					_Tp t0 = c * Vi[k] + s * Vj[k];
					_Tp t1 = -s * Vi[k] + c * Vj[k];
					Vi[k] = t0;
					Vj[k] = t1;
				}
			}
			}
		if (!changed)
			break;
	}

	for (i = 0; i < n; i++)
	{
		for (k = 0, sd = 0; k < m; k++)
		{
			_Tp t = At[i * astep + k];
			sd += (double)t*t;
		}
		W[i] = std::sqrt(sd);
	}

	for (i = 0; i < n - 1; i++)
	{
		j = i;
		for (k = i + 1; k < n; k++)
		{
			if (W[j] < W[k])
				j = k;
		}
		if (i != j)
		{
			std::swap(W[i], W[j]);
			if (Vt)
			{
				for (k = 0; k < m; k++)
					std::swap(At[i * astep + k], At[j * astep + k]);

				for (k = 0; k < n; k++)
					std::swap(Vt[i * vstep + k], Vt[j * vstep + k]);
			}
		}
	}

	for (i = 0; i < n; i++)
		_W[i] = (_Tp)W[i];

	if (!Vt)
		return;

	uint8_t rndstate = 0x02; //PRBS-7 from http://en.wikipedia.org/wiki/Pseudorandom_binary_sequence
	for (i = 0; i < n1; i++)
	{
		sd = i < n ? W[i] : 0;

		while (sd <= minval)
		{
			// if we got a zero singular value, then in order to get the corresponding left singular vector
			// we generate a random vector, project it to the previously computed left singular vectors,
			// subtract the projection and normalize the difference.
			const _Tp val0 = (_Tp)(1. / m);
			for (k = 0; k < m; k++)
			{
				int rndbit = (((rndstate >> 6) ^ (rndstate >> 5)) & 1);
				rndstate = ((rndstate << 1) | rndbit) & 0x7f;
				_Tp val = rndbit == 0 ? val0 : -val0;
				At[i * astep + k] = val;
			}
			for (iter = 0; iter < 2; iter++)
			{
				for (j = 0; j < i; j++)
				{
					sd = 0;
					for (k = 0; k < m; k++)
						sd += At[i * astep + k] * At[j * astep + k];
					_Tp asum = 0;
					for (k = 0; k < m; k++)
					{
						_Tp t = (_Tp)(At[i * astep + k] - sd * At[j * astep + k]);
						At[i * astep + k] = t;
						asum += std::abs(t);
					}
					asum = asum ? 1 / asum : 0;
					for (k = 0; k < m; k++)
						At[i * astep + k] *= asum;
				}
			}
			sd = 0;
			for (k = 0; k < m; k++)
			{
				_Tp t = At[i * astep + k];
				sd += (double)t*t;
			}
			sd = std::sqrt(sd);
		}

		s = (_Tp)(1 / sd);
		for (k = 0; k < m; k++)
			At[i * astep + k] *= s;
	}
}

void Lapack::JacobiSVD(double* At, size_t astep, double* W, double* Vt, size_t vstep, int m, int n, int n1 = -1)
{
	JacobiSVDImpl_(At, astep, W, Vt, vstep, m, n, !Vt ? 0 : n1 < 0 ? n : n1, DBL_MIN, DBL_EPSILON * 10);
}
