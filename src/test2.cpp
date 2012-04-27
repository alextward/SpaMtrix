#include <ircmatrix.h>
#include <vector.h>
#include <matrixmaker.h>
#include <spamtrix_blas.h>
#include <pcg.h>
#include <cg.h>
#include <iostream>
// TEST 2. SOLVES 1D POISSON FINITE DIFFERENCES PROBLEM
using std::cout;
using std::endl;
int main(int nargs, char* args[])
{
  // CONSTRUCT 1D FINITE DIFFERENCES MATRIX
  unsigned int np = 10;
  
  if (nargs > 1)
    np = atoi( args[1] );
  
  cout << "creating sparse matrix of size:" << np <<"x" <<np<<"...";
  MatrixMaker mm;
  mm.setMatrixSize(np, np);
  for (unsigned int i = 0 ; i < np ; i++)
  {
    mm.addNonZero(i,i); // ALWAYS ADD DIAGONAL ENTRY
    if ( i > 0)
      mm.addNonZero(i,i-1); // IF NOT ROW 0 
    if (i < np-1)
      mm.addNonZero(i,i+1);	// IF NOT LAST ROW
  }
  IRCMatrix A = mm.getIRCMatrix();
  mm.clear();
  cout << "OK" << endl;
  //A.spy();

  // FILL IN MATRIX VALUES
  //		    | 2  -1     |
  // A = 1/(h^2) *  | -1  2  -1 |
  //		    |    -1   2 |		  
  //
  // USING h = 1:
  cout <<"filling in values...";
  for (unsigned int i = 0 ; i < np ; i++ )
  {
    A.sparse_set(i,i, 2.0);	// SET DIAGONAL
    if ( i > 0 )
      A.sparse_set(i, i-1, -1.0);
    if ( i < np - 1)
      A.sparse_set(i , i+1 , -1.0);
  }
  cout << "OK" << endl;
  //A.print(); 
  
  // CREATE UNKNOWN VECTOR x, WITH FIXED VALUES 1, -1 AT BOTH ENDS
  Vector x(np);
  x[0] = 1.0;
  x[np-1] = -1.0;
 
  
  // R.H.S VECTOR b
  Vector b(np);
  
  // APPLY BOUNDARY CONDITIONS
  multiply(A,x,b); 	// b = Ax;
  multiply(b, -1.0, b); // WANT TO SOLVE Ax = -b, SO PREMULTIPLY BY -1 HERE
  // FIRST NODE
  A.sparse_set(0, 0, 1.0); 
  A.sparse_set(0, 1, 0.0); 
  A.sparse_set(1, 0, 0.0);
  // LAST NODE
  A.sparse_set(np-1, np-1, 1.0);
  A.sparse_set(np-2, np-1, 0.0);
  A.sparse_set(np-1, np-2, 0.0);
  //A.print();
  
  // CREATE PRECONDITIONER
  DiagPreconditioner D(A);
 
  // SOLVE Ax=b
  idx maxIter = np;
  real toler(1e-7);
  cout << "solving pcg..."; fflush(stdout);
  bool conv = pcg(A, b, x, D, maxIter, toler);
  //bool conv = cg(A,b,x,maxIter,toler);
  cout << "OK" << endl;
  // WRITE BACK FIXED NODES TO x
  x[0] = 1.0;
  x[np-1] = -1.0;
  
  
  
  //x.print();
  cout << "CONVERGENCE FOR MATRIX SIZE " << np << " = " ;
  if (conv)
    cout <<"OK"<<endl;
  else
    cout << "NO!!"<<endl;
  
  cout << "iterations used: " << maxIter << endl;
  cout << "Tolerance achieved: " << toler << endl;
  
  return 0;
}