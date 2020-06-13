#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <chrono>
#include "frams/genetics/fS/fS_general.h"
#include "frams/genetics/fS/fS_conv.h"
#include "frams/genetics/fS/fS_oper.h"
#include "frams/genetics/preconfigured.h"

using std::cout;
using std::endl;

int countChars(SString genotype, string chars, int count)
{
	int result = 0;
	for (int i = 0; i < genotype.len(); i++)
	{
		for (int j = 0; j < count; j++)
		{
			if (genotype[i] == chars[j])
				result += 1;
		}
	}
	return result;
}

int countJoints(SString genotype)
{
	return countChars(genotype, JOINTS, 2);
}

int countParams(SString genotype)
{
	return countChars(genotype, "=", 1);
}

int countModifiers(SString genotype)
{
	return countChars(genotype, "IiFfSs", 6);
}

int countNeuroConnections(fS_Genotype &geno)
{
	vector < fS_Neuron * > neurons = geno.getAllNeurons();
	int result = 0;
	for (int i = 0; i < int(neurons.size()); i++)
		result += neurons[i]->inputs.size();
	return result;
}

void testRearrangeBeforeCrossover()
{
	GenoOper_fS operators;
	string test_cases[][2] = {
			{"S:EE[]",                                   "S:EE[]"},
			{"S:E[;;]E[;;]",                             "S:E[;;]E[;;]"},
			{"S:E[;;]E[3;4_5;3]",                        "S:E[;;]E[;;]"},
			{"S:E[3;4;]E[3;4_5;3]",                      "S:E[;;]E[;;]"},
			{"S:E[1;2;0]E[;;]",                          "S:E[1;2;0]E[;;]"},
			{"S:E[1_3;2_4;]E[3;4_5;3]",                  "S:E[1;2;]E[;;]"},
			{"S:E[Sin;;G]E[Rnd;;T]",                     "S:E[Sin;;G]E[Rnd;;T]"},
			{"S:E[1_3;2_4;](E[3;4_5;3],E[3;4_6_7])",     "S:E[1;2;](E[;;],E[;3_4])"},
			{"S:E[1_3;2_4;](E[0_3;4_5;3_6],E[3;4_6_7])", "S:E[1;2;](E[0;;3],E[;3_4])"},
	};
	int expectedSubStart[] = {
			0, 3, 3, 3, 3, 3, 3, 3, 3
	};
	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		fS_Genotype geno(test_cases[i][0]);
		Node *subtree = geno.getAllNodes()[1];
		int subStart;

		operators.rearrangeConnectionsBeforeCrossover(&geno, subtree, subStart);

		assert(geno.getGeno().c_str() == test_cases[i][1]);
		assert(subStart == expectedSubStart[i]);
	}
}

void testRearrangeAfterCrossover()
{
	GenoOper_fS operators;
	string test_cases[][2] = {
			{"S:E[0_1;0]E[]",                       "S:E[0_1;0]E[]"},
			{"S:E[0_1;0]E[Rnd;;]",                  "S:E[0_1;0]E[Rnd;;]"},
			{"S:E[0_1;0]E[Rnd;;]E[;]",              "S:E[0_1;0]E[Rnd;;]E[;]"},
			{"S:E[0_1;0](E[Rnd;;],E[2_3;2])",       "S:E[0_1;0](E[Rnd;;],E[5_6;5])"},
			{"S:E[0_1;0](E[Rnd;;],E[2_3;2]C[2_4])", "S:E[0_1;0](E[Rnd;;],E[5_6;5]C[5_7])"},
	};
	int subStart[] {
			0, 0, 0, 0, 0,
	};
	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		fS_Genotype geno(test_cases[i][0]);
		Node *subtree = geno.getAllNodes()[1];

		operators.rearrangeConnectionsAfterCrossover(&geno, subtree, subStart[i]);

		assert(geno.getGeno().c_str() == test_cases[i][1]);
	}
}

double EPSILON = 0.01;

bool doubleCompare(double a, double b)
{
	return fabs(a - b) < EPSILON;
}

void testAddPart()
{
	string test_cases[] = {
			"S:E",
			"S:SE",
			"S:sE",
			"S:SSSSSSSE",
			"S:sssssssE",
			"S:SSSSSSSSSE",    // More than max
			"S:sssssssssE", // Less than min
	};
	double expectedVolume[] = {
			4.19,
			5.58,
			3.15,
			20.94,
			0.84,
			20.94,
			0.84

	};

	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		fS_Genotype geno(test_cases[i]);

		geno.addPart(true, "ECR", false);

		geno.getState();
		Node *newNode = geno.getAllNodes()[1];
		assert(doubleCompare(newNode->calculateVolume(), expectedVolume[i]));
	}
}

void testChangePartType()
{
	string test_cases[] = {
			"S:C",
			"S:SSSSC",
			"S:sssssC",
			"S:sssssC{x=0.3;y=2.3;z=1.1}",
			"S:SSSSSSC{x=0.3;y=2.3;z=1.1}",
	};

	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		fS_Genotype geno(test_cases[i]);
		geno.getState();
		double oldVolume = geno.startNode->calculateVolume();

		geno.changePartType(true);

		geno.getState();
		std::cout << geno.getGeno().c_str() << " " << geno.startNode->calculateVolume() << " " << oldVolume << std::endl;
		assert(doubleCompare(geno.startNode->calculateVolume(), oldVolume));
	}

}
void testUsePartType()
{
	fS_Genotype geno("S:E");
	geno.changePartType(true, "C");
	assert(geno.getAllNodes()[0]->partType == Part::Shape::SHAPE_CUBOID);
	geno.changePartType(true, "E");
	assert(geno.getAllNodes()[0]->partType == Part::Shape::SHAPE_ELLIPSOID);
	geno.changePartType(true, "R");
	assert(geno.getAllNodes()[0]->partType == Part::Shape::SHAPE_CYLINDER);
	geno.addPart(true, "C");
	assert(geno.getAllNodes()[1]->partType == Part::Shape::SHAPE_CUBOID);
	geno.addPart(true, "E");
	assert(geno.getAllNodes()[2]->partType == Part::Shape::SHAPE_ELLIPSOID);
	geno.addPart(true, "R");
	assert(geno.getAllNodes()[3]->partType == Part::Shape::SHAPE_CYLINDER);

}

/**
 * Cases when exchanging trees with similar size aways makes children of the equal parents equal to them
 * Test cases will almost always work when crossoverTries is big enough
 */
void testCrossoverSimilarTrees()
{
	GenoOper_fS operators;
	string test_cases[] = {
			"S:EE",
			"S:E(E,E)",
			"S:EEEE",
			"S:ECRE",
			"S:E(RE,CRE)",
			"S:E(EEE,EEE,EEE)",
			"S:E(CRE,CRE,CRE)",
			"S:EEEEEECRE(CRE,CRE,CRE)",
	};

	float f1, f2;
	// Repeat the test several times as crossover is not deterministic
	for (int z = 0; z < 10; z++)
	{
		for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
		{
			char *arr1 = strdup(test_cases[i].c_str());
			char *arr2 = strdup(arr1);

			operators.crossOver(arr1, arr2, f1, f2);

			assert(strcmp(arr1, test_cases[i].c_str()) == 0);
			free(arr1);
			free(arr2);
		}
	}
}

void testAllPartSizesValid()
{
	string test_cases[] = {
			"S:C{x=2000.0}",    // Too big dimension
			"S:C{y=2000.0}",
			"S:C{z=2000.0}",
			"S:C{x=0.0005}",    // Too small dimension
			"S:C{y=0.0005}",
			"S:C{z=0.0005}",
			"S:E{x=1.1}",        // Invalid size params
			"S:E{y=1.1}",
			"S:E{z=1.1}",
			"S:R{x=1.1;y=1.2}",
			"S:R{x=1.1}",
			"S:R{y=1.1}",
			"S:SR{x=999.0}",
			"S:C(R,E{z=1.1})",
			"S:C{x=1.5;y=1.5;z=1.5}",    // Test volume
			"S:C{x=1.8;y=1.8}",
	};

	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		fS_Genotype geno(test_cases[i]);
		assert(geno.allPartSizesValid() == false);
	}
}


void testOneGenotype(SString *test, int expectedPartCount)
{
	GenoConv_fS0 converter = GenoConv_fS0();
	MultiMap map;
	int tmp = -1;
	SString tmpStr;
	SString genotype_str = test[0];

	/// Test translate
	cout << "Geno: " << test[0].c_str() << endl;
	cout << "Result:\n" << converter.convert(genotype_str, &map, false).c_str() << endl;
	cout << "Expected: \n" << test[1].c_str() << endl << endl;
	assert(test[1] == converter.convert(genotype_str, &map, false).c_str());

	/// Test get geno
	fS_Genotype geno(test[0].c_str());
	cout << geno.getGeno().c_str() << endl;
	assert(geno.getGeno() == test[0]);

	////Test operations
	// Test part count
	assert(geno.getNodeCount() == expectedPartCount);

	// Test add part
	tmp = geno.getNodeCount();
	geno.addPart(true);
	assert(tmp + 1 == geno.getNodeCount());

	// Test change part
	tmp = geno.getNodeCount();
	if (geno.changePartType(true))
		assert(tmp == geno.getNodeCount());

	// Test remove part
	tmp = geno.getNodeCount();
	if (geno.removePart())
		assert(tmp == 1 + geno.getNodeCount());

	// Test add joint
	tmp = countJoints(geno.getGeno());
	if (geno.addJoint())
		assert(tmp + 1 == countJoints(geno.getGeno()));

	// Test remove joint
	tmp = countJoints(geno.getGeno());
	if (geno.removeJoint())
		assert(tmp - 1 == countJoints(geno.getGeno()));

	// Test add param
	tmp = countParams(geno.getGeno());
	if (geno.addParam(true))
		assert(tmp + 1 == countParams(geno.getGeno()));

	// Test change param
	tmpStr = geno.getGeno();
	tmp = countParams(geno.getGeno());
	if (geno.changeParam(true))
	{
		SString resultGeno = geno.getGeno();
		assert(tmp == countParams(resultGeno));
		// TODO figure out how to test it. Param can be changed by very small value
//		assert(tmpStr != resultGeno);
	}

	// Test remove param
	tmp = countParams(geno.getGeno());
	if (geno.removeParam())
		assert(tmp == 1 + countParams(geno.getGeno()));

	// Test add modifier
	tmp = countModifiers(geno.getGeno());
	if (geno.addModifier())
		assert(tmp + 1 == countModifiers(geno.getGeno()));

	// Test remove modifier
	tmp = countModifiers(geno.getGeno());
	if (geno.removeModifier())
		assert(tmp == 1 + countModifiers(geno.getGeno()));

	// Test add neuro
	tmp = geno.getAllNeurons().size();
	if (geno.addNeuro())
		assert(tmp + 1 == int(geno.getAllNeurons().size()));

	// Test add neuro connections
	tmp = countNeuroConnections(geno);
	if (geno.addNeuroConnection())
		assert(tmp + 1 == countNeuroConnections(geno));

	// Test change neuro connection
	tmpStr = geno.getGeno();
	if (geno.changeNeuroConnection())
		assert(genotype_str != geno.getGeno());

	// Test remove neuro connections
	tmp = countNeuroConnections(geno);
	if (geno.removeNeuroConnection())
		assert(tmp - 1 == countNeuroConnections(geno));

	// Test remove neuro
	tmp = geno.getAllNeurons().size();
	if (geno.removeNeuro())
		assert(tmp - 1 == int(geno.getAllNeurons().size()));
}

void validationTest()
{
	GenoConv_fS0 converter = GenoConv_fS0();
	GenoOper_fS operators;
	SString invalidGenotypes[] = {
			"EEE",      // No mode specifier
			"S:FFF",    // No part type
			"S:FFF{x=5.0}",    // No part type
			"M:qqE",    // Invalid modifier
			"S:E{f}",    // No equal sign
			"S:E{qw=1.0}",    // Wrong param key
			"S:E{f=}",    // Wrong param value
			"S:E{f=fr}",    // Wrong param value
	};
	const int invalidCount = 5;
	for (int i = 0; i < invalidCount; i++)
	{
		MultiMap map;
		assert(1 == operators.checkValidity(invalidGenotypes[i].c_str(), ""));
		SString genes = converter.convert(invalidGenotypes[i], &map, false);
		assert(genes == "");
	}
}

void testRearrangeInputs()
{
	const int size = 6;
	string before = "MSJ:E[T]bE[2_3]cRbC[T;G_1_2]bE[1_2_3;T]{x=3.0;y=3.0;z=3.0}";
	SHIFT shift[size]{
			SHIFT::RIGHT,
			SHIFT::RIGHT,
			SHIFT::RIGHT,
			SHIFT::LEFT,
			SHIFT::LEFT,
			SHIFT::LEFT,
	};
	int neuronNumber[size]{
			0,    // First
			2,   // Middle
			5,    // Last
			0,
			2,
			5
	};
	string after[size]{
			"MSJ:E[T]bE[3_4]cRbC[T;G_2_3]bE[2_3_4;T]{x=3.0;y=3.0;z=3.0}",
			"MSJ:E[T]bE[3_4]cRbC[T;G_1_3]bE[1_3_4;T]{x=3.0;y=3.0;z=3.0}",
			"MSJ:E[T]bE[2_3]cRbC[T;G_1_2]bE[1_2_3;T]{x=3.0;y=3.0;z=3.0}",
			"MSJ:E[T]bE[1_2]cRbC[T;G_0_1]bE[0_1_2;T]{x=3.0;y=3.0;z=3.0}",
			"MSJ:E[T]bE[2]cRbC[T;G_1]bE[1_2;T]{x=3.0;y=3.0;z=3.0}",
			"MSJ:E[T]bE[2_3]cRbC[T;G_1_2]bE[1_2_3;T]{x=3.0;y=3.0;z=3.0}"
	};

	for (int i = 0; i < size; i++)
	{
		fS_Genotype geno(before);
		vector < fS_Neuron * > allNeurons = geno.getAllNeurons();
		fS_Neuron *neuron = allNeurons[neuronNumber[i]];

		geno.rearrangeNeuronConnections(neuron, shift[i]);

		assert(geno.getGeno().c_str() == after[i]);
	}

}

void evolutionTest(int operationCount)
{
	fS_Genotype::precision = 6;
	GenoConv_fS0 converter = GenoConv_fS0();
	int gen_size = 5;
	GenoOper_fS operators;
	cout<<operators.getSimplest()<<endl;
	assert(strcmp(operators.getSimplest(), "S:C{x=0.80599;y=0.80599;z=0.80599}") == 0);

	SString **gens = new SString *[gen_size];
	gens[0] = new SString("SMJ:EbcE[1_2]cRbC[G_0_2]bC[0_1_2]{x=1.02;y=1.02;z=1.03}");
	gens[1] = new SString("SMJ:R{j=3.9}cR[0]bR[0_1]");
	gens[2] = new SString("SMJ:R[0;0_1]{j=3.9;ty=2.1;tz=4.3;z=1.1}bRcR");
	gens[3] = new SString("SMJ:R[1]{j=3.9;z=1.04}R[1]cRC[0;1]{x=1.03}");
	gens[4] = new SString("SMJ:E(cE(bE[T;T_1_2],cE,bC[0],cR),bE[0_2;0_2],cE(bcE,bcE[;0_1_2]),E)");


	FILE *pFile = fopen("output.txt", "w");
	int methodUsages[FS_OPCOUNT];
	for (int i = 0; i < FS_OPCOUNT; i++)
		methodUsages[i] = 0;

	for (int i = 0; i < operationCount; i++)
	{
		int i1 = rand() % gen_size;
		int i2 = rand() % gen_size;
		if (i2 == i1)
			i2 = (i1 + 1) % gen_size;

		if (i % 100 == 0)
		{
			cout << i << " out of " << operationCount << " Length: " << gens[i1]->len() + gens[i2]->len() << endl;
			cout << gens[i1]->c_str() << endl;
			cout << gens[i2]->c_str() << endl;
		}

		int method;
		float f1, f2, gp;

		char *arr1 = strdup(gens[i1]->c_str());
		char *arr2 = strdup(gens[i2]->c_str());

		if (operators.mutate(arr1, gp, method) == GENOPER_OK)
			methodUsages[method]++;
		if (operators.mutate(arr2, gp, method) == GENOPER_OK)
			methodUsages[method]++;

		int crossOverResult = operators.crossOver(arr1, arr2, f1, f2);

		// TODO remove checkValidity condition
		if (crossOverResult == GENOPER_OK && 0 == operators.checkValidity(arr1, "") && 0 == operators.checkValidity(arr2, ""))
		{
			assert(0. < f1 && f1 < 1.);
			assert(0. < f2 && f2 < 1.);
			assert(0 == operators.checkValidity(arr1, ""));
			assert(0 == operators.checkValidity(arr2, ""));

			delete gens[i1];
			delete gens[i2];
			gens[i1] = new SString(arr1);
			gens[i2] = new SString(arr2);

			// Check if genotypes convert correctly
			MultiMap map;
			converter.convert(*gens[i1], &map, false);
			converter.convert(*gens[i2], &map, false);
		}

		free(arr1);
		free(arr2);
	}
	cout << "Method usages:" << endl;
	for (int i = 0; i < FS_OPCOUNT; i++)
		cout << i << ": " << methodUsages[i] << endl;
	for (int i = 0; i < gen_size; i++)
		delete gens[i];
	delete[] gens;
	fclose(pFile);
}

int main(int argc, char *argv[])
{
	SString test_cases[][2] = {
			{"S:E",                                               "p:sh=1\n"},
			{"S:C",                                               "p:sh=2\n"},
			{"S:R",                                               "p:sh=3\n"},
			{"S:EEE",                                             "p:sh=1\np:2.0, sh=1\np:4.0, sh=1\nj:0, 1, sh=1\nj:1, 2, sh=1\n"},
			{"S:E(E,E)",                                          "p:sh=1\np:2.0, sh=1\np:2.0, sh=1\nj:0, 1, sh=1\nj:0, 2, sh=1\n"},
			{"S:E(E(E,E),E,E(E,E),E)",                            "p:sh=1\n"
																  "p:2.0, sh=1\n"
																  "p:4.0, sh=1\n"
																  "p:4.0, sh=1\n"
																  "p:2.0, sh=1\n"
																  "p:2.0, sh=1\n"
																  "p:4.0, sh=1\n"
																  "p:4.0, sh=1\n"
																  "p:2.0, sh=1\n"
																  "j:0, 1, sh=1\n"
																  "j:1, 2, sh=1\n"
																  "j:1, 3, sh=1\n"
																  "j:0, 4, sh=1\n"
																  "j:0, 5, sh=1\n"
																  "j:5, 6, sh=1\n"
																  "j:5, 7, sh=1\n"
																  "j:0, 8, sh=1\n"
			},
			{"S:EbE",                                             "p:sh=1\n"
																  "p:2.0, sh=1\n"
																  "j:0, 1, sh=2\n"}, // Carametrized joints
			{"S:CcC",                                             "p:sh=2\n"
																  "p:2.0, sh=2\n"
																  "j:0, 1, sh=3\n"}, // Many parametrized joints
			{"S:ERbRcCRbCbE",                                     "p:sh=1\n"
																  "p:2.0, sh=3\n"
																  "p:4.0, sh=3\n"
																  "p:6.0, sh=2\n"
																  "p:8.0, sh=3\n"
																  "p:10.0, sh=2\n"
																  "p:12.0, sh=1\n"
																  "j:0, 1, sh=1\n"
																  "j:1, 2, sh=2\n"
																  "j:2, 3, sh=3\n"
																  "j:3, 4, sh=1\n"
																  "j:4, 5, sh=2\n"
																  "j:5, 6, sh=2\n"
			},
// Modifier mode
			{"M:E",                                               "p:sh=1\n"},  // Basic modifier mode
			{"M:FE",                                              "p:sh=1, fr=0.44\n"},  // Friction modifier
			{"M:fE",                                              "p:sh=1, fr=0.36\n"},  // Friction modifier
			{"M:FFFFffE",                                         "p:sh=1, fr=0.48\n"},  // Friction modifier
			{"S:E{f=0.3}E{f=0.5}",                                "p:sh=1, fr=0.3\n"
																  "p:2.0, sh=1, fr=0.5\n"
																  "j:0, 1, sh=1\n"},
			{"S:EE{ty=90.0}",                                     "p:sh=1\n"
																  "p:z=2.0, sh=1\n"
																  "j:0, 1, sh=1\n"},
			{"S:EE{tz=90.0}",                                     "p:sh=1\n"
																  "p:y=2.0, sh=1\n"
																  "j:0, 1, sh=1\n"},  // Z rotation
			{"S:EE{tz=90.0}E{tx=90.0}E{ty=90.0}",                 "p:sh=1\n"
																  "p:y=2.0, sh=1\n"
																  "p:y=2.0, 2.0, sh=1\n"
																  "p:-1.99, 2.0, 2.0, sh=1\n"
																  "j:0, 1, sh=1\n"
																  "j:1, 2, sh=1\n"
																  "j:2, 3, sh=1\n"},  // All rotations
			{"S:EE{tz=45.0}E{tx=45.0}E{ty=45.0}",                 "p:sh=1\n"
																  "p:1.41, 1.41, sh=1\n"
																  "p:2.83, 2.41, 1.0, sh=1\n"
																  "p:3.12, 3.41, 2.71, sh=1\n"
																  "j:0, 1, sh=1\n"
																  "j:1, 2, sh=1\n"
																  "j:2, 3, sh=1\n"
			},  // Acute angle rotations
			{"S:EE{tz=-90.0}E{tx=-90.0}E{ty=-90.0}",              "p:sh=1\n"
																  "p:y=-1.99, sh=1\n"
																  "p:y=-1.99, 2.0, sh=1\n"
																  "p:2.0, -1.99, 2.0, sh=1\n"
																  "j:0, 1, sh=1\n"
																  "j:1, 2, sh=1\n"
																  "j:2, 3, sh=1\n"},   // Negative rotations
			{"S:E{j=4.1}EE",                                      "p:sh=1\n"
																  "p:2.0, sh=1\n"
																  "p:4.0, sh=1\n"
																  "j:0, 1, sh=1\n"
																  "j:1, 2, sh=1\n"
																  "j:0, 2, sh=1\n"},
			{"S:E{j=3.9}EE",                                      "p:sh=1\n"
																  "p:2.0, sh=1\n"
																  "p:4.0, sh=1\n"
																  "j:0, 1, sh=1\n"
																  "j:1, 2, sh=1\n"},
			{"S:E{j=4.1}EEE",                                     "p:sh=1\n"
																  "p:2.0, sh=1\n"
																  "p:4.0, sh=1\n"
																  "p:6.0, sh=1\n"
																  "j:0, 1, sh=1\n"
																  "j:1, 2, sh=1\n"
																  "j:2, 3, sh=1\n"
																  "j:0, 2, sh=1\n"},
			{"S:EE{x=3.0}",                                       "p:sh=1\n"
																  "p:4.0, sh=1, sx=3.0\n"
																  "j:0, 1, sh=1\n"},
			{"S:EE{x=3.0;y=3.0;z=3.0}",                           "p:sh=1\n"
																  "p:4.0, sh=1, sx=3.0, sy=3.0, sz=3.0\n"
																  "j:0, 1, sh=1\n"},
			{"M:SSE",                                             "p:sh=1, sx=1.21, sy=1.21, sz=1.21\n"},  // sx modifier
			{"M:ssE",                                             "p:sh=1, sx=0.83, sy=0.83, sz=0.83\n"},  // sx modifier
			{"M:SSSE",                                            "p:sh=1, sx=1.33, sy=1.33, sz=1.33\n"},  // size modifiers
			{"M:ESSSE",                                           "p:sh=1\n"
																  "p:2.33, sh=1, sx=1.33, sy=1.33, sz=1.33\n"
																  "j:0, 1, sh=1\n"},  // size modifiers
			{"M:SSSEE",                                           "p:sh=1, sx=1.33, sy=1.33, sz=1.33\n"
																  "p:2.66, sh=1, sx=1.33, sy=1.33, sz=1.33\n"
																  "j:0, 1, sh=1\n"},  // size modifiers
			{"M:IE",                                              "p:sh=1, ing=0.28\n"},  // Ingestion modifier
			{"M:iE",                                              "p:sh=1, ing=0.23\n"},  // Ingestion modifier
			{"M:IIIIiiE",                                         "p:sh=1, ing=0.3\n"},  // Ingestion modifier
			{"S:E{i=0.3}E{i=0.5}",                                "p:sh=1, ing=0.3\n"     // Ingestion param
																  "p:2.0, sh=1, ing=0.5\n"
																  "j:0, 1, sh=1\n"},
			{"MS:IIIIiiE{i=0.5}",                                 "p:sh=1, ing=0.61\n"},  // Ingestion modifier and param
			// Test collisions
			{"S:EE{ty=180.0;x=3.0}",                              "p:sh=1\n"
																  "p:-3.99, sh=1, sx=3.0\n"
																  "j:0, 1, sh=1\n"},
			{"S:EE{ty=90.0;z=5.0}",                               "p:sh=1\n"
																  "p:z=6.0, sh=1, sz=5.0\n"
																  "j:0, 1, sh=1\n"},
			{"S:E{x=1.5;z=2.0}E{tz=90.0;x=2.0;y=5.0;z=3.0}",      "p:sh=1, sx=1.5, sz=2.0\n"
																  "p:y=5.99, sh=1, sx=2.0, sy=5.0, sz=3.0\n"
																  "j:0, 1, sh=1\n"},
			{"S:E{y=3.0;z=4.0}E{y=4.0;z=5.0}",                    "p:sh=1, sy=3.0, sz=4.0\n"
																  "p:1.99, sh=1, sy=4.0, sz=5.0\n"
																  "j:0, 1, sh=1\n"},
			{"S:E{y=399.0;z=599.0}E{y=799.0;z=999.0}",            "p:sh=1, sy=399.0, sz=599.0\n"
																  "p:53.06, sh=1, sy=799.0, sz=999.0\n"
																  "j:0, 1, sh=1\n"},  // Test a lot of spheres
			{"S:EE{ty=45.0}",                                     "p:sh=1\n"
																  "p:1.41, z=1.41, sh=1\n"
																  "j:0, 1, sh=1\n"},
			{"S:EE{ty=120.0}",                                    "p:sh=1\n"
																  "p:-0.99, z=1.73, sh=1\n"
																  "j:0, 1, sh=1\n"},
			{"S:EE{ry=90.0;z=2.0}",                               "p:sh=1\n"
																  "p:3.0, sh=1, sz=2.0, ry=90.0\n"
																  "j:0, 1, sh=1\n"},
			{"S:E{ry=90.0;z=2.0}E{ry=90.0;z=2.0}",                "p:sh=1, sz=2.0, ry=90.0\n"
																  "p:4.0, sh=1, sz=2.0, ry=90.0\n"
																  "j:0, 1, sh=1\n"},
			{"S:EE{ry=45.0;z=2.0}",                               "p:sh=1\n"
																  "p:2.57, sh=1, sz=2.0, ry=45.0\n"
																  "j:0, 1, sh=1\n"},
			{"S:EE{ry=30.0;z=2.0}",                               "p:sh=1\n"
																  "p:2.3, sh=1, sz=2.0, ry=30.0\n"
																  "j:0, 1, sh=1\n"},

			{"S:E[]",                                             "p:sh=1\n"
																  "n:p=0\n"},
			{"S:E[;]",                                            "p:sh=1\n"
																  "n:p=0\n"
																  "n:p=0\n"},
			{"S:E[]E[]",                                          "p:sh=1\n"
																  "p:2.0, sh=1\n"
																  "j:0, 1, sh=1\n"
																  "n:p=0\n"
																  "n:p=1\n"},
			{"S:E[G]",                                            "p:sh=1\n"
																  "n:p=0, d=G\n"},
			{"S:E[]E[0]",                                         "p:sh=1\n"
																  "p:2.0, sh=1\n"
																  "j:0, 1, sh=1\n"
																  "n:p=0\n"
																  "n:p=1\n"
																  "c:1, 0\n"},
			{"S:E[G;T_0]",                                        "p:sh=1\n"
																  "n:p=0, d=G\n"
																  "n:p=0, d=T\n"
																  "c:1, 0\n"},
			{"S:E[G_2;T_0;T_0_1]",                                "p:sh=1\n"
																  "n:p=0, d=G\n"
																  "n:p=0, d=T\n"
																  "n:p=0, d=T\n"
																  "c:0, 2\n"
																  "c:1, 0\n"
																  "c:2, 0\n"
																  "c:2, 1\n"},
			{"S:E[G_2;T_0;T_0_1]{ry=90.0;z=2.0}E{ry=90.0;z=2.0}", "p:sh=1, sz=2.0, ry=90.0\n"
																  "p:4.0, sh=1, sz=2.0, ry=90.0\n"
																  "j:0, 1, sh=1\n"
																  "n:p=0, d=G\n"
																  "n:p=0, d=T\n"
																  "n:p=0, d=T\n"
																  "c:0, 2\n"
																  "c:1, 0\n"
																  "c:2, 0\n"
																  "c:2, 1\n"},
			{"S:E[G_2:2.0;T_0:3.0;T_0:4.0_1:5.0]",                "p:sh=1\n"
																  "n:p=0, d=G\n"
																  "n:p=0, d=T\n"
																  "n:p=0, d=T\n"
																  "c:0, 2, 2.0\n"
																  "c:1, 0, 3.0\n"
																  "c:2, 0, 4.0\n"
																  "c:2, 1, 5.0\n"},
			{"S:E[]E[G_0]",                                       "p:sh=1\n"
																  "p:2.0, sh=1\n"
																  "j:0, 1, sh=1\n"
																  "n:p=0\n"
																  "n:j=0, d=G\n"
																  "c:1, 0\n"},
			{"S:E[]E[Rnd_0]",                                     "p:sh=1\n"
																  "p:2.0, sh=1\n"
																  "j:0, 1, sh=1\n"
																  "n:p=0\n"
																  "n:p=1, d=Rnd\n"
																  "c:1, 0\n"},
			{"S:E[Rnd]E[Rnd_0_1;Sin_0]",                          "p:sh=1\n"
																  "p:2.0, sh=1\n"
																  "j:0, 1, sh=1\n"
																  "n:p=0, d=Rnd\n"
																  "n:p=1, d=Rnd\n"
																  "n:p=1, d=Sin\n"
																  "c:1, 0\n"
																  "c:1, 1\n"
																  "c:2, 0\n"
			},
			{"S:E{s=1.5}",                                        "p:sh=1, sx=1.5, sy=1.5, sz=1.5\n"},
			{"MS:SE{s=1.1;x=1.2;z=1.3}",                          "p:sh=1, sx=1.45, sy=1.21, sz=1.57\n"},
			{"MS:SE{s=0.9}E{s=1.1;x=1.2;z=1.3}",                  "p:sh=1, sx=0.99, sy=0.99, sz=0.99\n"
																  "p:2.42, sh=1, sx=1.45, sy=1.21, sz=1.57\n"
																  "j:0, 1, sh=1\n"},
	};
	srand(time(NULL));


	int expectedPartCount[] = {
			1, 1, 1, 3, 3, 9, 2, 2, 7, 1,
			1, 1, 1, 2, 2, 2, 4, 4, 4, 3,
			3, 4, 2, 2, 1, 1, 1, 2, 2, 1,
			1, 1, 2, 1, 2, 2, 2, 2, 2, 2,
			2, 2, 2, 2, 2, 1, 1, 2, 1, 2,
			1, 1, 2, 1, 2, 2, 2, 1, 1, 2,};
	auto start = std::chrono::steady_clock::now();
	PreconfiguredGenetics genetics;

	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		testOneGenotype(test_cases[i], expectedPartCount[i]);
	}

	testAllPartSizesValid();
	testRearrangeInputs();
	validationTest();
	testCrossoverSimilarTrees();
	testRearrangeBeforeCrossover();
	testRearrangeAfterCrossover();
	testAddPart();
	testChangePartType();
	testUsePartType();
	int operationCount;
	if (argc > 1)
		operationCount = std::stod(argv[1]);
	else
		operationCount = 100;
	evolutionTest(operationCount);

	auto end = std::chrono::steady_clock::now();
	cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << endl;
	cout << "FINISHED" << endl;
	return 0;
}