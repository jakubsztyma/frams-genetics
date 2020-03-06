// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _PRINTCONVMAP_H_
#define _PRINTCONVMAP_H_

#include <frams/util/sstring.h>
#include <frams/model/model.h>
#include <frams/util/multimap.h>

class MultiMap;

void printConvMap(const SString& gen1, const SString& gen2, const MultiMap& map, int left_column_padding = 15);

class ModelDisplayMap
{
	MultiMap map;
	Model &model;
	int joint_offset, neuron_offset, max_element;
public:
	ModelDisplayMap(Model &m);
	void print(int left_column_padding = 15);
	const MultiMap & getMap() { return map; } //mapping: true model -> display (so the regular map printing/debugging tools can be used for model maps, avoiding invonveniently huge numbers)
};

#endif
