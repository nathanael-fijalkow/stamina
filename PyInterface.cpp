#include "Matrix.hpp"
#include "ProbMatrix.hpp"
#include "MarkovMonoid.hpp"
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

namespace PyInterface 
{
	class Monoid
	{
	public:
		Monoid(uint);
		~Monoid();
		void set_initial_state(int);
		void add_final_state(int);
		void add_letter(char, ExplicitMatrix&);
		int compute_monoid();
		void print();
	private:
		int dimension;
		int initialState;
		int* finalStates;
		UnstableMarkovMonoid* mon;
	};
	int dimension;
	int initialState;
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
		const Vector & ones = *((m->getRowOnes())[initialState]);
		for (int i = 0; i < Vector::GetStateNb(); i++)
			if(ones.contains(i)&&not_final(i))
				return false;
		return true;
	}
}

PyInterface::Monoid::Monoid(uint dim)
{
	dimension = dim;
	mon = new UnstableMarkovMonoid(dim);
	finalStates = (int*) malloc(sizeof(int)*dim);
	memset(finalStates,-1,sizeof(int)*dim);
	initialState = -1;
}
PyInterface::Monoid::~Monoid()
{
	delete mon;
	free(finalStates);
}
void PyInterface::Monoid::set_initial_state(int i)
{
	initialState = i;
}
void PyInterface::Monoid::add_final_state(int f)
{
	for(int i=0;i<dimension;i++) {
		if(finalStates[i]==-1) {
			finalStates[i]=f;
			break;
		}
	}
}
void PyInterface::Monoid::add_letter(char c, ExplicitMatrix& m)
{
	mon->addLetter(c,m);
}
int PyInterface::Monoid::compute_monoid()
{
	PyInterface::initialState = initialState;
	PyInterface::finalStates = finalStates;
	PyInterface::dimension = dimension;
	mon->setWitnessTest((bool (*)(const Matrix*))&PyInterface::test_witness);
	if(mon->ComputeMonoid())
		return 1;
	else
		return 0;
}

void PyInterface::Monoid::print()
{
	mon->print();
}
BOOST_PYTHON_MODULE(libacme)
{
	class_<vector<char>>("VChar")
		.def(vector_indexing_suite<vector<char>>());
	class_<vector<vector<char>>>("VVChar")
		.def(vector_indexing_suite<vector<vector<char>>>());

	class_<ExplicitMatrix>("MyMatrix", init<uint>())
		.def_readwrite("coefficients", &ExplicitMatrix::coefficients);
	class_<PyInterface::Monoid>("Monoid",init<uint>())
		.def("set_initial_state", &PyInterface::Monoid::set_initial_state)
		.def("add_final_state",&PyInterface::Monoid::add_final_state)
		.def("add_letter",&PyInterface::Monoid::add_letter)
		.def("compute_monoid",&PyInterface::Monoid::compute_monoid)
		.def("printmon",&PyInterface::Monoid::print);
}
