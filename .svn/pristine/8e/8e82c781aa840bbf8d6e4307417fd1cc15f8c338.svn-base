// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2016  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <common/virtfile/stdiofile.h>

#include <frams/model/model.h>
#include <common/loggers/loggertostdout.h>
#include <frams/genetics/preconfigured.h>

/**
 @file
 Sample code: Adding/removing default properties in f0 genotypes
 Usage: full_props [-r[everse]] [genotype_or_stdin]

 Calling examples:

 # convert f1 -> f0 + explicitly set all properties to their default values  (the result is still valid f0)
 ./full_props "X[|]arX[-1:1,fo:0.2]"

   //0
   p:x=0, y=0, z=0, m=1, s=1, dn=1, fr=0.4, ing=0.25, as=0.25, rx=0, ry=0, rz=0, i=, Vstyle=part, vs=0.2, vr=0.5, vg=0.5, vb=0.5
   p:x=1, y=0, z=0, m=2, s=1, dn=1, fr=0.4, ing=0.25, as=0.25, rx=0, ry=0, rz=0, i=, Vstyle=part, vs=0.2, vr=0.5, vg=0.5, vb=0.5
   p:x=2, y=0, z=0, m=1, s=1, dn=1, fr=0.4, ing=0.277778, as=0.166667, rx=0, ry=0, rz=0, i=, Vstyle=part, vs=0.2, vr=0.5, vg=0.5, vb=0.5
   j:p1=0, p2=1, rx=0, ry=0, rz=0, dx=1, dy=0, dz=0, stif=1, rotstif=1, stam=0.25, i=, Vstyle=joint, vr=0.5, vg=0.5, vb=0.5
   j:p1=1, p2=2, rx=-0.7853, ry=0, rz=0, dx=1, dy=0, dz=0, stif=1, rotstif=1, stam=0.277778, i=, Vstyle=joint, vr=0.5, vg=0.5, vb=0.5
   n:p=-1, j=0, d="|:p=0.25, r=1", i=, Vstyle=neuro
   n:p=2, j=-1, d="N:in=0.8, fo=0.2, si=2, s=0", i=, Vstyle=neuro
   c:n1=1, n2=0, w=1, i=

 # convert the previous output to the regular f0 format (removing redundant default properties)
 ./full_props "X[|]arX[-1:1,fo:0.2]" | ./full_props -r

   //0
   p:
   p:1, m=2
   p:2, ing=0.277778, as=0.166667
   j:0, 1, dx=1
   j:1, 2, rx=-0.7853, dx=1, stam=0.277778
   n:j=0, d=|
   n:p=2, d=N:fo=0.2
   c:1, 0

*/

int main(int argc,char*argv[])
{
StdioFILE::setStdio();//setup VirtFILE::Vstdin/out/err
LoggerToStdout messages_to_stderr(LoggerBase::Enable | LoggerBase::DontBlock,VirtFILE::Vstderr); //errors -> stderr, don't interfere with stdout

PreconfiguredGenetics genetics;

bool reverse=false;
char* gen_arg=0;
for(int i=1;i<argc;i++)
	{
	char* ar=argv[i];
	if (ar[0]=='-')
		switch(ar[1])
			{
			case 'r': reverse=true; break;
			case 'h': puts("usage: full_props [-r[everse]] [genotype_or_stdin]\n"); break;
			}
	else
		if (!gen_arg)
			gen_arg=ar;
	}
SString gen;
if (gen_arg)
	gen=gen_arg;
else
	loadSString(VirtFILE::Vstdin,gen);
Geno g(gen);
Model m(g);

if (!m.isValid())
	{
	logPrintf("","full_props",LOG_ERROR,"Cannot build Model from the supplied genotype\n");
	return 2;	
	}

m.open();
for(int i=0;i<m.getNeuroCount();i++)
	{
	Neuro *n=m.getNeuro(i);
	SyntParam p=n->classProperties(reverse);
	p.update();// ...so everyone reading the source can recognize that p was created to update the neuro d field
	// but actually, calling update() here is not necessary, because ~SyntParam() would do it anyway
	}
m.close();

// normal f0 (omitting default values) would be retrieved using m.getF0Geno()
// the following form allows for more control:
Geno f0_g;
m.makeGeno(f0_g,NULL,reverse);//third arg is "handle_defaults" == whether f0 should omit default property values
puts(f0_g.getGenesAndFormat().c_str());

return 0;
}

