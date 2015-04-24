#include <iostream>
#include <list>
#include "MarkovMonoid.hpp"
//#include "StabilisationMonoid.hpp"
#include "MultiMonoid.hpp"
#include "Parser.hpp"

#include <fstream>
#include <sstream>

#include <math.h>

#ifdef MSVC
#include <windows.h>
#include <time.h>
#else
#include <unistd.h>
#endif

#include "StarHeight.hpp"

using namespace std;

void pusage(char* s)
{
  cerr << "Usage: " << s << " [-v] file" << endl;
  exit(-1);
}

int main(int argc, char **argv)
{
	
	//3 letters 3 states
	//
	/*
	ClassicAut *aut=new ClassicAut(3,4);
	
	aut->trans[0][0][1]=true;
	aut->trans[1][1][0]=true;
	aut->trans[2][0][2]=true;
	aut->trans[2][2][0]=true;
	
	aut->initialstate[0]=true;
	aut->finalstate[0]=true;
	*/

	/*
	//(aa)*
	ClassicAut *aut=new ClassicAut(1,2);

	aut->trans[0][0][1]=true;
	aut->trans[0][1][0]=true;

	aut->initialstate[0]=true;
	aut->finalstate[0]=true;
	*/


	//(a + b(a+b)^2)^*
	/*
	ClassicAut *aut=new ClassicAut(2,3);
	aut->trans[0][0][1]=true;
	aut->trans[0][1][1] = true;
	aut->trans[0][2][0] = true;
	
	aut->trans[1][0][1]=true;
	aut->trans[1][1][2]=true;
	aut->trans[1][2][0] = true;	

	aut->initialstate[0]=true;
	aut->finalstate[0]=true;
	*/
	
	//(a + bb)^*
	/*
	ClassicAut *aut=new ClassicAut(2,2);
	aut->trans[0][0][0]=true;	
	aut->trans[1][0][1]=true;
	aut->trans[1][1][0]=true;

	aut->initialstate[0]=true;
	aut->finalstate[0]=true;
	*/
	
	//b*(b*ab*a)*	
	/*
	ClassicAut *aut=new ClassicAut(2,2);

	aut->trans[0][0][1]=true;
	aut->trans[0][1][0]=true;
	aut->trans[1][0][0] = true;
	aut->trans[1][1][1] = true;

	aut->initialstate[0]=true;
	aut->finalstate[0]=true;
	*/

	//(a*b*c)*

	/*
	ClassicAut *aut=new ClassicAut(3,4);

	aut->trans[0][0][1]= true;
	aut->trans[0][1][1] = true;
	aut->trans[0][2][3] = true;
	aut->trans[0][3][3] = true;

	aut->trans[1][0][2] = true;
	aut->trans[1][1][2] = true;
	aut->trans[1][2][2] = true;
	aut->trans[1][3][3] = true;

	aut->trans[2][0][0] = true;
	aut->trans[2][1][0] = true;
	aut->trans[2][2][0] = true;
	aut->trans[2][3][3] = true;

	aut->initialstate[0]=true;
	aut->finalstate[0]=true;
	*/

	//(aa(ab)*bb(ab*))*
	
	//2 letters 3 states
	/*
	ClassicAut *aut=new ClassicAut(2,3);

	aut->trans[0][0][0] = true;
	aut->trans[0][1][2] = true;
	aut->trans[0][2][2] = true;

	aut->trans[1][0][1] = true;
	aut->trans[1][1][1] = true;
	aut->trans[1][2][0] = true;

	aut->initialstate[0]=true;
	aut->finalstate[0]=true;
	*/

	//2 letters 3 states
	/*
	ClassicAut *aut=new ClassicAut(2,3);

	aut->trans[0][0][2] = true;
	aut->trans[0][1][0] = true;
	aut->trans[0][2][1] = true;

	aut->trans[1][0][1] = true;
	aut->trans[1][1][1] = true;
	aut->trans[1][2][1] = true;

	aut->initialstate[0]=true;
	aut->finalstate[0]=true;
	*/
	
	//2 letters 3 states
	ClassicAut *aut=new ClassicAut(2,3);

	aut->trans[0][0][1] = true;
	aut->trans[0][1][2] = true;
	aut->trans[0][2][0] = true;

	aut->trans[1][0][2] = true;
	aut->trans[1][1][1] = true;
	aut->trans[1][2][2] = true;

	aut->initialstate[2]=true;
	aut->finalstate[1]=true;

	//1 letters 1 state
	/*
	ClassicAut *aut=new ClassicAut(1,1);

	aut->trans[0][0][0] = true;

	aut->initialstate[0]=true;
	aut->finalstate[0]=true;
	*/

	printf("Automaton created\n");
	
	aut->print();

	cout << "******************************" << endl;
	pair<char,list<uint>> res = LoopComplexity(aut);
	int LC = (int)res.first ;
	list<uint> order = res.second;
	const RegExp* regexpr = Aut2RegExp(aut,order);
	const ExtendedExpression* sharp_expr = Reg2Sharp(regexpr);
	cout << "Loop Complexity : Star-height is at most " << LC << endl;
	cout << "Regex:" << endl;
	regexpr->print();
	cout << endl;
	cout << "The Loop Complexity produces the following potential unboundedness witness:" << endl;
	cout << *sharp_expr << endl;
		
	int h = 0;
	while (h<LC)
	{
		ofstream output("monoid " + to_string(h) + ".txt");
	
		cout << "******************************" << endl;
		cout << "Testing starheight " << h << endl;
		cout << "*****************************************" << endl;
	
		cout << "Computing the nested automaton..." << endl;
		MultiCounterAut *Baut = toNestedBaut(aut, h);
		
//		cout << "The nested automaton " << endl;
//		Baut->print();

		cout << "Before computing the monoid, we check whether the Loop Complexity induces an unboundedness witness:" << endl;
		
		UnstableMultiMonoid monoid(*Baut);
		const Matrix* mat = monoid.ExtendedExpression2Matrix(sharp_expr,*Baut);

//		cout << *mat << endl;
		
		if(monoid.IsUnlimitedWitness(mat)){
			cout << "It does, proceed" << endl;			
		}
		else{
			cout << "It does not, we compute the monoid" << endl;
			cout << "Computing the monoid, and checking for the existence of an unboundedness witness on the fly..." << endl;
			auto expr = monoid.containsUnlimitedWitness();
			monoid.print_summary();
			if (monoid.expr_to_mat.size() < 5000)
				output << monoid;
			delete Baut;

			if (expr)
			{
				cout << "An unlimited witness for h = " << h << endl;
				cout << *expr << endl;
			}
			else
			{
			cout << "The automaton is limited, star-height is " << h << endl;
			break;
			}
			//cout << "Press key to continue..." << endl;
			//std::cin.get();
		}
		h++;
	}
	
	if(h==LC) {
		cout << "*********************************" << endl;
		cout << "Star-height is " << LC << endl;
		cout << "Witnessed by standard automaton->expression construction" << endl;
		cout << "*****************************************" << endl;
	}

    std::cout << "End of Computation\n";
	std::cin.get();

    return 0; 
}
