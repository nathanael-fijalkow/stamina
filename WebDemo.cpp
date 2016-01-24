#include <iostream>

#include "MarkovMonoid.hpp"
#include "ExplicitAutomaton.hpp"
#include "Output.hpp"
#include "MultiMonoid.hpp"
#include "Parser.hpp"
#include "StarHeight.hpp"

#include <fstream>
#include <sstream>

#ifdef MSVC
#include <windows.h>
#include <time.h>
#endif

#ifdef UNIX
#include <unistd.h>
#endif

using namespace std;

#define MAX_JOBS 10

vector<bool> finalStates;
int initialState;
int size;

bool test_witness(const ProbMatrix* m) 
{
  //  cout << "Call to test_witness for " << endl << *m;
  auto row = m->getRowOnes();
  /*
  cout << "Row ones " << endl;
	for (int i = 0; i < size; i++)
	  cout << row[i] << endl;
  */
  /*
  cout << initialState << endl;
	for (int i = 0; i < size; i++)
	  cout << row[initialState]->contains(i) << endl;
  */
       auto ones = row[initialState];
  

       /*	for (int i = 0; i < size; i++)
	  cout << i << ": " << finalStates[i] << " " << ones->contains(i) << endl;
       */

	for (int i = 0; i < size; i++)
	  if( !finalStates[i]  && ones->contains(i))
			return false;
	return true;
}

UnstableMarkovMonoid* toMarkovMonoid(ExplicitAutomaton* aut)
{
	initialState = aut->initialState;
	size = aut->size;
	finalStates.resize(size,false);
	for(int i = 0 ; i < size; i++)
	  {
	    auto s = aut->finalStates[i];
	    //	    cout << i << " fstate: " << s << endl;
	    if(s>= 0 && s< size)
	      finalStates[s] = true;
	  }
	UnstableMarkovMonoid* ret = new UnstableMarkovMonoid(aut->size);
       	ret->setWitnessTest((bool(*)(const Matrix*))&test_witness);
	for(int i=0;i<aut->alphabet.length();i++)
		ret->addLetter(aut->alphabet[i],*(aut->matrices[i]));
	return ret;
}

/*
passthru() est similaire à la fonction exec() car les deux exécutent la commande command. Si l'argument return_var est présent, le code de statut de réponse UNIX y sera placé. Cette fonction devrait être préférée aux commandes exec() ou system() lorsque le résultat attendu est de type binaire, et doit être passé tel quel à un navigateur. Une utilisation classique de cette fonction est l'exécution de l'utilitaire pbmplus qui peut retourner une image. En fixant le résultat du contenu (Content-Type) à image/gif puis en appelant pbmplus pour obtenir une image gif, vous pouvez créer des scripts PHP qui retournent des images.


  Si vous allez passer des données provenant de l'utilisateur à cette fonction, vous devez utiliser escapeshellarg() ou escapeshellcmd() pour être sûr qu'ils n'endommagent pas le système en exécutant des commandes arbitraires.*/

/*Line by line description of the input file format for automata:

the first line is the size of the automaton (number of states).
the second line is the type of the automaton: c for classical, p for probabilistic.
the third line is the alphabet. Each character is a letter, they should not be separated.
the fourth line is the initial states. Each state should be separated by spaces.
the fifth line is the final states. Each state should be separated by spaces.
the next lines are the transition matrices, one for each letter in the input order. A transition matrix is given by actions (1 and _) separated by spaces. Each matrix is preceded by a single character line, the letter (for readability and checking purposes).
*/


int main(int argc, char **argv)
{
  if(argc <= 1)
    {
      cout << "Usage " << string(argv[0]) << " [input file] [ (opt) timeout in sec]" << endl;
      exit(0);
    }
#ifdef UNIX
  int timeout = 3 * 60;
  if(argc >= 3)
    try
      {
    timeout = stoi(argv[2]);
      }
    catch(...)
      {
	cout << "Failed to parse " << string(argv[2]) << " as an integer " << endl;
      }
  cout << "using timeout " << timeout << " sec" << endl;
  sigalarm(tiemout);
#endif

  ExplicitAutomaton* expa = NULL;
  {
    ifstream input(argv[1]);
    if(!input)
      {
      cout << "Failed to open input '" << string(argv[1]) << "'" << endl;
      exit(0);
    }
    expa = Parser::parseFile(input);
  }
  
  if(expa == NULL)
    {
      cout << "Failed to parse automaton " << endl;
      exit(0);
    }
  
  if(expa->type==PROB)
    {
      UnstableMarkovMonoid* m = toMarkovMonoid(expa);
      auto expr = m->ComputeMonoid();

      if(expr == NULL)
	cout << "The monoid has exactly than " << m->expr_to_mat.size() << " elements" << endl;
      else
	{
	cout << "The monoid has at least " << m->expr_to_mat.size() << " elements,<br/>";
      cout << "the computation stopped because a value 1 witness was found." << endl;
	}

      pair<int, const ExtendedExpression*> r = m->maxLeakNb();
      auto lnb = r.first;
      if(lnb == 0)
	{
	  cout << "This automaton is leaktight" << endl;
	}
      else
	{
	  cout << "This automaton is NOT leaktight, it has " << lnb << " leak, for example" << endl;
	  cout << *expr << endl;
	    cout << "is a leak" << endl;
	}
  
      if (expr)
	{
	cout << "The automaton has value 1, a value 1 witness is " << endl;
	cout << *expr << endl;
	}
      else if(r.first == 0)
	{
	cout << "The automaton has not value 1, because it is leaktight and there is no value 1 witness." << endl;
	}
      else
	{
	  cout << "Since the automaton is not leaktight, and since there is no value1 witness,";
	  cout << "the algorithm could not determine whether the automaton has value 1 or not." << endl;
	}
      cout << endl << endl;
	m->print();
	//      if(toOut)
	//ofs << Dot::toDot(expa,m);
		
    }
  else if (expa->type==CLASSICAL)
    {
      /*
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
      */
    }
  cout << "Computation over " << endl;
}
