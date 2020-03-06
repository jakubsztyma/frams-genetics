// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "fn_conv.h"
#include <frams/vm/classes/collectionobj.h>

GenoConv_fn0::GenoConv_fn0()
{
	name = "Vector of real values, no phenotype"; //for numerical optimization; custom fitness function must be provided in script. See oper_fn.cpp for more details.
	in_format = 'n';
	out_format = '0';
	mapsupport = 0;
}



SString GenoConv_fn0::convert(SString &in, MultiMap *map, bool using_checkpoints)
{
	vector<double> values = stringToVector(in.c_str());
	if (values.size() == 0) //invalid input genotype?
		return ""; //so we return an invalid f0 genotype

	return SString("p:\n"); //phenotype not relevant for this genetic encoding
}



vector<double> GenoConv_fn0::stringToVector(const char *input) //returns empty vector on error
{
	vector<double> empty;
	ExtValue val;
	const char* after_des = val.deserialize(input);
	if (after_des == NULL) //deserialization failed
	{
		logPrintf("GenoConv_fn0", "stringToVector", LOG_ERROR, "Unable to deserialize - expecting a vector of real values, got '%s'", input);
		return empty;
	}
	if (after_des[0] != '\0') //not everything was consumed
	{
		logPrintf("GenoConv_fn0", "stringToVector", LOG_ERROR, "Extra characters after deserialized '%s'", input);
		return empty;
	}

	VectorObject *vec = VectorObject::fromObject(val.getObject(), false);
	if (vec)
	{
		vector<double> output;
		for (int i = 0; i < vec->data.size(); i++)
		{
			ExtValue* val = (ExtValue*)vec->data(i);
			if (val == NULL)
			{
				logPrintf("GenoConv_fn0", "stringToVector", LOG_ERROR, "Expecting a real value in a vector, got NULL");
				return empty;
			}
			else
				output.push_back(val->getDouble());
		}
		return output;
	}
	else
	{
		logPrintf("GenoConv_fn0", "stringToVector", LOG_ERROR, "Expecting a vector of real values, got '%s'", input);
		return empty;
	}
}


string GenoConv_fn0::vectorToString(const vector<double> vec)
{
	char buffer[32];
	string out = "[";
	for (unsigned int i = 0; i < vec.size(); i++)
	{
		if (i > 0)
			out += ", ";
		snprintf(buffer, sizeof(buffer), "%.8g", vec[i]);
		out += buffer;
	}
	return out + "]";
}
