//
// Created by jakub on 17.07.2020.
//

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <chrono>
#include <frams/util/rndutil.h>
#include "frams/model/modelparts.h"
#include "frams/genetics/fS/fS_general.h"
#include "frams/genetics/fS/part_distance_estimator.h"

double calculateWithParams(Node *node, double precision, double relativeDensity)
{

	node->genotypeParams.distanceTolerance = precision;
	node->genotypeParams.relativeDensity = relativeDensity;
	return node->calculateDistanceFromParent();
}

int main(int argc, char *argv[])
{
	auto start = std::chrono::steady_clock::now();

	int genoCount = 10;
	if(argc >= 2)
		genoCount = std::stod(argv[1]);
	int precisionsCount = 10;
	int densitiesCount = 10;

	double precisions[precisionsCount];
	for (int pi = 0; pi < precisionsCount; pi++)
		precisions[pi] = 1.0 * pow(0.25, pi);

	double densities[precisionsCount];
	for (int di = 0; di < densitiesCount; di++)
		densities[di] = 1.0 * pow(1.6, di);

	char test_cases[genoCount][300];
	for(int i=0; i<genoCount; i++)
	{
		sprintf(
				test_cases[i],
				"1.1:E{x=%f;y=%f;z=%f;rx=%f;ry=%f;rz=%f;tx=%f;ty=%f;tz=%f}E{x=%f;y=%f;z=%f;rx=%f;ry=%f;rz=%f;tx=%f;ty=%f;tz=%f}",
				RndGen.Uni(0.01, 10.0), RndGen.Uni(0.01, 10.0), RndGen.Uni(0.01, 10.0),
				RndGen.Uni(-1.57, 1.57), RndGen.Uni(-1.57, 1.57),RndGen.Uni(-1.57, 1.57),
				RndGen.Uni(-1.57, 1.57), RndGen.Uni(-1.57, 1.57),RndGen.Uni(-1.57, 1.57),
				RndGen.Uni(0.01, 10.0), RndGen.Uni(0.01, 10.0), RndGen.Uni(0.01, 10.0),
				RndGen.Uni(-1.57, 1.57), RndGen.Uni(-1.57, 1.57),RndGen.Uni(-1.57, 1.57),
		RndGen.Uni(-1.57, 1.57), RndGen.Uni(-1.57, 1.57),RndGen.Uni(-1.57, 1.57)
				);
	}

	double resultsSum[precisionsCount][densitiesCount];
	double resultsTimeSum[precisionsCount][densitiesCount];	// In nanoseconds
	memset(resultsSum, 0, sizeof(resultsSum));
	memset(resultsTimeSum, 0, sizeof(resultsTimeSum));

	for(int gi=0; gi<genoCount; gi++)
	{
		fS_Genotype geno(test_cases[gi]);
		geno.getState(false);
		Node *parent = geno.getAllNodes()[0];
		Node *child = geno.getAllNodes()[1];
		Pt3D scale, parentScale;
		child->calculateScale(scale);
		parent->calculateScale(parentScale);
		double correctResult = calculateWithParams(child, 1e-6, 1e2);

		for (int pi = 0; pi < precisionsCount; pi++)
		{
			for (int di = 0; di < densitiesCount; di++)
			{
				auto s = std::chrono::steady_clock::now();
				double result = calculateWithParams(child, precisions[pi], densities[di]);
				auto e = std::chrono::steady_clock::now();

				double difference = fabs(result - correctResult);
				double normalizedDifference = difference / correctResult;

				resultsSum[pi][di] += normalizedDifference;
				resultsTimeSum[pi][di] += std::chrono::duration_cast<std::chrono::nanoseconds>(e - s).count();
			}
		}
	}

	// Print results
	for (int pi = 0; pi < precisionsCount; pi++)
	{
		for (int di = 0; di < densitiesCount; di++)
		{
			std::cout<<precisions[pi]<<" "<<densities[di]<<" "<<resultsSum[pi][di] / genoCount<<" "<<resultsTimeSum[pi][di] / genoCount<<std::endl;
		}
	}

	auto end = std::chrono::steady_clock::now();
	std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<<std::endl;
	return 0;
}
