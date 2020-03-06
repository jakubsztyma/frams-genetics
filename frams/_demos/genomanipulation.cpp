// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <common/virtfile/stdiofile.h>

#include <frams/model/model.h>
#include <frams/genetics/preconfigured.h>
#include <common/loggers/loggertostdout.h>

/**
 @file
 Sample code: Accessing model elements
 */

void printNiceBanner(const char* title)
{
	printf("    #############################################\n"
		"   ##                                           ##\n"
		"  ##    %-37s    ##\n"
		"   ##                                           ##\n"
		"    #############################################\n", title);
}
void printProperties(Param &pi)
{
	printf(" #        id                      type  name        group (%d properties)\n", pi.getPropCount());
	for (int i = 0; i < pi.getPropCount(); i++)
	{
		const char* type = pi.type(i);
		if (*type == 'p') continue;
		printf("%2d. %8s = %-20s %-3s %-10s  %-10s\n", i, pi.id(i), pi.get(i).c_str(), pi.type(i), pi.name(i), pi.grname(pi.group(i)));
	}
}

#define PRINT_PROPERTIES(p) {Param tmp_param(p); printProperties(tmp_param);}

void changeOneProperty(Param &pi)
{
	if (pi.getPropCount() <= 0) return;
	int i = rand() % pi.getPropCount();
	double maxprop = 1, minprop = 0, def;
	pi.getMinMaxDouble(i, minprop, maxprop, def);
	printf("      Change property #%d to random value from range [%g..%g]\n", i, minprop, maxprop);
	printf("      Current value of '%s' (%s) is '%s'\n", pi.id(i), pi.name(i), pi.get(i).c_str());
	char t[100];
	sprintf(t, "%g", minprop + rndDouble(maxprop - minprop));
	printf("      Setting new value... [ using ParamInterface::set() ]\n");
	pi.setFromString(i, t);
	printf("      The value is now '%s'\n", pi.get(i).c_str());
}

#define CHANGE_ONE_PROPERTY(p) {Param tmp_param(p); changeOneProperty(tmp_param);}

void moreAboutPart(Part* p)
{
	printf("Here is the full listing of properties as they are printed in f0\n"
		" (please compare with f0 genotype).\n"
		"Some properties have special meaning (eg. geometry and connections groups)\n"
		"and should be handled with care, because they influence other elements of the model.\n\n"
		" [this data is provided by Part::properties() ]\n");
	PRINT_PROPERTIES(p->properties());
	printf("\nHowever, there is a subset of properties which may be modified more freely.\n"
		"Properties on this list are related only to this part and can be changed\n"
		"without much consideration. They are guaranteed to be always valid; any inconsistencies\n"
		"will be silently repaired.\n"
		"\n [this data is provided by Part::extraProperties() ]\n");
	PRINT_PROPERTIES(p->extraProperties());
	printf("\nThis set of properties can vary from release to release,\n"
		"but can be safely accessed by using extraProperties() call.\n"
		"This method accesses the full set of properies (even those\n"
		"which appear in future releases).\n"
		"Now we will try to change some of properties:\n\n");
	p->getModel().open();
	CHANGE_ONE_PROPERTY(p->extraProperties());
	p->getModel().close();
	printf("\nLet's see f0... (check out part #%d !)\n\n%s\n", p->refno, p->getModel().getF0Geno().getGenes().c_str());
}

void playWithAbsolute(Joint *j)
{
	printf("\nAbsolute Joints adapt to its Parts' positions.\n"
		"We can move a Part, and it does not influence the second part, nor the Joint.\n"
		"Let's move the first Part along y axis by -0.1...\n");
	j->getModel().open();
	j->part1->p.y -= 0.1;
	j->getModel().close();
	printf("The Part's position is changed, but everything else stays intact:\n\n%s\n",
		j->getModel().getF0Geno().getGenes().c_str());
}

void playWithDelta(Joint *j)
{
	printf("\nDelta fields (dx,dy,dz) describe relative location of the second part.\n"
		"This joint will change the second Part's positions to preserve delta distance.\n"
		"Let's move the first Part (#%d) along y axis (+0.1) and change delta.z (dz) by 0.1.\n", j->part1->refno);
	j->getModel().open();
	j->part1->p.y += 0.1;
	j->d.z += 0.1;
	j->getModel().close();
	printf("Position of the second Part referenced by this joint (part #%d) is now changed:\n\n%s\n",
		j->part2->refno, j->getModel().getF0Geno().getGenes().c_str());
	printf("If no delta fields are defined, they will be computed automatically.\n"
		"You can always delete existing delta values by using Joint::resetDelta().\n"
		"Now we will change the second Part's z position by -0.2 and call resetDelta()...\n");
	j->getModel().open();
	j->part2->p.z -= 0.2;
	j->resetDelta();
	j->getModel().close();
	printf("As you can see, Joint's delta fields have altered:\n\n%s\n", j->getModel().getF0Geno().getGenes().c_str());
}

void switchDelta(Joint *j)
{
	int option = !j->isDelta();
	printf("How would this joint look like with delta option %s?\n[ by calling Joint::useDelta(%d) ]\n", option ? "enabled" : "disabled", option);
	j->getModel().open();
	j->useDelta(!j->isDelta());
	j->getModel().close();
	printf("f0 is now:\n\n%s\n...so this is %s joint.\n",
		j->getModel().getF0Geno().getGenes().c_str(), option ? "a delta" : "an absolute");

}

void moreAboutJoint(Joint* j)
{
	printf("Similarly as with Part, the full list of properties comes first:\n\n");
	PRINT_PROPERTIES(j->properties());
	printf("\nActually, there are two kinds of Joints: delta and absolute.\n"
		"For this object, Joint::isDelta() returns %d, so this is the %s Joint.\n",
		j->isDelta(), j->isDelta() ? "delta" : "absolute");
	if (j->isDelta())
	{
		playWithDelta(j);
		switchDelta(j);
		playWithAbsolute(j);
	}
	else
	{
		playWithAbsolute(j);
		switchDelta(j);
		playWithDelta(j);
	}

	printf("Part references and delta fields are the 'core' properties of the Joint.\n"
		"The other properties are available from Joint::extraProperties()\n"
		"and at the moment are defined as follows:\n\n");
	PRINT_PROPERTIES(j->extraProperties());
	printf("\nThey can be changed just like Part's extra properties:\n");
	j->getModel().open();
	CHANGE_ONE_PROPERTY(j->extraProperties());
	j->getModel().close();
	printf("And after that we have this genotype:\n\n%s\n", j->getModel().getF0Geno().getGenes().c_str());
}



void moreAboutNeuro(Neuro* n)
{
	printf("Basic features of Neuro object are similar to those of Part and Joint.\n"
		"We can request a property list:\n\n");
	PRINT_PROPERTIES(n->properties());
	printf("\n...and extra properties (which are designed to be always valid and easy to change):\n\n");
	PRINT_PROPERTIES(n->extraProperties());
	printf("\nAs usual, we will change something:\n");
	n->getModel().open();
	CHANGE_ONE_PROPERTY(n->extraProperties());
	n->getModel().close();
	printf("Each neuron can have any number of inputs = weighted connections\n with other neurons.\n"
		"According to Neuro::getInputCount(), this one has %d inputs.\n", n->getInputCount());
	printf("Standard API is provided for accessing those inputs (getInput(int)),\n"
		"adding inputs (addInput(Neuro*)) and removing them (removeInput(int)).\n\n");

	printf("\nThe most unusual thing is 'details' field (d).\n"
		"It is something like separate object with its own set of properties.\n"
		"Currently the value of 'd' is '%s'.\n", n->getDetails().c_str());

	{
		NeuroClass* cl = n->getClass();
		if (!cl)
			printf("It should contain the class name but the meaning of '%s' is unknown\n", n->getDetails().c_str());
		else
		{

			printf("'%s' is the class name (Neuro::getClassName() == '%s') and means '%s'.\n",
				cl->getName().c_str(), cl->getName().c_str(), cl->getLongName().c_str());
			printf("Neuro::getClass() gives you information about basic characteristic\n"
				"of the class, that can be analyzed automatically.\n");
			printf("For the current object we can learn that it supports ");
			if (cl->getPreferredInputs() < 0) printf("any number of inputs");
			else if (cl->getPreferredInputs() == 0) printf("no inputs");
			else printf("%d inputs", cl->getPreferredInputs());
			printf(" (getPreferredInputs()) ");
			printf(cl->getPreferredOutput() ? "and provides meaningful output signal (getPreferredOutput()==1).\n" : "and doesn't provide useful output signal (getPreferredOutput()==0).\n");

			SyntParam p = n->classProperties();
			if (p.getPropCount() > 0)
			{
				printf("The class defines its own properties:\n\n [ data provided by Neuro::classProperties() ]\n");
				printProperties(p);
				printf("and they can be changed:\n");
				n->getModel().open();
				changeOneProperty(p);
				p.update();
				n->getModel().close();
				printf("After that, 'details' contains the new object: '%s'.\n", n->getDetails().c_str());
			}
			else
				printf("(This class does not have its own properties\n"
				" - Neuro::classProperties().getPropCount()==0)\n");
		}
	}

	printf("The class of this object can be changed using Neuro::setClassName()\n"
		"The following classes are available:\n"
		" [ data provided by Neuro::getClassInfo()->getProperties() ]\n\n");
	printf(" #  class  description       properties\n");
	for (int i = 0; i < n->getClassCount(); i++)
	{
		NeuroClass* cl = n->getClass(i);
		Param p = cl->getProperties();
		printf("%2d.%6s  %-20s  %2d\n", i, cl->getName().c_str(), cl->getLongName().c_str(), p.getPropCount());
	}
	int cl = rand() % n->getClassCount();
	printf("\nLet's change the Neuro's class to '%s'...\n", n->getClassName(cl).c_str());
	n->getModel().open();
	n->setClass(n->getClass(cl));
	{
		SyntParam p = n->classProperties();
		if (p.getPropCount()>0)
		{
			printProperties(p);
			changeOneProperty(p);
			p.update();
		}
	}

	if (n->getInputCount() > 0)
	{
		printf("Info for input #0 = \"%s\"\n", n->getInputInfo(0).c_str());
		printf("Info for input #0, field \"%s\" = \"%s\"\n", "abc", n->getInputInfo(0, "abc").c_str());
		n->setInputInfo(0, "test", 44);
		n->setInputInfo(0, "abc", "yeah");
	}

	n->getModel().close();
	printf("The final object description will be then: '%s'\nAnd the full f0 genotype:\n\n%s\n",
		n->getDetails().c_str(), n->getModel().getF0Geno().getGenes().c_str());


}

void findingConverters()
{
	GenoConverter *gc = Geno::getConverters()->findConverters(0, '1');
	if (gc) printf("found converter accepting f1: \"%s\"\n", gc->name);
	SListTempl<GenoConverter*> found;
	Geno::getConverters()->findConverters(&found, -1, '0');
	printf("found %d converter(s) producing f0\n", found.size());
}

int main(int argc, char*argv[])
{
	LoggerToStdout messages_to_stdout(LoggerBase::Enable); //redirect model-related errors to stdout
	PreconfiguredGenetics genetics;

	srand(time(0));
	printNiceBanner("Welcome to Genotype Manipulation App!");

	findingConverters();

	SString gen(argc > 1 ? argv[1] : "X[|G:1.23]");
	if (!strcmp(gen.c_str(), "-"))
	{
		gen = 0;
		StdioFILEDontClose in(stdin);
		loadSString(&in, gen);
	}
	Geno g(gen);
	printf("\nSource genotype: '%s'\n", g.getGenes().c_str());
	printf("                  ( format %c %s)\n",
		g.getFormat(), g.getComment().c_str());

	Model m(g);//.getConverted('0'));

	if (!m.isValid())
	{
		printf("Cannot build Model from this genotype!\n");
		return 2;
	}
	printf("Converted to f0:\n%s\n", m.getF0Geno().getGenes().c_str());

	printf("Model contains: %d part(s)\n"
		"                %d joint(s)\n"
		"                %d neuron(s)\n", m.getPartCount(), m.getJointCount(), m.getNeuroCount());

	printf("\nInvestigating details...\n");

	if (m.getPartCount() > 0)
	{
		int p = rand() % m.getPartCount();
		printNiceBanner("P A R T    O B J E C T");
		printf("            (part # %d)\n", p);
		moreAboutPart(m.getPart(p));
	}

	if (m.getJointCount() > 0)
	{
		int j = rand() % m.getJointCount();
		printNiceBanner("J O I N T    O B J E C T");
		printf("            (joint # %d)\n", j);
		moreAboutJoint(m.getJoint(j));
	}

	if (m.getNeuroCount() > 0)
	{
		int n = rand() % m.getNeuroCount();
		printNiceBanner("N E U R O    O B J E C T");
		printf("            (neuro # %d)\n", n);
		moreAboutNeuro(m.getNeuro(n));
	}

#ifdef MODEL_V1_COMPATIBLE
	printNiceBanner("Old Neuro/NeuroItem view");
	int nc = m.old_getNeuroCount();
	printf("Model::old_getNeuroCount() = %d\n", nc);
	for (int i = 0; i < nc; i++)
	{
		Neuro *n = m.old_getNeuro(i);
		printf("neuron #%d: p=%d, j=%d, force=%g, inertia=%g, sigmoid=%g\n",
			i, n->part_refno, n->joint_refno,
			n->force, n->inertia, n->sigmo);
		int nicount = n->getItemCount();
		printf("    %d items\n", nicount);
		for (int j = 0; j < nicount; j++)
		{
			NeuroItem *ni = n->getNeuroItem(j);
			printf("        item #%d - '%s', conn=%d, weight=%g\n",
				j, ni->getDetails().c_str(), ni->conn_refno, ni->weight);
		}
	}
	printf("end.\n");
#endif

	printf("\n######### THE END ###########\n\n"
		"Hints:\n"
		"  1. You can redirect output: genomanipulation >filename.txt\n"
		"  2. Each run can yield different results, because some\n"
		"     values are randomly generated.\n"
		"  3. This application will use custom genotype passed as\n"
		"     a commandline parameter: genomanipulation XX\n"
		"\n");
	return 0;
}
