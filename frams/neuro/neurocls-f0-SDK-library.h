// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.


// do not edit - generated automatically from "f0.def"
// (to be included in "neurolibrary.cpp")






     
static ParamEntry NI_StdNeuron_tab[]={
{"Neuron",1, 4 ,"N",},

{"in",1,0,"Inertia","f 0.0 1.0 0.8",},
{"fo",1,0,"Force","f 0.0 999.0 0.04",},
{"si",1,0,"Sigmoid","f -99999.0 99999.0 2.0",},
{"s",2,0,"State","f -1.0 1.0 0.0",},
 
{0,0,0,},};
addClass(new NeuroClass(NI_StdNeuron_tab,"Standard neuron",-1,1,0, NULL,false, 2));

     
static ParamEntry NI_StdUNeuron_tab[]={
{"Unipolar neuron [EXPERIMENTAL!]",1, 4 ,"Nu",},
{"in",1,0,"Inertia","f 0.0 1.0 0.8",},
{"fo",1,0,"Force","f 0.0 999.0 0.04",},
{"si",1,0,"Sigmoid","f -99999.0 99999.0 2.0",},
{"s",2,0,"State","f -1.0 1.0 0.0",},
 
{0,0,0,},};
addClass(new NeuroClass(NI_StdUNeuron_tab,"Works like standard neuron (N) but the output value is scaled to 0...+1 instead of -1...+1.\nHaving 0 as one of the saturation states should help in \"gate circuits\", where input signal is passed through or blocked depending on the other singal.",-1,1,0, NULL,false, 0));

     static int Gyro_xy[]={83,8,7,100,50,90,50,90,40,70,40,80,50,70,60,90,60,90,50,12,43,24,48,24,48,19,38,19,38,24,43,24,43,54,48,54,48,64,43,69,38,64,38,54,43,54,5,63,69,58,74,48,79,38,79,28,74,23,69,1,43,79,43,74,1,23,69,26,66,1,63,69,60,66,1,55,76,53,73,1,31,75,33,72};   
static ParamEntry NI_Gyro_tab[]={
{"Gyroscope",1, 0 ,"G",},


 
{0,0,0,},};
addClass(new NeuroClass(NI_Gyro_tab,"Equilibrium sensor.\n0=the stick is horizontal\n+1/-1=the stick is vertical",0,1,2, Gyro_xy,false, 32));

     static int Touch_xy[]={43,2,7,100,50,90,50,90,40,70,40,80,50,70,60,90,60,90,50,11,75,50,65,50,60,55,55,45,50,55,45,45,40,50,35,50,30,45,25,50,30,55,35,50};   
static ParamEntry NI_Touch_tab[]={
{"Touch",1, 1 ,"T",},


{"r",1,0,"Range","f 0.0 1.0 1.0",},
 
{0,0,0,},};
addClass(new NeuroClass(NI_Touch_tab,"Touch sensor.\n-1=no contact\n0=just touching\n>0=pressing, value depends on the force applied",0,1,1, Touch_xy,false, 32));

     static int Smell_xy[]={64,5,7,100,50,90,50,90,40,70,40,80,50,70,60,90,60,90,50,3,10,40,15,45,15,55,10,60,5,20,30,25,35,30,45,30,55,25,65,20,70,4,15,35,20,40,22,50,20,60,15,65,5,75,50,50,50,45,45,40,50,45,55,50,50};   
static ParamEntry NI_Smell_tab[]={
{"Smell",1, 0 ,"S",},


 
{0,0,0,},};
addClass(new NeuroClass(NI_Smell_tab,"Smell sensor. Aggregated \"smell of energy\" experienced from all energy objects (creatures and food pieces).\nClose objects have bigger influence than the distant ones: for each energy source, its partial feeling is proportional to its energy/(distance^2)",0,1,1, Smell_xy,false, 32));

     static int Const_xy[]={29,4,4,26,27,26,73,73,73,73,27,26,27,1,73,50,100,50,1,56,68,46,68,2,41,47,51,32,51,68};   
static ParamEntry NI_Const_tab[]={
{"Constant",1, 0 ,"*",},


 
{0,0,0,},};
addClass(new NeuroClass(NI_Const_tab,"Constant value",0,1,0, Const_xy,false, 1));

     static int BendMuscle_xy[]={63,6,5,25,40,35,40,45,50,35,60,25,60,25,40,4,65,85,65,50,75,50,75,85,65,85,3,65,56,49,29,57,24,72,50,4,68,53,70,53,70,55,68,55,68,53,5,50,21,60,15,70,14,79,15,87,20,81,10,1,86,20,77,21};   
static ParamEntry NI_BendMuscle_tab[]={
{"Bend muscle",1, 2 ,"|",},


{"p",0,0,"power","f 0.01 1.0 0.25",},
{"r",0,0,"bending range","f 0.0 1.0 1.0",},
 
{0,0,0,},};
addClass(new NeuroClass(NI_BendMuscle_tab,"",1,0,2, BendMuscle_xy,false, 2+16+64+4));

     static int RotMuscle_xy[]={62,5,5,25,40,35,40,45,50,35,60,25,60,25,40,4,65,85,65,50,75,50,75,85,65,85,1,69,10,77,17,10,59,15,57,17,57,22,60,26,69,27,78,26,82,21,82,16,79,12,69,10,80,6,3,65,50,65,20,75,20,75,50};   
static ParamEntry NI_RotMuscle_tab[]={
{"Rotation muscle",1, 1 ,"@",},


{"p",0,0,"power","f 0.01 1.0 1.0",},
 
{0,0,0,},};
addClass(new NeuroClass(NI_RotMuscle_tab,"",1,0,2, RotMuscle_xy,false, 2+16+128+4));

     static int Diff_xy[]={24,3,3,25,0,25,100,75,50,25,0,1,75,50,100,50,3,44,42,51,57,36,57,44,42};   
static ParamEntry NI_Diff_tab[]={
{"Differentiate",1, 0 ,"D",},

 
{0,0,0,},};
addClass(new NeuroClass(NI_Diff_tab,"Calculate the difference between the current and previous input value. Multiple inputs are aggregated with respect to their weights",-1,1,0, Diff_xy,false, 0));

     static int FuzzyNeuro_xy[]={44,5,2,30,65,37,37,44,65,3,37,65,44,37,51,37,58,65,2,51,65,58,37,65,65,6,100,50,70,50,70,25,25,10,25,90,70,75,70,50,1,70,65,25,65};   
static ParamEntry NI_FuzzyNeuro_tab[]={
{"Fuzzy system [EXPERIMENTAL!]",1, 4 ,"Fuzzy",},

{"ns",0,0,"number of fuzzy sets","d 1  ",},
{"nr",0,0,"number of rules","d 1  ",},
{"fs",0,0,"fuzzy sets","s 0 -1 ",},
{"fr",0,0,"fuzzy rules","s 0 -1 ",},
 
{0,0,0,},};
addClass(new NeuroClass(NI_FuzzyNeuro_tab,"Refer to publications to learn more about this neuron.",-1,1,0, FuzzyNeuro_xy,false, 0));

     
static ParamEntry NI_Sticky_tab[]={
{"Sticky [EXPERIMENTAL!]",1, 0 ,"Sti",},

 
{0,0,0,},};
addClass(new NeuroClass(NI_Sticky_tab,"",1,0,1, NULL,false, 16));

     
static ParamEntry NI_LinearMuscle_tab[]={
{"Linear muscle [EXPERIMENTAL!]",1, 1 ,"LMu",},

{"p",0,0,"power","f 0.01 1.0 1.0",},
 
{0,0,0,},};
addClass(new NeuroClass(NI_LinearMuscle_tab,"",1,0,2, NULL,false, 16));

     
static ParamEntry NI_WaterDetect_tab[]={
{"Water detector",1, 0 ,"Water",},

 
{0,0,0,},};
addClass(new NeuroClass(NI_WaterDetect_tab,"Output signal:\n0=on or above water surface\n1=under water (deeper than 1)\n0..1=in the transient area just below water surface",0,1,1, NULL,false, 32));

     
static ParamEntry NI_Energy_tab[]={
{"Energy level",1, 0 ,"Energy",},

 
{0,0,0,},};
addClass(new NeuroClass(NI_Energy_tab,"The current energy level divided by the initial energy level.\nUsually falls from initial 1.0 down to 0.0 and then the creature dies. It can rise above 1.0 if enough food is ingested",0,1,0, NULL,false, 32));

     static int Channelize_xy[]={57,10,4,25,0,25,100,75,70,75,30,25,0,1,75,50,100,50,1,70,50,55,50,1,30,80,55,50,1,30,20,55,50,1,30,35,55,50,1,30,45,55,50,1,30,55,55,50,1,61,53,65,47,1,30,65,55,50};   
static ParamEntry NI_Channelize_tab[]={
{"Channelize",1, 0 ,"Ch",},

 
{0,0,0,},};
addClass(new NeuroClass(NI_Channelize_tab,"Combines all input signals into a single multichannel output; Note: ChSel and ChMux are the only neurons which support multiple channels. Other neurons discard everything except the first channel.",-1,1,0, Channelize_xy,false, 0));

     static int ChMux_xy[]={52,7,4,25,0,25,100,75,70,75,30,25,0,1,75,50,100,50,1,70,50,55,50,3,50,55,55,50,50,45,50,55,3,30,67,45,67,45,50,50,50,1,35,70,39,64,2,30,33,53,33,53,48};   
static ParamEntry NI_ChMux_tab[]={
{"Channel multiplexer",1, 0 ,"ChMux",},

 
{0,0,0,},};
addClass(new NeuroClass(NI_ChMux_tab,"Outputs the selected channel from the second (multichannel) input. The first input is used as the selector value (-1=select first channel, .., 1=last channel)",2,1,0, ChMux_xy,false, 0));

     static int ChSel_xy[]={41,6,4,25,0,25,100,75,70,75,30,25,0,1,75,50,100,50,1,70,50,55,50,3,50,55,55,50,50,45,50,55,1,30,50,50,50,1,35,53,39,47};   
static ParamEntry NI_ChSel_tab[]={
{"Channel selector",1, 1 ,"ChSel",},

{"ch",0,0,"channel","d   ",},
 
{0,0,0,},};
addClass(new NeuroClass(NI_ChSel_tab,"Outputs a single channel (selected by the \"ch\" parameter) from multichannel input",1,1,0, ChSel_xy,false, 0));

     
static ParamEntry NI_Random_tab[]={
{"Random noise",1, 0 ,"Rnd",},
 
{0,0,0,},};
addClass(new NeuroClass(NI_Random_tab,"Generates random noise (subsequent random values in the range of -1..+1)",0,1,0, NULL,false, 0));

     static int Sinus_xy[]={46,3,12,75,50,71,37,62,28,50,25,37,28,28,37,25,50,28,62,37,71,50,75,62,71,71,62,75,50,1,75,50,100,50,5,35,50,40,35,45,35,55,65,60,65,65,50};   
static ParamEntry NI_Sinus_tab[]={
{"Sinus generator",1, 2 ,"Sin",},

{"f0",0,0,"base frequency","f -1.0 1.0 0.06283185307",},
{"t",0,0,"time","f 0 6.283185307 0",},
 
{0,0,0,},};
addClass(new NeuroClass(NI_Sinus_tab,"Output frequency = f0+input",1,1,0, Sinus_xy,false, 0));
