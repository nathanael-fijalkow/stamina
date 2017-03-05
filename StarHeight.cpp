#include "StarHeight.hpp"
#include <fstream>


//functions for subsets computations
bool intersect(uint s1, uint s2) { return ((s1 & s2) != 0);}

uint addin(uint s, uint i){ return (s|TwoPow(i));}


//GraphAut constructor, from normal non-deterministic automaton
GraphAut::GraphAut(ClassicAut *aut){
	NbStates=aut->NbStates;
	order.clear();// should not be necessary but just in case
	trans.resize(NbStates,0);
	char a;
	uint i,j;
	bool msg=true, msgcomplete=true;
	for(a=0;a<aut->NbLetters;a++){
		for(i=0;i<NbStates;i++){
			bool found=false;
			for(j=0;j<NbStates;j++){
				if(aut->trans[a][i][j]){
					if (found && msg) {
						cout<<"WARNING: the input automaton is not deterministic !"<<endl;
						cout<<"The star-height algorithm with non-deterministic input for the complement language is not implemented yet."<<endl;
						cout<<"We let the algorithm run but it is not to be trusted."<<endl<<endl;
						msg=false;
					}
					trans[i]=addin(trans[i], j);
					found=true;
				}
			}
			if(!found && msgcomplete){
				cout<<"WARNING: the input automaton is not complete !"<<endl;
				cout<<"This can cause the algorithm to fail. Please add a sink state."<<endl<<endl;
				msgcomplete=false;
			}
		}
	}
}

//set of direct neighbours of s in the subgraph induced by subset
uint neighbours(GraphAut *aut, uint subset, uint s){
	uint res=0;
	uint n=aut->NbStates;
	uint i;
	for(i=0;i<n;i++){
		
		if (bit(s,i)) res=res|(aut->trans[i] & subset);		
	}
	return res;
}

/*
//returns a member of a nonempty set (smaller one)
uint member(uint subset){
	if(subset==0){printf("Error: member of empty set\n"); return 0;}
	uint i=0;
	uint temp=subset;
	while((temp & 1)==0){i++; temp=temp>>1;}
	return i;
}
*/

//auxiliary function for acyclicity of graph induced by a subset of states
bool acyclic(GraphAut *aut, uint subset){
	if(subset==0) return true;
	uint n=aut->NbStates;
	uint i=0;
	uint alive=subset;
	while(i<n & alive!=0) {alive=neighbours(aut,subset,alive);i++;}
	return (alive==0); // acyclic iff nothing is alive after n iterations
}

uint mindef(uint i,uint j){ 
	if (i==-1) return j;
	if (j==-1) return i;
	return min(i,j);
}

//recursive function for Tarjan Algorithm
void strongconnect(uint v, GraphAut *aut, uint *subset, uint *index, vector<int> *indexof, vector<int> *lowlink, vector<bool> *onStack, stack<int> *S, uint *remain, vector<uint> *comps){
	
	uint newcomp,w;
			
	// Set the depth index for v to the smallest unused index
	(*indexof)[v]=*index;
	(*lowlink)[v]=*index;
	do {(*index)++; *remain=*remain>>1;} while((*remain & 1)==0 & *index<aut->NbStates); // go to the next index
	S->push(v);
	(*onStack)[v] = true;
	
	// Consider successors of v
	w=0;
	while(bit(aut->trans[v] & *subset,w)==0 & w<aut->NbStates) w++;// first available neighbour of v
	while(w<aut->NbStates){
;
		
		if ((*indexof)[w]==-1){
		// Successor w has not yet been visited; recurse on it
			strongconnect(w,aut, subset, index, indexof, lowlink, onStack, S, remain,comps);
			(*lowlink)[v] = mindef((*lowlink)[v], (*lowlink)[w]);
		}else if ((*onStack)[w]){
			// Successor w is in stack S and hence in the current SCC
			(*lowlink)[v]  = mindef((*lowlink)[v], (*indexof)[w]);
		}
	
		do{w++;} while(bit(aut->trans[v] & *subset,w)==0); // next neighbour of v
	}

	// If v is a root node, pop the stack and generate an SCC
	if ((*lowlink)[v] == (*indexof)[v]){
	//start a new strongly connected component
	newcomp=0;
	do{
		w = S->top();
		S->pop();
		(*onStack)[w] = false;
		newcomp=addin(newcomp,w);
	}while (w != v);
	comps->push_back(newcomp);
	} 
}

//list the connected components of the graph restricted to subset, using Tarjan's algorithm
vector<uint> SCC(GraphAut *aut, uint subset){
	vector<uint> comps; //empty vector of components
	if(subset==0) return comps;
	uint n=aut->NbStates;
	uint index=0;
	uint remain=subset;
	vector<int> indexof(n,-1); //-1 stands for "undefined"
	vector<int> lowlink(n,-1);
	vector<bool> onStack(n,false);
	stack<int> S;

	while((remain & 1)==0 & index<n) {index++; remain=remain>>1;}
	uint remv=remain;
	uint v=index; //smallest available state
	//cout<< "v initialized to "<<v<<endl;
	while(v < n){
		if (indexof[v]==-1){
			strongconnect(v,aut, &subset, &index, &indexof, &lowlink, &onStack, &S, &remain, &comps);
		}
		do  {v++; remv=remv>>1;} while((remv & 1)==0 & v<n); //go to the next v available
	}
	return comps;

 } 


void printorder(list<uint> orderlist){
	list<uint>::iterator it;
	cout <<"order: ";
	for (it=orderlist.begin(); it!=orderlist.end(); ++it){
		 cout<< *it<<" ";
	}
	cout<<endl;
}
//recursive auxiliary function for Loop Complexity, on automaton induced by a subset.
char RecLC(GraphAut *aut, uint subset){
	//cout<< "Recursive call on subset "<<subset<<endl;
	if (acyclic(aut, subset)) {
		//add all elements of subset to the end of the order
		for(uint p=0;p<aut->NbStates;p++){	
			if (bit(subset,p)) aut->order.push_back(p);
		}
		return 0;
		}
	vector<uint> comps=SCC(aut,subset);
	list<uint> callorder=aut->order;
	list<uint> neworder;
	uint beststate;
	bool debug = false;
	if (debug) cout<< comps.size() << " components"<<endl;
	if (comps.size()==1){
		//compute 1+min(lc(A-p))
		if(debug) cout <<"Min step "<<subset<<endl;
		uint minloop=aut->NbStates+1;
		uint newmin;
		for(uint p=0;p<aut->NbStates;p++){
			if (bit(subset,p)){				
				if(debug) cout << "Trying cutting state " << p << " from "<< subset << endl;
				aut->order=callorder;
				newmin=RecLC(aut, subset-TwoPow(p));
				if(debug) cout << "The try " << p << " from " << subset << " gives " << newmin << endl;
				if (newmin<minloop) {
					minloop=newmin;
					neworder=aut->order;
//					(aut->order).push_back(p);
					beststate=p;
				} 
//				else {aut->order=callorder;}
			}
			if(minloop==0) break;
		}
		
		if(debug) cout <<"Min step "<<subset<<" returning "<<1+minloop<< " cutting state "<<beststate << endl;
		aut->order=neworder;
		aut->order.push_back(beststate);
		return 1+minloop;
	}
	//else max(lc(SCC)) 
	if(debug) cout <<"Max step"<<endl;
	vector<uint> vec=SCC(aut, subset);
	uint newmax, maxloop=0;
	for (vector<uint>::iterator it = vec.begin() ; it != vec.end(); ++it){ 
		newmax=RecLC(aut, *it);
		if(newmax>maxloop) maxloop=newmax;
	}
	if(debug) cout <<"Max step "<<subset<<" returns "<<maxloop<<endl;
	return maxloop;	
}

//final function for computing the loop complexity
pair<char,list<uint>> LoopComplexity(ClassicAut *aut){
	
	//for each set of states, we compute the loop complexity of the corresponding induced automaton.
	//This is done inductively according to the size of the set of states
	
	bool debug = false;

	if(debug) cout << "Computing the Loop Complexity..." << endl;
	uint n=aut->NbStates;	
	uint S=TwoPow(n); //number of subsets of the states. element S-1 represents the full automaton, element 0 the empty set.
	GraphAut *gaut=new GraphAut(aut);
	if(debug) cout << "Graph automaton created..." << endl;
	char lc = RecLC(gaut, S-1);
	if(debug) printorder(gaut->order);
	return pair<char, list<uint>>(lc, gaut->order);
}
		
MultiCounterAut * toNestedBaut(ClassicEpsAut *Subsetaut, char k){

	bool debug = true;


	uint ns=Subsetaut->NbStates;
	char nl=Subsetaut->NbLetters;
	
	// */

	//states of the resulting automaton are words of Q* of length in [1,k+1]
	//there are n+n^2+...+n^(k+1)=(n^(k+2)-n)/(n-1)
	//there are k+1 counters
	
	uint N = ns;
	for (uint i=0;i<k;i++){
		N=N*ns+ns;
	}
	if (ns==0) N=k + 1;
	
	if(debug) cout << "Computing the multicounter epsilon automaton... (" <<N<<" states)"<< endl;
	
	MultiCounterEpsAut* EpsBaut=new MultiCounterEpsAut(nl,N,k+1);

	//the last state on the pile is the remainder modulo ns

	//initial states are the ones from SubAut of length 1, i.e. the same
	for(uint i=0;i<ns;i++){ EpsBaut->initialstate[i]=(Subsetaut->initial==i); }
	
	//final states are the ones where the last state is final in Subsetaut
	uint p,w;
	for(uint i=0;i<N;i++){ 
		p = i % ns;
		if(Subsetaut->finalstate[p]) EpsBaut->finalstate[i]=true;
	}
	
	//TRANSITION table
	
	if(debug) cout << "Adding transitions..." << endl;

	uint l,bound;

	l=1;//length of the sequence
	bound=ns;//next time the length increases

    ExplicitMatrix trans_eps_mat(VectorInt::GetStateNb()); //stores eps trans
    trans_eps_mat.clear();
    for(int i = 0 ; i < VectorInt::GetStateNb(); i++)
        for(int j = 0; j < VectorInt::GetStateNb() ; j++)
            trans_eps_mat.coefficients[i][j] = MultiCounterMatrix::bottom();
    
    
   // trans_eps_mat.print();
    
	for(uint i=0;i<N;i++){
		if(i==bound)
		{
			l++;
			bound=bound*ns+ns;
		}
		p = i % ns;//current state
		w = i-p;

		/* NEW DEBUG , better labels for edges */

		int x = w / ns - 1;  //parent state
		char action;

		//si state de la forme upp
		if (l>1 && (x%ns == p))
		{ //alors reset
			action = MultiCounterMatrix::reset(l-1);
		}
		else{ //sinon increment
			action = MultiCounterMatrix::inc(l-1);
		}

		for (unsigned char a = 0; a < nl; a++)
		{
	    auto dd=Subsetaut->transdet[a][p];
            auto c =  (dd<ns) ? w + dd: 0; //going to state 0 with action bottom if undefined.
	    auto newaction=(dd<ns)?action:MultiCounterMatrix::bottom(); //if dd==ns it means that there is no transition, we express this with bottom.
	    
            EpsBaut->set_transdet_state(a,i, c);
			EpsBaut->set_transdet_action(a,i, newaction);
		}
		for (uint q = 0; q < ns; q++)
			if (Subsetaut->trans_eps[p][q])
				trans_eps_mat.coefficients[i][w + q] = action;

		int nouv = (ns > 1) ? (i + 1) * ns + p : i + 1;
		if (l < k + 1)
            trans_eps_mat.coefficients[i][nouv] = action; /*  EpsBaut->reset(l); */

		nouv = (ns > 1) ? x - (x % ns) + p : i - 1;
		//creates a reset 
		if (l > 1)
			trans_eps_mat.coefficients[i][nouv] = action; /* l - 1; */

	}
    //trans_eps_mat.print();
    EpsBaut->set_trans_eps(trans_eps_mat);

    /*
	if(debug){
		cout << "****************************************" << endl << "MultiCounterEpsAut " << endl << "****************************************" << endl;
		EpsBaut->print();
		ofstream file("multicountereps_automaton.txt");
		trans_eps_mat.print(file);
	}
     */

    ofstream file("multicountereps_stnb_"+ to_string(VectorInt::GetStateNb()) + ".txt");
    EpsBaut->print(file);
    
	if(debug) cout << "Removing epsilon transitions..." << endl;

	auto epsremoved = EpsRemoval(EpsBaut);
/*
	if(debug){
		cout << "****************************************" << endl << "Epsilon removed " << endl << "****************************************" << endl;
        ofstream file2("epsremoved_automaton.txt");
		epsremoved->print(file2);
	}*/

    ofstream file2("epsremoved_stnb_"+ to_string(VectorInt::GetStateNb()) + ".txt");
    epsremoved->print(file2);

	return epsremoved;
}
