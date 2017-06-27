
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




int main(int argc, char **argv)
{
    
    int opt,verbose=0,toOut=0;
    ifstream ifs;
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

#endif
    
    ExplicitAutomaton* expa = Parser::parseFile(ifs);
    ifs.close();
    
    if(expa->type==PROB)
    {
        cout << endl << endl << "The input automaton is a probabilistic automaton. Stamina will construct the Markov Monoid to check whether it has value 1." << endl << endl ;
        
        UnstableMarkovMonoid m(*expa);
        m.setWitnessTest(&(m.value1Test));
        auto expr = m.ComputeMonoid();
        
        cout << "***************************************" << endl;
        cout << "***********MAIN RESULTS****************" << endl;
        cout << "***************************************" << endl << endl;
        
        pair<int, const ExtendedExpression*> r = m.maxLeakNb();
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
            
            m.print();
            cout << endl;
        }
        if(toOut) {
            ofstream ofs(outputFilename + ".dot");
            ofs << Dot::toDot(expa,&m, -1);
        }
    }
    else if (expa->type==CLASSICAL)
    {
        ClassicAut aut(*expa);
        if(!aut.isdet()) {
            cout << "Only deterministic automata are handled" << endl;
            return 0;
        }
        
        //outputs
        UnstableMultiMonoid * monoid = NULL;
        const ExtendedExpression * witness = NULL;
        int loopComplexity = 0;
        auto h = computeStarHeight(aut, monoid, witness, loopComplexity, true, true);
        cout << "RESULT: the star height is " << h << "." << endl;

        if(toOut) {
            ofstream ofs(outputFilename + ".dot");
            ofs << Dot::toDot(expa,monoid, -1);
        }
        
        delete expa; expa = NULL;
        delete monoid; monoid = NULL;
    }
    return 0;
}
