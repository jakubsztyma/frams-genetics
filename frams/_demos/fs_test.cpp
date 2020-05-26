#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <chrono>
#include "frams/genetics/fs/fS_general.h"
#include "frams/genetics/fs/fS_conv.h"
#include "frams/genetics/fs/fS_oper.h"

using namespace std;


int countSigns(SString genotype, string chars, int count)
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
	return countSigns(genotype, JOINTS, 2);
}

int countParams(SString genotype)
{
	return countSigns(genotype, "=", 1);
}

int countModifiers(SString genotype)
{
	return countSigns(genotype, "IiFfXxYyZz", 10);
}

int countNeuroConnections(fS_Genotype &geno)
{
	vector<Neuron*> neurons = geno.getAllNeurons();
	int result = 0;
	for (unsigned int i = 0; i < neurons.size(); i++)
		result += neurons[i]->inputs.size();
	return result;
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
	fS_Genotype geno(test[0]);
	cout << geno.getGeno().c_str() << endl;
	assert(geno.getGeno() == test[0]);

	////Test operations
	// Test part count
	assert(geno.getNodeCount() == expectedPartCount);

	// Test add part
	tmp = geno.getNodeCount();
	geno.addPart();
	assert(tmp + 1 == geno.getNodeCount());

	// Test change part
	tmp = geno.getNodeCount();
	if (geno.changePartType())
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
	if (geno.addParam())
		assert(tmp + 1 == countParams(geno.getGeno()));

	// Test change param
	tmpStr = geno.getGeno();
	tmp = countParams(geno.getGeno());
	if (geno.changeParam())
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
		assert(tmp + 1 == (int) geno.getAllNeurons().size());

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
		assert(tmp - 1 == (int) geno.getAllNeurons().size());
}

void validationTest()
{
	GenoConv_fS0 converter = GenoConv_fS0();
	fS_Operators operators;
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
		cout << invalidGenotypes[i].c_str() << endl;
		assert(1 == operators.checkValidity(invalidGenotypes[i].c_str(), ""));
		SString genes = converter.convert(invalidGenotypes[i], &map, false);
		assert(genes == "");
	}
}

void testRearrangeInputs()
{
	int size = 6;
	SString before = "MSJ:E[T]bcE[2_3]cCbP[T;G1_2]bE[1_2_3;T]{x=3.0;y=3.0;z=3.0}";
	int shift[size] = {
			1,
			1,
			1,
			-1,
			-1,
			-1,
	};
	int neuronNumber[size] = {
			0,	// First
			2,   // Middle
			5,	// Last
			0,
			2,
			5,
	};
	SString after[size] = {
			"MSJ:E[T]bcE[3_4]cCbP[T;G2_3]bE[2_3_4;T]{x=3.0;y=3.0;z=3.0}",
			"MSJ:E[T]bcE[3_4]cCbP[T;G1_3]bE[1_3_4;T]{x=3.0;y=3.0;z=3.0}",
			"MSJ:E[T]bcE[2_3]cCbP[T;G1_2]bE[1_2_3;T]{x=3.0;y=3.0;z=3.0}",
			"MSJ:E[T]bcE[1_2]cCbP[T;G0_1]bE[0_1_2;T]{x=3.0;y=3.0;z=3.0}",
			"MSJ:E[T]bcE[2]cCbP[T;G1]bE[1_2;T]{x=3.0;y=3.0;z=3.0}",
			"MSJ:E[T]bcE[2_3]cCbP[T;G1_2]bE[1_2_3;T]{x=3.0;y=3.0;z=3.0}"
	};

	for(int i=0; i<size; i++)
	{
		fS_Genotype geno(before);
		vector<Neuron*> allNeurons = geno.getAllNeurons();
		Neuron *neuron = allNeurons[neuronNumber[i]];

		geno.rearrangeNeuronConnections(neuron, shift[i]);

		cout<<geno.getGeno().c_str()<<endl;
		assert(geno.getGeno() == after[i]);
	}

}

void evolutionTest()
{
	int gen_size = 5;
	fS_Operators operators;
	SString **gens = new SString *[gen_size];
	gens[0] = new SString("SMJ:EbcE[1_2]cCbP[G0_2]bE[0_1_2]{x=3.0;y=3.0;z=3.0}");
	gens[1] = new SString("SMJ:C{j=3.9}cC[0]bC[0_1]");
	gens[2] = new SString("SMJ:C[0;0_1]{j=3.9;ty=2.1;tz=4.3;x=2.0;y=3.4;z=5.1}bCcC");
	gens[3] = new SString("SMJ:C[1]{j=3.9;x=2.0;y=3.4;z=5.1}C[1]cCP[0;1]{x=4.3}");
	gens[4] = new SString("SMJ:E(cE(bE[T;T1_2],cE,bP[0],cC),bE[0_2;0_2],cE(bcE,bcE[;0_1_2]),E)");


	FILE *pFile = fopen("output.txt", "w");
	int operationCount = 2000;
	int methodUsages[FS_OPCOUNT];
	for (int i = 0; i < FS_OPCOUNT; i++)
		methodUsages[i] = 0;

	for (int i = 0; i < operationCount; i++)
	{
		int i1 = rand() % gen_size;
		int i2 = rand() % gen_size;
		if (i2 == i1)
			i2 = (i1 + 1) % gen_size;

		cout << i << " out of " << operationCount << " Length: " << gens[i1]->len() + gens[i2]->len() << endl;
		cout << gens[i1]->c_str() << endl;
		cout << gens[i2]->c_str() << endl;
		int method;
		float f1, f2, gp;

		char *arr1 = strdup(gens[i1]->c_str());
		char *arr2 = strdup(gens[i2]->c_str());

		if (operators.mutate(arr1, gp, method) == GENOPER_OK)
			methodUsages[method] ++;
		if (operators.mutate(arr2, gp, method) == GENOPER_OK)
			methodUsages[method] ++;

		int crossOverResult = operators.crossOver(arr1, arr2, f1, f2);

		assert(0. < f1 && f1 < 1.);
		assert(0. < f2 && f2 < 1.);

		if (crossOverResult == GENOPER_OK)
		{
			if (1 == operators.checkValidity(arr2, ""))
				cout << arr2;
			assert(0 == operators.checkValidity(arr1, ""));
			assert(0 == operators.checkValidity(arr2, ""));
		}

		delete gens[i1];
		delete gens[i2];
		gens[i1] = new SString(arr1);
		gens[i2] = new SString(arr2);

//		fprintf(pFile, gens[i1]->c_str());
//		fprintf(pFile, "\n");
//		fprintf(pFile, converter.convert(*gens[i1], &map, false).c_str());
//		fprintf(pFile, "\n");
//		fprintf(pFile, gens[i2]->c_str());
//		fprintf(pFile, "\n");
//		fprintf(pFile, converter.convert(*gens[i2], &map, false).c_str());
//		fprintf(pFile, "\n");

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

int main()
{
	srand(time(NULL));

	SString test_cases[][2] = {
			{"S:E",                                            "p:sh=1\n"},
			{"S:P",                                            "p:sh=2\n"},
			{"S:C",                                            "p:sh=3\n"},
			{"S:EEE",                                          "p:sh=1\np:2.0, sh=1\np:4.0, sh=1\nj:1, 2, sh=1\nj:0, 1, sh=1\n"},
			{"S:E(E,E)",                                       "p:sh=1\np:2.0, sh=1\np:2.0, sh=1\nj:0, 1, sh=1\nj:0, 2, sh=1\n"},
			{"S:E(E(E,E),E,E(E,E),E)",                         "p:sh=1\n"
															   "p:2.0, sh=1\n"
															   "p:4.0, sh=1\n"
															   "p:4.0, sh=1\n"
															   "p:2.0, sh=1\n"
															   "p:2.0, sh=1\n"
															   "p:4.0, sh=1\n"
															   "p:4.0, sh=1\n"
															   "p:2.0, sh=1\n"
															   "j:1, 2, sh=1\n"
															   "j:1, 3, sh=1\n"
															   "j:0, 1, sh=1\n"
															   "j:0, 4, sh=1\n"
															   "j:5, 6, sh=1\n"
															   "j:5, 7, sh=1\n"
															   "j:0, 5, sh=1\n"
															   "j:0, 8, sh=1\n"
			},
			{"S:EbE",                                          "p:sh=1\n"
															   "p:2.0, sh=1\n"
															   "j:0, 1, sh=2\n"}, // Parametrized joints
			{"S:PbcP",                                         "p:sh=2\n"
															   "p:2.0, sh=2\n"
															   "j:0, 1, sh=2\n"
															   "j:0, 1, sh=3\n"}, // Many parametrized joints
			{"S:ECbCcPCbcPbcE",                                "p:sh=1\n"
															   "p:2.0, sh=3\n"
															   "p:4.0, sh=3\n"
															   "p:6.0, sh=2\n"
															   "p:8.0, sh=3\n"
															   "p:10.0, sh=2\n"
															   "p:12.0, sh=1\n"
															   "j:5, 6, sh=2\n"
															   "j:5, 6, sh=3\n"
															   "j:4, 5, sh=2\n"
															   "j:4, 5, sh=3\n"
															   "j:3, 4, sh=1\n"
															   "j:2, 3, sh=3\n"
															   "j:1, 2, sh=2\n"
															   "j:0, 1, sh=1\n"},
// Modifier mode
			{"M:E",                                            "p:sh=1\n"},  // Basic modifier mode
			{"M:FE",                                           "p:sh=1, fr=0.44\n"},  // Friction modifier
			{"M:fE",                                           "p:sh=1, fr=0.36\n"},  // Friction modifier
			{"M:FFFFffE",                                      "p:sh=1, fr=0.48\n"},  // Friction modifier
			{"S:E{f=0.3}E{f=0.5}",                             "p:sh=1, fr=0.3\n"
															   "p:2.0, sh=1, fr=0.5\n"
															   "j:0, 1, sh=1\n"},
			{"S:EE{ty=90.0}",                                  "p:sh=1\n"
															   "p:z=2.0, sh=1\n"
															   "j:0, 1, sh=1\n"},
			{"S:EE{tz=90.0}",                                  "p:sh=1\n"
															   "p:y=2.0, sh=1\n"
															   "j:0, 1, sh=1\n"},  // Z rotation
			{"S:EE{tz=90.0}E{tx=90.0}E{ty=90.0}",              "p:sh=1\n"
															   "p:y=2.0, sh=1\n"
															   "p:y=2.0, 2.0, sh=1\n"
															   "p:-1.99, 2.0, 2.0, sh=1\n"
															   "j:2, 3, sh=1\n"
															   "j:1, 2, sh=1\n"
															   "j:0, 1, sh=1\n"},  // All rotations
			{"S:EE{tz=45.0}E{tx=45.0}E{ty=45.0}",              "p:sh=1\n"
															   "p:1.41, 1.41, sh=1\n"
															   "p:2.83, 2.41, 1.0, sh=1\n"
															   "p:3.12, 3.41, 2.71, sh=1\n"
															   "j:2, 3, sh=1\n"
															   "j:1, 2, sh=1\n"
															   "j:0, 1, sh=1\n"},  // Acute angle rotations
			{"S:EE{tz=-90.0}E{tx=-90.0}E{ty=-90.0}",           "p:sh=1\n"
															   "p:y=-1.99, sh=1\n"
															   "p:y=-1.99, 2.0, sh=1\n"
															   "p:2.0, -1.99, 2.0, sh=1\n"
															   "j:2, 3, sh=1\n"
															   "j:1, 2, sh=1\n"
															   "j:0, 1, sh=1\n"},   // Negative rotations
			{"S:E{j=4.1}EE",                                   "p:sh=1\n"
															   "p:2.0, sh=1\n"
															   "p:4.0, sh=1\n"
															   "j:1, 2, sh=1\n"
															   "j:0, 1, sh=1\n"
															   "j:0, 2, sh=1\n"},
			{"S:E{j=3.9}EE",                                   "p:sh=1\n"
															   "p:2.0, sh=1\n"
															   "p:4.0, sh=1\n"
															   "j:1, 2, sh=1\n"
															   "j:0, 1, sh=1\n"},
			{"S:E{j=4.1}EEE",                                  "p:sh=1\n"
															   "p:2.0, sh=1\n"
															   "p:4.0, sh=1\n"
															   "p:6.0, sh=1\n"
															   "j:2, 3, sh=1\n"
															   "j:1, 2, sh=1\n"
															   "j:0, 1, sh=1\n"
															   "j:0, 2, sh=1\n"},
			{"S:EE{x=3.0}",                                    "p:sh=1\n"
															   "p:4.0, sh=1, sx=3.0\n"
															   "j:0, 1, sh=1\n"},
			{"S:EE{x=3.0;y=3.0;z=3.0}",                        "p:sh=1\n"
															   "p:4.0, sh=1, sx=3.0, sy=3.0, sz=3.0\n"
															   "j:0, 1, sh=1\n"},
			{"M:XXE",                                          "p:sh=1, sx=1.21\n"},  // sx modifier
			{"M:xxE",                                          "p:sh=1, sx=0.83\n"},  // sx modifier
			{"M:XYYZZZE",                                      "p:sh=1, sx=1.1, sy=1.21, sz=1.33\n"},  // size modifiers
			{"M:EXYYZZZE",                                     "p:sh=1\n"
															   "p:2.08, sh=1, sx=1.1, sy=1.21, sz=1.33\n"
															   "j:0, 1, sh=1\n"},  // size modifiers
			{"M:XYYZZZEE",                                     "p:sh=1, sx=1.1, sy=1.21, sz=1.33\n"
															   "p:2.18, sh=1, sx=1.1, sy=1.21, sz=1.33\n"
															   "j:0, 1, sh=1\n"},  // size modifiers
			{"M:IE",                                           "p:sh=1, ing=0.28\n"},  // Ingestion modifier
			{"M:iE",                                           "p:sh=1, ing=0.23\n"},  // Ingestion modifier
			{"M:IIIIiiE",                                      "p:sh=1, ing=0.3\n"},  // Ingestion modifier
			{"S:E{i=0.3}E{i=0.5}",                             "p:sh=1, ing=0.3\n"     // Ingestion param
															   "p:2.0, sh=1, ing=0.5\n"
															   "j:0, 1, sh=1\n"},
			{"MS:IIIIiiE{i=0.5}",                              "p:sh=1, ing=0.61\n"},  // Ingestion modifier and param
			// Test collisions
			{"S:EE{ty=180.0;x=3.0}",                           "p:sh=1\n"
															   "p:-3.99, sh=1, sx=3.0\n"
															   "j:0, 1, sh=1\n"},
			{"S:EE{ty=90.0;z=5.0}",                            "p:sh=1\n"
															   "p:z=6.0, sh=1, sz=5.0\n"
															   "j:0, 1, sh=1\n"},
			{"S:E{x=1.5;z=2.0}E{tz=90.0;x=2.0;y=5.0;z=3.0}",   "p:sh=1, sx=1.5, sz=2.0\n"
															   "p:y=5.99, sh=1, sx=2.0, sy=5.0, sz=3.0\n"
															   "j:0, 1, sh=1\n"},
			{"S:E{y=3.0;z=4.0}E{y=4.0;z=5.0}",                 "p:sh=1, sy=3.0, sz=4.0\n"
															   "p:1.99, sh=1, sy=4.0, sz=5.0\n"
															   "j:0, 1, sh=1\n"},
			{"S:E{y=399.0;z=599.0}E{y=799.0;z=999.0}",         "p:sh=1, sy=399.0, sz=599.0\n"
															   "p:53.06, sh=1, sy=799.0, sz=999.0\n"
															   "j:0, 1, sh=1\n"},  // Test a lot of spheres
			{"S:EE{ty=45.0}",                                  "p:sh=1\n"
															   "p:1.41, z=1.41, sh=1\n"
															   "j:0, 1, sh=1\n"},
			{"S:EE{ty=120.0}",                                 "p:sh=1\n"
															   "p:-0.99, z=1.73, sh=1\n"
															   "j:0, 1, sh=1\n"},
			{"S:EE{ry=90.0;z=2.0}",                            "p:sh=1\n"
															   "p:3.0, sh=1, sz=2.0, ry=90.0\n"
															   "j:0, 1, sh=1\n"},
			{"S:E{ry=90.0;z=2.0}E{ry=90.0;z=2.0}",             "p:sh=1, sz=2.0, ry=90.0\n"
															   "p:4.0, sh=1, sz=2.0, ry=90.0\n"
															   "j:0, 1, sh=1\n"},
			{"S:EE{ry=45.0;z=2.0}",                            "p:sh=1\n"
															   "p:2.57, sh=1, sz=2.0, ry=45.0\n"
															   "j:0, 1, sh=1\n"},
			{"S:EE{ry=30.0;z=2.0}",                            "p:sh=1\n"
															   "p:2.3, sh=1, sz=2.0, ry=30.0\n"
															   "j:0, 1, sh=1\n"},

			{"S:E[]",                                          "p:sh=1\n"
															   "n:p=0\n"},
			{"S:E[;]",                                         "p:sh=1\n"
															   "n:p=0\n"
															   "n:p=0\n"},
			{"S:E[]E[]",                                       "p:sh=1\n"
															   "p:2.0, sh=1\n"
															   "j:0, 1, sh=1\n"
															   "n:p=0\n"
															   "n:p=1\n"},
			{"S:E[G]",                                         "p:sh=1\n"
															   "n:p=0, d=G\n"},
			{"S:E[]E[0]",                                      "p:sh=1\n"
															   "p:2.0, sh=1\n"
															   "j:0, 1, sh=1\n"
															   "n:p=0\n"
															   "n:p=1\n"
															   "c:1, 0\n"},
			{"S:E[G;T0]",                                      "p:sh=1\n"
															   "n:p=0, d=G\n"
															   "n:p=0, d=T\n"
															   "c:1, 0\n"},
			{"S:E[G2;T0;T0_1]",                                "p:sh=1\n"
															   "n:p=0, d=G\n"
															   "n:p=0, d=T\n"
															   "n:p=0, d=T\n"
															   "c:0, 2\n"
															   "c:1, 0\n"
															   "c:2, 0\n"
															   "c:2, 1\n"},
			{"S:E[G2;T0;T0_1]{ry=90.0;z=2.0}E{ry=90.0;z=2.0}", "p:sh=1, sz=2.0, ry=90.0\n"
															   "p:4.0, sh=1, sz=2.0, ry=90.0\n"
															   "j:0, 1, sh=1\n"
															   "n:p=0, d=G\n"
															   "n:p=0, d=T\n"
															   "n:p=0, d=T\n"
															   "c:0, 2\n"
															   "c:1, 0\n"
															   "c:2, 0\n"
															   "c:2, 1\n"},
			{"S:E[G2:2.0;T0:3.0;T0:4.0_1:5.0]",                "p:sh=1\n"
															   "n:p=0, d=G\n"
															   "n:p=0, d=T\n"
															   "n:p=0, d=T\n"
															   "c:0, 2, 2.0\n"
															   "c:1, 0, 3.0\n"
															   "c:2, 0, 4.0\n"
															   "c:2, 1, 5.0\n"},
	};

	const int size = 54;
	int expectedPartCount[] = {1, 1, 1, 3, 3, 9, 2, 2, 7, 1, 1, 1, 1, 2, 2, 2, 4, 4, 4, 3, 3, 4, 2, 2, 1, 1, 1, 2,
							   2, 1, 1, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 1, 2, 1, 1, 2, 1};
	auto start = chrono::steady_clock::now();

	for (int i = 0; i < size; i++)
	{
		testOneGenotype(test_cases[i], expectedPartCount[i]);
	}

	testRearrangeInputs();
	validationTest();
	evolutionTest();

	auto end = chrono::steady_clock::now();
	cout << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
	cout << "FINISHED" << endl;
	return 0;
}