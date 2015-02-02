#include <iostream>

//#include "MarkovMonoid.hpp"
#include "StabilisationMonoid.hpp"
#include "MultiMonoid.hpp"

#include <fstream>
#include <sstream>

#include <math.h>

#ifdef MSVC
#include <windows.h>
#include <time.h>
#endif

using namespace std;


int main(int argc, char **argv)
{
	cout << "Acme++ rules" << endl;
	
	ExplicitMatrix mata(3);
	mata.coefficients[0] = 0;
	mata.coefficients[1] = 1;
	mata.coefficients[2] = 2;
	mata.coefficients[3] = 3;
	mata.coefficients[4] = 2;
	mata.coefficients[5] = 1;
	mata.coefficients[6] = 2;
	mata.coefficients[7] = 1;
	mata.coefficients[8] = 0;

	ExplicitMatrix matb(3);
	matb.coefficients[0] = 1;
	matb.coefficients[1] = 2;
	matb.coefficients[2] = 6;
	matb.coefficients[3] = 6;
	matb.coefficients[4] = 6;
	matb.coefficients[5] = 4;
	matb.coefficients[6] = 0;
	matb.coefficients[7] = 5;
	matb.coefficients[8] = 1;
	
	string amat = "";
	amat += "___I__E";
	amat += "__R____";
	amat += "_I__E__";
	amat += "_____E_";
	amat += "_I_____";
	amat += "_I__E__";
	amat += "E_R____";


	string bmat = "";
	bmat += "__E____";
	bmat += "__E__I_";
	bmat += "____I__";
	bmat += "__I____";
	bmat += "_I_____";
	bmat += "E_____I";
	bmat += "_______";

	/*
	string cmat = "";
	cmat += "_R_____";
	cmat += "___R___";
	cmat += "__E__E_";
	cmat += "_______";
	cmat += "____I__";
	cmat += "__II___";
	cmat += "R____RR";
	*/

	/*
	string amat = "";
	amat += "___I__E___I__E";
	amat += "__R_____I__E__";
	amat += "_I__E____R____";
	amat += "_____E__I_____";
	amat += "_I__E___I_____";
	amat += "_____E__I__E__";
	amat += "___I__EE_R____";
	amat += "___I__E___I__E";
	amat += "__R_____I__E__";
	amat += "_I__E____R____";
	amat += "_____E__I_____";
	amat += "_I__E___I_____";
	amat += "_____E__I__E__";
	amat += "___I__EE_R____";


	string bmat = "";
	bmat += "__E__I___E____";
	bmat += "____I____E__I_";
	bmat += "__I________I__";
	bmat += "_I_______I____";
	bmat += "E_____I_I_____";
	bmat += "__I________I__";
	bmat += "______________";
	bmat += "__E__I___E____";
	bmat += "____I____E__I_";
	bmat += "__I________I__";
	bmat += "_I_______I____";
	bmat += "E_____I_I_____";
	bmat += "__I________I__";
	bmat += "______________";
	*/

/*
	int_vector<ExplicitMatrix> mats;
	mats.emplace_back( sqrt(amat.size()) );
	mats.emplace_back( sqrt(amat.size()) );
	//mats.push_back( sqrt(amat.size()) );

	vector<string> cmats;
	cmats.push_back(amat);
	cmats.push_back(bmat);
	//cmats.push_back(cmat);

	for (int i = 0; i < mats.size(); i++)
	{
		auto & mat = mats[i];
		auto & cmat = cmats[i];
		for (int i = 0; i< mat.stateNb * mat.stateNb; i++)
			mat.coefficients[i] =
			(cmat[i] == '_') ? BOT :
			(cmat[i] == 'I') ? INC :
			(cmat[i] == 'E') ? EPS :
			(cmat[i] == 'R') ? RESET :
			BOT;
	}

	/*
	ExplicitMatrix mata(3);
	mata.coefficients = new char[64] { BOT, EPS };
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
	*/
/*
	
 	ExplicitMatrix mata(2);
	mata.coefficients[0] = INC;
	mata.coefficients[1] = BOT;
	mata.coefficients[2] = BOT;
	mata.coefficients[3] = INC;

	ExplicitMatrix matb(2);
	matb.coefficients[0] = BOT;
	matb.coefficients[1] = EPS;
	matb.coefficients[2] = BOT;
	matb.coefficients[3] = E;
	*/
	
	/*
	OneCounterMatrix a(mata);
    OneCounterMatrix b(matb);
	OneCounterMatrix c(matc);
	*/

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

MultiCounterMatrix a(mata,2);
MultiCounterMatrix b(matb,2);
//OneCounterMatrix c(matc);
// OneCounterMatrix d(matd);

	a.print();
	cout<<"\n";
	b.print();
		
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

/*	
	UnstableStabMonoid monoid(mats[0].stateNb);

 monoid.addLetter('a', mats[0]);
 monoid.addLetter('b', mats[1]);
//	monoid.addLetter('c', matc);
	//   monoid.addLetter('c', matc);
//	monoid.addLetter('d', matd);

	monoid.ComputeMonoid();
	
	cout << monoid.expr_to_mat.size() << " elements." << endl;
	cout << monoid.rewriteRules.size() << " rewrite rules." << endl;
	
	mats.clear();
	cmats.clear();

	//monoid.print() ;
	system("pause");
*/
}
