#include <iostream>

#include "MarkovMonoid.hpp"
#include "StabilisationMonoid.hpp"

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
	mata.coefficients[0] = BOT;
	mata.coefficients[1] = EPS;
	mata.coefficients[2] = INC;
	mata.coefficients[3] = INC;
	mata.coefficients[4] = EPS;
	mata.coefficients[5] = BOT;
	mata.coefficients[6] = BOT;
	mata.coefficients[7] = BOT;
	mata.coefficients[8] = RESET;

	ExplicitMatrix matb(3);
	matb.coefficients[0] = RESET;
	matb.coefficients[1] = INC;
	matb.coefficients[2] = RESET;
	matb.coefficients[3] = BOT;
	matb.coefficients[4] = EPS;
	matb.coefficients[5] = INC;
	matb.coefficients[6] = RESET;
	matb.coefficients[7] = BOT;
	matb.coefficients[8] = INC;
	

	/*
 	ExplicitMatrix mata(2);
	mata.coefficients[0] = 2;
	mata.coefficients[1] = 5;
	mata.coefficients[2] = 5;
	mata.coefficients[3] = 2;

	ExplicitMatrix matb(2);
	matb.coefficients[0] = 5;
	matb.coefficients[1] = 1;
	matb.coefficients[2] = 5;
	matb.coefficients[3] = 1;

	*/
	OneCounterMatrix a(mata);
    OneCounterMatrix b(matb);


//ExplicitMatrix mata(1);
//mata.coefficients[0] = RESET;
//
//ExplicitMatrix matb(1);
//matb.coefficients[0] =EPS;
//
//ExplicitMatrix matc(1);
//matc.coefficients[0] = INC;
//
//ExplicitMatrix matd(1);
//matd.coefficients[0] =OM;
//

//OneCounterMatrix a(mata);
// OneCounterMatrix b(matb);
//OneCounterMatrix c(matc);
// OneCounterMatrix d(matd);

	//a.print();
	//cout<<"\n";
	//b.print();
	
	UnstableStabMonoid monoid(3);
	
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


	ProbMatrix a(mata);
    ProbMatrix b(matb);


	UnstableMarkovMonoid monoid(2);

	*/
	a.print();
	cout << "\n";
	b.print();

	
	
 monoid.addLetter('a', mata);
	monoid.addLetter('b', matb);
 //   monoid.addLetter('c', matc);
//	monoid.addLetter('d', matd);

	monoid.ComputeMonoid();
	
	cout << monoid.expr_to_mat.size() << " elements." << endl;
	cout << monoid.rewriteRules.size() << " rewrite rules." << endl;

	monoid.print() ;
	system("pause");
}
