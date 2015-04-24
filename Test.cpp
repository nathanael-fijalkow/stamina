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
	cerr << "Usage: " << s << " [-v] [-o dot_output_file]  input_file" << endl;
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

UnstableMarkovMonoid* toMarkovMonoid(ExplicitAutomaton* aut)
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

ClassicAut* fromExplicitToClassic(ExplicitAutomaton* aut)
{
	ClassicAut* ret = new ClassicAut(aut->alphabet.length(),aut->size);

	for(int i=0;i<aut->alphabet.length();i++)
		ret->addLetter(i,*(aut->matrices[i]));

	ret->initialstate[aut->initialState]=true;
	for(int i=0;aut->finalStates[i]!=-1 && i<aut->size;i++)
		ret->finalstate[aut->finalStates[i]]=true;
	return ret;
}

int main(int argc, char **argv)
{
	// ExplicitMatrix mata(1);
	// mata.coefficients[0][0] = INC;
	// UnstableMultiMonoid monoid(1,1);
	// monoid.addLetter('a',mata);
	// monoid.ComputeMonoid();
	


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
	ifs.close();

	if(expa->type==PROB)
	{
		UnstableMarkovMonoid* m = toMarkovMonoid(expa);
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
		int height = 0;
		ClassicAut* aut = fromExplicitToClassic(expa);

		pair<char,list<uint>> res = LoopComplexity(aut);
		int lc = (int) res.first;
		list<uint> order = res.second;
		const RegExp* regexpr = Aut2RegExp(aut,order);
		if(!regexpr) {
		  cout << "This automaton does not accept any words." << endl;
		  cout << "This automaton has star-height: 0" << endl;
		  exit(0);
		}
		  
		const ExtendedExpression* sharp_expr = Reg2Sharp(regexpr);
		cout << "Automaton with regexp: ";
		regexpr->print();
		cout << endl;
		cout << "And loop complexity: " << lc << endl;
		while(height < lc) {
			cout << "Checking for height: " << height << endl;
			MultiCounterAut* baut = toNestedBaut(aut, height);
			UnstableMultiMonoid monoid(*baut);
			const Matrix* mat = monoid.ExtendedExpression2Matrix(sharp_expr, *baut);
			if(!monoid.IsUnlimitedWitness(mat) && 
			   !monoid.containsUnlimitedWitness())  
				break;
			if(monoid.IsUnlimitedWitness(mat))
			  cout << "We guessed an unlimited witness" << endl;
			else
			  cout << "The guess was not good, but we found an unlimited witness" << endl;
			if(verbose)
				monoid.print();
			delete baut;
			height++;
		}
		cout << "This automaton has star-height: " << height << endl;
		if(height == lc)
		  cout << "And it is optimal (loop complexity is equal to the star height)" << endl;
	}
	ofs.close();
	// system("pause");
}
