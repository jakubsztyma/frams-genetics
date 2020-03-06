// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "autoname.h"
#include "common/nonstd_stl.h"
#include <ctype.h>

SString AutoName::makeName(Model &model)
{
	SString t;
	t = firstName(model);
	SString last=lastName(model);
	if (last.len()>0)
		{
		t += ' ';
		t += last;
		}
	return t;
}

///////////////////////////

static char* cat_syl(char* str, unsigned int x)
{
	static char sp[] = "bcdfghklmnprstwz", sa[] = "aeiouy";
	x %= 6 * 16;
	str[0] = sa[x % 6];
	if (x > 5)
	{
		str[1] = sp[x / 6]; str[2] = 0; return str + 2;
	}
	else
	{
		str[1] = 0; return str + 1;
	}
}

SString AutoName::firstName(const SString& g)
{
	char buf[8];
	unsigned int s1 = 0, s2 = 0, s3 = 0;
	const char *x = g.c_str();
	if (*x==0) return SString();
	for (; *x; x++) { s1 += *x; s2 = s2**x + *x; s3 = (s3^*x) + *x; }
	char* t = buf;
	t = cat_syl(t, s1);
	t = cat_syl(t, s2);
	t = cat_syl(t, s3);
	buf[0] = (char)toupper(buf[0]);
	return SString(buf);
}

SString AutoName::firstName(Model& model)
{
	return firstName(model.getGeno().getGenes());
}

static void przeplatanka(char *out, char *in1, char *in2)
{
	int d1 = strlen(in1), d2 = strlen(in2);
	int p1 = 0, p2 = 0;
	int pp = d1 + d2;
	int i, p;
	if (d2 < d1)
	{
		int t = d2; d2 = d1; d1 = t;
		char *c = in1; in1 = in2; in2 = c;
	}
	if (pp)
		for (i = 0; i <= pp; i++)
		{
		p = (i*d1) / pp - 1; for (; p1 <= p; p1++) *(out++) = *(in1++);
		p = (i*d2) / pp - 1; for (; p2 <= p; p2++) *(out++) = *(in2++);
		}
	*out = 0;
}

SString AutoName::lastName(Model& model)
{
	char Sam[] = "yeaou";
	char Sp[] = "shtkdgmr";

#define NAME_MAXLENBODY 5
#define NAME_MAXLENBRAIN 5
#define NAME_BODYLEN 0.8
#define NAME_BRAINLEN 0.8
#define NAME_BODYMASS 1.0
#define NAME_BRAININP 1.0

	char naz[NAME_MAXLENBODY + NAME_MAXLENBRAIN + 1];
	int poz, nextpoz, i;

	double w;
	int cialo = -1;
	int mozg = -1;

	char tmpc[NAME_MAXLENBODY + 1], tmpm[NAME_MAXLENBRAIN + 1];

	if (model.getPartCount() > 0)
	{
		cialo = min((int)(sqrt(double(model.getPartCount()) - 1)*NAME_BODYLEN), NAME_MAXLENBODY - 1);
		poz = 0;
		for (i = 0; i <= cialo; i++) // budowanie "opisu" ciala
		{
			nextpoz = ((model.getPartCount())*(i + 1)) / (cialo + 1) - 1;
			w = 1.0;
			for (; poz <= nextpoz; poz++) w = max(w, model.getPart(poz)->mass);
			tmpc[i] = Sp[min(int((w - 1.0)*NAME_BODYMASS), int(sizeof(Sp)) - 2)];
		}
		tmpc[i] = 0;
	}
	else tmpc[0] = 0;

	int wint;

	if (model.getNeuroCount() > 0)
	{
		mozg = min((int)(sqrt((double)model.getNeuroCount())*NAME_BRAINLEN), NAME_MAXLENBRAIN - 1);
		poz = 0;
		for (i = 0; i <= mozg; i++) // budowanie "opisu" mozgu
		{
			nextpoz = (model.getNeuroCount()*(i + 1)) / (mozg + 1) - 1;
			wint = 0;
			for (; poz <= nextpoz; poz++) wint = max(wint, model.getNeuro(poz)->getInputCount());
			tmpm[i] = Sam[min(int(wint*NAME_BRAININP), int(sizeof(Sam)) - 2)];
		}
		tmpm[i] = 0;
	}
	else tmpm[0] = 0;

	if ((mozg + 1) < ((cialo + 2) / 2))
	{
		for (i = mozg + 1; i <= cialo / 2; i++) tmpm[i] = 'i';
		tmpm[i] = 0;
	}

	przeplatanka(naz, tmpc, tmpm);

	naz[0] = (char)toupper(naz[0]);
	return SString(naz);
}
