// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2016  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <common/virtfile/stdiofile.h>

#include <frams/genetics/defgenoconv.h>
#include <frams/model/model.h>
#include <common/loggers/loggertostdout.h>

void save_as_f0(SString &gen,Model &m,bool omit_default_values)
{
// copied from Model::makeGeno() (with small changes)

static Param modelparam(f0_model_paramtab);
static Param partparam(f0_part_paramtab);
static Param jointparam(f0_joint_paramtab);
static Param neuroparam(f0_neuro_paramtab);
static Param connparam(f0_neuroconn_paramtab);

static Part defaultpart;
static Joint defaultjoint;
static Neuro defaultneuro;
static Model defaultmodel;
static NeuroConn defaultconn;

modelparam.select(&m);
gen+="m:";
modelparam.saveSingleLine(gen,omit_default_values ? &defaultmodel : NULL);

Part *p;
Joint *j;
Neuro *n;

for (int i=0;p=(Part*)m.getPart(i);i++)
	{
	partparam.select(p);
	gen+="p:";
	partparam.saveSingleLine(gen,omit_default_values ? &defaultpart : NULL);
	}
for (int i=0;j=(Joint*)m.getJoint(i);i++)
	{
	jointparam.select(j);
	jointparam.setParamTab(j->usedelta?f0_joint_paramtab:f0_nodeltajoint_paramtab);
	gen+="j:";
	jointparam.saveSingleLine(gen,omit_default_values ? &defaultjoint : NULL);
	}
for (int i=0;n=(Neuro*)m.getNeuro(i);i++)
	{
	neuroparam.select(n);
	gen+="n:";
	neuroparam.saveSingleLine(gen,omit_default_values ? &defaultneuro : NULL);
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
		gen+="c:";
		connparam.saveSingleLine(gen,omit_default_values ? &defaultconn : NULL);
		}
	}
}

int main(int argc,char*argv[])
{
LoggerToStdout messages_to_stdout(LoggerBase::Enable);

//without converters the application would only handle "format 0" genotypes
DefaultGenoConvManager gcm;
gcm.addDefaultConverters();
Geno::useConverters(&gcm);

Geno::Validators validators;
Geno::useValidators(&validators);
ModelGenoValidator model_validator;
validators+=&model_validator; //This simple validator handles all cases where a converter for a particular format is available but there is no genetic operator. Converters may be less strict in detecting invalid genotypes but it is better than nothing

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

printf("\nthis example shows how to save a f0 genotype using low-level ParamInterface::saveSingleLine() calls\n");

SString f0_skipping_defaults;
SString f0_no_skipping_defaults;

save_as_f0(f0_skipping_defaults,m,true);
save_as_f0(f0_no_skipping_defaults,m,false);

printf("\n==== with defdata (skips default values) ======\n%s\n",f0_skipping_defaults.c_str());
printf("\n==== without defdata (saves all fields) ======\n%s\n",f0_no_skipping_defaults.c_str());

return 0;
}

/*********************** EXAMPLE OUTPUT *********************************

Source genotype: 'X[|G:1.23]'
                  ( format 1 )

this example shows how to save a f0 genotype using low-level ParamInterface::saveSingleLine() calls

==== with defdata (skips default values) ======
m:
p:
p:1
j:0, 1, dx=1
n:p=1, d=N
n:j=0, d="|:p=0.25,r=1"
n:j=0, d=G
c:0, 2, 1.23
c:1, 0


==== without defdata (saves all fields) ======
m:se=1, Vstyle=
p:0, 0, 0, m=1, s=1, dn=1, fr=0.4, ing=0.25, as=0.25, rx=0, 0, 0, i=, Vstyle=part
p:1, 0, 0, m=1, s=1, dn=1, fr=0.4, ing=0.25, as=0.25, rx=0, 0, 0, i=, Vstyle=part
j:0, 1, rx=0, 0, 0, dx=1, 0, 0, stif=1, rotstif=1, stam=0.25, i=, Vstyle=joint
n:p=1, j=-1, d=N, i=, Vstyle=neuro
n:p=-1, j=0, d="|:p=0.25,r=1", i=, Vstyle=neuro
n:p=-1, j=0, d=G, i=, Vstyle=neuro
c:0, 2, 1.23, i=
c:1, 0, 1, i=

*************************************************************************/

