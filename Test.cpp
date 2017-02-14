
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
//	ret->setWitnessTest((bool(*)(const Matrix*))&test_witness);
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
		
		cout << endl << endl << "The input automaton is a probabilistic automaton. Stamina will construct the Markov Monoid to check whether it has value 1." << endl << endl ;
    	
		UnstableMarkovMonoid* m = toMarkovMonoid(expa);
		auto expr = m->ComputeMonoid();


		cout << "***************************************" << endl;
		cout << "***********MAIN RESULTS****************" << endl;
		cout << "***************************************" << endl << endl;

		pair<int, const ExtendedExpression*> r = m->maxLeakNb();

		if(r.first == 0){
			cout << "No leak found: the automaton is leaktight." << endl;
			if (expr) {
				cout << "The automaton has value 1; a witness is ";
				expr->print();
				cout << endl << endl;
			}
			else cout << "No value 1 witness found: the automaton does not have value 1." << endl << endl;
		}
		else{
			cout << "A leak was found: the automaton is not leaktight." << endl;
			if (expr) {
				cout << "The automaton has value 1; a witness is ";
				expr->print();
				cout << endl << endl;
			}
			else cout << "No value 1 witness found: the automaton may or may not have value 1." << endl << endl;
		}

	
		if(verbose){
			cout << "***************************************" << endl;
			cout << "***********VERBOSE MODE****************" << endl;
			cout << "***************************************" << endl << endl;

			m->print();
			cout << endl;
		}
		if(toOut)
		  ofs << Dot::toDot(expa,m, -1);
	}
	else if (expa->type==CLASSICAL)
	{

		cout << endl << endl << "The input automaton is a classical automaton. Stamina will compute its star height." << endl ;
		cout << "It first finds an upper bound using the Loop Complexity heuristics, and then proceeds with the star height computation." << endl << endl ;

		ClassicAut* aut = fromExplicitToClassic(expa);

		cout << "************LOOP COMPLEXITY******************" << endl << endl;
		pair<char,list<uint>> res = LoopComplexity(aut);
		int LC = (int)res.first ;
		list<uint> order = res.second;
		RegExp* regexpr = Aut2RegExp(aut,order);
		
		cout << "According to the Loop Complexity heuristics, the star-height is at most " << LC << "." << endl;
		cout << "A regular expression for the language (omitting a finite number of words) is:  "<<endl;
		regexpr->print();
		cout << endl;
		
		list<ExtendedExpression*> sharplist = Reg2Sharps(regexpr);
		
		if(sharplist.size()==0)// empty language
			{
			cout <<"The language is empty, the star height is 0."<<endl;
			ofs.close();
			return 0;
			}
		cout << "The Loop Complexity suggests the following unboundedness witnesses:   "<<endl;
		
		for(ExtendedExpression *sharp_expr: sharplist){
			cout << *sharp_expr << endl;
		}
		
		cout << "************STAR HEIGHT COMPUTATION**********" << endl;
		int h = 0;
		while (h<LC){
//			ofstream output("monoid " + to_string(h) + ".txt");
		
			cout << endl << "******************************" << endl;
			cout << "Testing star height " << h << endl;
			cout << "******************************" << endl;
	
    		if(verbose) cout << "First step: computing the automaton with counters." << endl << endl;
			MultiCounterAut *Baut = toNestedBaut(aut, h);
		
			if(verbose) cout << "Second step: checking whether the Loop Complexity suggestions are unboundedness witnesses." << endl;
		
			UnstableMultiMonoid monoid(*Baut);
			
			bool witness_found=false;
			for(ExtendedExpression *sharp_expr: sharplist){
				const Matrix* mat = monoid.ExtendedExpression2Matrix(sharp_expr,*Baut);
				if (verbose) cout <<"."<<flush; 
				//if(verbose) {mat->print();cout<<endl<<endl;}
				if(monoid.IsUnlimitedWitness(mat)){
					if(verbose) cout << "--> The heuristic found a witness, the star height is not " << h << ", it is larger." << endl;
					witness_found=true;
					break;
				}
			}
			if(!witness_found){
				if(verbose){
					cout << "-->Heuristic found no witness." << endl << endl;
					cout << "Third step: computing the monoid, and checking for the existence of an unboundedness witness on the fly." << endl << endl;
				}
				
				const ExtendedExpression * expr = monoid.containsUnlimitedWitness();

				if(verbose) monoid.print_summary();

				delete Baut;

				if (expr){
					if(verbose){
						cout << "An unlimited witness is ";
						expr->print();
						cout << endl;
					}
				}
				else{
					if(verbose) cout << "The automaton is limited." << endl;

					cout << "RESULTS: the star height is " << h << "." << endl;
					break;
				}
			}
			if(toOut)
			  ofs << Dot::toDot(expa, &monoid, h);
			h++;
		}
		if(h==LC){
			cout << endl << "RESULTS: the star height is " << LC << ", as predicted by the Loop Complexity heuristics, and a regular expression witnessing it is ";
			regexpr->print();
			cout << "." << endl;
		}
	}
	ofs.close();
	return 0;
}
