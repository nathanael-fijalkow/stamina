
#include <sstream>
#include "Output.hpp"

const string start = "digraph G{\n"
	"rankdir = LR;overlap = false;\n"
	"labelloc=\"t\";\n";
const string initialStyle  = "node [shape = circle, color = navy, style=filled];";
const string finalStyle = "node [shape = doublecircle, color = lightblue, style=filled];";
const string initialFinalStyle = "node [shape = doublecircle, color = navy, style=filled];";
const string normalStyle = "node [shape = circle, color = lightblue, style = filled];";

int Dot::SH;
int Dot::size;
int Dot::initialState;
vector<int> Dot::finalStates;
//TODO: Factor this out (with Test.cpp). These functions do not belong
//here
bool Dot::not_final(int s) 
{
    for(auto st : finalStates)
        if(st == s)
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

    for(auto fs : finalStates) {
        if(fs == initialState)
        {
            ret+=initialFinalStyle + std::to_string(initialState)+";\n";
            both = initialState;
        }
    }
    finalStatesNb = finalStates.size();
    
	if(both < 0)
		ret += initialStyle + std::to_string(initialState) + ";\n";
	if(finalStatesNb>1 || (finalStatesNb==1 && both>=0))
	{
		ret+=finalStyle;
        for(auto fs : finalStates) {
            if(both == fs)
                continue;
            ret+=std::to_string(fs) + ";";
        }
		ret += "\n";
	}
	ret += normalStyle + "\n";
	return ret;
}

string Dot::toDot(const ExplicitAutomaton* a)
{
	string ret=start;
	if(SH == -1)
		ret += "label=\"Automaton\"\n";
	else
		ret += "label=\"Monoid for SH=" + std::to_string(SH) +"\"\n";
	ret += giveStyles();
	for(int i=0;i<a->alphabet.length();i++)
		for(int j=0;j<size;j++)
			for(int k=0;k<size;k++)
				if( a->matrices[i]->coefficients[j][k] == 2 )
					ret+=std::to_string(j) + " -> " + std::to_string(k) + " [label = \""+a->alphabet[i]+"\"];\n";
	ret+="}\n";
	return ret;
}

string Dot::toDot(UnstableMultiMonoid* m, const ExtendedExpression* e, const MultiCounterMatrix* mat)
{
	string ret; 
	ostringstream s;
	int elem;
	int n = MultiCounterMatrix::counterNb();
	s << *e;
  
	ret += "label=\""+s.str();
	if(UnstableMultiMonoid::IsUnlimitedWitness(mat))
		ret+=" unlimitedness witness";
	ret += "\";\n";
	ret += initialStyle;
	for (int s : m->initial_states)
		ret += std::to_string(s) + ";";
	ret += "\n" + finalStyle;
	for (int s : m->final_states)
		ret += std::to_string(s) + ";";
	ret += "\n" + normalStyle + "\n";
	for(int i = 0; i < VectorInt::GetStateNb(); i++) {
		for (int j =  0; j < VectorInt::GetStateNb(); j++) {
			elem = mat->get(i,j); 
			if(elem == 2 * n + 2)
				continue;
			ret += std::to_string(i) + " -> " + std::to_string(j);
			ret += " [label=\"";
			if (elem == 2 * n + 1) ret += "O";
			else if (elem == n) ret += "E";
			else if (elem < n) ret += "r" + to_string(elem);
			else ret += "i" + to_string(elem - n - 1);
			ret += "\"];\n";
		}
	}
	ret += "}\n";
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
	else if(UnstableMultiMonoid* u = dynamic_cast<UnstableMultiMonoid*>(m))
	{
		for(auto & g : u->expr_to_mat)
		{
			ret += start;
			ret += toDot(u,g.first, (const MultiCounterMatrix*) g.second);
		}
	}
	return ret;
}

string Dot::toDot(const ExplicitAutomaton* a, Monoid* m, int sh)
{
	string ret;
	size = a->size;
	initialState = a->initialState;
	finalStates = a->finalStates;
	SH = sh;
	ret += toDot(a);
	ret += toDot(m);
	return ret;
}
