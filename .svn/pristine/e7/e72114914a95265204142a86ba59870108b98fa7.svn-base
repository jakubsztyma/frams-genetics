// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FB_CONV_H_
#define _FB_CONV_H_

#include <frams/genetics/genoconv.h>
#include "../fH/fH_general.h"

class GenoConv_fBH : public GenoConverter
{
private:
	bool getNextCharId(const SString& genotype, int &i);
	double convertCharacterTo01(char c);
	double convertCharacterToWeight(char c);
	static fH_Handle* convertCharacterToHandle(char c, int dims, int start, int end, std::vector<IRange> ranges[3]);
	int processNextLetter(fH_Builder &creature, fH_Handle *&currhandle, Param &par, const SString& gene, int &propindex, int &i, std::vector<IRange> ranges[3], int &nclassdefcount);
	bool getNeuroClass(const SString& gene, SString &def, int nclassdefcount);

public:
	GenoConv_fBH() :GenoConverter()
	{
		name = "Biological encoding";

		in_format = 'B';
		out_format = 'H';
		mapsupport = 1;
	}
	SString convert(SString &i, MultiMap *map, bool using_checkpoints);
	~GenoConv_fBH() {};
};

#endif //_FB_CONV_H_
