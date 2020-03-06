// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.


// do not edit - generated automatically from "f0.def"
// (to be included in "neurofactory.cpp")






#define FIELDSTRUCT NI_StdNeuron 
ParamEntry NI_StdNeuron_tab []={
{"Neuron",1, 4 ,"N",},
{"in",1,0,"Inertia","f 0.0 1.0 0.8",FIELD(inertia),},
{"fo",1,0,"Force","f 0.0 999.0 0.04",FIELD(force),},
{"si",1,0,"Sigmoid","f -99999.0 99999.0 2.0",FIELD(sigmo),},
{"s",2,0,"State","f -1.0 1.0 0.0",FIELD(newstate),},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_StdUNeuron 
ParamEntry NI_StdUNeuron_tab []={
{"Unipolar neuron [EXPERIMENTAL!]",1, 4 ,"Nu",},
{"in",1,0,"Inertia","f 0.0 1.0 0.8",FIELD(inertia),},
{"fo",1,0,"Force","f 0.0 999.0 0.04",FIELD(force),},
{"si",1,0,"Sigmoid","f -99999.0 99999.0 2.0",FIELD(sigmo),},
{"s",2,0,"State","f -1.0 1.0 0.0",FIELD(newstate),},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_Gyro 
ParamEntry NI_Gyro_tab []={
{"Gyroscope",1, 0 ,"G",},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_Touch 
ParamEntry NI_Touch_tab []={
{"Touch",1, 1 ,"T",},
{"r",1,0,"Range","f 0.0 1.0 1.0",FIELD(range),},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_Smell 
ParamEntry NI_Smell_tab []={
{"Smell",1, 0 ,"S",},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_Const 
ParamEntry NI_Const_tab []={
{"Constant",1, 0 ,"*",},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_BendMuscle 
ParamEntry NI_BendMuscle_tab []={
{"Bend muscle",1, 2 ,"|",},
{"p",0,0,"power","f 0.01 1.0 0.25",FIELD(power),},
{"r",0,0,"bending range","f 0.0 1.0 1.0",FIELD(bendrange),},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_RotMuscle 
ParamEntry NI_RotMuscle_tab []={
{"Rotation muscle",1, 1 ,"@",},
{"p",0,0,"power","f 0.01 1.0 1.0",FIELD(power),},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_Diff 
ParamEntry NI_Diff_tab []={
{"Differentiate",1, 0 ,"D",},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_FuzzyNeuro 
ParamEntry NI_FuzzyNeuro_tab []={
{"Fuzzy system [EXPERIMENTAL!]",1, 4 ,"Fuzzy",},
{"ns",0,0,"number of fuzzy sets","d 1  ",FIELD(fuzzySetsNr),},
{"nr",0,0,"number of rules","d 1  ",FIELD(rulesNr),},
{"fs",0,0,"fuzzy sets","s 0 -1 ",FIELD(fuzzySetString),},
{"fr",0,0,"fuzzy rules","s 0 -1 ",FIELD(fuzzyRulesString),},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_Sticky 
ParamEntry NI_Sticky_tab []={
{"Sticky [EXPERIMENTAL!]",1, 0 ,"Sti",},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_LinearMuscle 
ParamEntry NI_LinearMuscle_tab []={
{"Linear muscle [EXPERIMENTAL!]",1, 1 ,"LMu",},
{"p",0,0,"power","f 0.01 1.0 1.0",FIELD(power),},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_WaterDetect 
ParamEntry NI_WaterDetect_tab []={
{"Water detector",1, 0 ,"Water",},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_Energy 
ParamEntry NI_Energy_tab []={
{"Energy level",1, 0 ,"Energy",},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_Channelize 
ParamEntry NI_Channelize_tab []={
{"Channelize",1, 0 ,"Ch",},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_ChMux 
ParamEntry NI_ChMux_tab []={
{"Channel multiplexer",1, 0 ,"ChMux",},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_ChSel 
ParamEntry NI_ChSel_tab []={
{"Channel selector",1, 1 ,"ChSel",},
{"ch",0,0,"channel","d   ",FIELD(ch),},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_Random 
ParamEntry NI_Random_tab []={
{"Random noise",1, 0 ,"Rnd",},
 {0,0,0,},};
#undef FIELDSTRUCT

#define FIELDSTRUCT NI_Sinus 
ParamEntry NI_Sinus_tab []={
{"Sinus generator",1, 2 ,"Sin",},
{"f0",0,0,"base frequency","f -1.0 1.0 0.06283185307",FIELD(f0),},
{"t",0,0,"time","f 0 6.283185307 0",FIELD(t),},
 {0,0,0,},};
#undef FIELDSTRUCT

#define SETIMPLEMENTATION \
setImplementation("N",new NI_StdNeuron); \
setImplementation("Nu",new NI_StdUNeuron); \
setImplementation("G",new NI_Gyro); \
setImplementation("T",new NI_Touch); \
setImplementation("S",new NI_Smell); \
setImplementation("*",new NI_Const); \
setImplementation("|",new NI_BendMuscle); \
setImplementation("@",new NI_RotMuscle); \
setImplementation("D",new NI_Diff); \
setImplementation("Fuzzy",new NI_FuzzyNeuro); \
setImplementation("Sti",new NI_Sticky); \
setImplementation("LMu",new NI_LinearMuscle); \
setImplementation("Water",new NI_WaterDetect); \
setImplementation("Energy",new NI_Energy); \
setImplementation("Ch",new NI_Channelize); \
setImplementation("ChMux",new NI_ChMux); \
setImplementation("ChSel",new NI_ChSel); \
setImplementation("Rnd",new NI_Random); \
setImplementation("Sin",new NI_Sinus); \

