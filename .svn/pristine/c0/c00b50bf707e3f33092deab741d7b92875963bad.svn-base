// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _UTIL_STL_H_
#define _UTIL_STL_H_

#include "nonstd_stl.h"

template<typename T, std::size_t N> void push_back(vector<T>& v, T(&d)[N])
{
	for (unsigned int i = 0; i < N; i++)
		v.push_back(d[i]);
}

template<typename T> void erase(vector<T>& v, const T& e)
{
	typename vector<T>::iterator it = std::find(v.begin(), v.end(), e);
	if (it != v.end())
		v.erase(it);
}

template<typename T> void deleteVectorElements(vector<T*>& v)
{
	for (typename vector<T*>::iterator it = v.begin(); it != v.end(); it++)
		delete *it;
	v.clear();
}

template<typename T> int findIndex(vector<T>& v, const T& e)
{
	typename vector<T>::iterator it = find(v.begin(), v.end(), e);
	if (it != v.end())
		return int(&*it - &v.front());
	return -1;
}

template<class T> class DeletingVector  // deletes the elements (pointers) in destructor
{
public:
	std::vector<T*> vector;
	~DeletingVector()
	{
		for (int i = (int)vector.size() - 1; i >= 0; i--)
			delete vector[i];
	}
	T* operator[](int i) { return vector[i]; }
	int size() { return vector.size(); }
	void push_back(T* x) { vector.push_back(x); }
};

#endif
