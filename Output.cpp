#include <sstream>
#include "Output.hpp"
const string start = "digraph G{\n"
	"rankdir = LR;overlap = false;\n"
	"labelloc=\"t\";\n";
const string initialStyle  = "node [shape = circle, color = navy, style=filled];";
const string finalStyle = "node [shape = doublecircle, color = lightblue, style=filled];";
const string initialFinalStyle = "node [shape = doublecircle, color = navy, style=filled];";
const string normalStyle = "node [shape = circle, color = lightblue, style = filled];";

int Dot::size;
int Dot::initialState;
const int* Dot::finalStates;
//TODO: Factor this out (with Test.cpp). These functions do not belong
//here
bool Dot::not_final(int s) 
{
	for(int i=0;i<size;i++)
		if(finalStates[i]==s)
			return false;
	return true;
}

bool Dot::test_witness(const ProbMatrix* m) 
{
	const Vector & ones = *((m->getRowOnes())[initialState]);
	for (int i = 0; i < Vector::GetStateNb(); i++)
		if(ones.contains(i)&&not_final(i))
			return false;
	return true;
}


string Dot::giveStyles ()
{
	string ret;
	int both = -1;
	int finalStatesNb=0;

	for(int i=0;i<size;i++)
	{
		if(finalStates[i]==initialState)
		{
			ret+=initialFinalStyle + std::to_string(initialState)+";\n";
			both = initialState;
		}
		if(finalStates[i]>=0)
			finalStatesNb++;
	}
	if(both < 0)
		ret += initialStyle + std::to_string(initialState) + ";\n";
	if(finalStatesNb>1 || (finalStatesNb==1 && both>=0))
	{
		ret+=finalStyle;
		for(int i=0;(i<size) && (finalStates[i]>=0);i++)
		{
			if(both>=0 && finalStates[i]==both)
				continue;
			ret+=std::to_string(finalStates[i]) + ";";
		}
		ret += "\n";
	}
	ret += normalStyle + "\n";
	return ret;
}

string Dot::toDot(const ExplicitAutomaton* a)
{
	string ret=start;
	ret += "label=\"Automaton\"\n";
	ret += giveStyles();
	for(int i=0;i<a->alphabet.length();i++)
		for(int j=0;j<size;j++)
			for(int k=0;k<size;k++)
				if( a->matrices[i]->coefficients[j][k] == 2 )
					ret+=std::to_string(j) + " -> " + std::to_string(k) + " [label = \""+a->alphabet[i]+"\"];\n";
	ret+="}\n";
	return ret;
}

string Dot::toDot(UnstableMarkovMonoid* m,const ExtendedExpression* e,const ProbMatrix* mat)
{
	string ret; 
	ostringstream s;
	s << *e;

	ret+="label=\""+s.str();

	if(mat->countLeaks(m->recurrence_classes(mat)))
		ret+=" leak";
	if(test_witness(mat))
		ret+=" value 1 witness";
	ret+="\";\n";
	ret+=giveStyles();
	
	for (int i = 0; i<size;i++)
	{
		const Vector & ones = *((mat->getRowOnes())[i]);
		const Vector & pluses = *((mat->getRowPluses())[i]);
		for (int j = 0; j < size; j++)
		{
			if(pluses.contains(j))
			{
				ret+=std::to_string(i) + " -> " + std::to_string(j);
				if(!ones.contains(j))
					ret+=" [label=\"+\"]";
				ret+=";\n";
			}
		}
	}
	ret+="}\n";
	return ret;
}

string Dot::toDot(Monoid* m)
{
	string ret;
	if(UnstableMarkovMonoid* u = dynamic_cast<UnstableMarkovMonoid*>(m))
	{
		for(auto & g : u->expr_to_mat)
		{
			ret += start;
			ret += toDot(u,g.first, (const ProbMatrix*) g.second);
		}
	}
	return ret;
}

string Dot::toDot(const ExplicitAutomaton* a, Monoid* m)
{
	string ret;
	size = a->size;
	initialState = a->initialState;
	finalStates = a->finalStates;
	ret += toDot(a);
	ret += toDot(m);
	return ret;
}
