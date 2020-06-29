#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <chrono>
#include <common/nonstd_math.h>
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
	return countChars(genotype, "IiFfSsTt", 8);
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
			{"1.1:EE[N]",                                   "1.1:EE[N]"},
			{"1.1:E[N;N;N]E[N;N;N]",                             "1.1:E[N;N;N]E[N;N;N]"},
			{"1.1:E[N;N;N]E[N_3;N_4_5;N_3]",                        "1.1:E[N;N;N]E[N;N;N]"},
			{"1.1:E[N_3;4;]E[N_3;N_4_5;N_3]",                      "1.1:E[N;N;N]E[N;N;N]"},
			{"1.1:E[N_1;2;0]E[N;N;N]",                          "1.1:E[N_1;N_2;N_0]E[N;N;N]"},
			{"1.1:E[N_1_3;N_2_4;N]E[N_3;N_4_5;N_3]",                  "1.1:E[N_1;N_2;N]E[N;N;N]"},
			{"1.1:E[Sin;N;G]E[Rnd;N;T]",                     "1.1:E[Sin;N;G]E[Rnd;N;T]"},
			{"1.1:E[N_1_3;N_2_4;N](E[N_3;N_4_5;N_3]^E[N_3;N_4_6_7])",     "1.1:E[N_1;N_2;N](E[N;N;N]^E[N;N_3_4])"},
			{"1.1:E[N_1_3;N_2_4;N](E[N_0_3;N_4_5;N_3_6]^E[N_3;N_4_6_7])", "1.1:E[N_1;N_2;N](E[N_0;N;N_3]^E[N;N_3_4])"},
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
			{"1.1:E[N_0_1;N_0]E[N]",                       "1.1:E[N_0_1;N_0]E[N]"},
			{"1.1:E[N_0_1;N_0]E[Rnd;N;N]",                  "1.1:E[N_0_1;N_0]E[Rnd;N;N]"},
			{"1.1:E[N_0_1;N_0]E[Rnd;N;N]E[N;N]",              "1.1:E[N_0_1;N_0]E[Rnd;N;N]E[N;N]"},
			{"1.1:E[N_0_1;N_0](E[Rnd;N;N]^E[N_2_3;N_2])",       "1.1:E[N_0_1;N_0](E[Rnd;N;N]^E[N_5_6;N_5])"},
			{"1.1:E[N_0_1;N_0](E[Rnd;N;N]^E[N_2_3;N_2]C[N_2_4])", "1.1:E[N_0_1;N_0](E[Rnd;N;N]^E[N_5_6;N_5]C[N_5_7])"},
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
	GenoOper_fS operators;
	string test_cases[] = {
			"1.1:E",
			"1.1:SE",
			"1.1:sE",
			"1.1:SSSSSSSE",
			"1.1:sssssssE",
			"1.1:SSSSSSSSSE",    // More than max
			"1.1:sssssssssE", // Less than min
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

		operators.addPart(geno, "ECR", false);

		geno.getState();
		Node *newNode = geno.getAllNodes()[1];
		assert(doubleCompare(newNode->calculateVolume(), expectedVolume[i]));
	}
}

void testChangePartType()
{
	GenoOper_fS operators;
	string test_cases[] = {
			"1.1:C",
			"1.1:E",
			"1.1:R",
			"1.1:SSSSC",
			"1.1:sssssE",
			"1.1:sssssC{x=0.3;y=2.3;z=1.1}",
			"1.1:SSSSSSE{x=0.3;y=2.3;z=1.1}",
	};

	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		fS_Genotype geno(test_cases[i]);
		geno.getState();
		double oldVolume = geno.startNode->calculateVolume();

		operators.changePartType(geno, "CER");

		geno.getState();
		assert(doubleCompare(geno.startNode->calculateVolume(), oldVolume));
	}

}
void testUsePartType()
{
	GenoOper_fS operators;
	fS_Genotype geno("1.1:E");
	operators.changePartType(geno, "C");
	assert(geno.getAllNodes()[0]->partType == Part::Shape::SHAPE_CUBOID);
	operators.changePartType(geno, "E");
	assert(geno.getAllNodes()[0]->partType == Part::Shape::SHAPE_ELLIPSOID);
	operators.changePartType(geno, "R");
	assert(geno.getAllNodes()[0]->partType == Part::Shape::SHAPE_CYLINDER);
	operators.addPart(geno, "R");
	assert(geno.getAllNodes()[1]->partType == Part::Shape::SHAPE_CYLINDER);
	operators.removePart(geno);
	operators.addPart(geno, "C");
	assert(geno.getAllNodes()[1]->partType == Part::Shape::SHAPE_CUBOID);
	operators.removePart(geno);
	operators.addPart(geno, "E");
	assert(geno.getAllNodes()[1]->partType == Part::Shape::SHAPE_ELLIPSOID);

}

void testTurnWithRotation()
{
	GenoConv_fS0 converter = GenoConv_fS0();
	fS_Genotype::TURN_WITH_ROTATION = true;
	MultiMap map;
	SString test_cases[]{
			"1.1:EE{ty=1.56}",
			"1.1:EE{ty=0.78;ry=0.78}",
			"1.1:EE{tx=30;ty=1.56;tz=45}",
	};

	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		SString genotype_str = test_cases[i];
		/// Test translate
		cout << converter.convert(genotype_str, &map, false).c_str() << endl;
	}
	fS_Genotype::TURN_WITH_ROTATION = false;

}

/**
 * Cases when exchanging trees with similar size aways makes children of the equal parents equal to them
 * Test cases will almost always work when crossoverTries is big enough
 */
void testCrossoverSimilarTrees()
{
	GenoOper_fS operators;
	string test_cases[] = {
			"1.1:EE",
			"1.1:E(E^E)",
			"1.1:EEEE",
			"1.1:ECRE",
			"1.1:E(RE^CRE)",
			"1.1:E(EEE^EEE^EEE)",
			"1.1:E(CRE^CRE^CRE)",
			"1.1:EEEEEECRE(CRE^CRE^CRE)",
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
			"1.1:C{x=2000.0}",    // Too big dimension
			"1.1:C{y=2000.0}",
			"1.1:C{z=2000.0}",
			"1.1:C{x=0.0005}",    // Too small dimension
			"1.1:C{y=0.0005}",
			"1.1:C{z=0.0005}",
			"1.1:E{x=1.1}",        // Invalid size params
			"1.1:E{y=1.1}",
			"1.1:E{z=1.1}",
			"1.1:R{x=1.1;y=1.2}",
			"1.1:R{x=1.1}",
			"1.1:R{y=1.1}",
			"1.1:SR{x=999.0}",
			"1.1:C(R^E{z=1.1})",
			"1.1:C{x=1.5;y=1.5;z=1.5}",    // Test volume
			"1.1:C{x=1.8;y=1.8}",
	};

	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		fS_Genotype geno(test_cases[i]);
		assert(geno.checkValidityOfPartSizes() != 0);
	}
}

void testRandomModifications(string test)
{
	GenoOper_fS operators;
	for(int i=0; i<20; i++)
	{
		int index = rndUint(test.length());
		test.insert(index, string(1, (char)rndUint(256)));
		operators.checkValidity(test.c_str(), "");
	}
}

void testOneGenotype(SString test, int expectedPartCount)
{
	GenoOper_fS operators;
	GenoConv_fS0 converter = GenoConv_fS0();
	MultiMap map;
	int tmp = -1;
	SString tmpStr;

	/// Test get geno
	fS_Genotype geno(test.c_str());
	cout << geno.getGeno().c_str() << endl;

	/// Test translate
	cout << converter.convert(test, &map, false).c_str() << endl;

	////Test operations
	// Test part count
	assert(geno.getNodeCount() == expectedPartCount);

	// Test add part
	tmp = geno.getNodeCount();
	operators.addPart(geno);
	assert(tmp + 1 == geno.getNodeCount());

	// Test change part
	tmp = geno.getNodeCount();
	tmpStr = geno.getGeno();
	if (operators.changePartType(geno))
	{
		assert(geno.getNodeCount() == tmp);
		assert(geno.getGeno() != tmpStr);
	}

	// Test remove part
	tmp = geno.getNodeCount();
	if (operators.removePart(geno))
		assert(tmp == 1 + geno.getNodeCount());

	// Test change joint
	char firstJoint;
	if(geno.getNodeCount() == 2)
		firstJoint = geno.getAllNodes()[1]->joint;
	tmpStr = geno.getGeno();
	if (operators.changeJoint(geno))
	{
		assert(tmpStr != geno.getGeno());
		if(geno.getNodeCount() == 2)
			// If there are only 2 nodes, we know which joint has been changed
			assert(geno.getAllNodes()[1]->joint != firstJoint);
	}

	// Test add param
	tmp = countParams(geno.getGeno());
	if (operators.addParam(geno))
		assert(tmp + 1 == countParams(geno.getGeno()));

	// Test change param
	tmpStr = geno.getGeno();
	tmp = countParams(geno.getGeno());
	fS_Genotype::precision = 6;
	if (operators.changeParam(geno))
	{
		SString resultGeno = geno.getGeno();
		assert(tmp == countParams(resultGeno));
		assert(tmpStr != resultGeno);
	}
	fS_Genotype::precision = 2;

	// Test remove param
	tmp = countParams(geno.getGeno());
	if (operators.removeParam(geno))
		assert(tmp == 1 + countParams(geno.getGeno()));

	// Test add modifier
	tmp = countModifiers(geno.getGeno());
	if (operators.changeModifier(geno))
		assert(tmp != countModifiers(geno.getGeno()));

	// Test add neuro
	tmp = geno.getAllNeurons().size();
	if (operators.addNeuro(geno))
		assert(tmp + 1 == int(geno.getAllNeurons().size()));

	// Test add neuro connections
	tmp = countNeuroConnections(geno);
	if (operators.addNeuroConnection(geno))
		assert(tmp + 1 == countNeuroConnections(geno));

	// Test change neuro connection
	tmpStr = geno.getGeno();
	if (operators.changeNeuroConnection(geno))
		assert(test != geno.getGeno());

	// Test remove neuro connections
	tmp = countNeuroConnections(geno);
	if (operators.removeNeuroConnection(geno))
		assert(tmp - 1 == countNeuroConnections(geno));

	// Test remove neuro
	tmp = geno.getAllNeurons().size();
	if (operators.removeNeuro(geno))
		assert(tmp - 1 == int(geno.getAllNeurons().size()));

	// Test change neuro params
	tmpStr = geno.getGeno();
	if(operators.changeNeuroParam(geno))
		assert(tmpStr != geno.getGeno());

	testRandomModifications(test.c_str());
}

void validationTest()
{
	GenoConv_fS0 converter = GenoConv_fS0();
	GenoOper_fS operators;
	SString invalidGenotypes[] = {
			"1.1:FFF",    // No part type
			"1.1:FFF{x=5.0}",    // No part type
			"1.1:qqE",    // Invalid modifier
			"1.1:E{f}",    // No equal sign
			"1.1:E{qw=1.0}",    // Wrong param key
			"1.1:E{f=}",    // Wrong param value
			"1.1:E{f=fr}",    // Wrong param value
			"1.1:E[G_w_2]",    // Invalid neuro connection key
			"1.1:E[G_1:w_2]",    // Invalid neuro connection value
			"1.1:E{",    // Lacking param end
			"1.1:E[",    // Lacking neuro end
			"1.1:E{x=1.5;y=0.0}",    // Lacking param end
			"1.1:E[2]",    // Invalid neuron connection key
			"1.1:E[-2]",    // Invalid neuron connection key
			"1.1:E[;;3]",    // Invalid neuron connection key
	};
	int errorIndexes[] = {
			5, 5, 5, 6,
			6, 8, 8, 6, 6,
			6, 6, 14, 1, 1,
			1,
	};
	for (int i = 0; i < int(sizeof(invalidGenotypes) / sizeof(invalidGenotypes[0])); i++)
	{
		MultiMap map;
		assert(operators.checkValidity(invalidGenotypes[i].c_str(), "") == errorIndexes[i]);
		SString genes = converter.convert(invalidGenotypes[i], &map, false);
		assert(genes == "");
	}
}

void testRearrangeInputs()
{
	const int size = 6;
	string before = "1.1:E[T]bE[N_2_3]cRbC[T;G_1_2]bE[N_1_2_3;T]{x=3.0;y=3.0;z=3.0}";
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
			"1.1:E[T]bE[N_3_4]cRbC[T;G_2_3]bE[N_2_3_4;T]{x=3.0;y=3.0;z=3.0}",
			"1.1:E[T]bE[N_3_4]cRbC[T;G_1_3]bE[N_1_3_4;T]{x=3.0;y=3.0;z=3.0}",
			"1.1:E[T]bE[N_2_3]cRbC[T;G_1_2]bE[N_1_2_3;T]{x=3.0;y=3.0;z=3.0}",
			"1.1:E[T]bE[N_1_2]cRbC[T;G_0_1]bE[N_0_1_2;T]{x=3.0;y=3.0;z=3.0}",
			"1.1:E[T]bE[N_2]cRbC[T;G_1]bE[N_1_2;T]{x=3.0;y=3.0;z=3.0}",
			"1.1:E[T]bE[N_2_3]cRbC[T;G_1_2]bE[N_1_2_3;T]{x=3.0;y=3.0;z=3.0}"
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
	int failCount = 0;
	assert(strcmp(operators.getSimplest(), "C{x=0.80599;y=0.80599;z=0.80599}") == 0);

	SString **gens = new SString *[gen_size];
	gens[0] = new SString("1.1:EbcE[1_2]cRbC[G_0_2]bC[0_1_2]{x=1.02;y=1.02;z=1.03}");
	gens[1] = new SString("1.1:RcR[N_0]bR[0_1]");
	gens[2] = new SString("1.1:R[0;0_1]{ty=2.1;tz=4.3;z=1.1}bRcR");
	gens[3] = new SString("1.1:R[1]{z=1.04}R[1]cRC[0;1]{x=1.03}");
	gens[4] = new SString("1.1:E(cE(bE[T;T_1_2]^cE^bC[N_0]^cR)^bE[0_2;0_2]^cE(bcE^bcE[;0_1_2])^E)");


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
//			cout << i << " out of " << operationCount << " Length: " << gens[i1]->len() + gens[i2]->len() << endl;
		}

		int method;
		float f1, f2, gp;

		char *arr1 = strdup(gens[i1]->c_str());
		char *arr2 = strdup(gens[i2]->c_str());

		testRandomModifications(arr1);
		testRandomModifications(arr2);

		if (operators.mutate(arr1, gp, method) == GENOPER_OK)
			methodUsages[method]++;

		if (operators.mutate(arr2, gp, method) == GENOPER_OK)
			methodUsages[method]++;

		int crossOverResult = operators.crossOver(arr1, arr2, f1, f2);

		if (crossOverResult == GENOPER_OK && 0 == operators.checkValidity(arr1, "") && 0 == operators.checkValidity(arr2, ""))
		{
			assert(0. <= f1 && f1 <= 1.);
			assert(0. <= f2 && f2 <= 1.);

			delete gens[i1];
			delete gens[i2];
			gens[i1] = new SString(arr1);
			gens[i2] = new SString(arr2);

			// Check if genotypes convert correctly
			MultiMap map;
			assert(converter.convert(*gens[i1], &map, false) != "");
			assert(converter.convert(*gens[i2], &map, false) != "");
		}
		else
		{
			failCount++;
		}

		free(arr1);
		free(arr2);
	}
//	cout<< "Fails: "<<failCount<<std::endl<<std::endl;
//	cout << "Method usages:" << endl;
//	for (int i = 0; i < FS_OPCOUNT; i++)
//		cout << i << ": " << methodUsages[i] << endl;
	for (int i = 0; i < gen_size; i++)
		delete gens[i];
	delete[] gens;
	fclose(pFile);
}

int main(int argc, char *argv[])
{
	SString test_cases[] = {
			"1.1:E",
			"1.1:C",
			"1.1:R",
			"1.1:EEE",
			"1.1:E(E^E)",
			"1.1:E(E(E^E)^E^E(E^E)^E)",
			"1.1:EbE",
			"1.1:CcC",
			"1.1:ERbRcCRbCbE",
			"1.1:E",
			"1.1:FE",
			"1.1:fE",
			"1.1:FFE",
			"1.1:E{f=0.3}E{f=0.5}",
			"1.1:EE{ty=1.56}",
			"1.1:EE{tz=1.56}",
			"1.1:EE{tz=1.56}E{tx=1.56}E{ty=1.56}",
			"1.1:EE{tz=0.78}E{tx=0.78}E{ty=0.78}",
			"1.1:EE{tz=-1.56}E{tx=-1.56}E{ty=-1.56}",
			"1.1:EE{x=3.0}",
			"1.1:EE{x=3.0;y=3.0;z=3.0}",
			"1.1:SSE",
			"1.1:ssE",
			"1.1:SSSE",
			"1.1:ESSSE",
			"1.1:SSSEE",
			"1.1:IE",
			"1.1:iE",
			"1.1:IIE",
			"1.1:E{i=0.3}E{i=0.5}",
			"1.1:IIE{i=0.5}",
			"1.1:EE{ty=180.0;x=3.0}",
			"1.1:EE{ty=1.56;z=5.0}",
			"1.1:E{x=1.5;z=2.0}E{tz=1.56;x=2.0;y=5.0;z=3.0}",
			"1.1:E{y=3.0;z=4.0}E{y=4.0;z=5.0}",
			"1.1:E{y=399.0;z=599.0}E{y=799.0;z=999.0}",
			"1.1:EE{ty=0.78}",
			"1.1:EE{ty=1.56}",
			"1.1:EE{ry=1.56;z=2.0}",
			"1.1:E{ry=1.56;z=2.0}E{ry=1.56;z=2.0}",
			"1.1:EE{ry=0.78;z=2.0}",
			"1.1:EE{ry=0.52;z=2.0}",
			"1.1:E[N]",
			"1.1:E[N;N]",
			"1.1:E[N]E[N]",
			"1.1:E[T]",
			"1.1:E[N]E[N_0]",
			"1.1:E[Sin;T_0]",
			"1.1:E[Sin_2;T_0;T_0_1]",
			"1.1:E[Sin_2;T_0;T_0_1]{ry=1.56;z=2.0}E{ry=1.56;z=2.0}",
			"1.1:E[Sin_2:2.0;T_0:3.0;T_0:4.0_1:5.0]",
			"1.1:E[N]E[G_0]",
			"1.1:E[N]E[Rnd_0]",
			"1.1:E[Rnd]E[Rnd_0_1;Sin_0]",
			"1.1:E{s=1.5}",
			"1.1:SE{s=1.1;x=1.2;z=1.3}",
			"1.1:SE{s=0.9}E{s=1.1;x=1.2;z=1.3}",
			"1.1:ETTE{st=0.5}",
			"1.1:EttE{st=0.5}",
			"1.3:SE",
	};
	srand(time(NULL));


	int expectedPartCount[] = {
			1, 1, 1, 3, 3, 9, 2, 2, 7, 1,
			1, 1, 1, 2, 2, 2, 4, 4, 4, 2,
			2, 1, 1, 1, 2, 2, 1,
			1, 1, 2, 1, 2, 2, 2, 2, 2, 2,
			2, 2, 2, 2, 2, 1, 1, 2, 1, 2,
			1, 1, 2, 1, 2, 2, 2, 1, 1, 2,
			2, 2, 1};
	auto start = std::chrono::steady_clock::now();
	PreconfiguredGenetics genetics;

	fS_Genotype::precision = 2;

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
	testTurnWithRotation();
	int operationCount;
	if (argc > 1)
		operationCount = std::stod(argv[1]);
	else
		operationCount = 100;

	evolutionTest(operationCount);

	auto end = std::chrono::steady_clock::now();
//	cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << endl;
	cout << "FINISHED";
	return 0;
}