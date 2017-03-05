
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
    
    int opt,verbose=1,toOut=0;
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
        
        cout << endl << endl << "The input automaton is a classical automaton. Stamina will compute its star height." << endl ;
        cout << "It first finds an upper bound using the Loop Complexity heuristics, and then proceeds with the star height computation." << endl << endl ;
        
        ClassicAut aut(*expa);
        
        if(! aut.isdet()){
            cout<<"The input automaton is non-deterministic."<<endl;
            cout<<"This functionality is not implemented yet for star-height"<<endl;
            cout<<"Exiting"<<endl;
            return 0;
        }
        
        if(! aut.iscomplete()){
            cout<<"The input automaton is not complete, adding sink state."<<endl<<endl;
            aut.addsink();
        }
        
        cout << "************LOOP COMPLEXITY******************" << endl << endl;
        pair<char,list<uint>> res = LoopComplexity(&aut);
        int LC = (int)res.first ;
        list<uint> order = res.second;
        RegExp* regexpr = Aut2RegExp(&aut,order);
        
        cout << "According to the Loop Complexity heuristics, the star-height is at most " << LC << "." << endl;
        cout << "A regular expression for the language (omitting a finite number of words) is:  "<<endl;
        regexpr->print();
        cout << endl;
        
        list<ExtendedExpression*> sharplist = Reg2Sharps(regexpr);
        
        if(sharplist.size()==0)// empty language
        {
            cout <<"The language is empty, the star height is 0."<<endl;
            return 0;
        }
        
        if (verbose){
            cout << "The Loop Complexity suggests the following unlimitedness witnesses:   "<<endl;
            
            for(ExtendedExpression *sharp_expr: sharplist){
                cout << *sharp_expr << endl;
            }
        }
        
        cout <<endl<< "************STAR HEIGHT COMPUTATION**********" << endl;
        if(verbose) cout << "Computing the Subset Automaton..." << endl;
        //We start by computing the subset automaton of aut
        //It has deterministic letters
        ClassicEpsAut* Subsetaut=toSubsetAut(&aut);
        
        uint ns=Subsetaut->NbStates;
        char nl=Subsetaut->NbLetters;
        
        if(verbose){
            printf("Subset Automaton Built, %d states\n\n",ns);
            ofstream file("subset_aut.txt");
            Subsetaut->print(file);
        }
        
        // /* minimisation part, optional if we have doubts
        
        if (verbose) cout <<"Minimizing the Subset Automaton..."<<endl;
        Subsetaut=SubPrune(Subsetaut);
        //Subsetaut=SubMin(Subsetaut); //optional for now, to test later
        
        ns=Subsetaut->NbStates;
        nl=Subsetaut->NbLetters;
        
        if(verbose){
            printf("Pruned Subset Automaton Built, %d states\n\n",ns);
            //Subsetaut->print();
            ofstream file("subset_aut_pruned.txt");
            Subsetaut->print(file);
        }
        int h = 1;
        while (h<LC){
            //			ofstream output("monoid " + to_string(h) + ".txt");
            
            cout << endl << "******************************" << endl;
            cout << "Testing star height " << h << endl;
            cout << "******************************" << endl;
            
            if(verbose) cout << "First step: computing the automaton with counters." << endl << endl;
            MultiCounterAut *Baut = toNestedBaut(Subsetaut, h);
            
            UnstableMultiMonoid monoid(*Baut);
            
            /*
             cout << "DEBUG: computing monoid for sh=" << h << endl;
             monoid.ComputeMonoid();
             ofstream f("limited_monoid_sh_" + to_string(h) + ".txt");
             f << monoid;
             h++;
             continue;
             */
            bool witness_found=false;
            
            if(verbose) cout << "Second step: checking whether the Loop Complexity suggestions are unlimitedness witnesses." << endl;
            for(ExtendedExpression *sharp_expr: sharplist){
                if (h>1) {cout<<"aborted"<<endl;break;} //not using heuristic
                const Matrix* mat = monoid.ExtendedExpression2Matrix(sharp_expr,*Baut);
                if (verbose) cout <<"."<<flush;
                //if(verbose) {mat->print();cout<<endl<<endl;}
                if(monoid.IsUnlimitedWitness(mat)){
                    if(verbose) cout << "--> The heuristic found a witness, the star height is not " << h << ", it is larger." << endl;
                    witness_found=true;
                    ofstream f("unlimited_witness_sh_" +  to_string(h) + ".txt");
                    f << *mat;
                    break;
                }
            }
            if(verbose && !witness_found)
                cout << "-->Heuristic found no witness." << endl << endl;
            
            
            if(!witness_found){
                if(verbose){
                    cout << "Third step: computing the monoid, and checking for the existence of an unlimitedness witness on the fly." << endl << endl;
                }
                
                const ExtendedExpression * expr = monoid.containsUnlimitedWitness();
                
                
                if(verbose) {
                    monoid.print_summary();
                    ofstream f("monoid_sh_" + to_string(h) + ".txt");
                    f << monoid;
                }
                //if(verbose) monoid.print();
                
                delete Baut;
                
                if (expr){
                    if(verbose){
                        cout << "An unlimited witness is ";
                        expr->print();
                        cout << endl;
                        ofstream f("unlimited_witness_sh_" +   to_string(h) + ".txt");
                        f << *expr;
                    }
                }
                else{
                    if(verbose) cout << "The automaton is limited." << endl;
                    cout << "RESULTS: the star height is " << h << "." << endl;
                    break;
                }
            }
            if(toOut) {
                ofstream ofs(outputFilename + " sh " + to_string(h) + ".dot");
                if(!ofs) throw runtime_error("Could not open output dot file ");
                ofs << Dot::toDot(expa, &monoid, h);
            }
            h++;
        }
        if(h==LC){
            cout << endl << "RESULTS: the star height is " << LC << ", matching the Loop Complexity, and a regular expression witnessing it is ";
            regexpr->print();
            cout << "." << endl;
        }
    }
    return 0;
}
