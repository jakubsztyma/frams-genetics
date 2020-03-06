// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <common/virtfile/stdiofile.h>

#include <frams/genetics/preconfigured.h>
#include <frams/model/model.h>
#include <common/loggers/loggertostdout.h>
#include <common/virtfile/stringfile.h>

int main(int argc,char*argv[])
{
LoggerToStdout messages_to_stdout(LoggerBase::Enable);
PreconfiguredGenetics genetics;

SString gen(argc>1?argv[1]:"X[|G:1.23]");
if (!strcmp(gen.c_str(),"-"))
	{
	gen=0;
	StdioFILEDontClose in(stdin);
	loadSString(&in,gen);
	}
Geno g(gen);
printf("\nSource genotype: '%s'\n",g.getGenes().c_str());
printf("                  ( format %c %s)\n",
       g.getFormat(), g.getComment().c_str());

Model m(g);//.getConverted('0'));

if (!m.isValid())
	{
	printf("Cannot build Model from this genotype!\n");
	return 2;	
	}
printf("Converted to f0:\n%s\n",m.getF0Geno().getGenes().c_str());

printf("\nusing Param::saveMultiLine() to create the \"expanded\" form of the f0 genotype...\n(MultiParamLoader should be able to load this)");

StringFILE2 f;

static Param modelparam(f0_model_paramtab);
static Param partparam(f0_part_paramtab);
static Param jointparam(f0_joint_paramtab);
static Param neuroparam(f0_neuro_paramtab);
static Param connparam(f0_neuroconn_paramtab);

modelparam.select(&m);
modelparam.saveMultiLine(&f,"m");

Part *p;
Joint *j;
Neuro *n;

for (int i=0;p=(Part*)m.getPart(i);i++)
	{
	partparam.select(p);
	partparam.saveMultiLine(&f,"p");
	}
for (int i=0;j=(Joint*)m.getJoint(i);i++)
	{
	jointparam.select(j);
	jointparam.setParamTab(j->usedelta?f0_joint_paramtab:f0_nodeltajoint_paramtab);
	jointparam.saveMultiLine(&f,"j");
	}
for (int i=0;n=(Neuro*)m.getNeuro(i);i++)
	{
	neuroparam.select(n);
	neuroparam.saveMultiLine(&f,"n");
	}
for (int a=0;n=(Neuro*)m.getNeuro(a);a++)
	{ // inputs
	for (int b=0;b<n->getInputCount();b++)
		{
		double w;
		NeuroConn nc;
		Neuro* n2=n->getInput(b,w);
		nc.n1_refno=n->refno; nc.n2_refno=n2->refno;
		nc.weight=w;
		nc.info=n->getInputInfo(b);
		connparam.select(&nc);
		connparam.saveMultiLine(&f,"c");
		}
	}

printf("\n============================\n%s\n",f.getString().c_str());

return 0;
}

/*********************** EXAMPLE OUTPUT *********************************

Source genotype: 'X[|G:1.23]'
                  ( format 1 )
Converted to f0:
p:
p:1
j:0, 1, dx=1
n:p=1
n:j=0, d="|:p=0.25,r=1"
n:j=0, d=G
c:0, 2, 1.23
c:1, 0


using Param::saveMultiLine() to create the "expanded" form of the f0 genotype...
(MultiParamLoader should be able to load this)
============================
m:
se:1
Vstyle:

p:
x:0
y:0
z:0
m:1
s:1
dn:1
fr:0.4
ing:0.25
as:0.25
rx:0
ry:0
rz:0
i:
Vstyle:part

p:
x:1
y:0
z:0
m:1
s:1
dn:1
fr:0.4
ing:0.25
as:0.25
rx:0
ry:0
rz:0
i:
Vstyle:part

j:
p1:0
p2:1
rx:0
ry:0
rz:0
dx:1
dy:0
dz:0
stif:1
rotstif:1
stam:0.25
i:
Vstyle:joint

n:
p:1
j:-1
d:N
i:
Vstyle:neuro

n:
p:-1
j:0
d:|:p=0.25,r=1
i:
Vstyle:neuro

n:
p:-1
j:0
d:G
i:
Vstyle:neuro

c:
n1:0
n2:2
w:1.23
i:

c:
n1:1
n2:0
w:1
i:


*************************************************************************/
