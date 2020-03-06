#ifndef _LAPACK_H_
#define	_LAPACK_H_

class Lapack
{
public:
	static void JacobiSVD(double* At, size_t astep, double* W, double* Vt, size_t vstep, int m, int n, int n1);
};

#endif
