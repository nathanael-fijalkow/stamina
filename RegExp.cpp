#include "RegExp.hpp"

using namespace std;





// Dynamic casts
const LetterRegExp * isLetter(const RegExp * expr) { return dynamic_cast<const LetterRegExp *>(expr); }
const ConcatRegExp * isConcat(const RegExp * expr){ return dynamic_cast<const ConcatRegExp *>(expr); }
const UnionRegExp * isUnion(const RegExp * expr){ return dynamic_cast<const UnionRegExp *>(expr); }
const StarRegExp * isStar(const RegExp *expr){ return dynamic_cast<const StarRegExp *>(expr); }



// Printing functions
void LetterRegExp::print() const
{
	cout << letter;
}

void ConcatRegExp::print() const
{
	left->print();
	right->print();
}

void UnionRegExp::print() const
{
	cout<<"(";
	one->print();
	cout<<"+";
	two->print();
	cout<<")";
}

void StarRegExp::print() const
{
	if (isLetter(base))
	{
		base->print();
		cout << "*";
	}
	else
	{
		cout << "(";
		base->print();
		cout << ")*";
	}
}



RegExp *Aut2RegExp(ClassicAut *Aut, list<uint> order){
	return NULL;
}	


ExtendedExpression *Reg2Sharp(RegExp *reg){
	
	return NULL;
}
