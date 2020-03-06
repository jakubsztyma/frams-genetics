// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "printconvmap.h"
#include <stdio.h>

void printN(const char* t, int maxlen)
{
	while (maxlen-- > 0)
		if (*t) putchar(*(t++));
		else putchar(' ');
}

void printN(char t, int maxlen)
{
	while (maxlen-- > 0) putchar(t);
}

void printmapping(const char* gen1, int len1, const MultiRange &mr, const char* gen2, int len2, int left_column_padding)
{
	printN(' ', left_column_padding - len1);
	printN(gen1, len1);
	printf(" : ");
	int i;
	for (i = 0; i < len2; i++)
		if (mr.contains(i))
			putchar(gen2[i]);
		else
			putchar('.');
	putchar('\n');
}

void stripstring(SString &str)
{
	char *t = str.directWrite();
	for (; *t; t++)
		if (strchr("\n\r\t", *t)) *t = ' ';
}

void printConvMap(const SString& gen1, const SString& gen2, const MultiMap& map, int left_column_padding)
{
	int y, y2, len1;
	int id = 0;
	if (map.isEmpty())
	{
		printf("{ empty }\n");
		return;
	}
	len1 = gen1.len();
	SString g1 = gen1;
	stripstring(g1);
	SString g2 = gen2;
	stripstring(g2);
	const char* g = g1.c_str();
	y = 0;
	MultiRange *mr;
	MultiRange emptyrange;
	printN(' ', left_column_padding);
	printf("   %s\n", g2.c_str());
	int begin = map.getBegin();
	int end = map.getEnd();
	while (y < len1)
	{
		if (y < begin)
		{
			mr = &emptyrange;
			y2 = begin;
		}
		else if (y > end)
		{
			mr = &emptyrange;
			y2 = len1;
		}
		else	{
			id = map.findMappingId(y);
			mr = &map.getMapping(id)->to;
			y2 = map.getMapping(id + 1)->begin;
		}
		if ((y2 - y) > left_column_padding) y2 = y + left_column_padding;
		if (y2 > (y + len1)) y2 = y + len1;
		printmapping(g + y, y2 - y, *mr, g2.c_str(), g2.len(), left_column_padding);
		y = y2;
	}
}

ModelDisplayMap::ModelDisplayMap(Model &m)
	:model(m)
{
	joint_offset = max(10, (int(9 + model.getPartCount()) / 10) * 10);
	neuron_offset = joint_offset + max(10, (int(9 + model.getJointCount()) / 10) * 10);
	max_element = neuron_offset + max(10, (int(9 + model.getNeuroCount()) / 10) * 10);
	for (int i = 0; i < model.getPartCount(); i++)
		map.add(Model::partToMap(i), Model::partToMap(i), i, i);
	for (int i = 0; i < model.getJointCount(); i++)
		map.add(Model::jointToMap(i), Model::jointToMap(i), joint_offset + i, joint_offset + i);
	for (int i = 0; i < model.getNeuroCount(); i++)
		map.add(Model::neuroToMap(i), Model::neuroToMap(i), neuron_offset + i, neuron_offset + i);
}

void ModelDisplayMap::print(int left_column_padding)
{
	MultiMap combined_map;
	combined_map.addCombined(model.getMap(), getMap());
	static const SString t09("0123456789");
	SString g;
	for (int i = 0; i < max_element; i += 10)
		g += t09;
	printN(' ', left_column_padding);
	printf("   Parts     ");
	printN(' ', joint_offset - 10);
	printf("Joints    ");
	printN(' ', (neuron_offset - joint_offset) - 10);
	printf("Neurons\n");
	printConvMap(model.getGeno().getGenes(), g, combined_map, left_column_padding);
}
