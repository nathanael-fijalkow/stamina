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
    try
    {
    if(argc <= 2)
    {
        cout << "Usage " << string(argv[0]);
        cout << " $action [input file] [ (opt) timeout in sec]" << endl;
        cout << " with $action" << endl;
        cout << "\t starheight / sh: compute starheight of a classical automaton" << endl;
        cout << "\t value1 / vo: check whether a probabilistic automaton has value 1" << endl;
        cout << "\t monoid / mon: compute monoid of the automaton" << endl;
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
    } else {
        string filename(argv[2]);
        ofstream ofs(filename + ".auto.dot");
        ofs << Dot::toDot(expa,NULL, -1);
        ofs.close();
    }
    
    //    cout << "Performin action '" << action << "'" << endl;
    
    
    if(action == "starheight" || action == "sh") {
        cout << "Computing starheight" << endl;
        ClassicAut aut(*expa);
        if(!aut.isdet()) {
            cout << "Only deterministic automata are handled" << endl;
            return 0;
        }
        
        //outputs
        UnstableMultiMonoid * monoid = NULL;
        const ExtendedExpression * witness = NULL;
        int loopComplexity = 0;
        auto h = computeStarHeight(aut, monoid, witness, loopComplexity, false, true);
        
        cout << endl << endl << "RESULT: the star height is " << h << "." << endl;
        
        if(witness) {
            cout << endl << "Witness expression: ";
            witness->print();
            cout << endl;
        }
        
        if(monoid) {
            cout << endl;
            monoid->print();
            cout << endl;
            /*
            string filename(argv[2]);
            ofstream ofs(filename + ".monoid.dot");
            ofs << Dot::toDot(expa,monoid,h);
            ofs.close();
             */
        }
        
        
        delete expa; expa = NULL;
        delete monoid; monoid = NULL;
    } else {
        
        bool find_witness = (action != "monoid" && action != "mon");
        
        
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
                cout << endl << endl << "This automaton is LEAKTIGHT " << endl;
            }
            else
            {
                cout << endl << endl << "This automaton is NOT LEAKTIGHT, it has " << lnb << " LEAK, for example" << endl;
                cout << *(r.second) << endl;
                cout << "is a leak" << endl;
            }
            if(!find_witness)
                expr = m.hasValue1();
            if (expr)
            {
                cout << "The automaton HAS VALUE=1.\nA value 1 witness is " << endl;
                cout << *expr << endl << endl;
            }
            else if(r.first == 0)
            {
                cout << "The automaton HAS NOT VALUE=1, because it is leaktight and there is no value 1 witness." << endl << endl;
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
                    cout << endl << endl << "RESULT: This automaton is LIMITED, its monoid contains No UNLIMITED WITNESS." << endl << endl;
                }
                else
                {
                    cout << endl << endl << "RESULT: This automaton is NOT LIMITED, its monoid contains the following UNLIMITED WITNESS:" << endl;
                    cout << *expr << endl;
                }
            }
            m.print();
        }
        cout << "Computation over " << endl;
    }
    } catch(runtime_error & exc)
    {
        cout << "Computation failed: " << exc.what() << endl;
    }
}
