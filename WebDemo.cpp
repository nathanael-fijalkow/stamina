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
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#endif

using namespace std;

#define MAX_JOBS 10



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

#ifdef UNIX
void signal_callback_handler(int signum)
{
    cout << "Computation timeout, CPU is limited on the WebDemo...\n" << endl;
    exit(0);
}
#endif

int main(int argc, char **argv)
{
    if(argc <= 2)
    {
        cout << "Usage " << string(argv[0]);
        cout << " [compute-monoid,has-value1,is-unbounded] [input file] [ (opt) timeout in sec]" << endl;
        exit(0);
    }
#ifdef UNIX
    int timeout = 3 * 60;
    if(argc >= 4)
        try
    {
        timeout = stoi(argv[2]);
    }
    catch(...)
    {
        cout << "Failed to parse " << string(argv[2]) << " as an integer " << endl;
    }
    cout << "Computation will stop after timeout " << timeout << " sec" << endl;
    signal(SIGALRM, signal_callback_handler);
    alarm(timeout);
#endif
    
    string action(argv[1]);
    bool find_witness = (action != "compute-monoid");
    
    ExplicitAutomaton* expa = NULL;
    {
        ifstream input(argv[2]);
        if(!input)
        {
            cout << "Failed to open input '" << string(argv[2]) << "'" << endl;
            exit(0);
        }
        cout << "Parsing input file..." << endl;
        expa = Parser::parseFile(input);
    }
    
    if(expa == NULL)
    {
        cout << "Failed to parse automaton " << endl;
        exit(0);
    }
    
    if(expa->type==PROB)
    {
        UnstableMarkovMonoid m(*expa);
        
        if(find_witness)
        {
            cout << "Looking for a value 1 witness in the Markov monoid..." << endl;
            m.setWitnessTest(&UnstableMarkovMonoid::value1Test);
        }
        else
        {
            cout << "Computing the Markov monoid..." << endl;
        }
        auto expr = m.ComputeMonoid();
        
        if(expr == NULL)
            cout << "The monoid has exactly " << m.expr_to_mat.size() << " elements" << endl;
        else
        {
            cout << "The monoid has at least " << m.expr_to_mat.size() << " elements,<br/>";
            cout << "the computation stopped because a value 1 witness was found." << endl;
        }
        
        pair<int, const ExtendedExpression*> r = m.maxLeakNb();
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
        if(!find_witness)
            expr = m.hasValue1();
        if (expr)
        {
            cout << "The automaton has value 1.\nA value 1 witness is " << endl;
            cout << *expr << endl << endl;
        }
        else if(r.first == 0)
        {
            cout << "The automaton does not have value 1, because it is leaktight and there is no value 1 witness." << endl;
        }
        else
        {
            cout << "Since the automaton is not leaktight, and since there is no value1 witness,";
            cout << "the algorithm could not determine whether the automaton has value 1 or not." << endl;
        }
        m.print();
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
    else if (expa->type > 0)
    {
        UnstableMultiMonoid m(* expa);
        const ExtendedExpression * expr =
            find_witness ?
            m.containsUnlimitedWitness() :
            m.ComputeMonoid();
        
        if(expr == NULL)
            cout << "The monoid has exactly " << m.expr_to_mat.size() << " elements" << endl;
        else
        {
            cout << "The monoid has at least " << m.expr_to_mat.size() << " elements,<br/>";
            cout << "the computation stopped because an unlimited witness was found." << endl;
        }
        
        if(find_witness)
        {
            if(expr == NULL)
            {
                cout << "This automaton is bounded, its monoid contains no unlimited witness." << endl;
            }
            else
            {
                cout << "This automaton is NOT bounded, its monoid contains the following unlimited witness:" << endl;
                cout << *expr << endl;
            }
        }
        m.print();
    }
    cout << "Computation over " << endl;
}
