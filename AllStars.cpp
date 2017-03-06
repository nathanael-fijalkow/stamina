
#include <iostream>
#include "Expressions.hpp"
#include "Matrix.hpp"
#include "MarkovMonoid.hpp"
#include "StabilisationMonoid.hpp"
#include "StarHeight.hpp"
#include <chrono>

#include <fstream>
#include <sstream>

#ifdef MSVC
#include <windows.h>
#include <time.h>
#else
#include <unistd.h>
#endif

using namespace std;


ExplicitAutomaton * expa = NULL;

int letters_nb = 2;
bool random_mode = false;
int max_state_nb = 5;

//return true iff an increment was performed
bool inc_mat(ExplicitMatrix & m) {
    auto stnb = m.coefficients.size();
    for(int i = 0 ; (i < stnb); i++) {
        for(int j =0; (j < stnb); j++) {
            if(m.coefficients[i][j]==0) {
                m.coefficients[i][j] = 1;
                for(int i0 = 0 ; i0 < i; i0++) {
                    for(int j0 =0; j0 < stnb; j0++) {
                        m.coefficients[i0][j0] = 0;
                    }
                }
                for(int j0 =0; j0 < j; j0++) {
                    m.coefficients[i][j0] = 0;
                }
                return true;
            }
        }
    }
    return false;
}

bool inc_mat_and_states() {
    auto & matrices = expa->matrices;
    for(int letter = 0; (letter < letters_nb); letter++) {
        auto res = inc_mat(matrices[letter]);
        if(res) {
            for(int letter0 = 0; letter0 < letter; letter0++)
                matrices[letter0].clear(0);
            return true;
        }
    }
    return false;
}

void random_automaton(int stnb) {
    delete expa;    
    expa = new ExplicitAutomaton(stnb,letters_nb);
    expa->initialState = 0;
    expa->finalStates.push_back(stnb - 1);
    auto & matrices = expa->matrices;
    for(int letter = 0 ; letter < letters_nb; letter++) {
        auto & mat = matrices[letter];
        mat.clear(0);
        for(int i = 0 ; (i < stnb); i++)
            mat.coefficients[i][ rand() % stnb ] = 1;
    }
}

void pusage(char* s)
{
    cout << "Usage: " << s << "[-v] [-f] [-a automaton_id] [-r] [-m max_state_nb]" << endl;
    cout << "\t-v verbose on [default off]" << endl;
    cout << "\t-f file output on [default off]" << endl;
    cout << "\t-a automaton_id compute for a specific automaton id# " << endl;
    cout << "\t-r random choice of automaton [default off]" << endl;
    cout << "\t-m max_state_nb specifies max number of states [default 15]" << endl;
    exit(0);
}

int main(int argc, char **argv)
{
    
    cout << "Stamina rules" << endl;
    
    //    unsigned int seed = time(NULL);
    int opt;
    bool verbose = false;
    bool out_file = false;
    int aut_id = -1;
    
    
    while((opt = getopt(argc,argv, "vhfra:m:")) != -1)
    {
        switch(opt)
        {
            case 'h':
                pusage(argv[0]);
            case 'v':
                cout << "Verbose on "<< endl;
                verbose = true;
                break;
            case 'f':
                cout << "Output file on "<< endl;
                out_file = true;
                break;
            case 'r':
                cout << "Random mode on "<< endl;
                random_mode = true;
                break;
            case 'a':
                aut_id = atoi(optarg);
                cout << "Automaton id #" << aut_id<< endl;
                break;
            case 'm':
                max_state_nb = atoi(optarg);
                if(max_state_nb <= 0) max_state_nb = 1;
                cout << "Automaton id #" << aut_id<< endl;
                break;
            default:
                pusage(argv[0]);
        }
    }
    
    if(aut_id>=0) random_mode = false;
    

    
    stringstream filename;
    filename << "starheight_" << (random_mode ? "random" : "enumerative");
    
    ofstream file(filename.str() + ".csv", ofstream::app);
    file << "#;StarHeight;LoopComplexity;AutomatonSize;MonoidDim;MonoidSize;RewriteRulesNb;VectorsNb;ComputationTime(ms)" << endl;
    file.close();
    
    uint nb = 0;

    auto base = std::chrono::high_resolution_clock::now();

    
    for(int stnb = 1 ; stnb < max_state_nb; stnb++) {
        
        expa = new ExplicitAutomaton(stnb,letters_nb);
        expa->initialState = 0;
        expa->finalStates.push_back(stnb -1);
        
        while(true) {
            nb++;
            if(aut_id < 0 || nb == aut_id) {
                
                auto start = std::chrono::high_resolution_clock::now();
                unsigned int seed
                    = chrono::duration_cast<chrono::microseconds>(start - base).count();

                if(random_mode)
                {
                    cout << "Random seed " << seed << endl;
                    srand(seed);
                    stnb = 1 + (rand() % (max_state_nb - 1));
                    cout << "New random automaton with " << stnb << " states and seed " << seed << endl;
                    random_automaton(stnb);
                }

                ClassicAut aut(*expa);
                if(!aut.iscomplete()) {
                    // cout << "Automaton #" << nb << " not complete" << endl;
                } else if (!aut.isdet()){
                    //cout << "Automaton #" << nb << " not deterministic" << endl;
                } else {
                    if(!random_mode) {
                        cout << "Automaton #" << nb << " complete and deterministic" << endl;
                    }

                    UnstableMultiMonoid * monoid = NULL;
                    const ExtendedExpression * witness = NULL;
                    int loopComplexity;
                    auto h = computeStarHeight(
                                               aut,
                                               monoid,
                                               witness,
                                               loopComplexity,
                                               verbose,
                                               out_file,
                                               "AllStars");
                    
                    auto end = std::chrono::high_resolution_clock::now();
                    auto ctime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                    
                    if(monoid != NULL) {
                        cout << "StarHeight " << h << endl;
                        ofstream file(filename.str() + ".csv", ofstream::app);
                        file << (random_mode ? nb : seed) << ";" << h << ";" << loopComplexity << ";";
                        file << stnb  << ";" << VectorInt::GetStateNb() << ";";
                        file << monoid->expr_to_mat.size();
                        file << ";" << monoid->rewriteRules.size() << ";" << int_vectors.size();
                        file << ";" <<  ctime << endl;
                        file.close();
                        delete monoid; monoid = NULL;
                    } else  {
                        cout << "StarHeight " << h << endl;
                        ofstream file(filename.str() + ".csv", ofstream::app);
                        file << (random_mode ? nb : seed) << ";" << h << ";" << loopComplexity << ";";
                        file << stnb << ";" << VectorInt::GetStateNb()<< ";" << 0;
                        file << ";" << 0 << ";" << 0;
                        file << ";" << ctime << endl;
                        file.close();
                    }
                }
            }
            else {
                if(!inc_mat_and_states()) break;
            }
        }
        delete expa;
        if(nb >= aut_id) break;
    }
    
    cout << "Experiment over " << endl;
    
}
