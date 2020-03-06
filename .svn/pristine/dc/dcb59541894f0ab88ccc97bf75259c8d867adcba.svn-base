// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2016  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.


#include <vector>
#include "common/loggers/loggertostdout.h"
#include "frams/_demos/genotypeloader.h"
#include "frams/genetics/preconfigured.h"
#include "common/virtfile/stdiofile.h"
#include "frams/model/similarity/simil_model.h"



using namespace std;

/** Computes a matrix of distances between all genotypes in the specified
	.gen file, using the matching and measure weights as specified in the
	command line. */
int main(int argc, char *argv[])
{
	LoggerToStdout messages_to_stdout(LoggerBase::Enable);
	typedef double *pDouble;
	int iCurrParam = 0; // index of the currently processed parameter
	char *szCurrParam = NULL;
	ModelSimil M; // similarity computing object
	bool bPrintNames = false; // specifies if names of genotypes are to be printed
	int nResult = 0; // a temporary result

	if (argc < 8)
	{
		printf("Too few parameters!\n");
		printf("Command line: [-names] <genotypesFile> <measure> <w_dP> <w_dDEG> <w_dNEU> <w_dGEO> <fixZaxis?>\n\n");

		printf("Parameters:\n");
		printf("  <genotypesFile> name of a file with genotypes\n");
		printf("  <measure> similarity measure\n");
		printf("  <w_dP> weight of the difference in the number of parts\n");
		printf("  <w_dDEG> weight of the difference in degrees of matched parts\n");
		printf("  <w_dNEU> weight of the difference in neurons of matched parts\n");
		printf("  <w_dGEO> weight of the distance of matched parts\n");
		printf("  <fixZaxis?> should the 'z' (vertical) coordinate be fixed during the alignment? (0 or 1)\n\n");

		printf("Switches:\n");
		printf("  -names specifies that the number and names of genotypes are to be printed to output\n");
		printf("  before the distance matrix; by default the number and names are not printed\n\n");

		printf("Outputs a symmetric distance matrix in the format:\n");
		printf("  <row_1> (columns in a row are separated by TABs)\n");
		printf("  ...\n");
		printf("  <row_n>\n");

		return -1;
	}

	// prepare output parameters from .gen file
	vector<Geno *> pvGenos;
	vector<char *> pvNames;

	// check if there is a switch
	iCurrParam = 1;
	szCurrParam = argv[iCurrParam];
	if (strcmp(szCurrParam, "-names") == 0)
	{
		// switch "-names" was given; print names also
		bPrintNames = true;
		// pass to the next parameter
		iCurrParam++;
	}

	// check the parameters
	// get <genotypesFile> name from command line
	char *szFileName = argv[iCurrParam];

	// initially set measure components' weights to invalid values (negative)
	for (int i = 0; i < M.GetNOFactors(); i++)
	{
		M.m_adFactors[i] = -1.0;
	}
	
	iCurrParam++;
	szCurrParam = argv[iCurrParam];
	int measure_type = -1; 
	nResult = sscanf(szCurrParam, "%d", &measure_type);
	if (nResult != 1)
	{
		printf("Measure type should be a number!\n");
		return -1;
	}
	
	if (measure_type != 0 && measure_type != 1)
	{
		printf("Measure type should be 0 (flexible criteria order and optimal matching) or 1 (vertex degree order and greedy matching)!\n");
		return -1;		
	}
	
	M.matching_method = measure_type;

	const char *params[] = { "<w_dP>", "<w_dDEG>", "<w_dNEU>", "<w_dGEO>" };
	for (int i = 0; i < M.GetNOFactors(); i++)
	{
		iCurrParam++;
		szCurrParam = argv[iCurrParam];
		nResult = sscanf(szCurrParam, "%lf", &M.m_adFactors[i]);
		if (nResult != 1)
		{
			// <w_dX> is not a number -- error
			printf("%s", params[i]);
			printf(" should be a number\n");
			return -1;
		}
		else
		{
			// <w_dX> is a number; check if nonnegative
			if (M.m_adFactors[i] < 0.0)
			{
				printf("%s", params[i]);
				printf(" should be a nonnegative number\n");
				return -1;
			}
		}
	}

	iCurrParam++;
	szCurrParam = argv[iCurrParam];
	nResult = sscanf(szCurrParam, "%d", &M.fixedZaxis);
	if (nResult != 1)
	{
		// <isZFixed> is not a number -- error
		printf("<isZFixed> should be a number\n");
		return -1;
	}
	else if (M.fixedZaxis != 0 && M.fixedZaxis != 1)
	{
		printf("<isZFixed>=%d. <isZFixed> should be equal to 0 or 1\n", M.fixedZaxis);
		return -1;
	}

	// read the input file
	// prepare loading of genotypes from a .gen file
	// create some basic genotype converters
	PreconfiguredGenetics genetics;
	StdioFileSystem_autoselect stdiofilesys;

	long count = 0, totalsize = 0;
	GenotypeMiniLoader loader(szFileName);
	GenotypeMini *loaded;
	while (loaded = loader.loadNextGenotype())
	{
		// while a valid genotype was loaded
		count++;
		totalsize += loaded->genotype.len();
		// create a Geno object based on the MiniGenotype
		Geno *pNextGenotype = new Geno(loaded->genotype);
		if ((pNextGenotype != NULL) && (pNextGenotype->isValid()))
		{
			pvGenos.push_back(pNextGenotype);
			char *szNewName = new char[loaded->name.len() + 1];
			strcpy(szNewName, loaded->name.c_str());
			pvNames.push_back(szNewName);
		}
		else
		{
			printf("Genotype %2li is not valid\n", count);
			if (pNextGenotype != NULL) delete pNextGenotype;
		}
	}
	if (loader.getStatus() == GenotypeMiniLoader::OnError)
	{
		printf("Error: %s", loader.getError().c_str());
	}

	double dSimilarity = 0.0;
	double **aaSimil = NULL; // array of similarities

	// create the empty array of similarities
	aaSimil = new pDouble[pvGenos.size()];
	for (unsigned int k = 0; k < pvGenos.size(); k++)
	{
		aaSimil[k] = new double[pvGenos.size()];
		for (unsigned int l = 0; l < pvGenos.size(); l++)
			aaSimil[k][l] = 0.0;
	}

	// compute and remember similarities
	for (unsigned int i = 0; i < pvGenos.size(); i++)
	{
		for (unsigned int j = 0; j < pvGenos.size(); j++)
		{
			dSimilarity = M.EvaluateDistance(pvGenos.operator[](i), pvGenos.operator[](j));
			aaSimil[i][j] = dSimilarity;
		}
	}

	if (bPrintNames)
	{
		// if "-names" switch was given, print the number of genotypes and their names
		printf("%li\n", pvGenos.size());
		for (unsigned int iGen = 0; iGen < pvNames.size(); iGen++)
		{
			printf("%s\n", pvNames.at(iGen));
		}
	}

	// print out the matrix of similarities
	for (unsigned int i = 0; i < pvGenos.size(); i++)
	{
		for (unsigned int j = 0; j < pvGenos.size(); j++)
		{
			printf("%.2lf\t", aaSimil[i][j]);
		}
		printf("\n");
	}

	// delete vectors and arrays
	for (unsigned int i = 0; i < pvGenos.size(); i++)
	{
		delete pvGenos.operator[](i);
		delete[] pvNames.operator[](i);
		delete[] aaSimil[i];
	}

	delete[] aaSimil;

	return 0;
}
