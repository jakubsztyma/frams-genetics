// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.





void Part::defassign()
{
shape=0;
size=1.0;
scale.x=1.0;
scale.y=1.0;
scale.z=1.0;
hollow=0;
density=1.0;
friction=0.4;
ingest=0.25;
assim=0.25;
vis_style="part";
vsize=0.2;
vcolor.x=1.0;
vcolor.y=1.0;
vcolor.z=1.0;
}

































void Joint::defassign()
{
p1_refno=-1;
p2_refno=-1;
d.x=0;
d.y=0;
d.z=0;
shape=0;
stif=1.0;
rotstif=1.0;
stamina=0.25;
vis_style="joint";
vcolor.x=1.0;
vcolor.y=1.0;
vcolor.z=1.0;
}



































void Neuro::defassign()
{
part_refno=-1;
joint_refno=-1;

vis_style="neuro";
}

































void NeuroConn::defassign()
{
n1_refno=-1;
n2_refno=-1;
weight=1.0;
}



















