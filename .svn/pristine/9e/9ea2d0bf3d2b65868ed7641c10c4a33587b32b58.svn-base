// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

// implementation of the ModelSimil class.

#include "SVD/matrix_tools.h"
#include "hungarian/hungarian.h"
#include "simil_model.h"
#include "simil_match.h"
#include "frams/model/modelparts.h"
#include "frams/util/list.h"
#include "common/nonstd.h"
#include <frams/vm/classes/genoobj.h>
#ifdef EMSCRIPTEN
#include <cstdlib>
#else
#include <stdlib.h>
#endif
#include <math.h>
#include <string>
#include <limits>
#include <assert.h>
#include <vector>
#include <algorithm> 
#include <cstdlib> //required for std::qsort in macos xcode

#define DB(x)  //define as x if you want to print debug information

const int ModelSimil::iNOFactors = 4;
//depth of the fuzzy neighbourhood
int fuzDepth = 0;

#define FIELDSTRUCT ModelSimil

static ParamEntry MSparam_tab[] = {
		{ "Creature: Similarity", 1, 8, "ModelSimilarity", "Evaluates morphological dissimilarity. More information:\nhttp://www.framsticks.com/bib/Komosinski-et-al-2001\nhttp://www.framsticks.com/bib/Komosinski-and-Kubiak-2011\nhttp://www.framsticks.com/bib/Komosinski-2016\nhttps://doi.org/10.1007/978-3-030-16692-2_8", },
		{ "simil_method", 0, 0, "Similarity algorithm", "d 0 1 0 ~New (flexible criteria order, optimal matching)~Old (vertex degree order, greedy matching)", FIELD(matching_method), "",},
		{ "simil_parts", 0, 0, "Weight of parts count", "f 0 100 0", FIELD(m_adFactors[0]), "Differing number of parts is also handled by the 'part degree' similarity component.", },
		{ "simil_partdeg", 0, 0, "Weight of parts' degree", "f 0 100 1", FIELD(m_adFactors[1]), "", },
		{ "simil_neuro", 0, 0, "Weight of neurons count", "f 0 100 0.1", FIELD(m_adFactors[2]), "", },
		{ "simil_partgeom", 0, 0, "Weight of parts' geometric distances", "f 0 100 0", FIELD(m_adFactors[3]), "", },
		{ "simil_fixedZaxis", 0, 0, "Fix 'z' (vertical) axis?", "d 0 1 0", FIELD(fixedZaxis), "", },
		{ "simil_weightedMDS", 0, 0, "Should weighted MDS be used?", "d 0 1 0", FIELD(wMDS), "If activated, weighted MDS with vertex (i.e., Part) degrees as weights is used for 3D alignment of body structure.", },
		{ "evaluateDistance", 0, PARAM_DONTSAVE | PARAM_USERHIDDEN, "evaluate model dissimilarity", "p f(oGeno,oGeno)", PROCEDURE(p_evaldistance), "Calculates dissimilarity between two models created from Geno objects.", },
		{ 0, },
};

#undef FIELDSTRUCT

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** Constructor. Sets default weights. Initializes other fields with zeros.
 */
ModelSimil::ModelSimil() : localpar(MSparam_tab, this), m_iDV(0), m_iDD(0), m_iDN(0), m_dDG(0.0)
{
	localpar.setDefault();

	m_Gen[0] = NULL;
	m_Gen[1] = NULL;
	m_Mod[0] = NULL;
	m_Mod[1] = NULL;
	m_aDegrees[0] = NULL;
	m_aDegrees[1] = NULL;
	m_aPositions[0] = NULL;
	m_aPositions[1] = NULL;
	m_fuzzyNeighb[0] = NULL;
	m_fuzzyNeighb[1] = NULL;
	m_Neighbours[0] = NULL;
	m_Neighbours[1] = NULL;
	m_pMatching = NULL;

	//Determines whether "fuzzy vertex degree" should be used.
	//Currently "fuzzy vertex degree" is inactive.
	isFuzzy = false;
	fuzzyDepth = 10;

	//Determines whether weighted MDS should be used.
	wMDS = 0;
	//Determines whether best matching should be saved using hungarian similarity measure.
	saveMatching = false;
}

double ModelSimil::EvaluateDistanceGreedy(const Geno *G0, const Geno *G1)
{
	double dResult = 0.0;

	m_Gen[0] = G0;
	m_Gen[1] = G1;

	// create models of objects to compare
	m_Mod[0] = newModel(m_Gen[0]);
	m_Mod[1] = newModel(m_Gen[1]);

	if (m_Mod[0] == NULL || m_Mod[1] == NULL)
		return 0.0;

	// difference in the number of vertices (Parts) - positive
	// find object that has less parts (m_iSmaller)
	m_iDV = (m_Mod[0]->getPartCount() - m_Mod[1]->getPartCount());
	if (m_iDV > 0)
		m_iSmaller = 1;
	else
	{
		m_iSmaller = 0;
		m_iDV = -m_iDV;
	}

	// check if index of the smaller organism is a valid index
	assert((m_iSmaller == 0) || (m_iSmaller == 1));
	// validate difference in the parts number
	assert(m_iDV >= 0);

	// create Parts matching object
	m_pMatching = new SimilMatching(m_Mod[0]->getPartCount(), m_Mod[1]->getPartCount());
	// validate matching object
	assert(m_pMatching != NULL);
	assert(m_pMatching->IsEmpty() == true);


	// assign matching function
	int (ModelSimil::* pfMatchingFunction) () = NULL;

	pfMatchingFunction = &ModelSimil::MatchPartsGeometry;

	// match Parts (vertices of creatures)
	if ((this->*pfMatchingFunction)() == 0)
	{
		logPrintf("ModelSimil", "EvaluateDistanceGreedy", LOG_ERROR, "The matching function returned 0");
		return 0.0;
	}

	// after matching function call we must have full matching
	assert(m_pMatching->IsFull() == true);

	DB(SaveIntermediateFiles();)

		// count differences in matched parts
		if (CountPartsDistance() == 0)
		{
			logPrintf("ModelSimil", "EvaluateDistanceGreedy", LOG_ERROR, "CountPartDistance()==0");
			return 0.0;
		}

	// delete degree arrays created in CreatePartInfoTables 
	SAFEDELETEARRAY(m_aDegrees[0]);
	SAFEDELETEARRAY(m_aDegrees[1]);

	// and position arrays
	SAFEDELETEARRAY(m_aPositions[0]);
	SAFEDELETEARRAY(m_aPositions[1]);

	// in fuzzy mode delete arrays of neighbourhood and fuzzy neighbourhood
	if (isFuzzy)
	{
		for (int i = 0; i != 2; ++i)
		{
			for (int j = 0; j != m_Mod[i]->getPartCount(); ++j)
			{
				delete[] m_Neighbours[i][j];
				delete[] m_fuzzyNeighb[i][j];
			}
			delete[] m_Neighbours[i];
			delete[] m_fuzzyNeighb[i];
		}

	}

	// delete created models
	SAFEDELETE(m_Mod[0]);
	SAFEDELETE(m_Mod[1]);

	// delete created matching
	SAFEDELETE(m_pMatching);

	dResult = m_adFactors[0] * double(m_iDV) +
		m_adFactors[1] * double(m_iDD) +
		m_adFactors[2] * double(m_iDN) +
		m_adFactors[3] * double(m_dDG);

	return dResult;
}

ModelSimil::~ModelSimil()
{
	// matching should have been deleted earlier
	assert(m_pMatching == NULL);
}

/**	Creates files matching.txt, org0.txt and org1.txt containing information
 * about the matching and both organisms for visualization purpose.
 */
void ModelSimil::SaveIntermediateFiles()
{
	assert(m_pMatching->IsFull() == true);
	printf("Saving the matching to file 'matching.txt'\n");
	FILE *pMatchingFile = NULL;
	// try to open the file
	pMatchingFile = fopen("matching.txt", "wt");
	assert(pMatchingFile != NULL);

	int iOrgPart; // original index of a Part
	int nBigger; // index of the larger organism

	// check which object is bigger
	if (m_pMatching->GetObjectSize(0) >= m_pMatching->GetObjectSize(1))
	{
		nBigger = 0;
	}
	else
	{
		nBigger = 1;
	}

	// print first line - original indices of Parts of the bigger organism
	fprintf(pMatchingFile, "[ ");
	for (iOrgPart = 0; iOrgPart < m_pMatching->GetObjectSize(nBigger); iOrgPart++)
	{
		fprintf(pMatchingFile, "%2i ", iOrgPart);
	}
	fprintf(pMatchingFile, "] : ORG[%i]\n", nBigger);

	// print second line - matched original indices of the second organism
	fprintf(pMatchingFile, "[ ");
	for (iOrgPart = 0; iOrgPart < m_pMatching->GetObjectSize(nBigger); iOrgPart++)
	{
		int iSorted; // index of the iOrgPart after sorting (as used by matching)
		int iSortedMatched; // index of the matched Part (after sorting)
		int iOrginalMatched; // index of the matched Part (the original one)

		// find the index of iOrgPart after sorting (in m_aDegrees)
		for (iSorted = 0; iSorted < m_Mod[nBigger]->getPartCount(); iSorted++)
		{
			// for each iSorted, an index in the sorted m_aDegrees array
			if (m_aDegrees[nBigger][iSorted][0] == iOrgPart)
			{
				// if the iSorted Part is the one with iOrgPart as the orginal index
				// remember the index
				break;
			}
		}
		// if the index iSorted was found, then this condition is met
		assert(iSorted < m_Mod[nBigger]->getPartCount());

		// find the matched sorted index
		if (m_pMatching->IsMatched(nBigger, iSorted))
		{
			// if Part iOrgPart is matched
			// then get the matched Part (sorted) index
			iSortedMatched = m_pMatching->GetMatchedIndex(nBigger, iSorted);
			assert(iSortedMatched >= 0);
			// and find its original index
			iOrginalMatched = m_aDegrees[1 - nBigger][iSortedMatched][0];
			fprintf(pMatchingFile, "%2i ", iOrginalMatched);
		}
		else
		{
			// if the Part iOrgPart is not matched
			// just print "X"
			fprintf(pMatchingFile, " X ");
		}
	} // for ( iOrgPart )

	// now all matched Part indices are printed out, end the line
	fprintf(pMatchingFile, "] : ORG[%i]\n", 1 - nBigger);

	// close the file
	fclose(pMatchingFile);
	// END TEMP

	// TEMP
	// print out the 2D positions of Parts of both of the organisms
	// to files "org0.txt" and "org1.txt" using the original indices of Parts
	int iModel; // index of a model (an organism)
	FILE *pModelFile;
	for (iModel = 0; iModel < 2; iModel++)
	{
		// for each iModel, a model of a compared organism
		// write its (only 2D) positions to a file "org<iModel>.txt"
		// construct the model filename "org<iModel>.txt"
		std::string sModelFilename("org");
		//		char *szModelIndex = "0"; // the index of the model (iModel) in the character form
		char szModelIndex[2];
		sprintf(szModelIndex, "%i", iModel);
		sModelFilename += szModelIndex;
		sModelFilename += ".txt";
		// open the file for writing
		pModelFile = fopen(sModelFilename.c_str(), "wt"); //FOPEN_WRITE
		assert(pModelFile != NULL);
		// write the 2D positions of iModel to the file
		int iOriginalPart; // an original index of a Part
		for (iOriginalPart = 0; iOriginalPart < m_Mod[iModel]->getPartCount(); iOriginalPart++)
		{
			// for each iOriginalPart, a Part of the organism iModel
			// get the 2D coordinates of the Part
			double dPartX = m_aPositions[iModel][iOriginalPart].x;
			double dPartY = m_aPositions[iModel][iOriginalPart].y;
			// print the line: <iOriginalPart> <dPartX> <dPartY>
			fprintf(pModelFile, "%i %.4lf %.4lf\n", iOriginalPart, dPartX, dPartY);
		}
		// close the file
		fclose(pModelFile);
	}
}

/** Comparison function required for qsort() call. Used while sorting groups of
	Parts with respect to degree. Compares two TDN structures
	with respect to their [1] field (degree). Highest degree goes first.
	@param pElem1 Pointer to the TDN structure of some Part.
	@param pElem2 Pointer to the TDN structure of some Part.
	@return (-1) - pElem1 should go first, 0 - equal, (1) - pElem2 should go first.
	*/
int ModelSimil::CompareDegrees(const void *pElem1, const void *pElem2)
{
	int *tdn1 = (int *)pElem1;
	int *tdn2 = (int *)pElem2;

	if (tdn1[1] > tdn2[1])
	{
		// when degree - tdn1[1] - is BIGGER
		return -1;
	}
	else
		if (tdn1[1] < tdn2[1])
		{
			// when degree - tdn2[1] - is BIGGER
			return 1;
		}
		else
		{
			return 0;
		}
}

/** Comparison function required for qsort() call. Used while sorting groups of
	Parts with respect to fuzzy vertex degree. Compares two TDN structures
	with respect to their [4] field ( fuzzy vertex degree). Highest degree goes first.
	@param pElem1 Pointer to the TDN structure of some Part.
	@param pElem2 Pointer to the TDN structure of some Part.
	@return (-1) - pElem1 should go first, 0 - equal, (1) - pElem2 should go first.
	*/
int ModelSimil::CompareFuzzyDegrees(const void *pElem1, const void *pElem2)
{
	int *tdn1 = (int *)pElem1;
	int *tdn2 = (int *)pElem2;

	if (tdn1[4] > tdn2[4])
	{
		// when degree - tdn1[4] - is BIGGER
		return -1;
	}
	else
		if (tdn1[4] < tdn2[4])
		{
			// when degree - tdn2[4] - is BIGGER
			return 1;
		}
		else
		{
			return 0;
		}
}

/** Comparison function required for qsort() call. Used while sorting groups of Parts with
		the same degree. Firstly, compare NIt. Secondly, compare Neu. If both are equal -
		compare Parts' original index (they are never equal). So this sorting assures
		that the order obtained is deterministic.
		@param pElem1 Pointer to the TDN structure of some Part.
		@param pElem2 Pointer to the TDN structure of some Part.
		@return (-1) - pElem1 should go first, 0 - equal, (1) - pElem2 should go first.
		*/
int ModelSimil::CompareConnsNo(const void *pElem1, const void *pElem2)
{
	// pointers to TDN arrays 
	int *tdn1, *tdn2;
	// definitions of elements being compared
	tdn1 = (int *)pElem1;
	tdn2 = (int *)pElem2;

	// comparison according to Neural Connections (to jest TDN[2])
	if (tdn1[NEURO_CONNS] > tdn2[NEURO_CONNS])
	{
		// when number of NConn Elem1 is BIGGER
		return -1;
	}
	else
		if (tdn1[NEURO_CONNS] < tdn2[NEURO_CONNS])
		{
			// when number of NConn Elem1 is SMALLER
			return 1;
		}
		else
		{
			// when numbers of NConn are EQUAL
			// compare Neu numbers (TDN[3])
			if (tdn1[NEURONS] > tdn2[NEURONS])
			{
				// when number of Neu is BIGGER for Elem1 
				return -1;
			}
			else
				if (tdn1[NEURONS] < tdn2[NEURONS])
				{
					// when number of Neu is SMALLER for Elem1 
					return 1;
				}
				else
				{
					// when numbers of Nconn and Neu are equal we check original indices 
					// of Parts being compared

					// comparison according to OrgIndex
					if (tdn1[ORIG_IND] > tdn2[ORIG_IND])
					{
						// when the number of NIt Deg1 id BIGGER
						return -1;
					}
					else
						if (tdn1[ORIG_IND] < tdn2[ORIG_IND])
						{
							// when the number of NIt Deg1 id SMALLER
							return 1;
						}
						else
						{
							// impossible, indices are alway different
							return 0;
						}
				}
		}
}

/** Returns number of factors involved in final distance computation.
		These factors include differences in numbers of parts, degrees,
		number of neurons.
		*/
int ModelSimil::GetNOFactors()
{
	return ModelSimil::iNOFactors;
}

/** Prints the array of degrees for the given organism. Debug method.
 */
void ModelSimil::_PrintDegrees(int i)
{
	int j;
	printf("Organizm %i :", i);
	printf("\n      ");
	for (j = 0; j < m_Mod[i]->getPartCount(); j++)
		printf("%3i ", j);
	printf("\nInd:  ");
	for (j = 0; j < m_Mod[i]->getPartCount(); j++)
		printf("%3i ", (int)m_aDegrees[i][j][0]);
	printf("\nDeg:  ");
	for (j = 0; j < m_Mod[i]->getPartCount(); j++)
		printf("%3i ", (int)m_aDegrees[i][j][1]);
	printf("\nNIt:  ");
	for (j = 0; j < m_Mod[i]->getPartCount(); j++)
		printf("%3i ", (int)m_aDegrees[i][j][2]);
	printf("\nNeu:  ");
	for (j = 0; j < m_Mod[i]->getPartCount(); j++)
		printf("%3i ", (int)m_aDegrees[i][j][3]);
	printf("\n");
}

/** Prints one array of ints. Debug method.
	@param array Base pointer of the array.
	@param base First index of the array's element.
	@param size Number of elements to print.
	*/
void ModelSimil::_PrintArray(int *array, int base, int size)
{
	int i;
	for (i = base; i < base + size; i++)
	{
		printf("%i ", array[i]);
	}
	printf("\n");
}

void ModelSimil::_PrintArrayDouble(double *array, int base, int size)
{
	int i;
	for (i = base; i < base + size; i++)
	{
		printf("%f ", array[i]);
	}
	printf("\n");
}

/** Prints one array of parts neighbourhood.
	@param index of organism
	*/
void ModelSimil::_PrintNeighbourhood(int o)
{
	assert(m_Neighbours[o] != 0);
	printf("Neighbourhhod of organism %i\n", o);
	int size = m_Mod[o]->getPartCount();
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			printf("%i ", m_Neighbours[o][i][j]);
		}
		printf("\n");
	}
}

/** Prints one array of parts fuzzy neighbourhood.
	@param index of organism
	*/
void ModelSimil::_PrintFuzzyNeighbourhood(int o)
{
	assert(m_fuzzyNeighb[o] != NULL);
	printf("Fuzzy neighbourhhod of organism %i\n", o);
	int size = m_Mod[o]->getPartCount();
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < fuzzyDepth; j++)
		{
			printf("%f ", m_fuzzyNeighb[o][i][j]);
		}
		printf("\n");
	}
}

Model* ModelSimil::newModel(const Geno *g)
{
	if (g == NULL)
	{
		logPrintf("ModelSimil", "newModel", LOG_ERROR, "NULL genotype pointer");
		return NULL;
	}
	Model *m = new Model(*g);
	if (!m->isValid())
	{
		logPrintf("ModelSimil", "newModel", LOG_ERROR, "Invalid model for the genotype of '%s'", g->getName().c_str());
		delete m;
		return NULL;
	}
	return m;
}


/** Creates arrays holding information about organisms' Parts (m_aDegrees) andm_Neigh
	fills them with initial data (original indices and zeros).
	Assumptions:
	- Models (m_Mod) are created and available.
	*/
int ModelSimil::ModelSimil::CreatePartInfoTables()
{
	// check assumptions about models
	assert((m_Mod[0] != NULL) && (m_Mod[1] != NULL));
	assert(m_Mod[0]->isValid() && m_Mod[1]->isValid());

	int i, j, partCount;
	// utwórz tablice na informacje o stopniach wierzchołków i liczbie neuroitems
	for (i = 0; i < 2; i++)
	{
		partCount = m_Mod[i]->getPartCount();
		// utworz i wypelnij tablice dla Parts wartosciami poczatkowymi
		m_aDegrees[i] = new TDN[partCount];

		if (isFuzzy)
		{
			m_Neighbours[i] = new int*[partCount];
			m_fuzzyNeighb[i] = new float*[partCount];
		}

		if (m_aDegrees[i] != NULL && ((!isFuzzy) || (m_Neighbours[i] != NULL && m_fuzzyNeighb[i] != NULL)))
		{
			// wypelnij tablice zgodnie z sensem TDN[0] - orginalny index
			// TDN[1], TDN[2], TDN[3] - zerami
			DB(printf("m_aDegrees[%i]: %p\n", i, m_aDegrees[i]);)
				for (j = 0; j < partCount; j++)
				{
					m_aDegrees[i][j][0] = j;
					m_aDegrees[i][j][1] = 0;
					m_aDegrees[i][j][2] = 0;
					m_aDegrees[i][j][3] = 0;
					m_aDegrees[i][j][4] = 0;

					// sprawdz, czy nie piszemy po jakims szalonym miejscu pamieci
					assert(m_aDegrees[i][j] != NULL);

					if (isFuzzy)
					{
						m_Neighbours[i][j] = new int[partCount];
						for (int k = 0; k < partCount; k++)
						{
							m_Neighbours[i][j][k] = 0;
						}

						m_fuzzyNeighb[i][j] = new float[fuzzyDepth];
						for (int k = 0; k < fuzzyDepth; k++)
						{
							m_fuzzyNeighb[i][j][k] = 0;
						}

						assert(m_Neighbours[i][j] != NULL);
						assert(m_fuzzyNeighb[i][j] != NULL);
					}

				}
		}
		else
		{
			logPrintf("ModelSimil", "CreatePartInfoTables", LOG_ERROR, "Not enough memory?");
			return 0;
		}
		// utworz tablice dla pozycji 3D Parts (wielkosc tablicy: liczba Parts organizmu)
		m_aPositions[i] = new Pt3D[m_Mod[i]->getPartCount()];
		assert(m_aPositions[i] != NULL);
		// wypelnij tablice OnJoints i Anywhere wartościami początkowymi
		// OnJoint
		m_aOnJoint[i][0] = 0;
		m_aOnJoint[i][1] = 0;
		m_aOnJoint[i][2] = 0;
		m_aOnJoint[i][3] = 0;
		// Anywhere
		m_aAnywhere[i][0] = 0;
		m_aAnywhere[i][1] = 0;
		m_aAnywhere[i][2] = 0;
		m_aAnywhere[i][3] = 0;
	}
	return 1;
}

/** Computes degrees of Parts of both organisms. Fills in the m_aDegrees arrays
	with proper information about degrees.
	Assumptions:
	- Models (m_Mod) are created and available.
	- Arrays m_aDegrees are created.
	*/
int ModelSimil::CountPartDegrees()
{
	// sprawdz zalozenie - o modelach
	assert((m_Mod[0] != NULL) && (m_Mod[1] != NULL));
	assert(m_Mod[0]->isValid() && m_Mod[1]->isValid());

	// sprawdz zalozenie - o tablicach
	assert(m_aDegrees[0] != NULL);
	assert(m_aDegrees[1] != NULL);

	Part *P1, *P2;
	int i, j, i1, i2;

	// dla obu stworzen oblicz stopnie wierzcholkow
	for (i = 0; i < 2; i++)
	{
		// dla wszystkich jointow
		for (j = 0; j < m_Mod[i]->getJointCount(); j++)
		{
			// pobierz kolejny Joint
			Joint *J = m_Mod[i]->getJoint(j);
			// wez jego konce
			P1 = J->part1;
			P2 = J->part2;
			// znajdz ich indeksy w Modelu (indeksy orginalne)
			i1 = m_Mod[i]->findPart(P1);
			i2 = m_Mod[i]->findPart(P2);
			// zwieksz stopien odpowiednich Parts
			m_aDegrees[i][i1][DEGREE]++;
			m_aDegrees[i][i2][DEGREE]++;
			m_aDegrees[i][i1][FUZZ_DEG]++;
			m_aDegrees[i][i2][FUZZ_DEG]++;
			if (isFuzzy)
			{
				m_Neighbours[i][i1][i2] = 1;
				m_Neighbours[i][i2][i1] = 1;
			}
		}
		// dla elementow nie osadzonych na Parts (OnJoint, Anywhere) -
		// stopnie wierzchołka są już ustalone na zero
	}

	if (isFuzzy)
	{
		CountFuzzyNeighb();
	}

	return 1;
}

void ModelSimil::GetNeighbIndexes(int mod, int partInd, std::vector<int> &indexes)
{
	indexes.clear();
	int i, size = m_Mod[mod]->getPartCount();

	for (i = 0; i < size; i++)
	{
		if (m_Neighbours[mod][partInd][i] > 0)
		{
			indexes.push_back(i);
		}
	}
}

int cmpFuzzyRows(const void *pa, const void *pb)
{
	float **a = (float**)pa;
	float **b = (float**)pb;


	for (int i = 1; i < fuzDepth; i++)
	{
		if (a[0][i] > b[0][i])
		{

			return -1;
		}
		if (a[0][i] < b[0][i])
		{

			return 1;
		}
	}

	return 0;
}

void ModelSimil::FuzzyOrder()
{
	int i, depth, partInd, prevPartInd, partCount;
	for (int mod = 0; mod < 2; mod++)
	{
		partCount = m_Mod[mod]->getPartCount();
		partInd = m_fuzzyNeighb[mod][partCount - 1][0];
		m_aDegrees[mod][partInd][FUZZ_DEG] = 0;

		for (i = (partCount - 2); i >= 0; i--)
		{
			prevPartInd = partInd;
			partInd = m_fuzzyNeighb[mod][i][0];
			m_aDegrees[mod][partInd][FUZZ_DEG] = m_aDegrees[mod][prevPartInd][FUZZ_DEG];
			for (depth = 1; depth < fuzzyDepth; depth++)
			{
				if (m_fuzzyNeighb[mod][i][depth] != m_fuzzyNeighb[mod][i + 1][depth])
				{
					m_aDegrees[mod][partInd][FUZZ_DEG]++;
					break;
				}
			}
		}
	}
}

//sort according to fuzzy degree
void ModelSimil::SortFuzzyNeighb()
{
	fuzDepth = fuzzyDepth;
	for (int mod = 0; mod < 2; mod++)
	{
		std::qsort(m_fuzzyNeighb[mod], (size_t)m_Mod[mod]->getPartCount(), sizeof(m_fuzzyNeighb[mod][0]), cmpFuzzyRows);
	}
}

//computes fuzzy vertex degree
void ModelSimil::CountFuzzyNeighb()
{
	assert(m_aDegrees[0] != NULL);
	assert(m_aDegrees[1] != NULL);

	assert(m_Neighbours[0] != NULL);
	assert(m_Neighbours[1] != NULL);

	assert(m_fuzzyNeighb[0] != NULL);
	assert(m_fuzzyNeighb[1] != NULL);

	std::vector<int> nIndexes;
	float newDeg = 0;

	for (int mod = 0; mod < 2; mod++)
	{
		int partCount = m_Mod[mod]->getPartCount();

		for (int depth = 0; depth < fuzzyDepth; depth++)
		{
			//use first column for storing indices
			for (int partInd = 0; partInd < partCount; partInd++)
			{
				if (depth == 0)
				{
					m_fuzzyNeighb[mod][partInd][depth] = partInd;
				}
				else if (depth == 1)
				{
					m_fuzzyNeighb[mod][partInd][depth] = m_aDegrees[mod][partInd][DEGREE];
				}
				else
				{
					GetNeighbIndexes(mod, partInd, nIndexes);
					for (unsigned int k = 0; k < nIndexes.size(); k++)
					{
						newDeg += m_fuzzyNeighb[mod][nIndexes.at(k)][depth - 1];
					}
					newDeg /= nIndexes.size();
					m_fuzzyNeighb[mod][partInd][depth] = newDeg;
					for (int mod = 0; mod < 2; mod++)
					{
						int partCount = m_Mod[mod]->getPartCount();
						for (int i = partCount - 1; i >= 0; i--)
						{

						}
					}
					newDeg = 0;
				}
			}
		}
	}

	SortFuzzyNeighb();
	FuzzyOrder();
}

/** Gets information about Parts' positions in 3D from models into the arrays
		m_aPositions.
		Assumptions:
		- Models (m_Mod) are created and available.
		- Arrays m_aPositions are created.
		@return 1 if success, otherwise 0.
		*/
int ModelSimil::GetPartPositions()
{
	// sprawdz zalozenie - o modelach
	assert((m_Mod[0] != NULL) && (m_Mod[1] != NULL));
	assert(m_Mod[0]->isValid() && m_Mod[1]->isValid());

	// sprawdz zalozenie - o tablicach m_aPositions
	assert(m_aPositions[0] != NULL);
	assert(m_aPositions[1] != NULL);

	// dla każdego stworzenia skopiuj informację o polozeniu jego Parts
	// do tablic m_aPositions
	Part *pPart;
	int iMod; // licznik modeli (organizmow)
	int iPart; // licznik Parts
	for (iMod = 0; iMod < 2; iMod++)
	{
		// dla każdego z modeli iMod
		for (iPart = 0; iPart < m_Mod[iMod]->getPartCount(); iPart++)
		{
			// dla każdego iPart organizmu iMod
			// pobierz tego Part
			pPart = m_Mod[iMod]->getPart(iPart);
			// zapamietaj jego pozycje 3D w tablicy m_aPositions
			m_aPositions[iMod][iPart].x = pPart->p.x;
			m_aPositions[iMod][iPart].y = pPart->p.y;
			m_aPositions[iMod][iPart].z = pPart->p.z;
		}
	}

	return 1;
}

/** Computes numbers of neurons and neurons' inputs for each Part of each
	organisms and fills in the m_aDegrees array.
	Assumptions:
	- Models (m_Mod) are created and available.
	- Arrays m_aDegrees are created.
	*/
int ModelSimil::CountPartNeurons()
{
	// sprawdz zalozenie - o modelach
	assert((m_Mod[0] != NULL) && (m_Mod[1] != NULL));
	assert(m_Mod[0]->isValid() && m_Mod[1]->isValid());

	// sprawdz zalozenie - o tablicach
	assert(m_aDegrees[0] != NULL);
	assert(m_aDegrees[1] != NULL);

	Part *P1;
	Joint *J1;
	int i, j, i2, neuro_connections;

	// dla obu stworzen oblicz liczbe Neurons + connections dla Parts
	// a takze dla OnJoints i Anywhere
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < m_Mod[i]->getNeuroCount(); j++)
		{
			// pobierz kolejny Neuron
			Neuro *N = m_Mod[i]->getNeuro(j);
			// policz liczbe jego wejść i jego samego tez
			// czy warto w ogole liczyc polaczenia...? co to da/spowoduje?
			neuro_connections = N->getInputCount() + 1;
			// wez Part, na ktorym jest Neuron
			P1 = N->getPart();
			if (P1)
			{
				// dla neuronow osadzonych na Partach
				i2 = m_Mod[i]->findPart(P1); // znajdz indeks Part w Modelu
				m_aDegrees[i][i2][2] += neuro_connections; // zwieksz liczbe Connections+Neurons dla tego Part (TDN[2])
				m_aDegrees[i][i2][3]++; // zwieksz liczbe Neurons dla tego Part (TDN[3])
			}
			else
			{
				// dla neuronow nie osadzonych na partach
				J1 = N->getJoint();
				if (J1)
				{
					// dla tych na Jointach
					m_aOnJoint[i][2] += neuro_connections; // zwieksz liczbe Connections+Neurons
					m_aOnJoint[i][3]++; // zwieksz liczbe Neurons
				}
				else
				{
					// dla tych "gdziekolwiek"
					m_aAnywhere[i][2] += neuro_connections; // zwieksz liczbe Connections+Neurons
					m_aAnywhere[i][3]++; // zwieksz liczbe Neurons
				}
			}
		}
	}
	return 1;
}

/** Sorts arrays m_aDegrees (for each organism) by Part's degree, and then by
	number of neural connections and neurons in groups of Parts with the same
	degree.
	Assumptions:
	- Models (m_Mod) are created and available.
	- Arrays m_aDegrees are created.
	@saeDegrees, CompareItemNo
	*/
int ModelSimil::SortPartInfoTables()
{
	// sprawdz zalozenie - o modelach
	assert((m_Mod[0] != NULL) && (m_Mod[1] != NULL));
	assert(m_Mod[0]->isValid() && m_Mod[1]->isValid());

	// sprawdz zalozenie - o tablicach
	assert(m_aDegrees[0] != NULL);
	assert(m_aDegrees[1] != NULL);

	int i;
	int(*pfDegreeFunction) (const void*, const void*) = NULL;
	pfDegreeFunction = isFuzzy ? &CompareFuzzyDegrees : &CompareDegrees;
	// sortowanie obu tablic wg stopni punktów - TDN[1]
	for (i = 0; i < 2; i++)
	{
		DB(_PrintDegrees(i));
		std::qsort(m_aDegrees[i], (size_t)(m_Mod[i]->getPartCount()),
			sizeof(TDN), pfDegreeFunction);
		DB(_PrintDegrees(i));
	}

	// sprawdzenie wartosci parametru
	DB(i = sizeof(TDN);)
		int degreeType = isFuzzy ? FUZZ_DEG : DEGREE;

	// sortowanie obu tablic m_aDegrees wedlug liczby neuronów i
	// czesci neuronu - ale w obrebie grup o tym samym stopniu
	for (i = 0; i < 2; i++)
	{
		int iPocz = 0;
		int iDeg, iNewDeg, iPartCount, j;
		// stopien pierwszego punktu w tablicy Degrees  odniesienie
		iDeg = m_aDegrees[i][0][degreeType];
		iPartCount = m_Mod[i]->getPartCount();
		// po kolei dla kazdego punktu w organizmie
		for (j = 0; j <= iPartCount; j++)
		{
			// sprawdz stopien punktu (lub nadaj 0 - gdy juz koniec tablicy)
			//			iNewDeg = (j != iPartCount) ? m_aDegrees[i][j][1] : 0;
			// usunieto stara wersje porownania!!! wprowadzono znak porownania <

			iNewDeg = (j < iPartCount) ? m_aDegrees[i][j][degreeType] : 0;
			// skoro tablice sa posortowane wg stopni, to mamy na pewno taka kolejnosc
			assert(iNewDeg <= iDeg);
			if (iNewDeg != iDeg)
			{
				// gdy znaleziono koniec grupy o tym samym stopniu
				// sortuj po liczbie neuronow w obrebie grupy
				DB(_PrintDegrees(i));
				DB(printf("qsort( poczatek=%i, rozmiar=%i, sizeof(TDN)=%i)\n", iPocz, (j - iPocz), sizeof(TDN));)
					// wyswietlamy z jedna komorka po zakonczeniu tablicy
					DB(_PrintArray(m_aDegrees[i][iPocz], 0, (j - iPocz) * 4);)

					std::qsort(m_aDegrees[i][iPocz], (size_t)(j - iPocz),
						sizeof(TDN), ModelSimil::CompareConnsNo);
				DB(_PrintDegrees(i));
				// wyswietlamy z jedna komorka po zakonczeniu tablicy
				DB(_PrintArray(m_aDegrees[i][iPocz], 0, (j - iPocz) * 4);)
					// rozpocznij nowa grupe
					iPocz = j;
				iDeg = iNewDeg;
			}
		}
	}
	return 1;
}


/** Prints the state of the matching object. Debug method.
 */
void ModelSimil::_PrintPartsMatching()
{
	// assure that matching exists
	assert(m_pMatching != NULL);

	printf("Parts matching:\n");
	m_pMatching->PrintMatching();
}

void ModelSimil::ComputeMatching()
{
	// uniwersalne liczniki
	int i, j;

	assert(m_pMatching != NULL);
	assert(m_pMatching->IsEmpty() == true);

	// rozpoczynamy etap dopasowywania Parts w organizmach
	// czy dopasowano już wszystkie Parts?
	int iCzyDopasowane = 0;
	// koniec grupy aktualnie dopasowywanej w każdym organizmie
	int aiKoniecGrupyDopasowania[2] = { 0, 0 };
	// koniec grupy już w całości dopasowanej
	// (Pomiedzy tymi dwoma indeksami znajduja sie Parts w tablicy
	// m_aDegrees, ktore moga byc dopasowywane (tam nadal moga
	// byc tez dopasowane - ale nie musi to byc w sposob
	// ciagly)
	int aiKoniecPierwszejGrupy[2] = { 0, 0 };
	// Tablica przechowująca odległości poszczególnych Parts z aktualnych
	// grup dopasowania. Rozmiar - prostokąt o bokach równych liczbie elementów w 
	// dopasowywanych aktualnie grupach. Pierwszy wymiar - pierwszy organizm.
	// Drugi wymiar - drugi organizm (nie zależy to od tego, który jest mniejszy).
	// Wliczane w rozmiar tablicy są nawet już dopasowane elementy - tablice
	// paiCzyDopasowany pamiętają stan dopasowania tych elementów.
	typedef double *TPDouble;
	double **aadOdleglosciParts;
	// dwie tablice okreslajace Parts, ktore moga byc do siebie dopasowywane
	// rozmiary: [0] - aiRozmiarCalychGrup[1]
	//			 [1] - aiRozmiarCalychGrup[0]
	std::vector<bool> *apvbCzyMinimalnaOdleglosc[2];
	// rozmiar aktualnie dopasowywanej grupy w odpowiednim organizmie (tylko elementy
	// jeszcze niedopasowane).
	int aiRozmiarGrupy[2];
	// rozmiar aktualnie dopasowywanych grup w odpowiednim organizmie (włączone są
	// w to również elementy już dopasowane).
	int aiRozmiarCalychGrup[2] = { 0, 0 };

	// utworzenie tablicy rozmiarow
	for (i = 0; i < 2; i++)
	{
		m_aiPartCount[i] = m_Mod[i]->getPartCount();
	}

	// DOPASOWYWANIE PARTS
	while (!iCzyDopasowane)
	{
		// znajdz konce obu grup aktualnie dopasowywanych w obu organizmach
		for (i = 0; i < 2; i++)
		{
			// czyli poszukaj miejsca zmiany stopnia lub konca tablicy
			for (j = aiKoniecPierwszejGrupy[i] + 1; j < m_aiPartCount[i]; j++)
			{
				if (m_aDegrees[i][j][DEGREE] < m_aDegrees[i][j - 1][DEGREE])
				{
					break;
				}
			}
			aiKoniecGrupyDopasowania[i] = j;

			// sprawdz poprawnosc tego indeksu
			assert((aiKoniecGrupyDopasowania[i] > 0) &&
				(aiKoniecGrupyDopasowania[i] <= m_aiPartCount[i]));

			// oblicz rozmiary całych grup - łącznie z dopasowanymi już elementami
			aiRozmiarCalychGrup[i] = aiKoniecGrupyDopasowania[i] -
				aiKoniecPierwszejGrupy[i];

			// sprawdz teraz rozmiar tej grupy w sensie liczby niedopasowanzch
			// nie moze to byc puste!
			aiRozmiarGrupy[i] = 0;
			for (j = aiKoniecPierwszejGrupy[i]; j < aiKoniecGrupyDopasowania[i]; j++)
			{
				// od poczatku do konca grupy
				if (!m_pMatching->IsMatched(i, j))
				{
					// jesli niedopasowany, to zwieksz licznik
					aiRozmiarGrupy[i]++;
				}
			}
			// grupa nie moze byc pusta!
			assert(aiRozmiarGrupy[i] > 0);
		}

		// DOPASOWYWANIE PARTS Z GRUP

		// stworzenie tablicy odległości lokalnych
		// stwórz pierwszy wymiar - wg rozmiaru zerowego organizmu
		aadOdleglosciParts = new TPDouble[aiRozmiarCalychGrup[0]];
		assert(aadOdleglosciParts != NULL);
		// stwórz drugi wymiar - wg rozmiaru drugiego organizmu
		for (i = 0; i < aiRozmiarCalychGrup[0]; i++)
		{
			aadOdleglosciParts[i] = new double[aiRozmiarCalychGrup[1]];
			assert(aadOdleglosciParts[i] != NULL);
		}

		// stworzenie tablic mozliwosci dopasowania (indykatorow minimalnej odleglosci)
		apvbCzyMinimalnaOdleglosc[0] = new std::vector<bool>(aiRozmiarCalychGrup[1], false);
		apvbCzyMinimalnaOdleglosc[1] = new std::vector<bool>(aiRozmiarCalychGrup[0], false);
		// sprawdz stworzenie tablic
		assert(apvbCzyMinimalnaOdleglosc[0] != NULL);
		assert(apvbCzyMinimalnaOdleglosc[1] != NULL);

		// wypełnienie elementów macierzy (i,j) odległościami pomiędzy
		// odpowiednimi Parts: (i) w organizmie 0 i (j) w organizmie 1.
		// UWAGA! Uwzględniamy tylko te Parts, które nie są jeszcze dopasowane
		// (reszta to byłaby po prostu strata czasu).
		int iDeg, iNeu; // ilościowe określenie różnic stopnia, liczby neuronów i połączeń Parts
		//int iNIt;
		double dGeo; // ilościowe określenie różnic geometrycznych pomiędzy Parts
		// indeksy konkretnych Parts - indeksy sa ZERO-BASED, choć właściwy dostep
		// do informacji o Part wymaga dodania aiKoniecPierwszejGrupy[]
		// tylko aadOdleglosciParts[][] indeksuje sie bezposrednio zawartoscia iIndex[]
		int iIndex[2];
		int iPartIndex[2] = { -1, -1 }; // at [iModel]: original index of a Part for the specified model (iModel)

		// debug - wypisz zakres dopasowywanych indeksow
		DB(printf("Organizm 0: grupa: (%2i, %2i)\n", aiKoniecPierwszejGrupy[0],
			aiKoniecGrupyDopasowania[0]);)
			DB(printf("Organizm 1: grupa: (%2i, %2i)\n", aiKoniecPierwszejGrupy[1],
				aiKoniecGrupyDopasowania[1]);)

			for (i = 0; i < aiRozmiarCalychGrup[0]; i++)
			{

				// iterujemy i - Parts organizmu 0
				// (indeks podstawowy - aiKoniecPierwszejGrupy[0])

				if (!(m_pMatching->IsMatched(0, aiKoniecPierwszejGrupy[0] + i)))
				{
					// interesuja nas tylko te niedopasowane jeszcze (i)
					for (j = 0; j < aiRozmiarCalychGrup[1]; j++)
					{

						// iterujemy j - Parts organizmu 1
						// (indeks podstawowy - aiKoniecPierwszejGrupy[1])

						if (!(m_pMatching->IsMatched(1, aiKoniecPierwszejGrupy[1] + j)))
						{
							// interesuja nas tylko te niedopasowane jeszcze (j)
							// teraz obliczymy lokalne różnice pomiędzy Parts
							iDeg = abs(m_aDegrees[0][aiKoniecPierwszejGrupy[0] + i][1]
								- m_aDegrees[1][aiKoniecPierwszejGrupy[1] + j][1]);

							//iNit currently is not a component of distance measure            
							//iNIt = abs(m_aDegrees[0][ aiKoniecPierwszejGrupy[0] + i ][2]
							//           - m_aDegrees[1][ aiKoniecPierwszejGrupy[1] + j ][2]);

							iNeu = abs(m_aDegrees[0][aiKoniecPierwszejGrupy[0] + i][3]
								- m_aDegrees[1][aiKoniecPierwszejGrupy[1] + j][3]);

							// obliczenie także lokalnych różnic geometrycznych pomiędzy Parts
							// find original indices of Parts for both of the models
							iPartIndex[0] = m_aDegrees[0][aiKoniecPierwszejGrupy[0] + i][0];
							iPartIndex[1] = m_aDegrees[1][aiKoniecPierwszejGrupy[1] + j][0];
							// now compute the geometrical distance of these Parts (use m_aPositions
							// which should be computed by SVD)
							Pt3D Part0Pos(m_aPositions[0][iPartIndex[0]]);
							Pt3D Part1Pos(m_aPositions[1][iPartIndex[1]]);
							dGeo = m_adFactors[3] == 0 ? 0 : Part0Pos.distanceTo(Part1Pos); //no need to compute distane when m_dDG weight is 0

							// tutaj prawdopodobnie należy jeszcze dodać sprawdzanie
							// identyczności pozostałych własności (oprócz geometrii)
							// - żeby móc stwierdzić w ogóle identyczność Parts

							// i ostateczna odleglosc indukowana przez te roznice
							// (tutaj nie ma różnicy w liczbie wszystkich wierzchołków)
							aadOdleglosciParts[i][j] = m_adFactors[1] * double(iDeg) +
								m_adFactors[2] * double(iNeu) +
								m_adFactors[3] * dGeo;
							DB(printf("Parts Distance (%2i,%2i) = %.3lf\n", aiKoniecPierwszejGrupy[0] + i,
								aiKoniecPierwszejGrupy[1] + j, aadOdleglosciParts[i][j]);)
								DB(printf("Parts geometrical distance = %.20lf\n", dGeo);)
								DB(printf("Pos0: (%.3lf %.3lf %.3lf)\n", Part0Pos.x, Part0Pos.y, Part0Pos.z);)
								DB(printf("Pos1: (%.3lf %.3lf %.3lf)\n", Part1Pos.x, Part1Pos.y, Part1Pos.z);)
						}
					}
				}
			}

		// tutaj - sprawdzic tylko, czy tablica odleglosci lokalnych jest poprawnie obliczona

		// WYKORZYSTANIE TABLICY ODLEGLOSCI DO BUDOWY DOPASOWANIA

		// trzeba raczej iterować aż do zebrania wszystkich możliwych dopasowań w grupie
		// dlatego wprowadzamy dodatkowa zmienna - czy skonczyla sie juz grupa
		bool bCzyKoniecGrupy = false;
		while (!bCzyKoniecGrupy)
		{
			for (i = 0; i < 2; i++)
			{
				// iterujemy (i) po organizmach
				// szukamy najpierw jakiegoś niedopasowanego jeszcze Part w organizmach

				// zakładamy, że nie ma takiego Part
				iIndex[i] = -1;

				for (j = 0; j < aiRozmiarCalychGrup[i]; j++)
				{
					// iterujemy (j) - Parts organizmu (i)
					// (indeks podstawowy - aiKoniecPierwszejGrupy[0])
					if (!(m_pMatching->IsMatched(i, aiKoniecPierwszejGrupy[i] + j)))
					{
						// gdy mamy w tej grupie jakis niedopasowany element, to ustawiamy
						// iIndex[i] (chcemy w zasadzie pierwszy taki)
						iIndex[i] = j;
						break;
					}
				}

				// sprawdzamy, czy w ogole znaleziono taki Part
				if (iIndex[i] < 0)
				{
					// gdy nie znaleziono takiego Part - mamy koniec dopasowywania w
					// tych grupach
					bCzyKoniecGrupy = true;
				}
				// sprawdz poprawnosc indeksu niedopasowanego Part - musi byc w aktualnej grupie
				assert((iIndex[i] >= -1) && (iIndex[i] < aiRozmiarCalychGrup[i]));
			}


			// teraz mamy sytuacje:
			// - mamy w iIndex[0] i iIndex[1] indeksy pierwszych niedopasowanych Part
			//		w organizmach, albo
			// - nie ma w ogóle już czego dopasowywać (należy przejść do innej grupy)
			if (!bCzyKoniecGrupy)
			{
				// gdy nie ma jeszcze końca żadnej z grup - możemy dopasowywać
				// najpierw wyszukujemy w tablicy minimum odległości od tych
				// wyznaczonych Parts

				// najpierw wyczyscimy wektory potencjalnych dopasowan
				// dla organizmu 1 (o rozmiarze grupy z 0)
				for (i = 0; i < aiRozmiarCalychGrup[0]; i++)
				{
					apvbCzyMinimalnaOdleglosc[1]->operator[](i) = false;
				}
				// dla organizmu 0 (o rozmiarze grup z 1)
				for (i = 0; i < aiRozmiarCalychGrup[1]; i++)
				{
					apvbCzyMinimalnaOdleglosc[0]->operator[](i) = false;
				}

				// szukanie minimum dla Part o indeksie iIndex[0] w organizmie 0
				// wsrod niedopasowanych Parts z organizmu 1
				// zakładamy, że nie znaleliśmy jeszcze minimum
				double dMinimum = HUGE_VAL;
				for (i = 0; i < aiRozmiarCalychGrup[1]; i++)
				{
					if (!(m_pMatching->IsMatched(1, aiKoniecPierwszejGrupy[1] + i)))
					{

						// szukamy minimum obliczonej lokalnej odleglosci tylko wsrod
						// niedopasowanych jeszcze Parts
						if (aadOdleglosciParts[iIndex[0]][i] < dMinimum)
						{
							dMinimum = aadOdleglosciParts[iIndex[0]][i];
						}

						// przy okazji - sprawdz, czy HUGE_VAL jest rzeczywiscie max dla double
						assert(aadOdleglosciParts[iIndex[0]][i] < HUGE_VAL);
					}
				}
				// sprawdz, czy minimum znaleziono - musi takie byc, bo jest cos niedopasowanego
				assert((dMinimum >= 0.0) && (dMinimum < HUGE_VAL));

				// teraz zaznaczamy w tablicy te wszystkie Parts, ktore maja
				// rzeczywiscie te minimalna odleglosc do Part iIndex[0] w organizmie 0
				for (i = 0; i < aiRozmiarCalychGrup[1]; i++)
				{
					if (!(m_pMatching->IsMatched(1, aiKoniecPierwszejGrupy[1] + i)))
					{
						if (aadOdleglosciParts[iIndex[0]][i] == dMinimum)
						{
							// jesli w danym miejscu tablicy odleglosci jest faktyczne
							// minimum odleglosci, to mamy potencjalna pare dla iIndex[0]
							apvbCzyMinimalnaOdleglosc[0]->operator[](i) = true;
						}

						// sprawdz poprawnosc znalezionego wczesniej minimum
						assert(aadOdleglosciParts[iIndex[0]][i] >= dMinimum);
					}
				}

				// podobnie szukamy minimum dla Part o indeksie iIndex[1] w organizmie 1
				// wsrod niedopasowanych Parts z organizmu 0
				dMinimum = HUGE_VAL;
				for (i = 0; i < aiRozmiarCalychGrup[0]; i++)
				{
					if (!(m_pMatching->IsMatched(0, aiKoniecPierwszejGrupy[0] + i)))
					{
						// szukamy minimum obliczonej lokalnej odleglosci tylko wsrod
						// niedopasowanych jeszcze Parts
						if (aadOdleglosciParts[i][iIndex[1]] < dMinimum)
						{
							dMinimum = aadOdleglosciParts[i][iIndex[1]];
						}
						// przy okazji - sprawdz, czy HUGE_VAL jest rzeczywiscie max dla double
						assert(aadOdleglosciParts[i][iIndex[1]] < HUGE_VAL);
					}
				}
				// sprawdz, czy minimum znaleziono - musi takie byc, bo jest cos niedopasowanego
				assert((dMinimum >= 0.0) && (dMinimum < HUGE_VAL));

				// teraz zaznaczamy w tablicy te wszystkie Parts, ktore maja
				// rzeczywiscie te minimalne odleglosci do Part iIndex[1] w organizmie 1
				for (i = 0; i < aiRozmiarCalychGrup[0]; i++)
				{
					if (!(m_pMatching->IsMatched(0, aiKoniecPierwszejGrupy[0] + i)))
					{
						if (aadOdleglosciParts[i][iIndex[1]] == dMinimum)
						{
							// jesli w danym miejscu tablicy odleglosci jest faktyczne
							// minimum odleglosci, to mamy potencjalna pare dla iIndex[1]
							apvbCzyMinimalnaOdleglosc[1]->operator[](i) = true;
						}

						// sprawdz poprawnosc znalezionego wczesniej minimum
						assert(aadOdleglosciParts[i][iIndex[1]] >= dMinimum);
					}
				}

				// teraz mamy juz poszukane potencjalne grupy dopasowania - musimy
				// zdecydowac, co do czego dopasujemy!
				// szukamy Part iIndex[0] posrod mozliwych do dopasowania dla Part iIndex[1]
				// szukamy takze Part iIndex[1] posrod mozliwych do dopasowania dla Part iIndex[0]
				bool PartZ1NaLiscie0 = apvbCzyMinimalnaOdleglosc[0]->operator[](iIndex[1]);
				bool PartZ0NaLiscie1 = apvbCzyMinimalnaOdleglosc[1]->operator[](iIndex[0]);

				if (PartZ1NaLiscie0 && PartZ0NaLiscie1)
				{
					// PRZYPADEK 1. Oba Parts maja sie wzajemnie na listach mozliwych
					// dopasowan.
					// AKCJA. Dopasowanie wzajemne do siebie.

					m_pMatching->Match(0, aiKoniecPierwszejGrupy[0] + iIndex[0],
						1, aiKoniecPierwszejGrupy[1] + iIndex[1]);

					// zmniejsz liczby niedopasowanych elementow w grupach
					aiRozmiarGrupy[0]--;
					aiRozmiarGrupy[1]--;
					// debug - co zostalo dopasowane
					DB(printf("Przypadek 1.: dopasowane Parts: (%2i, %2i)\n", aiKoniecPierwszejGrupy[0]
						+ iIndex[0], aiKoniecPierwszejGrupy[1] + iIndex[1]);)

				}// PRZYPADEK 1.
				else
					if (PartZ1NaLiscie0 || PartZ0NaLiscie1)
					{
						// PRZYPADEK 2. Tylko jeden z Parts ma drugiego na swojej liscie
						// mozliwych dopasowan
						// AKCJA. Dopasowanie jednego jest proste (tego, ktory nie ma 
						// na swojej liscie drugiego). Dla tego drugiego nalezy powtorzyc
						// duza czesc obliczen (znalezc mu nowa mozliwa pare)

						// indeks organizmu, ktorego Part nie ma dopasowywanego Part
						// z drugiego organizmu na swojej liscie
						int iBezDrugiego;

						// okreslenie indeksu organizmu z dopasowywanym Part
						if (!PartZ1NaLiscie0)
						{
							iBezDrugiego = 0;
						}
						else
						{
							iBezDrugiego = 1;
						}
						// sprawdz indeks organizmu
						assert((iBezDrugiego == 0) || (iBezDrugiego == 1));

						int iDopasowywany = -1;
						// poszukujemy pierwszego z listy dopasowania
						for (i = 0; i < aiRozmiarCalychGrup[1 - iBezDrugiego]; i++)
						{
							if (apvbCzyMinimalnaOdleglosc[iBezDrugiego]->operator[](i))
							{
								iDopasowywany = i;
								break;
							}
						}
						// sprawdz poprawnosc indeksu dopasowywanego (musimy cos znalezc!)
						// nieujemny i w odpowiedniej grupie!
						assert((iDopasowywany >= 0) &&
							(iDopasowywany < aiRozmiarCalychGrup[1 - iBezDrugiego]));

						// znalezlismy 1. Part z listy dopasowania - dopasowujemy!
						m_pMatching->Match(
							iBezDrugiego,
							aiKoniecPierwszejGrupy[iBezDrugiego] + iIndex[iBezDrugiego],
							1 - iBezDrugiego,
							aiKoniecPierwszejGrupy[1 - iBezDrugiego] + iDopasowywany);
						DB(printf("Przypadek 2.1.: dopasowane Parts dopasowanie bez drugiego: (%2i, %2i)\n", aiKoniecPierwszejGrupy[iBezDrugiego] + iIndex[iBezDrugiego],
							aiKoniecPierwszejGrupy[1 - iBezDrugiego] + iDopasowywany);)

							// zmniejsz liczby niedopasowanych elementow w grupach
							aiRozmiarGrupy[0]--;
						aiRozmiarGrupy[1]--;

						// sprawdz, czy jest szansa dopasowania tego Part z drugiej strony
						// (ktora miala mozliwosc dopasowania tego Part z poprzedniego organizmu)
						if ((aiRozmiarGrupy[0] > 0) && (aiRozmiarGrupy[1] > 0))
						{
							// jesli grupy sie jeszcze nie wyczrpaly
							// to jest mozliwosc dopasowania w organizmie

							int iZDrugim = 1 - iBezDrugiego;
							// sprawdz indeks organizmu
							assert((iZDrugim == 0) || (iZDrugim == 1));

							// bedziemy szukac minimum wsrod niedopasowanych - musimy wykasowac
							// poprzednie obliczenia minimum
							// dla organizmu 1 (o rozmiarze grupy z 0)
							for (i = 0; i < aiRozmiarCalychGrup[0]; i++)
							{
								apvbCzyMinimalnaOdleglosc[1]->operator[](i) = false;
							}
							// dla organizmu 0 (o rozmiarze grup z 1)
							for (i = 0; i < aiRozmiarCalychGrup[1]; i++)
							{
								apvbCzyMinimalnaOdleglosc[0]->operator[](i) = false;
							}

							// szukamy na nowo minimum dla Part o indeksie iIndex[ iZDrugim ] w organizmie iZDrugim
							// wsrod niedopasowanych Parts z organizmu 1 - iZDrugim
							dMinimum = HUGE_VAL;
							for (i = 0; i < aiRozmiarCalychGrup[1 - iZDrugim]; i++)
							{
								if (!(m_pMatching->IsMatched(
									1 - iZDrugim,
									aiKoniecPierwszejGrupy[1 - iZDrugim] + i)))
								{
									// szukamy minimum obliczonej lokalnej odleglosci tylko wsrod
									// niedopasowanych jeszcze Parts
									if (iZDrugim == 0)
									{
										// teraz niestety musimy rozpoznac odpowiedni organizm
										// zeby moc indeksowac niesymetryczna tablice
										if (aadOdleglosciParts[iIndex[0]][i] < dMinimum)
										{
											dMinimum = aadOdleglosciParts[iIndex[0]][i];
										}
										// przy okazji - sprawdz, czy HUGE_VAL jest rzeczywiscie max dla double
										assert(aadOdleglosciParts[iIndex[0]][i] < HUGE_VAL);

									}
									else
									{
										if (aadOdleglosciParts[i][iIndex[1]] < dMinimum)
										{
											dMinimum = aadOdleglosciParts[i][iIndex[1]];
										}
										// przy okazji - sprawdz, czy HUGE_VAL jest rzeczywiscie max dla double
										assert(aadOdleglosciParts[i][iIndex[1]] < HUGE_VAL);
									}
								}
							}
							// sprawdz, czy minimum znaleziono - musi takie byc, bo jest cos niedopasowanego
							assert((dMinimum >= 0.0) && (dMinimum < HUGE_VAL));

							// teraz zaznaczamy w tablicy te wszystkie Parts, ktore maja
							// rzeczywiscie te minimalne odleglosci do Part w organizmie iZDrugim
							for (i = 0; i < aiRozmiarCalychGrup[1 - iZDrugim]; i++)
							{
								if (!(m_pMatching->IsMatched(
									1 - iZDrugim,
									aiKoniecPierwszejGrupy[1 - iZDrugim] + i)))
								{
									if (iZDrugim == 0)
									{
										// teraz niestety musimy rozpoznac odpowiedni organizm
										// zeby moc indeksowac niesymetryczna tablice
										if (aadOdleglosciParts[iIndex[0]][i] == dMinimum)
										{
											// jesli w danym miejscu tablicy odleglosci jest faktyczne 
											// minimum odleglosci, to mamy potencjalna pare dla iIndex[1]
											apvbCzyMinimalnaOdleglosc[0]->operator[](i) = true;
										}
									}
									else
									{
										if (aadOdleglosciParts[i][iIndex[1]] == dMinimum)
										{
											apvbCzyMinimalnaOdleglosc[1]->operator[](i) = true;
										}
									}
								}
							}

							// a teraz - po znalezieniu potencjalnych elementow do dopasowania
							// dopasowujemy pierwszy z potencjalnych
							iDopasowywany = -1;
							for (i = 0; i < aiRozmiarCalychGrup[1 - iZDrugim]; i++)
							{
								if (apvbCzyMinimalnaOdleglosc[iZDrugim]->operator[](i))
								{
									iDopasowywany = i;
									break;
								}
							}
							// musielismy znalezc jakiegos dopasowywanego
							assert((iDopasowywany >= 0) &&
								(iDopasowywany < aiRozmiarCalychGrup[1 - iZDrugim]));

							// no to juz mozemy dopasowac
							m_pMatching->Match(
								iZDrugim,
								aiKoniecPierwszejGrupy[iZDrugim] + iIndex[iZDrugim],
								1 - iZDrugim,
								aiKoniecPierwszejGrupy[1 - iZDrugim] + iDopasowywany);
							DB(printf("Przypadek 2.1.: dopasowane Parts dopasowaniebz drugim: (%2i, %2i)\n", aiKoniecPierwszejGrupy[iZDrugim] + iIndex[iZDrugim], aiKoniecPierwszejGrupy[1 - iZDrugim] + iDopasowywany);)

								//aiKoniecPierwszejGrupy[ 1-iBezDrugiego ] + iDopasowywany ;)
								//aiKoniecPierwszejGrupy[ 1-iBezDrugiego ] + iDopasowywany ;)
								// zmniejsz liczby niedopasowanych elementow w grupach
								aiRozmiarGrupy[0]--;
							aiRozmiarGrupy[1]--;

						}
						else
						{
							// jedna z grup sie juz wyczerpala
							// wiec nie ma mozliwosci dopasowania tego drugiego Partu
							/// i trzeba poczekac na zmiane grupy
						}

						DB(printf("Przypadek 2.\n");)

					}// PRZYPADEK 2.
					else
					{
						// PRZYPADEK 3. Zaden z Parts nie ma na liscie drugiego
						// AKCJA. Niezalezne dopasowanie obu Parts do pierwszych ze swojej listy

						// najpierw dopasujemy do iIndex[0] w organizmie 0
						int iDopasowywany = -1;
						// poszukujemy pierwszego z listy dopasowania - w organizmie 1
						for (i = 0; i < aiRozmiarCalychGrup[1]; i++)
						{
							if (apvbCzyMinimalnaOdleglosc[0]->operator[](i))
							{
								iDopasowywany = i;
								break;
							}
						}
						// musielismy znalezc jakiegos dopasowywanego
						assert((iDopasowywany >= 0) &&
							(iDopasowywany < aiRozmiarCalychGrup[1]));

						// teraz wlasnie dopasowujemy
						m_pMatching->Match(
							0,
							aiKoniecPierwszejGrupy[0] + iIndex[0],
							1,
							aiKoniecPierwszejGrupy[1] + iDopasowywany);

						// zmniejszamy liczbe niedopasowanych Parts
						aiRozmiarGrupy[0]--;
						aiRozmiarGrupy[1]--;

						// debug - dopasowanie
						DB(printf("Przypadek 3.: dopasowane Parts: (%2i, %2i)\n", aiKoniecPierwszejGrupy[0]
							+ iIndex[0], aiKoniecPierwszejGrupy[1] + iDopasowywany);)

							// teraz dopasowujemy do iIndex[1] w organizmie 1
							iDopasowywany = -1;
						// poszukujemy pierwszego z listy dopasowania - w organizmie 0
						for (i = 0; i < aiRozmiarCalychGrup[0]; i++)
						{
							if (apvbCzyMinimalnaOdleglosc[1]->operator[](i))
							{
								iDopasowywany = i;
								break;
							}
						}
						// musielismy znalezc jakiegos dopasowywanego
						assert((iDopasowywany >= 0) &&
							(iDopasowywany < aiRozmiarCalychGrup[0]));

						// no i teraz realizujemy dopasowanie
						m_pMatching->Match(
							0,
							aiKoniecPierwszejGrupy[0] + iDopasowywany,
							1,
							aiKoniecPierwszejGrupy[1] + iIndex[1]);

						// zmniejszamy liczbe niedopasowanych Parts
						aiRozmiarGrupy[0]--;
						aiRozmiarGrupy[1]--;

						// debug - dopasowanie
						DB(printf("Przypadek 3.: dopasowane Parts: (%2i, %2i)\n", aiKoniecPierwszejGrupy[0]
							+ iDopasowywany, aiKoniecPierwszejGrupy[1] + iIndex[1]);)


					} // PRZYPADEK 3.

			}// if (! bCzyKoniecGrupy)
			else
			{
				// gdy mamy juz koniec grup - musimy zlikwidowac tablice aadOdleglosciParts
				// bo za chwilke skonczy sie nam petla
				for (i = 0; i < aiRozmiarCalychGrup[0]; i++)
				{
					SAFEDELETEARRAY(aadOdleglosciParts[i]);
				}
				SAFEDELETEARRAY(aadOdleglosciParts);

				// musimy tez usunac tablice (wektory) mozliwosci dopasowania
				SAFEDELETE(apvbCzyMinimalnaOdleglosc[0]);
				SAFEDELETE(apvbCzyMinimalnaOdleglosc[1]);
			}
		} // while (! bCzyKoniecGrupy)

		// PO DOPASOWANIU WSZYSTKIEGO Z GRUP (CO NAJMNIEJ JEDNEJ GRUPY W CALOSCI)

		// gdy rozmiar ktorejkolwiek z grup dopasowania spadl do zera
		// to musimy przesunac KoniecPierwszejGrupy (wszystkie dopasowane)
		for (i = 0; i < 2; i++)
		{
			// grupy nie moga miec mniejszego niz 0 rozmiaru
			assert(aiRozmiarGrupy[i] >= 0);
			if (aiRozmiarGrupy[i] == 0)
				aiKoniecPierwszejGrupy[i] = aiKoniecGrupyDopasowania[i];
		}

		// sprawdzenie warunku konca dopasowywania - gdy nie
		// ma juz w jakims organizmie co dopasowywac
		if (aiKoniecPierwszejGrupy[0] >= m_aiPartCount[0] ||
			aiKoniecPierwszejGrupy[1] >= m_aiPartCount[1])
		{
			iCzyDopasowane = 1;
		}
	} // koniec WHILE - petli dopasowania
}

/** Matches Parts in both organisms so that computation of similarity is possible.
	New algorithm (assures symmetry of the similarity measure) with geometry
	taken into consideration.
	Assumptions:
	- Models (m_Mod) are created and available.
	- Matching (m_pMatching) is created, but empty
	Exit conditions:
	- Matching (m_pMatching) is full
	@return 1 if success, 0 otherwise
	*/
int ModelSimil::MatchPartsGeometry()
{
	// zaloz, ze sa modele i sa poprawne
	assert((m_Mod[0] != NULL) && (m_Mod[1] != NULL));
	assert(m_Mod[0]->isValid() && m_Mod[1]->isValid());

	if (!CreatePartInfoTables())
		return 0;
	if (!CountPartDegrees())
		return 0;
	if (!GetPartPositions())
	{
		return 0;
	}
	if (!CountPartNeurons())
		return 0;


	if (m_adFactors[3] > 0)
	{
		if (!ComputePartsPositionsBySVD())
		{
			return 0;
		}
	}

	DB(printf("Przed sortowaniem:\n");)
		DB(_PrintDegrees(0);)
		DB(_PrintDegrees(1);)

		if (!SortPartInfoTables())
			return 0;

	DB(printf("Po sortowaniu:\n");)
		DB(_PrintDegrees(0);)
		DB(_PrintDegrees(1);)

		if (m_adFactors[3] > 0)
		{
			// tutaj zacznij pętlę po przekształceniach  geometrycznych
			const int NO_OF_TRANSFORM = 8; // liczba transformacji geometrycznych (na razie tylko ID i O_YZ)
			// tablice transformacji współrzędnych; nie są to dokładnie tablice tranformacji, ale raczej tablice PRZEJŚĆ
			// pomiędzy transformacjami; 
			// wartości orginalne transformacji dOrig uzyskuje się przez:
			// for ( iTrans = 0; iTrans <= TRANS_INDEX; iTrans++ ) dOrig *= dMul[ iTrans ];
			//const char *szTransformNames[NO_OF_TRANSFORM] = { "ID", "S_yz", "S_xz", "S_xy", "R180_z", "R180_y", "R180_z", "S_(0,0,0)" };
			const int dMulX[NO_OF_TRANSFORM] = { 1, -1, -1, 1, -1, 1, -1, -1 };
			const int dMulY[NO_OF_TRANSFORM] = { 1, 1, -1, -1, -1, -1, -1, 1 };
			const int dMulZ[NO_OF_TRANSFORM] = { 1, 1, 1, -1, -1, -1, 1, 1 };

#ifdef max
#undef max //this macro would conflict with line below
#endif
			double dMinSimValue = std::numeric_limits<double>::max(); // minimum value of similarity
			int iMinSimTransform = -1; // index of the transformation with the minimum similarity
			SimilMatching *pMinSimMatching = NULL; // matching with the minimum value of similarity

			// remember the original positions of model 0 as computed by SVD in order to restore them later, after 
			// all transformations have been computed
			Pt3D *StoredPositions = NULL; // array of positions of Parts, for one (0th) model
			// create the stored positions
			StoredPositions = new Pt3D[m_Mod[0]->getPartCount()];
			assert(StoredPositions != NULL);
			// copy the original positions of model 0 (store them)
			int iPart; // a counter of Parts
			for (iPart = 0; iPart < m_Mod[0]->getPartCount(); iPart++)
			{
				StoredPositions[iPart].x = m_aPositions[0][iPart].x;
				StoredPositions[iPart].y = m_aPositions[0][iPart].y;
				StoredPositions[iPart].z = m_aPositions[0][iPart].z;
			}
			// now the original positions of model 0 are stored


			int iTransform; // a counter of geometric transformations
			for (iTransform = 0; iTransform < NO_OF_TRANSFORM; iTransform++)
			{
				// for each geometric transformation to be done
				// entry conditions:
				// - models (m_Mod) exist and are available
				// - matching (m_pMatching) exists and is empty
				// - all properties are created and available (m_aDegrees and m_aPositions)

				// recompute geometric properties according to the transformation iTransform
				// but only for model 0
				for (iPart = 0; iPart < m_Mod[0]->getPartCount(); iPart++)
				{
					// for each iPart, a part of the model iMod
					m_aPositions[0][iPart].x *= dMulX[iTransform];
					m_aPositions[0][iPart].y *= dMulY[iTransform];
					m_aPositions[0][iPart].z *= dMulZ[iTransform];
				}
				// now the positions are recomputed
				ComputeMatching();

				// teraz m_pMatching istnieje i jest pełne
				assert(m_pMatching != NULL);
				assert(m_pMatching->IsFull() == true);

				// wykorzystaj to dopasowanie i geometrię do obliczenia tymczasowej wartości miary
				int iTempRes = CountPartsDistance();
				// załóż sukces
				assert(iTempRes == 1);
				double dCurrentSim = m_adFactors[0] * double(m_iDV) +
					m_adFactors[1] * double(m_iDD) +
					m_adFactors[2] * double(m_iDN) +
					m_adFactors[3] * double(m_dDG);
				// załóż poprawną wartość podobieństwa
				assert(dCurrentSim >= 0.0);

				// porównaj wartość obliczoną z dotychczasowym minimum
				if (dCurrentSim < dMinSimValue)
				{
					// jeśli uzyskano mniejszą wartość dopasowania, 
					// to zapamiętaj to przekształcenie geometryczne i dopasowanie
					dMinSimValue = dCurrentSim;
					iMinSimTransform = iTransform;
					SAFEDELETE(pMinSimMatching);
					pMinSimMatching = new SimilMatching(*m_pMatching);
					assert(pMinSimMatching != NULL);
				}

				// teraz już można usunąć stare dopasowanie (dla potrzeb następnego przebiegu pętli)
				m_pMatching->Empty();
			} // for ( iTransform )

			// po pętli przywróć najlepsze dopasowanie
			delete m_pMatching;
			m_pMatching = pMinSimMatching;

			DB(printf("Matching has been chosen!\n");)
				// print the name of the chosen transformation:
				// printf("Chosen transformation: %s\n", szTransformNames[ iMinSimTransform ] );

				// i przywróć jednocześnie pozycje Parts modelu 0 dla tego dopasowania
				// - najpierw przywroc Parts pozycje orginalne, po SVD
				for (iPart = 0; iPart < m_Mod[0]->getPartCount(); iPart++)
				{
					m_aPositions[0][iPart].x = StoredPositions[iPart].x;
					m_aPositions[0][iPart].y = StoredPositions[iPart].y;
					m_aPositions[0][iPart].z = StoredPositions[iPart].z;
				}
			// - usun teraz zapamietane pozycje Parts
			delete[] StoredPositions;
			// - a teraz oblicz na nowo wspolrzedne wlasciwego przeksztalcenia dla model 0
			for (iTransform = 0; iTransform <= iMinSimTransform; iTransform++)
			{
				// for each transformation before and INCLUDING iMinTransform
				// do the transformation (only model 0)
				for (iPart = 0; iPart < m_Mod[0]->getPartCount(); iPart++)
				{
					m_aPositions[0][iPart].x *= dMulX[iTransform];
					m_aPositions[0][iPart].y *= dMulY[iTransform];
					m_aPositions[0][iPart].z *= dMulZ[iTransform];
				}
			}

		}
		else
		{
			ComputeMatching();
		}
	// teraz dopasowanie musi byc pelne - co najmniej w jednym organizmie musza byc
	// wszystkie elementy dopasowane
	assert(m_pMatching->IsFull() == true);

	//    _PrintDegrees(0);
	//    _PrintDegrees(1);

	DB(_PrintPartsMatching();)

		return 1;
}

void ModelSimil::_PrintSeamnessTable(std::vector<int> *pTable, int iCount)
{
	int i;
	printf("      ");
	for (i = 0; i < iCount; i++)
		printf("%3i ", i);
	printf("\n      ");
	for (i = 0; i < iCount; i++)
	{

		printf("%3i ", pTable->operator[](i));
	}
	printf("\n");
}

/** Computes elements of similarity (m_iDD, m_iDN, m_dDG) based on underlying matching.
	Assumptions:
	- Matching (m_pMatching) exists and is full.
	- Internal arrays m_aDegrees and m_aPositions exist and are properly filled in
	Exit conditions:
	- Elements of similarity are computed (m)iDD, m_iDN, m_dDG).
	@return 1 if success, otherwise 0.
	*/
int ModelSimil::CountPartsDistance()
{
	int i, temp;

	// assume existence of m_pMatching
	assert(m_pMatching != NULL);
	// musi byc pelne!
	assert(m_pMatching->IsFull() == true);

	// roznica w stopniach
	m_iDD = 0;
	// roznica w liczbie neuronów
	m_iDN = 0;
	// roznica w odleglosci dopasowanych Parts
	m_dDG = 0.0;

	int iOrgPart, iOrgMatchedPart; // orginalny indeks Part i jego dopasowanego Part
	int iMatchedPart; // indeks (wg sortowania) dopasowanego Part

	// wykorzystanie dopasowania do zliczenia m_iDD - roznicy w stopniach
	// i m_iDN - roznicy w liczbie neuronow
	// petla w wiekszej tablicy
	for (i = 0; i < m_aiPartCount[1 - m_iSmaller]; i++)
	{
		// dla kazdego elementu [i] z wiekszego organizmu
		// pobierz jego orginalny indeks w organizmie z tablicy TDN
		iOrgPart = m_aDegrees[1 - m_iSmaller][i][0];
		if (!(m_pMatching->IsMatched(1 - m_iSmaller, i)))
		{
			// gdy nie zostal dopasowany
			// dodaj jego stopien do DD
			m_iDD += m_aDegrees[1 - m_iSmaller][i][1];
			// dodaj liczbe neuronow do DN
			m_iDN += m_aDegrees[1 - m_iSmaller][i][3];
			// i oblicz odleglosc tego Part od srodka organizmu (0,0,0)
			// (uzyj orginalnego indeksu)
			//no need to compute distane when m_dDG weight is 0
			m_dDG += m_adFactors[3] == 0 ? 0 : m_aPositions[1 - m_iSmaller][iOrgPart].length();
		}
		else
		{
			// gdy byl dopasowany
			// pobierz indeks (po sortowaniu) tego dopasowanego Part
			iMatchedPart = m_pMatching->GetMatchedIndex(1 - m_iSmaller, i);
			// pobierz indeks orginalny tego dopasowanego Part
			iOrgMatchedPart = m_aDegrees[m_iSmaller][iMatchedPart][0];
			// dodaj ABS roznicy stopni do DD
			temp = m_aDegrees[1 - m_iSmaller][i][1] -
				m_aDegrees[m_iSmaller][iMatchedPart][1];
			m_iDD += abs(temp);
			// dodaj ABS roznicy neuronow do DN
			temp = m_aDegrees[1 - m_iSmaller][i][3] -
				m_aDegrees[m_iSmaller][iMatchedPart][3];
			m_iDN += abs(temp);
			// pobierz polozenie dopasowanego Part
			Pt3D MatchedPartPos(m_aPositions[m_iSmaller][iOrgMatchedPart]);
			// dodaj euklidesowa odleglosc Parts do sumy odleglosci
			//no need to compute distane when m_dDG weight is 0
			m_dDG += m_adFactors[3] == 0 ? 0 : m_aPositions[1 - m_iSmaller][iOrgPart].distanceTo(MatchedPartPos);
		}
	}

	// obliczenie i dodanie różnicy w liczbie neuronów OnJoint...
	temp = m_aOnJoint[0][3] - m_aOnJoint[1][3];
	m_iDN += abs(temp);
	DB(printf("OnJoint DN: %i\n", abs(temp));)
		// ... i Anywhere
		temp = m_aAnywhere[0][3] - m_aAnywhere[1][3];
	m_iDN += abs(temp);
	DB(printf("Anywhere DN: %i\n", abs(temp));)

		return 1;
}

/** Computes new positions of Parts of both of organisms stored in the object.
		Assumptions:
		- models (m_Mod) are created and valid
		- positions (m_aPositions) are created and filled with original positions of Parts
		- the sorting algorithm was not yet run on the array m_aDegrees
		@return true if successful; false otherwise
		*/
bool ModelSimil::ComputePartsPositionsBySVD()
{
	bool bResult = true; // the result; assume a success

	// check assumptions
	// the models
	assert(m_Mod[0] != NULL && m_Mod[0]->isValid());
	assert(m_Mod[1] != NULL && m_Mod[1]->isValid());
	// the position arrays
	assert(m_aPositions[0] != NULL);
	assert(m_aPositions[1] != NULL);

	int iMod; // a counter of models
	// use SVD to obtain different point of view on organisms
	// and store the new positions (currently the original ones are still stored)
	for (iMod = 0; iMod < 2; iMod++)
	{
		// prepare the vector of errors of approximation for the SVD
		std::vector<double> vEigenvalues;
		int nSize = m_Mod[iMod]->getPartCount();

		double *pDistances = new double[nSize * nSize];

		for (int i = 0; i < nSize; i++)
		{
			pDistances[i] = 0;
		}

		Model *pModel = m_Mod[iMod]; // use the model of the iMod (current) organism
		int iP1, iP2; // indices of Parts in the model
		Part *P1, *P2; // pointers to Parts
		Pt3D P1Pos, P2Pos; // positions of parts 
		double dDistance; // the distance between Parts

		double *weights = new double[nSize];
		for (int i = 0; i < nSize; i++)
		{
			if (wMDS == 1)
				weights[i] = 0;
			else
				weights[i] = 1;
		}

		if (wMDS == 1)
			for (int i = 0; i < pModel->getJointCount(); i++)
			{
				weights[pModel->getJoint(i)->p1_refno]++;
				weights[pModel->getJoint(i)->p2_refno]++;
			}

		for (iP1 = 0; iP1 < pModel->getPartCount(); iP1++)
		{
			// for each iP1, a Part index in the model of organism iMod
			P1 = pModel->getPart(iP1);
			// get the position of the Part
			P1Pos = P1->p;
			if (fixedZaxis == 1)
			{
				P1Pos.z = 0; //fixed vertical axis, so pretend all points are on the xy plane
			}
			for (iP2 = 0; iP2 < pModel->getPartCount(); iP2++)
			{
				// for each (iP1, iP2), a pair of Parts index in the model
				P2 = pModel->getPart(iP2);
				// get the position of the Part
				P2Pos = P2->p;
				if (fixedZaxis == 1)
				{
					P2Pos.z = 0; //fixed vertical axis, so pretend all points are on the xy plane
				}
				// compute the geometric (Euclidean) distance between the Parts
				dDistance = P1Pos.distanceTo(P2Pos);
				// store the distance
				pDistances[iP1 * nSize + iP2] = dDistance;
			} // for (iP2)
		} // for (iP1)

		MatrixTools::weightedMDS(vEigenvalues, nSize, pDistances, m_aPositions[iMod], weights);
		if (fixedZaxis == 1) //restore the original vertical coordinate of each Part
		{
			for (int part = 0; part < pModel->getPartCount(); part++)
			{
				m_aPositions[iMod][part].z = pModel->getPart(part)->p.z;
			}
		}

		delete[] pDistances;
		delete[] weights;
	}

	return bResult;
}

/**	Evaluates distance between two given genotypes. The distance depends strongly
	on weights set and the matching algorithm used.
	@param G0 Pointer to the first of compared genotypes
	@param G1 Pointer to the second of compared genotypes.
	@return Distance between two genotypes.
	@sa m_adFactors, matching_method
	*/
double ModelSimil::EvaluateDistance(const Geno *G0, const Geno *G1)
{
	return matching_method == 0 ? EvaluateDistanceHungarian(G0, G1) : EvaluateDistanceGreedy(G0, G1);
}

void ModelSimil::p_evaldistance(ExtValue *args, ExtValue *ret)
{
	Geno *g1 = GenoObj::fromObject(args[1]);
	Geno *g2 = GenoObj::fromObject(args[0]);
	if ((!g1) || (!g2))
		ret->setEmpty();
	else
		ret->setDouble(EvaluateDistance(g1, g2));
}

void ModelSimil::FillPartsDistances(double*& dist, int bigger, int smaller, bool geo)
{
	for (int i = 0; i < bigger; i++)
	{
		for (int j = 0; j < bigger; j++)
		{
			// assign penalty for unassignment for vertex from bigger model
			if (j >= smaller)
			{
				if (geo)
					dist[i*bigger + j] += m_adFactors[3] * m_aPositions[1 - m_iSmaller][i].length();
				else
					dist[i*bigger + j] = m_adFactors[1] * m_aDegrees[1 - m_iSmaller][i][DEGREE] + m_adFactors[2] * m_aDegrees[1 - m_iSmaller][i][NEURONS];
			}
			// compute distance between parts
			else
			{
				if (geo)
					dist[i*bigger + j] += m_adFactors[3] * m_aPositions[1 - m_iSmaller][i].distanceTo(m_aPositions[m_iSmaller][j]);
				else
					dist[i*bigger + j] = m_adFactors[1] * abs(m_aDegrees[1 - m_iSmaller][i][DEGREE] - m_aDegrees[m_iSmaller][j][DEGREE])
					+ m_adFactors[2] * abs(m_aDegrees[1 - m_iSmaller][i][NEURONS] - m_aDegrees[m_iSmaller][j][NEURONS]);
			}

		}
	}
}

double ModelSimil::EvaluateDistanceHungarian(const Geno *G0, const Geno *G1)
{
	double dResult = 0.0;

	m_Gen[0] = G0;
	m_Gen[1] = G1;

	// create models of objects to compare
	m_Mod[0] = newModel(m_Gen[0]);
	m_Mod[1] = newModel(m_Gen[1]);

	if (m_Mod[0] == NULL || m_Mod[1] == NULL)
		return 0.0;

	//Get information about vertex degrees, neurons and 3D location
	if (!CreatePartInfoTables())
		return 0.0;
	if (!CountPartDegrees())
		return 0.0;
	if (!GetPartPositions())
		return 0.0;
	if (!CountPartNeurons())
		return 0.0;

	m_iSmaller = m_Mod[0]->getPartCount() <= m_Mod[1]->getPartCount() ? 0 : 1;
	int nSmaller = m_Mod[m_iSmaller]->getPartCount();
	int nBigger = m_Mod[1 - m_iSmaller]->getPartCount();

	double* partsDistances = new double[nBigger*nBigger]();
	FillPartsDistances(partsDistances, nBigger, nSmaller, false);
	int *assignment = new int[nBigger]();

	HungarianAlgorithm hungarian;

	if (m_adFactors[3] > 0)
	{
		if (!ComputePartsPositionsBySVD())
		{
			return 0.0;
		}

		// tutaj zacznij pętlę po przekształceniach  geometrycznych
		const int NO_OF_TRANSFORM = 8; // liczba transformacji geometrycznych (na razie tylko ID i O_YZ)
		// tablice transformacji współrzędnych; nie są to dokładnie tablice tranformacji, ale raczej tablice PRZEJŚĆ
		// pomiędzy transformacjami; 
		const int dMulX[NO_OF_TRANSFORM] = { 1, -1, -1, 1, -1, 1, -1, -1 };
		const int dMulY[NO_OF_TRANSFORM] = { 1, 1, -1, -1, -1, -1, -1, 1 };
		const int dMulZ[NO_OF_TRANSFORM] = { 1, 1, 1, -1, -1, -1, 1, 1 };

		std::vector<int> minAssignment(nBigger);
#ifdef max
#undef max //this macro would conflict with line below
#endif
		double dMinSimValue = std::numeric_limits<double>::max(); // minimum value of similarity

		int iTransform; // a counter of geometric transformations
		for (iTransform = 0; iTransform < NO_OF_TRANSFORM; iTransform++)
		{
			// for each geometric transformation to be done
			// entry conditions:
			// - models (m_Mod) exist and are available
			// - all properties are created and available (m_aDegrees and m_aPositions)
			double* tmpPartsDistances = new double[nBigger*nBigger]();
			std::copy(partsDistances, partsDistances + nBigger * nBigger, tmpPartsDistances);
			// recompute geometric properties according to the transformation iTransform
			// but only for model 0
			for (int iPart = 0; iPart < m_Mod[m_iSmaller]->getPartCount(); iPart++)
			{
				// for each iPart, a part of the model iMod
				m_aPositions[m_iSmaller][iPart].x *= dMulX[iTransform];
				m_aPositions[m_iSmaller][iPart].y *= dMulY[iTransform];
				m_aPositions[m_iSmaller][iPart].z *= dMulZ[iTransform];
			}
			// now the positions are recomputed

			FillPartsDistances(tmpPartsDistances, nBigger, nSmaller, true);
			std::fill_n(assignment, nBigger, 0);
			double dCurrentSim = hungarian.Solve(tmpPartsDistances, assignment, nBigger, nBigger);

			delete[] tmpPartsDistances;
			// załóż poprawną wartość podobieństwa
			assert(dCurrentSim >= 0.0);

			// porównaj wartość obliczoną z dotychczasowym minimum
			if (dCurrentSim < dMinSimValue)
			{
				dMinSimValue = dCurrentSim;
				if (saveMatching)
				{
					minAssignment.clear();
					minAssignment.insert(minAssignment.begin(), assignment, assignment + nBigger);
				}
			}
		}

		dResult = dMinSimValue;
		if (saveMatching)
			std::copy(minAssignment.begin(), minAssignment.end(), assignment);
	}
	else
	{
		dResult = hungarian.Solve(partsDistances, assignment, nBigger, nBigger);
	}

	//add difference in anywhere and onJoint neurons
	dResult += m_adFactors[2] * (abs(m_aOnJoint[0][3] - m_aOnJoint[1][3]) + abs(m_aAnywhere[0][3] - m_aAnywhere[1][3]));
	//add difference in part numbers
	dResult += (nBigger - nSmaller) * m_adFactors[0];

	// delete degree arrays created in CreatePartInfoTables 
	SAFEDELETEARRAY(m_aDegrees[0]);
	SAFEDELETEARRAY(m_aDegrees[1]);

	// and position arrays
	SAFEDELETEARRAY(m_aPositions[0]);
	SAFEDELETEARRAY(m_aPositions[1]);

	// delete created models
	SAFEDELETE(m_Mod[0]);
	SAFEDELETE(m_Mod[1]);

	delete[] assignment;
	delete[] partsDistances;

	return dResult;
}
