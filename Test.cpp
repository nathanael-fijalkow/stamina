#include <iostream>

#include "MarkovMonoid.hpp"
#include "ExplicitAutomaton.hpp"
#include "Output.hpp"
//#include "StabilisationMonoid.hpp"
#include "MultiMonoid.hpp"
#include "Parser.hpp"
#include "StarHeight.hpp"

#include <fstream>
#include <sstream>

#include <math.h>

#ifdef MSVC
#include <windows.h>
#include <time.h>
#else
#include <unistd.h>
#endif

using namespace std;

void pusage(char* s)
{
	cerr << "Usage: " << s << " [-v] file" << endl;
	exit(-1);
}

int *finalStates;
int initialState;
int size; 

bool not_final(int s) 
{
	for(int i=0;i<size;i++)
		if(finalStates[i]==s)
			return false;
	return true;
}

bool test_witness(const ProbMatrix* m) 
{
	const Vector & ones = *((m->getRowOnes())[initialState]);
	for (int i = 0; i < Vector::GetStateNb(); i++)
		if(ones.contains(i)&&not_final(i))
			return false;
	return true;
}

Monoid* toMonoid(ExplicitAutomaton* aut) 
{
	if(aut->type==PROB) 
	{
		finalStates = aut->finalStates;
		initialState = aut->initialState;
		size = aut->size;
		UnstableMarkovMonoid* ret = new UnstableMarkovMonoid(aut->size);
		ret->setWitnessTest((bool(*)(const Matrix*))&test_witness);
		for(int i=0;i<aut->alphabet.length();i++)
			ret->addLetter(aut->alphabet[i],*(aut->matrices[i]));
		return ret;
	}
	if (aut->type==CLASSICAL) 
	{
		ClassicAut* a = new ClassicAut(aut->size,aut->alphabet.length());

		for(int i=0;i<aut->alphabet.length();i++)
			for(int j=0;i<aut->size;i++)
				for(int k=0;k<aut->size;k++)
					if(aut->matrices[i]->coefficients[j*aut->size+k]>0)
						a->trans[i][j][k]=true;

		a->initialstate[aut->initialState]=true;
		for(int i=0;aut->finalStates[i]!=-1 && i<aut->size;i++)
			a->finalstate[aut->finalStates[i]]=true;

		MultiCounterAut *Baut=toNestedBaut(a, 1);
		UnstableMultiMonoid* monoid = new UnstableMultiMonoid(*Baut);

		return monoid;
	}
	if (aut->type >= 1) 
	{
		// Do something with automata with counters
	}
	return NULL;
  
}

int main(int argc, char **argv)
{
	ExplicitMatrix mata(1);
	mata.coefficients[0] = INC;
	UnstableMultiMonoid monoid(1,1);
	monoid.addLetter('a',mata);
	monoid.ComputeMonoid();
	


	//cout << "Acme++ rules" << endl;
	
	// ExplicitMatrix mata(3);
	// mata.coefficients[0] = 0;
	// mata.coefficients[1] = 1;
	// mata.coefficients[2] = 2;
	// mata.coefficients[3] = 3;
	// mata.coefficients[4] = 2;
	// mata.coefficients[5] = 1;
	// mata.coefficients[6] = 2;
	// mata.coefficients[7] = 1;
	// mata.coefficients[8] = 0;

	// ExplicitMatrix matb(3);
	// matb.coefficients[0] = 1;
	// matb.coefficients[1] = 2;
	// matb.coefficients[2] = 6;
	// matb.coefficients[3] = 6;
	// matb.coefficients[4] = 6;
	// matb.coefficients[5] = 4;
	// matb.coefficients[6] = 0;
	// matb.coefficients[7] = 5;
	// matb.coefficients[8] = 1;
	
	// string amat = "";
	// amat += "___I__E";
	// amat += "__R____";
	// amat += "_I__E__";
	// amat += "_____E_";
	// amat += "_I_____";
	// amat += "_I__E__";
	// amat += "E_R____";


	// string bmat = "";
	// bmat += "__E____";
	// bmat += "__E__I_";
	// bmat += "____I__";
	// bmat += "__I____";
	// bmat += "_I_____";
	// bmat += "E_____I";
	// bmat += "_______";

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

// VectorInt::SetSize(3);
// MultiCounterMatrix a(mata,2);
// MultiCounterMatrix b(matb,2);
//OneCounterMatrix c(matc);
// OneCounterMatrix d(matd);

	// a.print();
	// cout<<"\n";
	// b.print();
		
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


	// 	UnstableMultiMonoid monoid(3,2);

	// monoid.addLetter('a', mata);
	// monoid.addLetter('b', matb);
//	monoid.addLetter('c', matc);
	//   monoid.addLetter('c', matc);
//	monoid.addLetter('d', matd);

	// monoid.ComputeMonoid();
	
	// cout << monoid.expr_to_mat.size() << " elements." << endl;
	// cout << monoid.rewriteRules.size() << " rewrite rules." << endl;
	
//	mats.clear();
//	cmats.clear();

	// monoid.print() ;

	int opt,verbose=0,toOut=0;
	ifstream ifs;
	ofstream ofs; 
	string outputFilename;
#ifndef WIN32
	while((opt = getopt(argc,argv, "vho:")) != -1) 
	{
		switch(opt) 
		{
		case 'h':
			pusage(argv[0]);
		case 'v':
			verbose = 1;
			break;
		case 'o':
			toOut=1;
			outputFilename = optarg;
			break;
		default:
			pusage(argv[0]);
		}
	}
	if (optind >= argc)
	{
		cerr << "Expected file" << endl;
		pusage(argv[0]);
	}
	
	ifs.open(argv[optind]);
	if(ifs.fail())
	{
		cerr << "Could not open file " << argv[optind] << endl;
		pusage(argv[0]);
	}
	
	if(toOut)
	{
		ofs.open(outputFilename);
		if(ofs.fail())
		{
			cerr << "Could not open file " << outputFilename << endl;
			pusage(argv[0]);
		}
	}
#endif

	ExplicitAutomaton* expa = Parser::parseFile(ifs);
	if(expa->type==PROB)
	{
		UnstableMarkovMonoid* m = dynamic_cast<UnstableMarkovMonoid*>(toMonoid(expa));
		auto expr = m->ComputeMonoid();
	  
		pair<int, const ExtendedExpression*> r = m->maxLeakNb();
		cout << r.first << " leak(s) found" << endl;
		cout << "The monoid has " << m->expr_to_mat.size() << " elements" << endl;
		cout << "The automaton has value 1: ";
		if (expr)
			cout << "Yes" << endl;
		else
			cout << "No" << endl;
		if(verbose)
			m->print();
		if(toOut)
			ofs << Dot::toDot(expa,m);
		
	}
	else if (expa->type==CLASSICAL)
	{
		UnstableMultiMonoid* m = dynamic_cast<UnstableMultiMonoid*>(toMonoid(expa));
		auto expr = m->containsUnlimitedWitness();
		if (expr) 
		{
			cout << "An unlimited witness: " << endl;
			cout << *expr << endl;
		}
		else
		{
			cout << " The automaton is limited " << endl;
		}
		if(verbose)
			m->print();
	}
	// system("pause");
	ifs.close();
}
