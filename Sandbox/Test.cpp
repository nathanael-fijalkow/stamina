#include <iostream>

#include "Expressions.hpp"
#include "Matrix.hpp"
#include "Monoid.hpp"

#include <fstream>
#include <sstream>

#ifdef MSVC
#include <windows.h>
#include <time.h>
#endif

using namespace std;


int main(int argc, char **argv)
{
	cout << "Acme++ rules" << endl;

	ExplicitMatrix mata(3);
	mata.coefficients[0] = 2;
	mata.coefficients[1] = 0;
	mata.coefficients[2] = 0;
	mata.coefficients[3] = 2;
	mata.coefficients[4] = 2;
	mata.coefficients[5] = 0;
	mata.coefficients[6] = 0;
	mata.coefficients[7] = 0;
	mata.coefficients[8] = 2;

	ExplicitMatrix matb(3);
	matb.coefficients[0] = 0;
	matb.coefficients[1] = 2;
	matb.coefficients[2] = 0;
	matb.coefficients[3] = 0;
	matb.coefficients[4] = 0;
	matb.coefficients[5] = 2;
	matb.coefficients[6] = 0;
	matb.coefficients[7] = 0;
	matb.coefficients[8] = 2;

	Matrix a(mata);
    Matrix b(matb);

	UnstableMonoid monoid(3);

/*
 	ExplicitMatrix mata(2);
	mata.coefficients[0] = 2;
	mata.coefficients[1] = 2;
	mata.coefficients[2] = 0;
	mata.coefficients[3] = 2;

	ExplicitMatrix matb(2);
	matb.coefficients[0] = 2;
	matb.coefficients[1] = 0;
	matb.coefficients[2] = 0;
	matb.coefficients[3] = 2;

	Matrix a(mata);
    Matrix b(matb);

	UnstableMonoid monoid(2);
*/

	monoid.addLetter('a', mata);
	monoid.addLetter('b', matb);

	monoid.ComputeMonoid();
	
	cout << monoid.expr_to_mat.size() << " elements." << endl;
	cout << monoid.rewriteRules.size() << " rewrite rules." << endl;

	monoid.print() ;
}
