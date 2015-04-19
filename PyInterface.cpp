#include "Matrix.hpp"
#include "ProbMatrix.hpp"
#include "MarkovMonoid.hpp"
#include "MultiMonoid.hpp"
#include "StarHeight.hpp"
#include "Automata.hpp"
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <list>
using namespace boost::python;

namespace PyInterface 
{
	class Monoid
	{
	public:
		Monoid(uint,uint);
		~Monoid();
		void add_initial_state(int);
		void add_final_state(int);
		void add_letter(char, ExplicitMatrix&);
		int has_val1();
		int starheight();
		string toregexp();
		void print();
	private:
		int dimension;
		int nbLetters;
		int cur_let=0;
		int* initialStates;
		int* finalStates;
		ExplicitMatrix** matrices;
		char* letters;
	};
	int dimension;
	int* initialStates;
	int* finalStates;
	bool not_final(int s) 
	{
		for(int i=0;i<dimension;i++)
			if(finalStates[i]==s)
				return false;
		return true;
	}

	bool test_witness(const ProbMatrix* m) 
	{
		for(int j=0;j<dimension && initialStates[j]!=-1;j++) {
			const Vector & ones = *((m->getRowOnes())[initialStates[j]]);
			for (int i = 0; i < Vector::GetStateNb(); i++)
				if(ones.contains(i)&&not_final(i))
					return false;
		}
		return true;
	}
}

PyInterface::Monoid::Monoid(uint dim,uint nlet)
{
	nbLetters = nlet;
	dimension = dim;
	finalStates = (int*) malloc(sizeof(int)*dim);
	memset(finalStates,-1,sizeof(int)*dim);
	initialStates = (int*) malloc(sizeof(int)*dim);
	memset(initialStates,-1,sizeof(int)*dim);

	matrices = (ExplicitMatrix**) malloc(sizeof(ExplicitMatrix*)*nlet);
	for(int i = 0;i < nlet; i++)
		matrices[i]=new ExplicitMatrix(dim);
	letters = (char*) malloc(sizeof(char)*nlet);
}
PyInterface::Monoid::~Monoid()
{
	free(finalStates);
	free(letters);
	for(int i = 0; i < nbLetters; i++)
		delete matrices[i];
	free(matrices);
}

void addToList(int dimension,int s, int* states) 
{
	for(int i=0;i<dimension;i++) {
		if(states[i]==-1) {
			states[i]=s;
			return;
		}
	}
}

void PyInterface::Monoid::add_initial_state(int i)
{
	addToList(dimension,i,initialStates);
}
void PyInterface::Monoid::add_final_state(int f)
{
	addToList(dimension,f,finalStates);
}
void PyInterface::Monoid::add_letter(char c, ExplicitMatrix& m)
{
	ExplicitMatrix* mat = new ExplicitMatrix(m);

	letters[cur_let]=c;
	matrices[cur_let]=mat;
	cur_let++;
}
int PyInterface::Monoid::has_val1()
{
	PyInterface::initialStates = initialStates;
	PyInterface::finalStates = finalStates;
	PyInterface::dimension = dimension;

	UnstableMarkovMonoid* markovMon = new UnstableMarkovMonoid(dimension);
	for(int i=0; i<cur_let;i++) 
		markovMon->addLetter(letters[i],*(matrices[i]));

	markovMon->setWitnessTest((bool (*)(const Matrix*))&PyInterface::test_witness);
	int ret = 0;
	if(markovMon->ComputeMonoid())
		ret = 1;
	delete markovMon;
	return ret;
}

int PyInterface::Monoid::starheight()
{
	int height=0;
	ClassicAut* automaton = new ClassicAut(nbLetters,dimension);
	for(int i = 0;i < dimension && initialStates[i]!=-1; i++)
		automaton->initialstate[initialStates[i]]=true;
	for(int i = 0;i < dimension && finalStates[i]!=-1; i++)
		automaton->finalstate[finalStates[i]]=true;
	for(int i=0; i<cur_let; i++)
		automaton->addLetter(i,*(matrices[i]));

	while(true) {
		MultiCounterAut *baut = toNestedBaut(automaton,height);
		UnstableMultiMonoid monoid(*baut);
		if(!monoid.containsUnlimitedWitness())
			return height;
		delete baut;
		height++;
	}
}
string PyInterface::Monoid::toregexp()
{
	ClassicAut* automaton = new ClassicAut(nbLetters,dimension);
	for(int i = 0;i < dimension && initialStates[i]!=-1; i++)
		automaton->initialstate[initialStates[i]]=true;
	for(int i = 0;i < dimension && finalStates[i]!=-1; i++)
		automaton->finalstate[finalStates[i]]=true;
	for(int i=0; i<cur_let; i++)
		automaton->addLetter(i,*(matrices[i]));
	std::list<uint> order;
	for(int i = 0;i < dimension; i++)
		order.push_front(i);
	return (Aut2RegExp(automaton,order))->flat;
}
BOOST_PYTHON_MODULE(libacme)
{
	class_<vector<char>>("VChar")
		.def(vector_indexing_suite<vector<char>>());
	class_<vector<vector<char>>>("VVChar")
		.def(vector_indexing_suite<vector<vector<char>>>());

	class_<ExplicitMatrix>("MyMatrix", init<uint>())
		.def_readwrite("coefficients", &ExplicitMatrix::coefficients);
	class_<PyInterface::Monoid>("Monoid",init<uint,uint>())
		.def("add_initial_state", &PyInterface::Monoid::add_initial_state)
		.def("add_final_state",&PyInterface::Monoid::add_final_state)
		.def("add_letter",&PyInterface::Monoid::add_letter)
		.def("has_val1",&PyInterface::Monoid::has_val1)
		.def("starheight",&PyInterface::Monoid::starheight)
		.def("toregexp",&PyInterface::Monoid::toregexp);
}
