
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
    cout << "Usage: " << s << "[-v] [-f] [-a automaton_id] [-b automaton_id] [-r] [-m max_state_nb] [-l] [-i] [-z] [-y exp_number]" << endl;
    cout << "\t-v verbose on [default off]" << endl;
    cout << "\t-f file output on [default off]" << endl;
    cout << "\t-a automaton_id compute for a specific automaton id# " << endl;
    cout << "\t-b automaton_id compute for automata with >= id# " << endl;
    cout << "\t-r random choice of automaton [default off]" << endl;
    cout << "\t-m max_state_nb specifies max number of states [default 15]" << endl;
    cout << "\t-l do not use loop heuristic [default yes]" << endl;
    cout << "\t-i do not use minimization [default yes]" << endl;
    cout << "\t-p do not use pruning [default yes]" << endl;
    cout << "\t-z try all combinations of heuristics [default no]" << endl;
    cout << "\t-y number_of_experiments [default 100]" << endl;
    exit(0);
}

bool use_loop_heuristic = true;
bool use_minimization = true;
bool use_prune = true;

string filename() {
    stringstream filename;
    filename << "starheight_" << (random_mode ? "random" : "enumerative") << "_";
    filename << "_loopheur_" << (use_loop_heuristic ? "on" : "off") << "_minim_heur" << (use_minimization ? "on" : "off") << "_prune_heur" << (use_prune ? "on" : "off");
    return filename.str();
}

int main(int argc, char **argv)
{
    
    cout << "Stamina rules" << endl;
    
    //    unsigned int seed = time(NULL);
    int opt;
    bool verbose = false;
    bool out_file = false;
    bool compare_heuristics = false;
    int experiments_nb = 100;
    bool just_one_id = true;
    int aut_id = -1;
    
    
    while((opt = getopt(argc,argv, "vhfrliza:m:y:")) != -1)
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
                just_one_id = true;
                break;
            case 'b':
                aut_id = atoi(optarg);
                cout << "Automaton id #" << aut_id<< endl;
                just_one_id = false;
                break;
            case 'm':
                max_state_nb = atoi(optarg);
                if(max_state_nb <= 0) max_state_nb = 1;
                cout << "Max states #" << max_state_nb<< endl;
                break;
            case 'y':
                experiments_nb = atoi(optarg);
                if(experiments_nb <= 0) experiments_nb = 1;
                break;
            case 'z':
                compare_heuristics = true;
                cout << "Comparing heuristics" << endl;
                break;
            case 'l':
                use_loop_heuristic = false;
                if(!compare_heuristics) cout << "Disabling loop heuristic" << endl;
                break;
            case 'i':
                use_minimization = false;
                if(!compare_heuristics) cout << "Disabling minimization" << endl;
                break;
            case 'p':
                use_prune = false;
                cout << "Disabling pruning" << endl;
                break;
            default:
                pusage(argv[0]);
        }
    }
    
    cout << "Experiments nb" << experiments_nb<< endl;
    
    if(aut_id>=0) random_mode = false;
    
    
    if(compare_heuristics) {
        use_loop_heuristic= false;
        use_minimization = false;
    }
    
    if(!compare_heuristics) {
        ofstream file(filename() + ".csv", ofstream::app);
        file << "#;StarHeight;LoopComplexity;AutomatonSize;MonoidDim;";
        file << "MonoidSize;RewriteRulesNb;VectorsNb;ComputationTime(ms);";
        file << "LoopHeur;MinHeur";
        file << endl;
        file.close();
    } else {
        for(int loop = 0; loop<=1; loop++) {
            for(int mini = 0; mini <= 1; mini++) {
                use_loop_heuristic = loop;
                use_minimization = mini;
                ofstream file(filename() + ".csv", ofstream::app);
                file << "#;StarHeight;LoopComplexity;AutomatonSize;MonoidDim;";
                file << "MonoidSize;RewriteRulesNb;VectorsNb;ComputationTime(ms);";
                file << "LoopHeur;MinHeur";
                file << endl;
                file.close();
            }
        }
    }
    
    uint cur_aut_id = 1;
    uint exp_nb = 0;
    auto base = std::chrono::high_resolution_clock::now();
    
    
    
    for(int stnb = 1 ; stnb < max_state_nb; stnb++) {

        delete expa;
        expa = new ExplicitAutomaton(stnb,letters_nb);
        expa->initialState = 0;
        expa->finalStates.push_back(stnb -1);
        
        while(true) {
            if(aut_id < 0 || cur_aut_id >= aut_id) {
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
                    // cout << "Automaton #" << cur_aut_id << " not complete" << endl;
                } else if (!aut.isdet()){
                    //cout << "Automaton #" << cur_aut_id << " not deterministic" << endl;
                } else {
                    if(!random_mode) {
                        cout << "Automaton #" << cur_aut_id << " complete and deterministic" << endl;
                    }
                    if(!compare_heuristics || (!use_minimization && !use_loop_heuristic))
                        exp_nb++;

                    
                    UnstableMultiMonoid * monoid = NULL;
                    const ExtendedExpression * witness = NULL;
                    int loopComplexity;
                    auto h = computeStarHeight(
                                               aut,
                                               monoid,
                                               witness,
                                               loopComplexity,
                                               out_file,
                                               verbose,
                                               "AllStars",
                                               use_loop_heuristic,
                                               use_minimization,
                                               use_prune
                                               );
                    
                    auto end = std::chrono::high_resolution_clock::now();
                    auto ctime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                    cout << "Computation time " << ctime << endl;
                    if(monoid != NULL) {
                        cout << "StarHeight " << h << endl;
                        ofstream file(filename() + ".csv", ofstream::app);
                        file << (random_mode ? seed : cur_aut_id) << ";" << h << ";" << loopComplexity << ";";
                        file << stnb  << ";" << VectorInt::GetStateNb() << ";";
                        file << monoid->expr_to_mat.size();
                        file << ";" << monoid->rewriteRules.size() << ";" << int_vectors.size();
                        file << ";" <<  ctime;
                        file << ";" << (int) use_loop_heuristic;
                        file << ";" << (int) use_minimization;
                        file << ";" << (int) use_prune;
                        file << endl;
                        file.close();
                        delete monoid; monoid = NULL;
                    } else  {
                        cout << "StarHeight " << h << endl;
                        ofstream file(filename() + ".csv", ofstream::app);
                        file << (random_mode ? seed : cur_aut_id) << ";" << h << ";" << loopComplexity << ";";
                        file << stnb << ";" << VectorInt::GetStateNb()<< ";" << 0;
                        file << ";" << 0 << ";" << 0;
                        file << ";" <<  ctime;
                        file << ";" << (int) use_loop_heuristic;
                        file << ";" << (int) use_minimization;
                        file << ";" << (int) use_prune;
                        file << endl;
                        file.close();
                    }
                }
            }
            if(compare_heuristics && !use_loop_heuristic && !use_minimization)
            { use_minimization = true; }
            else if(compare_heuristics && !use_loop_heuristic && use_minimization)
            { use_loop_heuristic =true; use_minimization = false;}
            else if(compare_heuristics && use_loop_heuristic && !use_minimization)
            { use_minimization = true;}
            else if(!random_mode){
                if(compare_heuristics) {
                    use_loop_heuristic = false;
                    use_minimization = false;
                }
                
                if(just_one_id && aut_id >= 0 && cur_aut_id >= aut_id) {
                    cout << "Performed single experiment on id " << aut_id << "." << endl;
                    break;
                }
                cur_aut_id++;
                if(exp_nb > experiments_nb){
                    cout << "Max experiments number attained." << endl;
                    break;
                }
                bool ret = inc_mat_and_states();
                if(!ret) break;
            }
        }
        if(just_one_id && aut_id >= 0 && cur_aut_id >= aut_id) {
            cout << "Performed single experiment on id " << aut_id << "." << endl;
            break;
        }
    }
    cout << "Experiment over " << endl;
    
}
