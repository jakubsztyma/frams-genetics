// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.


#include "matrix_tools.h"
#include "lapack.h"
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <stdlib.h> //malloc(), embarcadero
#include <math.h> //sqrt(), embarcadero


double *Create(int nSize)
{
	double *matrix = new double[nSize];

	for (int i = 0; i < nSize; i++)
	{
		matrix[i] = 0;
	}

	return matrix;
}

double *Multiply(double *&a, double *&b, int nrow, int ncol, double ncol2, double *&toDel, int delSize)
{
	double *c = Create(nrow * ncol2);
	int i = 0, j = 0, k = 0;

	for (i = 0; i < nrow; i++)
	{
		for (j = 0; j < ncol2; j++)
		{
			for (k = 0; k < ncol; k++)
				c[i * nrow + j] += a[i * nrow + k] * b[k * ncol + j];
		}
	}

	if (delSize != 0)
		delete[] toDel;
	return c;
}

double *Power(double *&array, int nrow, int ncol, double pow, double *&toDel, int delSize)
{
	double *m_Power = Create(nrow * ncol);
	if (pow == 2)
	{
		for (int i = 0; i < nrow; i++)
		{
			for (int j = 0; j < ncol; j++)
			{
				m_Power[i * nrow + j] = array[i * nrow + j] * array[i * nrow + j];
			}

		}
	}
	else
	{
		for (int i = 0; i < nrow; i++)
		{
			for (int j = 0; j < ncol; j++)
			{
				m_Power[i * nrow + j] = sqrt(array[i * nrow + j]);
			}

		}
	}

	if (delSize != 0)
		delete[] toDel;

	return m_Power;
}

void Print(double *&mat, int nelems)
{
	for (int i = 0; i < nelems; i++)
		printf("%6.2f ", mat[i]);
	printf("\n");

}

double *Transpose(double *&A, int arow, int acol, double *&toDel, int delSize)
{
	double *result = Create(acol * arow);

	for (int i = 0; i < acol; i++)
		for (int j = 0; j < arow; j++)
		{
		result[i * arow + j] = A[j * acol + i];
		}

	if (delSize != 0)
		delete[] toDel;
	
	return result;
}

//Weighted centring of a matrix.
//https://github.com/vegandevs/vegan/blob/master/src/goffactor.c
void wcentre(double *x, double *w, int *nr, int *nc)
{
	int i, j, ij;
	double sw, swx;

	for (i = 0, sw = 0.0; i < (*nr); i++)
		sw += w[i];

	for (j = 0; j < (*nc) ; j++)
	{
		for (i = 0, swx = 0.0, ij = (*nr)*j; i < (*nr); i++, ij++)
		{
			swx += w[i] * x[ij];
		}
		swx /= sw;
		for (i = 0,  ij = (*nr)*j; i < (*nr); i++, ij++)
		{
			x[ij] -= swx;
			x[ij] *= sqrt(w[i]);
		}
	}
}

/** Computes the weighted MDS of the nSize x nSize distance matrix
		@param vdEigenvalues [OUT] Vector of doubles. On return holds the eigenvalues of the
		decomposed distance matrix (or rather, to be strict, of the matrix of scalar products
		created from the matrix of distances). The vector is assumed to be empty before the function call and
		all variance percentages are pushed at the end of it.
		@param nSize size of the matrix of distances.
		@param pDistances [IN] matrix of distances between parts.
		@param Coordinates [OUT] array of three dimensional coordinates obtained from SVD of pDistances matrix.
  		@param weights [IN] vector of row weights.
		*/
void MatrixTools::weightedMDS(std::vector<double> &vdEigenvalues, int nSize, double *pDistances, Pt3D *&Coordinates, double *weights)
{
	// compute the matrix D that is the parameter of SVD
	double *D = Create(nSize * nSize);
	D = Power(pDistances, nSize, nSize, 2.0, D, nSize);

	for (int i = 0; i < 2; i++)
	{
		wcentre(D, weights, &nSize, &nSize);
		D = Transpose(D, nSize, nSize, D, nSize);
	}
		
	for (int i = 0; i < nSize; i++)
		for (int j = 0; j < nSize; j++)
		{
			D[i * nSize + j] *= -0.5;
		}

	double *Eigenvalues = Create(nSize);
	double *S = Create(nSize * nSize);

	// call the SVD function
	double *Vt = Create(nSize * nSize);
	size_t astep = nSize * sizeof(double);
	Lapack::JacobiSVD(D, astep, Eigenvalues, Vt, astep, nSize, nSize, nSize);

	double *W = Transpose(Vt, nSize, nSize, W, 0);

	delete[] D;
	delete[] Vt;
	
	// deweight
	double row_weight = 1;
	for (int i = 0; i < nSize; i++)
	{
		row_weight = weights[i];
		for (int j = 0; j < nSize; j++)
		{
			W[i * nSize + j] /= sqrt(row_weight);
		}
	}

	for (int i = 0; i < nSize; i++)
		for (int j = 0; j < nSize; j++)
		{
		if (i == j)
			S[i * nSize + j] = Eigenvalues[i];
		else
			S[i * nSize + j] = 0;
		}

	// compute coordinates of points 
	double *sqS, *dCoordinates;
	sqS = Power(S, nSize, nSize, 0.5, S, nSize);
	dCoordinates = Multiply(W, sqS, nSize, nSize, nSize, W, nSize);
	delete[] sqS;

	for (int i = 0; i < nSize; i++)
	{
		// set coordinate from the SVD solution
		Coordinates[i].x = dCoordinates[i * nSize];
		Coordinates[i].y = dCoordinates[i * nSize + 1];
		if (nSize > 2)
			Coordinates[i].z = dCoordinates[i * nSize + 2];
		else
			Coordinates[i].z = 0;
	}

	// store the eigenvalues in the output vector
	for (int i = 0; i < nSize; i++)
	{
		double dElement = Eigenvalues[i];
		vdEigenvalues.push_back(dElement);
	}

	delete[] Eigenvalues;
	delete[] dCoordinates;
}