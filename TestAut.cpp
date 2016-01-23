
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
	//3 letters 3 states
	ClassicAut *aut=new ClassicAut(3,4);
	
	aut->trans[0][0][1]=true;
	aut->trans[1][1][0]=true;
	aut->trans[2][0][2]=true;
	aut->trans[2][2][0]=true;
	
	aut->initialstate[0]=true;
	aut->finalstate[0]=true;
	
	printf("Automaton created\n");
	
	MultiCounterAut *Baut=toNestedBaut(aut, 1);
	
//	Baut->print();

	UnstableMultiMonoid monoid(*Baut);

//	monoid.print_letters();

	auto expr = monoid.containsUnlimitedWitness();
	if (expr)
	{
		cout << "An unlimited witness: " << endl;
		cout << *expr << endl;
	}
	else
	{
		cout << " The automaton is limited " << endl;
	}
	monoid.print_summary();
	//Baut->print();
	
    std::cout << "End of Computation\n";
    std::cin.get();
    
    return 0; 
}
