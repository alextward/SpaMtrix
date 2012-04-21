#ifndef CG_H
#define CG_H
#include <setup.h>
#include <ircmatrix.h>
#include <vector.h>

bool cg(const IRCMatrix& A,
	const Vector& b,
	Vector& x,
	idx& maxIter,
	real& toler);

#endif

