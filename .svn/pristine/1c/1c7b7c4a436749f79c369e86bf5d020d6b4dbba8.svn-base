// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "geneprops.h"

GeneProps GeneProps::standard_values;

GeneProps::GeneProps()
{
	length = 1.0;
	weight = 1.0;
	friction = 0.4;
	curvedness = 0.0;
	twist = 0.0;
	energy = 1.0;

	muscle_power = 0.25; // "biological" property
	assimilation = 0.25; // "biological" property
	stamina = 0.25; // "biological" property
	ingestion = 0.25; // "biological" property

	muscle_bend_range = 1.0;
	muscle_reset_range = true;

	visual_size = 0.2;
	cred = 0.5;
	cgreen = 0.5;
	cblue = 0.5;

	normalizeBiol4();
}

void GeneProps::normalizeBiol4()
{
	// make them sum to 1
	double sum = muscle_power + assimilation + stamina + ingestion;
	if (sum == 0)
	{
		muscle_power = assimilation = stamina = ingestion = 0.25;
	}
	else
	{
		muscle_power /= sum;
		assimilation /= sum;
		stamina /= sum;
		ingestion /= sum;
	}
}

int GeneProps::executeModifier(char modif)
{
	switch (modif)
	{
#ifdef v1f1COMPATIBLE
	case 'L': length += (3.0 - length)*0.3;
		length = min(length, Model::getMaxJoint().d.x); break;
#else
	case 'L': length += (2.0 - length)*0.3; //2.0 is currently Model::getMaxJoint().d.x so min() does not limit the range
		length = min(length, Model::getMaxJoint().d.x); break;
#endif
	case 'l': length += (0.33 - length)*0.3;
		length = max(length, Model::getMinJoint().d.x); break;

	case 'W': weight += (2.0 - weight)*0.3;  break;
	case 'w': weight += (0.5 - weight)*0.3;  break;
	case 'F': friction += (4 - friction)*0.2;  break;
	case 'f': friction -= friction*0.2;  break;
	case 'C': curvedness += (2.0 - curvedness) * 0.25; break;
	case 'c': curvedness += (-2.0 - curvedness) * 0.25; break;
	case 'Q': twist += (M_PI_2 - twist)*0.3; break;
	case 'q': twist += (-M_PI_2 - twist)*0.3; break;
	case 'E': energy += (10.0 - energy)*0.1;  break;
	case 'e': energy -= energy*0.1;	 break;

	case 'A': assimilation += (1 - assimilation)*0.8;	normalizeBiol4(); break;
	case 'a': assimilation -= assimilation*0.4;	normalizeBiol4(); break;
	case 'I': ingestion += (1 - ingestion)*0.8;	normalizeBiol4(); break;
	case 'i': ingestion -= ingestion * 0.4;	normalizeBiol4(); break;
	case 'S': stamina += (1 - stamina)*0.8;	normalizeBiol4(); break;
	case 's': stamina -= stamina*0.4;	normalizeBiol4(); break;
	case 'M': muscle_power += (1 - muscle_power)*0.8;	normalizeBiol4(); break;
	case 'm': muscle_power -= muscle_power*0.4;	normalizeBiol4(); break;

	case 'D': cred += (1.0 - cred)*0.25;  break;
	case 'd': cred += (0.0 - cred)*0.25;  break;
	case 'G': cgreen += (1.0 - cgreen)*0.25;  break;
	case 'g': cgreen += (0.0 - cgreen)*0.25;  break;
	case 'B': cblue += (1.0 - cblue)*0.25;  break;
	case 'b': cblue += (0.0 - cblue)*0.25;  break;
	case 'H': visual_size += (0.7 - visual_size)*0.25;  break;
	case 'h': visual_size += (0.05 - visual_size)*0.25;  break;

	default: return -1;
	}
	return 0;
}

void GeneProps::propagateAlong(bool use_f1_muscle_reset_range)
{
	length = 0.5*length + 0.5 * standard_values.length;
	weight += (standard_values.weight - weight) * 0.5;
	friction = 0.8 * friction + 0.2 * standard_values.friction;
	curvedness = 0.66 * curvedness;
	twist = 0.66 * twist;

	assimilation = 0.8 * assimilation + 0.2 * standard_values.assimilation;
	ingestion = 0.8 * ingestion + 0.2 * standard_values.ingestion;
	stamina = 0.8 * stamina + 0.2 * standard_values.stamina;
	muscle_power = 0.8 * muscle_power + 0.2 * standard_values.muscle_power;

	normalizeBiol4();

	visual_size = 0.5*visual_size + 0.5 * standard_values.visual_size;

	if (use_f1_muscle_reset_range)
	{
		if (muscle_reset_range) muscle_bend_range = 1.0; else muscle_reset_range = true;
	}
}
