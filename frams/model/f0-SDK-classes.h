// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.


#define FIELDSTRUCT Model
ParamEntry f0_model_paramtab[]=
{
 {"Properties",2,2,"m" },
 {"Visual",},
 {"se",0,1024,"startenergy","f",FIELD(startenergy),},
 {"Vstyle",1,0,"vis_style","s 0 0 ",FIELD(vis_style),},
 {0,0,0,}
};
ParamEntry f0_model_xtra_paramtab[]=
{
 {"Extra properties",1,0,"m"},
 {0,0,0,}
};
#undef FIELDSTRUCT


#define FIELDSTRUCT Part
ParamEntry f0_part_paramtab[]=
{
 {"Geometry",3,22,"p" },
 {"Other properties",},
 {"Visual",},
 {"x",0,1024,"position.x","f",FIELD(p.x),},
 {"y",0,1024,"position.y","f",FIELD(p.y),},
 {"z",0,1024,"position.z","f",FIELD(p.z),},
 {"sh",1,0,"shape","d 0 3 0",FIELD(shape),},
 {"s",1,0,"size","f 0.1 10.0 1.0",FIELD(size),},
 {"sx",1,0,"scale.x","f 0.001 1000.0 1.0",FIELD(scale.x),},
 {"sy",1,0,"scale.y","f 0.001 1000.0 1.0",FIELD(scale.y),},
 {"sz",1,0,"scale.z","f 0.001 1000.0 1.0",FIELD(scale.z),},
 {"h",1,0,"hollow","f 0 1 0",FIELD(hollow),},
 {"dn",1,0,"density","f 0.2 5.0 1.0",FIELD(density),},
 {"fr",1,0,"friction","f 0.0 4.0 0.4",FIELD(friction),},
 {"ing",1,0,"ingestion","f 0.0 1.0 0.25",FIELD(ingest),},
 {"as",1,0,"assimilation","f 0.0 1.0 0.25",FIELD(assim),},
 {"rx",0,0,"rot.x","f",FIELD(rot.x),},
 {"ry",0,1024,"rot.y","f",FIELD(rot.y),},
 {"rz",0,1024,"rot.z","f",FIELD(rot.z),},
 {"i",1,0,"info","s",FIELD(info),},
 {"Vstyle",2,0,"vis_style","s 0 0 part",FIELD(vis_style),},
 {"vs",2,0,"visual thickness","f 0.05 0.7 0.2",FIELD(vsize),},
 {"vr",2,0,"red component","f 0.0 1.0 1.0",FIELD(vcolor.x),},
 {"vg",2,1024,"green component","f 0.0 1.0 1.0",FIELD(vcolor.y),},
 {"vb",2,1024,"blue component","f 0.0 1.0 1.0",FIELD(vcolor.z),},
 {0,0,0,}
};
ParamEntry f0_part_xtra_paramtab[]=
{
 {"Extra properties",1,9,"p"},
 {"h",0,0,"hollow","f 0 1 0",FIELD(hollow),},
 {"dn",0,0,"density","f 0.2 5.0 1.0",FIELD(density),},
 {"fr",0,0,"friction","f 0.0 4.0 0.4",FIELD(friction),},
 {"ing",0,0,"ingestion","f 0.0 1.0 0.25",FIELD(ingest),},
 {"as",0,0,"assimilation","f 0.0 1.0 0.25",FIELD(assim),},
 {"vs",0,0,"visual thickness","f 0.05 0.7 0.2",FIELD(vsize),},
 {"vr",0,0,"red component","f 0.0 1.0 1.0",FIELD(vcolor.x),},
 {"vg",0,1024,"green component","f 0.0 1.0 1.0",FIELD(vcolor.y),},
 {"vb",0,1024,"blue component","f 0.0 1.0 1.0",FIELD(vcolor.z),},
 {0,0,0,}
};
#undef FIELDSTRUCT


#define FIELDSTRUCT Joint
ParamEntry f0_joint_paramtab[]=
{
 {"Connections",4,17,"j" },
 {"Geometry",},
 {"Other properties",},
 {"Visual",},
 {"p1",0,1024,"part1 ref#","d -1 999999 -1",FIELD(p1_refno),},
 {"p2",0,1024,"part2 ref#","d -1 999999 -1",FIELD(p2_refno),},
 {"rx",1,0,"rotation.x","f",FIELD(rot.x),},
 {"ry",1,1024,"rotation.y","f",FIELD(rot.y),},
 {"rz",1,1024,"rotation.z","f",FIELD(rot.z),},
 {"dx",1,0,"delta.x","f -2 2 0",FIELD(d.x),},
 {"dy",1,1024,"delta.y","f -2 2 0",FIELD(d.y),},
 {"dz",1,1024,"delta.z","f -2 2 0",FIELD(d.z),},
 {"sh",1,0,"shape","d 0 1 0",FIELD(shape),},
 {"stif",2,0,"stiffness","f 0.0 1.0 1.0",FIELD(stif),},
 {"rotstif",2,0,"rotation stiffness","f 0.0 1.0 1.0",FIELD(rotstif),},
 {"stam",2,0,"stamina","f 0.0 1.0 0.25",FIELD(stamina),},
 {"i",2,0,"info","s",FIELD(info),},
 {"Vstyle",3,0,"vis_style","s 0 0 joint",FIELD(vis_style),},
 {"vr",3,0,"red component","f 0.0 1.0 1.0",FIELD(vcolor.x),},
 {"vg",3,1024,"green component","f 0.0 1.0 1.0",FIELD(vcolor.y),},
 {"vb",3,1024,"blue component","f 0.0 1.0 1.0",FIELD(vcolor.z),},
 {0,0,0,}
};
ParamEntry f0_joint_xtra_paramtab[]=
{
 {"Extra properties",1,5,"j"},
 {"stif",0,0,"stiffness","f 0.0 1.0 1.0",FIELD(stif),},
 {"rotstif",0,0,"rotation stiffness","f 0.0 1.0 1.0",FIELD(rotstif),},
 {"vr",0,0,"red component","f 0.0 1.0 1.0",FIELD(vcolor.x),},
 {"vg",0,1024,"green component","f 0.0 1.0 1.0",FIELD(vcolor.y),},
 {"vb",0,1024,"blue component","f 0.0 1.0 1.0",FIELD(vcolor.z),},
 {0,0,0,}
};
#undef FIELDSTRUCT


#define FIELDSTRUCT Joint
ParamEntry f0_nodeltajoint_paramtab[]=
{
 {"Connections",4,11,"j" },
 {"Geometry",},
 {"Other properties",},
 {"Visual",},
 {"p1",0,1024,"part1 ref#","d -1 999999 -1",FIELD(p1_refno),},
 {"p2",0,1024,"part2 ref#","d -1 999999 -1",FIELD(p2_refno),},
 {"sh",1,0,"shape","d 0 1 0",FIELD(shape),},
 {"stif",2,0,"stiffness","f 0.0 1.0 1.0",FIELD(stif),},
 {"rotstif",2,0,"rotation stiffness","f 0.0 1.0 1.0",FIELD(rotstif),},
 {"stam",2,0,"stamina","f 0.0 1.0 0.25",FIELD(stamina),},
 {"i",2,0,"info","s",FIELD(info),},
 {"Vstyle",3,0,"vis_style","s 0 0 joint",FIELD(vis_style),},
 {"vr",3,0,"red component","f 0.0 1.0 1.0",FIELD(vcolor.x),},
 {"vg",3,1024,"green component","f 0.0 1.0 1.0",FIELD(vcolor.y),},
 {"vb",3,1024,"blue component","f 0.0 1.0 1.0",FIELD(vcolor.z),},
 {0,0,0,}
};
ParamEntry f0_nodeltajoint_xtra_paramtab[]=
{
 {"Extra properties",1,5,"j"},
 {"stif",0,0,"stiffness","f 0.0 1.0 1.0",FIELD(stif),},
 {"rotstif",0,0,"rotation stiffness","f 0.0 1.0 1.0",FIELD(rotstif),},
 {"vr",0,0,"red component","f 0.0 1.0 1.0",FIELD(vcolor.x),},
 {"vg",0,1024,"green component","f 0.0 1.0 1.0",FIELD(vcolor.y),},
 {"vb",0,1024,"blue component","f 0.0 1.0 1.0",FIELD(vcolor.z),},
 {0,0,0,}
};
#undef FIELDSTRUCT


#define FIELDSTRUCT Neuro
ParamEntry f0_neuro_paramtab[]=
{
 {"Connections",3,10,"n" },
 {"Other",},
 {"Visual",},
 {"p",0,0,"part ref#","d -1 999999 -1",FIELD(part_refno),},
 {"j",0,0,"joint ref#","d -1 999999 -1",FIELD(joint_refno),},
 {"d",1,0,"details","s",GETSET(details),},
 {"i",1,0,"info","s",FIELD(info),},
 {"Vstyle",2,0,"vis_style","s 0 0 neuro",FIELD(vis_style),},
 {"getInputCount",0,1+2,"input count","d",GETONLY(inputCount),},
 {"getInputNeuroDef",0,1+2,"get input neuron","p oNeuroDef(d)",PROCEDURE(p_getInputNeuroDef),},
 {"getInputNeuroIndex",0,1+2,"get input neuron index","p d(d)",PROCEDURE(p_getInputNeuroIndex),},
 {"getInputWeight",0,1+2,"get input weight","p f(d)",PROCEDURE(p_getInputWeight),},
 {"classObject",0,1+2,"neuron class","oNeuroClass",GETONLY(classObject),},
 {0,0,0,}
};
ParamEntry f0_neuro_xtra_paramtab[]=
{
 {"Extra properties",1,0,"n"},
 {0,0,0,}
};
#undef FIELDSTRUCT


#define FIELDSTRUCT NeuroConn
ParamEntry f0_neuroconn_paramtab[]=
{
 {"Connection",2,4,"c" },
 {"Other",},
 {"n1",0,1024,"this neuro ref#","d -1 999999 -1",FIELD(n1_refno),},
 {"n2",0,1024,"connected neuro ref#","d -1 999999 -1",FIELD(n2_refno),},
 {"w",0,1024,"weight","f -999999 999999 1.0",FIELD(weight),},
 {"i",1,0,"info","s",FIELD(info),},
 {0,0,0,}
};
ParamEntry f0_neuroconn_xtra_paramtab[]=
{
 {"Extra properties",1,0,"c"},
 {0,0,0,}
};
#undef FIELDSTRUCT




















