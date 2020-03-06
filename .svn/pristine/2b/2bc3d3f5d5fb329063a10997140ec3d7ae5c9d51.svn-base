// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <frams/genetics/geno.h>
#include <common/virtfile/stdiofile.h>
#include <frams/util/sstringutils.h>
#include <frams/genetics/preconfigured.h>
#include <frams/neuro/neuroimpl.h>
#include <frams/neuro/neurofactory.h>
#include <common/loggers/loggertostdout.h>

/**
 @file
 Sample code: Neural network tester (can run your custom neurons)
*/

#ifndef SDK_WITHOUT_FRAMS
#include <frams/mech/creatmechobj.h>
int CreatMechObject::modeltags_id=0;
int CreatMechObject::mechtags_id=0;
#endif

ParamEntry creature_paramtab[]={0};

#ifdef VEYETEST
#include <frams/neuro/impl/neuroimpl-vectoreye.h>

#define N_VEye 0
#define N_VMotor 1
#define N_Mode 2
#define N_Fitness 3
#define LEARNINGSTEPS 50

void veyeStep(Model &m,int step)
{
  static float angle=0;

  NeuroNetImpl::getImpl(m.getNeuro(N_Mode))->setState(step>=LEARNINGSTEPS); //0 (learning) or 1 (normal)

	NeuroImpl *ni=NeuroNetImpl::getImpl(m.getNeuro(N_VEye));
  ((NI_VectorEye*)ni)->relpos.y=0;
  ((NI_VectorEye*)ni)->relpos.z=0;
  if (NeuroNetImpl::getImpl(m.getNeuro(N_Mode))->getNewState()<0.5)
  { //learning
    ((NI_VectorEye*)ni)->relpos.x=5.0*sin(2*M_PI*step/LEARNINGSTEPS);
  }
  else
  { //VMotor controls location of VEye
    angle+=NeuroNetImpl::getImpl(m.getNeuro(N_VMotor))->getState();
    angle=fmod((double)angle,M_PI*2.0);
    ((NI_VectorEye*)ni)->relpos.x=5*sin(angle);
  }

  NeuroNetImpl::getImpl(m.getNeuro(N_Fitness))->setState(angle); //wymaga poprawy
  //oraz trzeba przemyslec kolejnosc get/set'ow neuronow zeby sygnal sie dobrze propagowal.
}
#endif

int main(int argc,char*argv[])
{
LoggerToStdout messages_to_stdout(LoggerBase::Enable);
PreconfiguredGenetics genetics;

if (argc<=1)
	{
		puts("Parameters: <genotype> [number of simulation steps]");
	  return 10;
	}
SString gen(argv[1]);
if (!strcmp(gen.c_str(),"-"))
	{
	gen=0;
	StdioFILEDontClose in(stdin);
	loadSString(&in,gen);
	}
Geno g(gen);
if (!g.isValid()) {puts("invalid genotype");return 5;}
Model m(g);
if (!m.getNeuroCount()) {puts("no neural network");return 1;}
printf("%d neurons,",m.getNeuroCount());
NeuroFactory neurofac;
neurofac.setStandardImplementation();
NeuroNetConfig nn_config(&neurofac);
NeuroNetImpl *nn=new NeuroNetImpl(m,nn_config);
int i; Neuro *n;
if (!nn->getErrorCount()) printf(" no errors\n");
else
	{
	printf(" %d errors:",nn->getErrorCount());
	int no_impl=0; SString no_impl_names;
	int init_err=0; SString init_err_names;
	for(i=0;i<m.getNeuroCount();i++)
		{
		n=m.getNeuro(i);
		NeuroImpl *ni=NeuroNetImpl::getImpl(n);
		if (!ni)
			{
			if (no_impl) no_impl_names+=',';
			no_impl_names+=SString::sprintf("#%d.%s",i,n->getClassName().c_str());
			no_impl++;
			}
		else if (ni->status==NeuroImpl::InitError)
			{
			if (init_err) init_err_names+=',';
			init_err_names+=SString::sprintf("#%d.%s",i,n->getClassName().c_str());
			init_err++;
			}
		}
	printf("\n");
	if (no_impl) printf("%d x missing implementation (%s)\n",no_impl,no_impl_names.c_str());
	if (init_err) printf("%d x failed initialization (%s)\n",init_err,init_err_names.c_str());
	}
int steps=1;
if (argc>2) steps=atol(argv[2]);
int st;
printf("step");
for(i=0;i<m.getNeuroCount();i++)
	{
	n=m.getNeuro(i);
	printf("\t#%d.%s",i,n->getClassName().c_str());
	}
printf("\n");
for(st=0;st<=steps;st++)
	{
#ifdef VEYETEST
  veyeStep(m,st);
#endif
	printf("%d",st);
	for(i=0;i<m.getNeuroCount();i++)
		{
		n=m.getNeuro(i);
		printf("\t%g",n->state);
		}
	printf("\n");
	nn->simulateNeuroNet();
	}
neurofac.freeImplementation();
}
