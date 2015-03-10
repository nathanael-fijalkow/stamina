#include <iostream>

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
	/*
	//3 letters 3 states
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

	//(b*ab*a)*
	ClassicAut *aut=new ClassicAut(2,2);

	aut->trans[0][0][1]=true;
	aut->trans[0][1][0]=true;
	aut->trans[1][0][0] = true;
	aut->trans[1][1][1] = true;

	aut->initialstate[0]=true;
	aut->finalstate[0]=true;

	
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

	//1 letters 1 state
	/*
	ClassicAut *aut=new ClassicAut(1,1);

	aut->trans[0][0][0] = true;

	aut->initialstate[0]=true;
	aut->finalstate[0]=true;
	*/

	printf("Automaton created\n");
	
	aut->print();


	int h = 0;
	while (true)
	{
		ofstream output("monoid " + to_string(h) + ".txt");

		cout << "******************************" << endl;
		cout << "Testing starheight " << h << endl;
		cout << "*****************************************" << endl;

		cout << "Computing the nested automaton..." << endl;
		MultiCounterAut *Baut = toNestedBaut(aut, h);
		
		/*
		cout << "The nested automaton " << endl;
		Baut->print();
		*/
		UnstableMultiMonoid monoid(*Baut);

		cout << "Checking witness existence on the fly..." << endl;
		auto expr = monoid.containsUnlimitedWitness();
		monoid.print_summary();
		if (monoid.expr_to_mat.size() < 5000)
			output << monoid;

		delete Baut;

		if (expr)
		{
			cout << "An unlimited witness for h=" << h << endl;
			cout << *expr << endl;
		}
		else
		{
			cout << " The automaton is limited, sharpheight is " << h << endl;
			break;
		}
		h++;
	}
    std::cout << "End of Computation\n";
    std::cin.get();
    
    return 0; 
}
