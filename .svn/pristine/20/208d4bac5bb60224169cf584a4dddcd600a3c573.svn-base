// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2016  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <stdio.h>
#include <frams/model/model.h>
#include <frams/model/modelparts.h>

int main()
{
	Model m;
	Part *p1, *p2;

	m.open();

	// chain of ellipsoids - subsequent parts are placed relative to the previous part's orientation and location
	p1 = m.addNewPart(Part::SHAPE_ELLIPSOID); //initial part
	p1->scale = Pt3D(1.0, 0.7, 0.4);

	Orient rotation = Orient_1; //must be initialized explicitly because the default Orient constructor does not initialize anything
	rotation.rotate(Pt3D(0.1, 0.2, 0.3));

	for (int N = 10; N > 0; N--, p1 = p2)
	{
		p2 = m.addNewPart(Part::SHAPE_ELLIPSOID);
		p2->scale = p1->scale*0.9; //each part is smaller than its predecessor

		Pt3D advance(p1->scale.x, 0, 0); //advance by previous part's ellipsoid x radius
		p2->p = p1->p + p1->o.transform(advance); //advance vector transformed by p1's orientation (i.e., in p1's local coordinates)
		p2->setOrient(p1->o.transform(rotation)); //rotation transformed by p1's orientation

		m.addNewJoint(p1, p2, Joint::SHAPE_FIXED); //all parts must be connected
	}

	// chain of cyllinders - line segments between points calculated from the parametric formula P(a)=(2-2*cos(a),2*sin(a)) (circle with r=2)
	Pt3D prev, next;
	p1 = m.getPart(0);
	for (float a = 0; a<M_PI; a += M_PI / 10)
	{
		Pt3D next(2 - 2 * cos(a), 0, 2 * sin(a));
		if (a>0)
		{
			p2 = m.addNewPart(Part::SHAPE_CYLINDER);
			p2->setPositionAndRotationFromAxis(prev, next);
			p2->scale = Pt3D(prev.distanceTo(next)*0.5, 0.05, 0.05);// distance*0.5 because scale is "radius", not cylinder length

			m.addNewJoint(p1, p2, Joint::SHAPE_FIXED); //all parts must be connected
		}
		p1 = p2;
		prev = next;
	}

	m.close();
	puts(m.getF0Geno().getGenesAndFormat().c_str());
	// the genotype can be fed directly to the genotype viewer, like this:
	// part_shapes | theater -g -
}
