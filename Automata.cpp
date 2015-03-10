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




//classical Automata with Epsilon-transitions
ClassicEpsAut::ClassicEpsAut(char Nletters,uint Nstates) : ClassicAut(Nletters,Nstates)
{
trans_eps=(bool**)malloc(Nstates*sizeof(bool *));
		for(int i=0;i<Nstates;i++){
			trans_eps[i]=(bool*)malloc(Nstates*sizeof(bool));
			memset(trans_eps[i], false, Nstates);
		}
		
transdet=(uint**)malloc(Nletters*sizeof(uint *));
		for(char a=0;a<Nletters;a++){
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


	for (char a = 0; a<NbLetters; a++){
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


	for (char a = 0; a<NbLetters; a++){
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
//It also works if a nondeterminstic automaton for the complement language is given, with final states reversed.
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

	//final states are subsets where all states are final state. This is because we actually powerset the automaton for the complement.
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
	for(char a=0;a<aut->NbLetters;a++){
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
			for(char a=0;a<aut->NbLetters;a++){		
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
	for(char a=0;a<aut->NbLetters;a++){
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


//Multi-counter Automata
void MultiCounterAut::init(char Nletters,uint Nstates, char Ncounters){
	NbLetters=Nletters;
	NbStates=Nstates;
	NbCounters=Ncounters;
	
	initialstate=(bool *)malloc(Nstates*sizeof(bool));
	memset(initialstate, false, Nstates);
	
	finalstate=(bool *)malloc(Nstates*sizeof(bool));
	memset(finalstate, false, Nstates);
	
	trans=(char***)malloc(Nletters*sizeof(char **));
	
	for(int a=0;a<Nletters;a++){
		trans[a]=(char**)malloc(Nstates*sizeof(char *));
		for(int i=0;i<Nstates;i++){
			trans[a][i]=(char*)malloc(Nstates*sizeof(char));
			memset(trans[a][i], 2*Ncounters+2, Nstates);
		}
	}
	
	char N=Ncounters;
	act_prod = (char **)malloc((2 * N + 3)  *  sizeof(char*));
	for (uint i = 0; i < (2 * N + 3); i++){
		act_prod[i] = (char *)malloc((2 * N + 3)  *  sizeof(char));
		for (uint j = 0; j < (2 * N + 3); j++){
			act_prod[i][j] =
				/* rule for infinite */
				(is_bottom(i) || is_bottom(j)) ? bottom()
				/* rule for unbounded */
				: (is_omega(i) || is_omega(j)) ? omega()
				/* rule for unbounded */
				: ((is_reset(i) && is_reset(j)) || (is_inc(i) && is_inc(j))) ? (i < j ? i : j)
				: (i <= N && N < j && i < j - N) ? i
				: (i <= N && N < j && i >= j - N) ? j
				: (j <= N && N < i && j < i - N) ? j
				: /*(j <= N & N < i & j > i - N) ?*/ i;
//			cout << elementToString(i) << "." << elementToString(j) << "=" << elementToString(act_prod[i][j]) << endl;
		}
	}
	
}

//Constructor
MultiCounterAut::MultiCounterAut(char Nletters,uint Nstates, char Ncounters){
	init(Nletters,Nstates,Ncounters);
	
}

string MultiCounterAut::elementToString(char element)
{
	char N = NbCounters;
	string result = "";
	if (element == 2 * N + 2) result = "_";
	else if (element == 2 * N + 1) result = "O";
	else if (element == N) result = "E";
	else if (element < N) result = "R" + to_string(element);
	else result = "I" + to_string(element - N - 1);
	if (result.size() <= 1) result.push_back(' ');
	return result;
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
				st << elementToString(trans[a][i][j]) << " ";
			}
			st << endl;
		}
		st << endl;
	}
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

	for (char a = 0; a<NbLetters; a++){
		st << "Letter " << (int)a << endl;
		for (uint i = 0; i<NbStates; i++){
			st << state_index_to_string(i) << " -- " << elementToString(transdet_action[a][i]) << " --> " << state_index_to_string(transdet_state[a][i]) << endl;
		}
		st << endl;
	}

	st << "Epsilon" << endl;
	for (uint i = 0; i<NbStates; i++)
	{
		st << state_index_to_string(i) << ": ";
		for (uint j = 0; j<NbStates; j++)
			st << elementToString(trans_eps[i][j]) << " ";
		st << endl;
	}
}

//product of matrices. No need to optimize as in the monoid because this won't be done too many times.
char** MultiCounterAut::prod_mat(char** mat1, char **mat2){
	char best_act;
	char **res=(char **)malloc(NbStates*sizeof(char *));
	for(uint i=0;i<NbStates;i++){
		res[i]=(char *)malloc(NbStates*sizeof(char));
		for(uint j=0;j<this->NbStates;j++){
			//compute res[i][j]
			best_act=2*NbCounters+2;
			for(uint k=0;k<NbStates;k++){
				best_act=min( best_act , act_prod[mat1[i][k]][mat2[k][j]]);
			}
			res[i][j]=best_act;
		}
	}
	return res;
}


//product of a deterministic transition table with a matrix 
char** MultiCounterAut::prod_det_mat(uint *det_state, char *det_act, char** mat2 ){
	char **res=(char **)malloc(NbStates*sizeof(char *));
	for(uint i=0;i<NbStates;i++){
		res[i]=(char *)malloc(NbStates*sizeof(char));
		uint k = det_state[i];
		uint act = det_act[i];
		for (uint j = 0; j<NbStates; j++){
			//compute res[i][j]
			if (k < NbStates)
				res[i][j] = act_prod[act][mat2[k][j]]; //if k=NbStates it means no transition
			else
				throw runtime_error("Problem in prod_det_mat: k should be less than NbStates");
		}
	}
	return res;
}

//classical Automata with Epsilon-transitions
MultiCounterEpsAut::MultiCounterEpsAut(char Nletters,uint Nstates, char Ncounters) : MultiCounterAut(Nletters,Nstates,Ncounters)
{
trans_eps=(char**)malloc(Nstates*sizeof(char *));
		for(int i=0;i<Nstates;i++){
			trans_eps[i]=(char*)malloc(Nstates*sizeof(char));
			memset(trans_eps[i], 2*Ncounters+2, Nstates);
		}
		
		
transdet_state=(uint**)malloc(Nletters*sizeof(uint *));
		for(char a=0;a<Nletters;a++){
			transdet_state[a]=(uint*)malloc(Nstates*sizeof(uint));
			memset(transdet_state[a], Nstates, Nstates); //initialize with Nstates, meaning "no transition"
		}

transdet_action=(char**)malloc(Nletters*sizeof(char *));
		for(char a=0;a<Nletters;a++){
			transdet_action[a]=(char*)malloc(Nstates*sizeof(char));
			memset(transdet_action[a], 2*Ncounters+2, Nstates);
		}

			
}

//equality of matrices
bool equal_mat(char ** mat1,char ** mat2, uint N){
	bool eq=true;
	uint i=0;
	while(eq && i<N){
		uint j=0;
		while(eq && j<N){
			eq=(mat1[i][j]==mat2[i][j]);
			j++;
			}
		i++;
	}
	return eq;
 }

 
//Epsilon removal in MultiCounter Automata
//We assume each state has an espilon-transtion to itself
//We assume letters are deterministic in epsaut
MultiCounterAut* EpsRemoval(MultiCounterEpsAut *epsaut){
	uint ns=epsaut->NbStates;
	char nl=epsaut->NbLetters;
	char nc=epsaut->NbCounters;
	
	MultiCounterAut *aut=new MultiCounterAut(nl,ns,nc);
	
	//copy initial states and final states
	aut->initialstate=epsaut->initialstate;
	aut->finalstate=epsaut->finalstate;
	
	//stabilize epsilon-transitions 
	char **eps=epsaut->trans_eps;
	char** new_eps=epsaut->prod_mat(eps,eps);
	char** prev_eps=eps;
	int steps = 0;
	while (!equal_mat(new_eps,prev_eps,ns)){
		cout << "Removing eps transitions step " << ++steps <<  endl;
		prev_eps=new_eps;
		new_eps=epsaut->prod_mat(new_eps,eps);
	}
	
	//update matrices of each letter : new_a=e*ae*
	char ** ae;
	for(char a=0;a<nl;a++){
		ae=epsaut->prod_det_mat(epsaut->transdet_state[a],epsaut->transdet_action[a],new_eps);
		aut->trans[a]=epsaut->prod_mat(new_eps,ae);
	}
	
	return aut;
}
