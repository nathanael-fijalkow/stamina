#include "Automata.hpp"

using namespace std;


void ClassicAut::init(char Nletters,uint Nstates){
	NbLetters=Nletters;
	NbStates=Nstates;
	
	initialstate=(bool *)malloc(Nstates*sizeof(bool));
	memset(initialstate, false, Nstates);
	
	finalstate=(bool *)malloc(Nstates*sizeof(bool));
	memset(finalstate, false, Nstates);
	
	trans=(bool***)malloc(Nletters*sizeof(bool **));
	
	for(int a=0;a<Nletters;a++){
		trans[a]=(bool**)malloc(Nstates*sizeof(bool *));
		for(int i=0;i<Nstates;i++){
			trans[a][i]=(bool*)malloc(Nstates*sizeof(bool));
			memset(trans[a][i], false, Nstates);
		}
	}
}

ClassicAut::ClassicAut(char Nletters,uint Nstates){
	init(Nletters,Nstates);
	
};

