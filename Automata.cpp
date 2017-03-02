
#include "Automata.hpp"
#include <algorithm>

using namespace std;

//Classical Automata
void ClassicAut::init(char Nletters,uint Nstates){
	NbLetters=Nletters;
	NbStates=Nstates;
	
	initialstate.resize(NbStates, false);	
	finalstate.resize(NbStates, false);
		
	for(int a=0;a<Nletters;a++)
	{
		trans[a].resize(NbStates);
		for(int i=0;i<Nstates;i++)
			trans[a][i].resize(NbStates, false);
	}
}

//Constructor
ClassicAut::ClassicAut(char Nletters,uint Nstates)
{
	init(Nletters,Nstates);
}


//test wether the automaton is deterministic
bool ClassicAut::isdet(){
	for (int a=0;a<NbLetters;a++){
		for(int i=0;i<NbStates;i++){
			bool found=false;
			for(int j=0;j<NbStates;j++){
				if(trans[a][i][j]){
					if (found) return false;
					found=true;
				}
			}
		}
	}
	return true;	
}
	
//test whether the automaton is complete
bool ClassicAut::iscomplete(){
	for (int a=0;a<NbLetters;a++){
		for(int i=0;i<NbStates;i++){
			bool found=false;
			for(int j=0;j<NbStates;j++){
				if(trans[a][i][j]) {
					found=true;
					break;
				}
			}
			if (!found) return false;
		}
	}
	return true;	
}

	
//add a rejecting sink
void ClassicAut::addsink(){
	NbStates++;
	
	initialstate.resize(NbStates, false);	
	finalstate.resize(NbStates, false);
		
	for(int a=0;a<NbLetters;a++)
	{
		trans[a].resize(NbStates);
		for(int i=0;i<NbStates-1;i++){
			trans[a][i].resize(NbStates);
			bool found=false;
			for(int j=0;j<NbStates-1;j++){
				if(trans[a][i][j]) {
					found=true;
					break;
				}
			}
			//add transition to sink if there was none
			trans[a][i][NbStates-1]=(!found);
		}
		//transitions from NbStates
		trans[a][NbStates-1].resize(NbStates);
		for(int j=0;j<NbStates-2;j++){
				trans[a][NbStates-1][j]=false;
		}
		//self-loop of sink state
		trans[a][NbStates-1][NbStates-1]=true;
			
	}
}


//classical Automata with Epsilon-transitions
ClassicEpsAut::ClassicEpsAut(char Nletters,uint Nstates) : ClassicAut(Nletters,Nstates)
{
trans_eps=(bool**)malloc(Nstates*sizeof(bool *));
		for(int i=0;i<Nstates;i++){
			trans_eps[i]=(bool*)malloc(Nstates*sizeof(bool));
			memset(trans_eps[i], false, Nstates);
		}
		
transdet=(uint**)malloc(Nletters*sizeof(uint *));
		for(unsigned char a=0;a<Nletters;a++){
			transdet[a]=(uint*)malloc(Nstates*sizeof(uint));
			memset(transdet[a], Nstates, Nstates); //initialize with Nstates, meaning "no transition"
		}
initial=0;
}


//We assume letters are deterministic
void ClassicEpsAut::print(ostream& st){
	st << (int)NbStates << " states " << (int)NbLetters << " letters" << endl;
	st << "Initial states: ";
	for (uint i = 0; i < NbStates; i++)
		if (initialstate[i])
			st <<  i << " ";
	st << endl;
	st << "Final states: ";
	for (uint i = 0; i < NbStates; i++)
		if (finalstate[i])
			st << i << " ";
	st << endl;


	for (unsigned char a = 0; a<NbLetters; a++){
		st << "Letter " << (int)a << endl;
		for (uint i = 0; i<NbStates; i++){
				st << transdet[a][i];
			st << endl;
		}
		st << endl;
	}
	st << endl;
	
	
	st << "Epsilon" << endl;
		for(uint i=0;i<NbStates;i++){
			for(uint j=0;j<NbStates;j++){
				st << (int) trans_eps[i][j] << " ";
			}
			st << endl;
		}
		st << endl;
}

void ClassicAut::print(ostream& st){
	st << (int) NbStates << " states " << (int) NbLetters << " letters" << endl;
	st << "Initial states: ";
	for (uint i = 0; i < NbStates; i++)
		if (initialstate[i])
			st << i << " ";
	st << endl;
	st << "Final states: ";
	for (uint i = 0; i < NbStates; i++)
		if (finalstate[i])
			st << i << " ";
	st << endl;


	for (unsigned char a = 0; a<NbLetters; a++){
		st << "Letter " << (int) a <<  endl;
		for (uint i = 0; i<NbStates; i++){
			for (uint j = 0; j<NbStates; j++){
				st << ((trans[a][i][j]) ? "1" : "_");
			}
			st << endl;
		}
		st << endl;
	}
	st << endl;
}

void ClassicAut::addLetter(char c, ExplicitMatrix m)
{
	if(m.stateNb!=NbStates)
		return;
	for(int i=0;i<NbStates;i++)
		for(int j=0;j<NbStates;j++)
			trans[c][i][j]=(m.coefficients[i][j]==EXP_MAT_1);
}

//compute x^p
uint myPow(uint x, uint p)
{
  if (p == 0) return 1;
  if (p == 1) return x;

  int tmp = myPow(x, p/2);
  if (p%2 == 0) return tmp * tmp;
  else return x * tmp * tmp;
}

uint TwoPow(uint n){ return (1 << n);} //compute 2^n

//kth bit of n
bool bit(uint n,uint k){ return (n >> k) & 1;}


//build a subset automaton, by taking the powerset automaton and adding epsilon-transitions.
//We assume that we come from a deterministic automaton.
//It also works if a nondeterministic automaton for the complement language is given, with final states reversed.
ClassicEpsAut* toSubsetAut(ClassicAut *aut){
	uint n=aut->NbStates;
	uint nspow=TwoPow(n); //new number of states
	
	ClassicEpsAut* Subaut=new ClassicEpsAut(aut->NbLetters,nspow);
	//each new state is a subset of Q, represented by the binary representation of uint 0,1,2,..., nspow-1
	
	//the only initial new state is {i:  i initial}. The function works if there are several initial states
	Subaut->initial=0;
	for(uint i=0;i<n;i++){
		if(aut->initialstate[i])
		Subaut->initial+=TwoPow(i);
	}
#if VERBOSE_AUTOMATA_COMPUTATION
	printf("Initial state created:%d\n",Subaut->initial);
#endif
	Subaut->initialstate[Subaut->initial]=true;

	//final states are subsets where all states are final. This is because we actually powerset the automaton for the complement.
	uint k;
	for(uint i=0;i<nspow;i++){
		k=0;
		bool fin=true;
		while(k<n && fin) {
			if (bit(i, k)) fin = aut->finalstate[k];
			//fin=aut->finalstate[bit(i,k)];
			k++;
		}
		Subaut->finalstate[i]=fin;
	}

#if VERBOSE_AUTOMATA_COMPUTATION
	printf("Final states created\n");
#endif

	//transitions are as usually in powerset
	
	bool* tabj=(bool *)malloc(n*sizeof(bool));
	for(unsigned char a=0;a<aut->NbLetters;a++){
		for(uint i=0;i<nspow;i++){
			//compute the j such that i--a-->j
			memset(tabj,false,n);
			for(uint k=0;k<n;k++){
				if(bit(i,k)){ 
					//put the destination(s) of k in tabj;
					for(uint x=0;x<n;x++){ if (aut->trans[a][k][x]) tabj[x]=true;}
				}				
			}
			//compute j from tabj
			uint j=0;
			for(uint k=0;k<n;k++){
				if(tabj[k]) j+= (1<<k);
			}
			
			//put transition (i,a,j), in both tables (nondeterministic and deterministic)
			Subaut->trans[a][i][j]=true;
			Subaut->transdet[a][i]=j;
		}
	}
	
	
	//we add epsilon-transitions witnessing the inclusion relation: P->Q if P is contained in Q
	for(uint i=0;i<nspow;i++){
		for(uint j=0;j<nspow;j++){
			Subaut->trans_eps[i][j]=( (i|j) == j);  //test for bitwise inclusion
		}
	}
		
	return Subaut;
}


//Minimization of subset automata
ClassicEpsAut* SubMin(ClassicEpsAut *aut){
	uint N=aut->NbStates;
	uint *part=(uint *)malloc(N*sizeof(uint)); //array containing the number of the partition.
	
	VectorUInt::SetSize(aut->NbLetters+N);
	
	std::vector<uint> data(VectorUInt::GetStateNb()); //vector for merging, of size Nletters+Nstates, giving the partition of p---a-->? and then p|q=?


	//initially, two partitions: 0 for rejecting and 1 for accepting
	for(uint i=0;i<N;i++){
		part[i]=(aut->finalstate[i])?1:0;
	}

	std::unordered_set<VectorUInt> vectors;
	
	uint *original=(uint *)malloc(N*sizeof(uint));// to go from representant to one original state.
		
	uint nbpart=0;
	uint new_nbpart=2;
	
	
	while(nbpart!=new_nbpart){
		vectors.clear();
		nbpart=new_nbpart;
		new_nbpart=0;
		//printf("nbpart:%d\n",nbpart);
		
		for(uint i=0;i<N;i++){
			for(unsigned char a=0;a<aut->NbLetters;a++){
				data[a]=part[aut->transdet[a][i]]; //store the partitions of destinations in data
			}
			for(uint j=0;j<N;j++){		
				data[aut->NbLetters+j]=part[i|j]; //store the partitions of unions in data
			}
			
			auto it = vectors.emplace(data,i);
			
			//if vector is new, we increase new_nb_part
			if(it.second){
				 part[i]=new_nbpart; //take the current identifier
				 original[new_nbpart]=i;// memorize an original state of this identifier
				 new_nbpart++;
			 } else {
			 part[i]=part[it.first->id]; //point to same representant as existing one 
			}	
			//maybe replace this with monotonicity instead of equality later... 
			
		}		
		
	}
	
	//after stabilization, compute new transition table.
	
	ClassicEpsAut *MinAut=new ClassicEpsAut(aut->NbLetters,nbpart);
	
	//initial and final states
	MinAut->initial=part[aut->initial];
	
	for(uint i=0;i<nbpart;i++){
		MinAut->finalstate[i]=aut->finalstate[original[i]];
	}
	
	//det transitions
	for(unsigned char a=0;a<aut->NbLetters;a++){
		for(uint n=0;n<nbpart;n++){
			MinAut->transdet[a][n]=part[aut->transdet[a][original[n]]];
		}
	}
	
	//epsilon transitions
	for(uint i=0;i<nbpart;i++){
		for(uint j=0;j<nbpart;j++){
			MinAut->trans_eps[i][j]=aut->trans_eps[original[i]][original[j]];
		}
	}
	
	return MinAut;
}


void MultiCounterAut::set_trans(char a, const MultiCounterMatrix & mat) {
    trans[a] = mat;
}


//Multi-counter Automata
void MultiCounterAut::init(char Nletters,uint Nstates, char Ncounters){
	NbLetters=Nletters;
	NbStates=Nstates;
	NbCounters=Ncounters;
	
    MultiCounterMatrix::set_counter_and_states_number(NbCounters, Nstates);
    
	initialstate.resize(Nstates, false);
	finalstate.resize(Nstates, false);
}

//Constructor
MultiCounterAut::MultiCounterAut(char Nletters,uint Nstates, char Ncounters){
	init(Nletters,Nstates,Ncounters);
	
}

MultiCounterAut::~MultiCounterAut()
{
};


char MultiCounterAut::coef_to_char(string coef, int NbCounters)
{
    if(coef == "") return 2 * NbCounters + 2;
    if(coef == "B") return 2 * NbCounters + 2;
    if(coef == "O") return 2 * NbCounters + 1;
    if(coef == "E") return NbCounters;
    if(coef[0]=='R' && coef.size()==2)
    {
        char c = coef[1] - '0';
        if(c >= 0 || c < NbCounters)
            return c;
    }
    if(coef[0]=='I' && coef.size()==2)
    {
        char c = coef[1] - '0';
        if(c >= 0 || c < NbCounters)
            return (NbCounters + c);
    }
    throw runtime_error("coef_to_char: cannot convert '" + coef + "' from string");
}

string MultiCounterAut::coef_to_string(char element)
{
    string result = "";
    if (is_bottom(element)) result = "_ ";
    else if (is_omega(element)) result = "O ";
    else if (is_epsilon(element)) result = "E ";
    else if (is_reset(element)) result = "R" + to_string( get_reset_counter(element));
    else result = "I" + to_string(get_inc_counter(element));
    return result;
}

char MultiCounterAut::coef_to_char(string coef)
{
    return coef_to_char(coef, NbCounters);
}



string state_index_to_tuple(int index, int NbStates)
{
	int N = (int)sqrt(NbStates);
	string res = "";
	if (index < N)
		res = "{ " + to_string(index) + " }";
	else
	{
		index -= N;
		res = "{" + to_string(index / N) + "," + to_string(index % N) + "}";
	}
	res.resize(7, ' ');
	return res;
}

string MultiCounterAut::state_index_to_string(int index)
{
	return state_index_to_tuple(index, NbStates);
}

//print B-automaton
void MultiCounterAut::print(ostream& st){
	st << (int)NbStates << " states " << (int)NbLetters << " letters" << endl;
	st << "Initial states: ";
	for (uint i = 0; i < NbStates; i++)
		if (initialstate[i])
			st << state_index_to_string(i) << " ";
	st << endl;
	st << "Final states: ";
	for (uint i = 0; i < NbStates; i++)
		if (finalstate[i])
			st << state_index_to_string(i) << " ";
	st << endl;

	for(char a=0;a<NbLetters;a++){
		st << "Letter " << (int) a << endl;
		for(uint i=0;i<NbStates;i++){
			st << state_index_to_string(i) << " ";
			for(uint j=0;j<NbStates;j++){
				st << coef_to_string(trans[a].get(i,j)) << " ";
			}
			st << endl;
		}
		st << endl;
	}
}

void MultiCounterEpsAut::set_transdet_state(char letter, uint state, uint val)
{
    if(letter >= NbLetters || state >= NbStates)
        throw runtime_error("Bad access to transdet_state");
    _transdet_state[letter * NbStates + state] = val;
}

void MultiCounterEpsAut::set_transdet_action(char letter, uint state, char val)
{
    if(letter >= NbLetters || state >= NbStates)
        throw runtime_error("Bad access to transdet_state");
    _transdet_action[letter * NbStates + state] = val;
}

void MultiCounterEpsAut::set_trans_eps(const ExplicitMatrix & mat)
{
    _trans_eps = new MultiCounterMatrix(mat, NbCounters);
}

const MultiCounterMatrix & MultiCounterEpsAut::get_trans_eps() const
{
    return _trans_eps;
}

uint MultiCounterEpsAut::transdet_state(char letter, uint state) const
{
    if(letter >= NbLetters || state >= NbStates)
        throw runtime_error("Bad access to transdet_state");
    return _transdet_state[letter * NbStates + state];
}

char MultiCounterEpsAut::transdet_action(char letter, uint state) const
{
    if(letter >= NbLetters || state >= NbStates)
        throw runtime_error("Bad access to transdet_action");
    return _transdet_action[letter * NbStates + state];
}

uint * MultiCounterEpsAut::transdet_states(char letter) const
{
    if(letter >= NbLetters)
        throw runtime_error("Bad access to transdet_states");
    return _transdet_state + letter * NbStates;
}

char * MultiCounterEpsAut::transdet_actions(char letter) const
{
    if(letter >= NbLetters)
        throw runtime_error("Bad access to transdet_actions");
    return _transdet_action + letter * NbStates;
    
}

void MultiCounterEpsAut::print(ostream& st){

	st << (int)NbStates << " states " << (int)NbLetters << " letters" << endl;
	st << "Initial states: ";
	for (uint i = 0; i < NbStates; i++)
		if (initialstate[i])
			st << state_index_to_string(i) << " ";
	st << endl;
	st << "Final states: ";
	for (uint i = 0; i < NbStates; i++)
		if (finalstate[i])
			st << state_index_to_string(i) << " ";
	st << endl;


/*	uint** transdet_state;
	char** transdet_action;*/

	for (unsigned char a = 0; a<NbLetters; a++){
		st << "Letter " << (int)a << endl;
		for (uint i = 0; i<NbStates; i++){
			st << state_index_to_string(i) << " -- "
            << coef_to_string(transdet_action(a,i)) << " --> "
            << state_index_to_string(transdet_state(a,i)) << endl;
		}
		st << endl;
	}

    
    st << "Epsilon" << endl;
    for (uint i = 0; i<NbStates; i++)
	{
		st << state_index_to_string(i) << ": ";
		for (uint j = 0; j<NbStates; j++)
			st << coef_to_string(_trans_eps.get(i,j)) << " ";
		st << endl;
	}
}


//product of a deterministic transition table with a matrix 
MultiCounterMatrix * MultiCounterAut::prod_det_mat(
                                                   uint *  det_state,
                                                   char *  det_act,
                                                   const MultiCounterMatrix * mat2)
{
    ExplicitMatrix res(NbStates);
	for(uint i=0;i<NbStates;i++){
		uint k = det_state[i];
		uint act = det_act[i];
		for (uint j = 0; j<NbStates; j++){
			//compute res[i][j]
			if (k < NbStates)
                res.coefficients[i][j] = MultiCounterMatrix::get_act_prod(act,mat2->get(k,j)); //if k=NbStates it means no transition
			else
				;//throw runtime_error("Problem in prod_det_mat: k should be less than NbStates");
		}
	}
	return new MultiCounterMatrix(res, NbCounters);
}

MultiCounterEpsAut::~MultiCounterEpsAut()
{
    free(_transdet_state);
    free(_transdet_action);
    _transdet_state = NULL;
    _transdet_action = NULL;
}

//classical Automata with Epsilon-transitions
MultiCounterEpsAut::MultiCounterEpsAut(
                                       char Nletters,
                                       uint Nstates,
                                       char Ncounters
                                       )
    : MultiCounterAut(Nletters, Nstates, Ncounters)
{
    uint s = Nletters * Nstates;

    _transdet_state = (uint *) malloc(s * sizeof(uint));
    memset(_transdet_state, Nstates, s); //initialize with Nstates, meaning "no transition"

    _transdet_action = (char *) malloc(s * sizeof(char));
    memset(_transdet_action, MultiCounterAut::bottom(), s); //initialize with Nstates, meaning "no transition"
    
    ExplicitMatrix m(Nstates);
    for(int i = 0 ; i < Nstates; i++)
        for(int j = 0; j < Nstates; j++)
            m.coefficients[i][j] = (i==j)
            ? MultiCounterAut::epsilon()
            : MultiCounterAut::bottom();
    set_trans_eps(m);

    //_trans_eps = MultiCounterMatrix(&m);
}


//Epsilon removal in MultiCounter Automata
//We assume each state has an espilon-transtion to itself
//We assume letters are deterministic in epsaut
MultiCounterAut* EpsRemoval(MultiCounterEpsAut *epsaut){
	bool debug = false;

	uint ns=epsaut->NbStates;
	char nl=epsaut->NbLetters;
	char nc=epsaut->NbCounters;
	
	MultiCounterAut *aut=new MultiCounterAut(nl,ns,nc);
	
	//copy initial states and final states
	aut->initialstate=epsaut->initialstate;
	aut->finalstate=epsaut->finalstate;
	
	//stabilize epsilon-transitions
    
    auto eps = epsaut->get_trans_eps();
    //we dont want to delete eps
    auto prev_eps = (const MultiCounterMatrix *) new MultiCounterMatrix(eps);
    auto new_eps = eps * &eps;
    //new_eps->print();
    int steps = 0;
    while (! (*new_eps == *prev_eps))
    {
        /*
        if(debug) {
            cout << "Removing eps transitions step " << ++steps <<  endl;
            cout << "prev_eps" << endl;
            prev_eps->print();
            cout << endl;
            cout << "new_eps" << endl;
            new_eps->print();
            cout << endl;
        }
         */
        delete prev_eps;
        prev_eps = new_eps;
        new_eps =  (*new_eps) * &eps;
    }
    delete prev_eps;
    
	//update matrices of each letter : new_a=e*ae*
	for(char a=0;a<nl;a++){
		auto ae = epsaut->prod_det_mat(
                                       epsaut->transdet_states(a),
                                       epsaut->transdet_actions(a),
                                       new_eps
                                       );
        auto trans = (*new_eps) * (*ae);

        if(debug) {
            cout << "New eps " << endl;
            new_eps->print();
            cout << "AE " << endl;
            ae->print();
            cout << "trans " << endl;
            trans->print();
        }
        aut->set_trans(a, trans );

        //cleanup
        delete trans;
        delete ae;
	}
    delete new_eps;
	
	return aut;
}
