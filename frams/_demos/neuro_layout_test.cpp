// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <common/virtfile/stdiofile.h>
#include <frams/util/sstringutils.h>
#include <frams/genetics/preconfigured.h>
#include <frams/model/model.h>
#include <common/loggers/loggertostdout.h>
#include <frams/canvas/nn_layout_model.h>

#include <algorithm>

/**
 @file
 Sample code: Neuron layout tester

 Hint: Use loader_test to extract genotypes from Framsticks *.gen files:
 loader_test "data/walking.gen" "Walking Lizard" | neuro_layout_test -
*/

// stl is fun? ;-) ForwardIterator implementation for element coordinates (required by min_element/max_element)
template <int MEMBER> struct NNIter: public std::iterator<std::forward_iterator_tag,int> //MEMBER: 0..3=x/y/w/h
{
NNLayoutState *nn; int index;
NNIter() {}
NNIter(NNLayoutState *_nn, int _index):nn(_nn),index(_index) {}
int operator*() {return nn->GetXYWH(index)[MEMBER];}
NNIter& operator++() {index++; return *this;}
bool operator!=(const NNIter& it) {return index!=it.index;}
bool operator==(const NNIter& it) {return index==it.index;}

static NNIter begin(NNLayoutState *_nn) {return NNIter(_nn,0);}
static NNIter end(NNLayoutState *_nn) {return NNIter(_nn,_nn->GetElements());}
};

class Screen
{
int min_x,max_x,min_y,max_y,scale_x,scale_y;
int rows,columns;
char* screen;

public:

Screen(int _min_x,int _max_x,int _min_y,int _max_y,int _scale_x,int _scale_y)
	:min_x(_min_x),max_x(_max_x),min_y(_min_y),max_y(_max_y),scale_x(_scale_x),scale_y(_scale_y)
		{
		columns=(max_x-min_x+scale_x-1)/scale_x;
		rows=(max_y-min_y+scale_y-1)/scale_y;
		screen=new char[rows*columns];
		memset(screen,' ',rows*columns);
		}

~Screen()
		{
		delete[] screen;
		}

void put(int x,int y,const char *str)
	{
	x=(x-min_x)/scale_x;
	y=(y-min_y)/scale_y;
	if (x<0) return;
	if (y<0) return;
	if (y>=rows) return;
	for(;*str;str++,x++)
		{
		if (x>=columns) return;
		screen[columns*y+x]=*str;
		}
	}

void print()
	{
	for(int y=0;y<rows;y++)
		{
		fwrite(&screen[columns*y],1,columns,stdout);
		printf("\n");
		}
	}
};

int main(int argc,char*argv[])
{
LoggerToStdout messages_to_stdout(LoggerBase::Enable);
PreconfiguredGenetics genetics;

if (argc<=1)
	{
		puts("Parameters:\n"
		     " 1. Genotype (or - character indicating the genotype will be read from stdin)\n"
		     " 2. (Optional) layout type (the only useful layout is 2, which is the default, see nn_simple_layout.cpp");
	  return 10;
	}
SString gen(argv[1]);
if (!strcmp(gen.c_str(),"-"))
	{
	gen=0;
	StdioFILEDontClose in(stdin);
	loadSString(&in,gen);
	}
int layout_type=2;
if (argc>2) layout_type=atol(argv[2]);
Geno g(gen);
if (!g.isValid()) {puts("invalid genotype");return 5;}
Model m(g);
if (!m.getNeuroCount()) {puts("no neural network");return 1;}
printf("%d neurons,",m.getNeuroCount());

NNLayoutState_Model nn_layout(&m);
struct NNLayoutFunction &nnfun=nn_layout_functions[layout_type];
printf(" using layout type=%d (%s)\n",layout_type,nnfun.name);
nnfun.doLayout(&nn_layout);

for(int i=0;i<nn_layout.GetElements();i++)
	{
	int *xywh=nn_layout.GetXYWH(i);
	printf("#%-3d %s\t%d,%d\t%dx%d\n",i,m.getNeuro(i)->getClassName().c_str(),
	       xywh[0],xywh[1],xywh[2],xywh[3]);
	}

Screen screen(*std::min_element(NNIter<0>::begin(&nn_layout),NNIter<0>::end(&nn_layout))-30,
	      *std::max_element(NNIter<0>::begin(&nn_layout),NNIter<0>::end(&nn_layout))+70,
	      *std::min_element(NNIter<1>::begin(&nn_layout),NNIter<1>::end(&nn_layout)),
	      *std::max_element(NNIter<1>::begin(&nn_layout),NNIter<1>::end(&nn_layout))+30,
	      10,35);

printf("===========================================\n");
for(int i=0;i<nn_layout.GetElements();i++)
	{
	int *xywh=nn_layout.GetXYWH(i);
	SString label=SString::sprintf("%d:%s",i,m.getNeuro(i)->getClassName().c_str());
	screen.put(xywh[0],xywh[1],label.c_str());
	}
screen.print();
printf("===========================================\n");

}
