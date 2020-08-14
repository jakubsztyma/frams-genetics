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
#include "frams/genetics/fS/fS_conv.h"
#include "frams/genetics/fS/part_distance_estimator.h"

const double S_MIN = 0.05;
const double S_MAX = 5.0;

double calculateWithParams(Node *node, double precision, double relativeDensity)
{

	node->genotypeParams.distanceTolerance = precision;
	node->genotypeParams.relativeDensity = relativeDensity;
	return node->calculateDistanceFromParent();
}

int main(int argc, char *argv[])
{
	GenoConv_fS0s converter = GenoConv_fS0s();
	auto start = std::chrono::steady_clock::now();

	int genoCount = 1;
	if(argc >= 2)
		genoCount = std::stod(argv[1]);
	int precisionsCount = 20;
	int densitiesCount = 20;

	double precisions[precisionsCount];
	for (int pi = 0; pi < precisionsCount; pi++)
		precisions[pi] = 0.001 + pi * 0.008;

	double densities[precisionsCount];
	for (int di = 0; di < densitiesCount; di++)
		densities[di] = 1.0 + 5.0 * di;

	char test_cases[genoCount][300];
	string partShapes = "ECR";
	for(int i=0; i<genoCount; i++)
	{
		sprintf(
				test_cases[i],
				"1.1:%c{x=%f;y=%f;z=%f;rx=%f;ry=%f;rz=%f;tx=%f;ty=%f;tz=%f}%c{x=%f;y=%f;z=%f;rx=%f;ry=%f;rz=%f;tx=%f;ty=%f;tz=%f}",
				partShapes[rndUint(3)], partShapes[rndUint(3)],
				RndGen.Uni(S_MIN, S_MAX), RndGen.Uni(S_MIN, S_MAX), RndGen.Uni(S_MIN, S_MAX),
				RndGen.Uni(-M_PI_2, M_PI_2), RndGen.Uni(-M_PI_2, M_PI_2),RndGen.Uni(-M_PI_2, M_PI_2),
				RndGen.Uni(-M_PI_2, M_PI_2), RndGen.Uni(-M_PI_2, M_PI_2),RndGen.Uni(-M_PI_2, M_PI_2),
				RndGen.Uni(S_MIN, S_MAX), RndGen.Uni(S_MIN, S_MAX), RndGen.Uni(S_MIN, S_MAX),
				RndGen.Uni(-M_PI_2, M_PI_2), RndGen.Uni(-M_PI_2, M_PI_2),RndGen.Uni(-M_PI_2, M_PI_2),
		RndGen.Uni(-M_PI_2, M_PI_2), RndGen.Uni(-M_PI_2, M_PI_2),RndGen.Uni(-M_PI_2, M_PI_2)
				);
	}

	double resultsSum[precisionsCount][densitiesCount];
	double resultsTimeSum[precisionsCount][densitiesCount];	// In nanoseconds
	memset(resultsSum, 0, sizeof(resultsSum));
	memset(resultsTimeSum, 0, sizeof(resultsTimeSum));

	for(int gi=0; gi<genoCount; gi++)
	{
		SString genotype_str = test_cases[gi];
		auto s = std::chrono::steady_clock::now();
		converter.convert(genotype_str, 0, false);
		auto e = std::chrono::steady_clock::now();
		double conversionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(e - s).count();

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
				double elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(e - s).count();
				resultsTimeSum[pi][di] += elapsed / conversionTime;
			}
		}
	}

	// Print results
	for (int pi = 0; pi < precisionsCount; pi++)
	{
		for (int di = 0; di < densitiesCount; di++)
		{
			std::cout<<precisions[pi]<<","<<densities[di]<<","<<resultsSum[pi][di] / genoCount<<","<<resultsTimeSum[pi][di] / genoCount<<std::endl;
		}
	}

	auto end = std::chrono::steady_clock::now();
	std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<<std::endl;
	return 0;
}
