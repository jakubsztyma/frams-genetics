// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.


#ifndef _SIMIL_MODEL_H_
#define _SIMIL_MODEL_H_

#include "frams/genetics/geno.h"
#include "frams/model/model.h"
#include "simil_match.h"

#define TDN_SIZE 5

enum TDNELEMS
{
	ORIG_IND = 0,
	DEGREE = 1,
	NEURO_CONNS = 2,
	NEURONS = 3,
	FUZZ_DEG = 4
};

/** This class defines similarity measure for Framsticks organisms.
 * Authors:
 * Marek Kubiak (concept, implementation)
 * Maciej Komosinski (concept, Framsticks interface)
 * Agnieszka Mensfelt (refactoring, improvements)
 */
class ModelSimil
{
public:
	ModelSimil();
	virtual ~ModelSimil();
	double EvaluateDistance(const Geno *G0, const Geno *G1); //chooses greedy or hungarian
	double EvaluateDistanceGreedy(const Geno *G0, const Geno *G1);
	double EvaluateDistanceHungarian(const Geno *G0, const Geno *G1);

	static int CompareDegrees(const void *pElem1, const void *pElem2);
	static int CompareFuzzyDegrees(const void *pElem1, const void *pElem2);
	static int CompareConnsNo(const void *pElem1, const void *pElem2);
	static int GetNOFactors();
#define STATRICKCLASS ModelSimil
	PARAMPROCDEF(p_evaldistance);
#undef STATRICKCLASS

protected:
	void _PrintSeamnessTable(std::vector<int> *pVector, int iCount);
	//matching function
	int MatchPartsGeometry();
	void ComputeMatching();
	void FillPartsDistances(double *&dist, int bigger, int smaller, bool geo);
	void _PrintPartsMatching();
	void SaveIntermediateFiles();

	int SortPartInfoTables();
	int CountPartNeurons();
	bool ComputePartsPositionsBySVD();
	int GetPartPositions();
	int CountPartDegrees();

	void CountFuzzyNeighb();
	void SortFuzzyNeighb();
	void GetNeighbIndexes(int mod, int partInd, std::vector<int> &indexes);
	void FuzzyOrder();

	static Model* newModel(const Geno *g);
	int CreatePartInfoTables();
	void _PrintDegrees(int i);
	void _PrintArray(int *array, int base, int size);
	void _PrintNeighbourhood(int i);
	void _PrintFuzzyNeighbourhood(int i);
	void _PrintArrayDouble(double *array, int base, int size);
	int CountPartsDistance();


public:
	/// Currently selected matching algorithm. Allowed values: 0 (more exact, slower), 1 (more greedy, faster). Details in https://doi.org/10.1007/978-3-030-16692-2_8
	/// @sa EvaluateDistance
	paInt matching_method;

	/// Table of weights for weighted distance function.
	/// Weights are for factors in the following order:
	/// [0]: m_iDV (difference in the number of vertices)
	/// [1]: m_iDD (difference in degrees over matching)
	/// [2]: m_iDN (difference in neurons over matching)
	/// [3]: m_dDG (difference in geometry over matching)
	/// @sa EvaluateDistance
	double m_adFactors[4];

	//for Zfixed = 1, the "z" (vertical) coordinates are not taken into account during PCA alignment
	paInt fixedZaxis;

	//Controls the depth of fuzzy neighbourhood 
	int fuzzyDepth;
	bool isFuzzy;

	//For wMDS = 1 weighted MDS with vertex degrees as weights is used for the alignment.
	paInt wMDS;

	//For saveMatching = 1 the best matching found will be saved.
	bool saveMatching;

	/// Interface to local parameters
	Param localpar;

protected:

	/// Between these genotypes distance is evaluated.
	const Geno *m_Gen[2];

	/// These models will be created to get the information about creatures
	/// from their genotypes.
	Model *m_Mod[2];

	/// Index (0 or 1) of the smaler creature (in the meaning of parts).
	/// Index of the bigger one is (1-m_iSmaller).
	int m_iSmaller;

	/// Number of parts of two creatures (index the same as for m_Mod).
	int m_aiPartCount[2];

	/// Difference between number of parts in organisms
	int m_iDV;

	/// Sum of absolute values of differences between matched part degrees
	int m_iDD;

	/// Sum of absolute values of differences between matched part 
	/// in neurons number.
	int m_iDN;
	//2 matrices of neighbourhood of parts - one for each genotype 

	/// Sum of Euclidean distances between matched parts
	/// Unmatched Parts have the distance measured to (0,0,0) (the middle of
	/// an organism)
	double m_dDG;

	/// Object that holds the matching of Parts.
	// It is not clear now whether the matching function is
	// created for orginal indices of Parts, or for sorted Parts
	// Most probably it is for sorted Parts.
	SimilMatching *m_pMatching;

	/// Type of 4 ints - describing one Part of the creature in
	/// its sorted table of degrees
	/// TDN[0] - original index of creature's Part (that is "i" from GetPart(i))
	/// TDN[1] - degree (number of adjacent joints) of one Part
	/// TDN[2] - number of NeuroConnections and Neurons belonging to one Part
	/// TDN[3] - number of Neurons of the Part
	/// TDN[4] - fuzzy degree
	typedef int TDN[5];

	/** 2 arrays holding information about compared organisms (one for each
	creature) of degree and neuro info for Parts.
	Index corresponds to the one in m_Mod
	m_aDegrees[i][j] is a TDN of the j-th Part of the i-th creature in m_Mod
	*/
	TDN *m_aDegrees[2];

	//std::pair<TDN, double> *m_aDegrees[2];
	/// Holds information on all on-joint neurons. Only TDN[3] and TDN[4]
	/// are important (original index and degree are not important).
	TDN m_aOnJoint[2];

	/// Holds information on all neurons that are not placed neither on
	/// a joint nor on a part. Only TDN[3] and TDN[4]
	/// are important (original index and degree are not important).
	TDN m_aAnywhere[2];

	//array of parts neighbourhood
	int **m_Neighbours[2];
	//array of "fuzzy neigbourhood" for each of organisms. Dimensions: parts_num x fuzzyDepth
	float **m_fuzzyNeighb[2];

	/// Two arrays of points which hold information about positions of Parts
	/// of both of the compared organisms.
	/// Matching methods which do not use geometry (MatchPartsOld
	/// and MatchPartsNew) simply copy these positions from models. The only
	/// matching method which uses geometry (MatchPartsNewGeometry) makes
	/// use of these arrays extensively.
	/// At m_aPositions[ iModel ][ iOriginalPart ] there is a Pt3D of 
	/// a Part with index iOriginalPart of the model iModel.
	/// iOriginalPart means that this index is the original index of a Part,
	/// (before sorting).
	Pt3D *m_aPositions[2];

	/// Number of weights in the function which evaluates distance.
	static const int iNOFactors;

};

#endif
