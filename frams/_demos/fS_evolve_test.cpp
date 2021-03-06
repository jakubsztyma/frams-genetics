#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <chrono>
#include <common/nonstd_math.h>
#include "frams/genetics/fS/fS_general.h"
#include "frams/genetics/fS/fS_conv.h"
#include "frams/genetics/fS/fS_oper.h"
#include "frams/genetics/preconfigured.h"
#include "frams/model/geometry/geometryutils.h"
#include "frams/model/geometry/modelgeometryinfo.h"

using std::cout;
using std::endl;

void ensure(bool condition)
{
	if(!condition)
	{
		std::cout<<"ensure() failed!"<<std::endl;
		exit(0);
	}
}

int countChars(SString genotype, string chars, int count)
{
	int result = 0;
	for (int i = 0; i < genotype.length(); i++)
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

double evaluate(const char *geno)
{
	int nodeCount = fS_Genotype(geno).getNodeCount();
//	return 1.0 / nodeCount;
	return -fabs(5.0 - nodeCount);
}

void evolutionTest(int operationCount)
{
	GenoConv_fS0s converter = GenoConv_fS0s();
	int gen_size = 5;
	GenoOper_fS operators;
	PreconfiguredGenetics genetics;
	GenMan genman;
	int failCount = 0;
	ensure(strcmp(operators.getSimplest(), "1.1:C{x=0.80599;y=0.80599;z=0.80599}") == 0);

	SString **gens = new SString *[gen_size];
	gens[0] = new SString("1.1:EcE[N'1'2]cRbC[G'0'2]bC[N'0'1'2]{x=1.02;y=1.02;z=1.03}");
	gens[1] = new SString("1.1:RcR[N'0]bR[N'0'1]");
	gens[2] = new SString("1.1:EcE[N'1'2]cRbC[G'0'2]bC[N'0'1'2]{x=1.02;y=1.02;z=1.03}");
	gens[3] = new SString("1.1:R[N'1]{x=1.04}R[N'1]cRC[N'0;N'1]{x=1.03}");
	gens[4] = new SString("1.1:E(cE(bE[T;T'1'2]^cE^bC[N'0]^cR)^bE[N'0'2;N'0'2]^cE(bcE^bcE[N;N'0'1'2])^E)");


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

		if (i % 10 == 0)
		{
			cout << i << " out of " << operationCount << " Length: " << gens[i1]->length() + gens[i2]->length() << " Evaluation: " << evaluate(gens[i1]->c_str()) << endl;
		}

		int method;
		float f1, f2, gp;

		char *arr1 = strdup(gens[i1]->c_str());
		char *arr2 = strdup(gens[i2]->c_str());

//		Model model;
//		model.open(true);
//		fS_Genotype(arr1).buildModel(model);
//		model.close();
//		const double density = 1.0;
//		double volume = ModelGeometryInfo::volume(model, density);

		testRandomModifications(arr1);
		testRandomModifications(arr2);

		// Test actual GenMan calls
		Geno result1 = genman.mutate(Geno(arr1, "S"));
		Geno result2 = genman.mutate(Geno(arr2, "S"));
		Geno result3 = genman.crossOver(Geno(arr1, "S"), Geno(arr2, "S"));

		if (operators.mutate(arr1, gp, method) == GENOPER_OK)
			methodUsages[method]++;

		if (operators.mutate(arr2, gp, method) == GENOPER_OK)
			methodUsages[method]++;

		int crossOverResult = operators.crossOver(arr1, arr2, f1, f2);


		if (crossOverResult == GENOPER_OK && 0 == operators.checkValidity(arr1, "") && 0 == operators.checkValidity(arr2, ""))
		{
			ensure(0. <= f1 && f1 <= 1.);
			ensure(0. <= f2 && f2 <= 1.);

			if(evaluate(arr1) >= evaluate(gens[i1]->c_str()))
			{
				delete gens[i1];
				gens[i1] = new SString(arr1);
			}
			if(evaluate(arr2) >= evaluate(gens[i2]->c_str()))
			{
				delete gens[i2];
				gens[i2] = new SString(arr2);
			}

			// Check if genotypes convert correctly
			MultiMap map;
			ensure(converter.convert(*gens[i1], &map, true) != "");
			ensure(!map.isEmpty());
			ensure(converter.convert(*gens[i2], &map, true) != "");
			ensure(!map.isEmpty());
		}
		else
		{
			failCount++;
		}

		free(arr1);
		free(arr2);
	}
	ensure(failCount < 0.1 * operationCount);

	cout<< "Fails: "<<failCount<<std::endl<<std::endl;
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
	auto start = std::chrono::steady_clock::now();
	int operationCount;
	if (argc > 1)
		operationCount = std::stod(argv[1]);
	else
		operationCount = 100;

	evolutionTest(operationCount);

	auto end = std::chrono::steady_clock::now();
	cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << endl;
	cout << "FINISHED";
	return 0;
}