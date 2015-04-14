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
	/* multiple sons variant
	uint i;
	cout<<"(";
	sons[0]->print();
	for(i=1;i<sonsNb;i++){
		cout<<".";
		sons[i]->print();
		}
	cout<<")";
	*/
}

void UnionRegExp::print() const
{
	cout<<"(";
	one->print();
	cout<<"+";
	two->print();
	cout<<")";
	
	/* multiple sons variant
	 * uint i;
	cout<<"(";
	sons[0]->print();
	for(i=1;i<sonsNb;i++){
		cout<<"+";
		sons[i]->print();
		}
	cout<<")";
	*/
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
	 //TODO with Charles function+parsing
	return NULL;
}	


ExtendedExpression *Reg2Sharp(RegExp *reg){
	uint i;
	ExtendedExpression *exp;
	const LetterRegExp *lexp=isLetter(reg);
	if (lexp!=NULL) {
		LetterExpr *res= new LetterExpr(lexp->letter);
		return res;
		}
	const UnionRegExp *uexp=isUnion(reg);
	if (uexp!=NULL){
		/*multiple sons int maxs=-1,temp;
		ExtendedExpression  *maxexp;
		for(i=0;i<uexp->sonsNb;i++){
			exp=Reg2Sharp(uexp->sons[i]);
			temp=exp->sharp_height() ;
			if (temp>maxs) {maxs=temp;maxexp=exp;}
		}
		return maxexp;
		*/
		ExtendedExpression *exp1=Reg2Sharp(uexp->one);
		ExtendedExpression *exp2=Reg2Sharp(uexp->two);		
		char s1=exp1->sharp_height();
		char s2=exp2->sharp_height();
		if (s1>s2) return exp1;
		else return exp2;
	}
	const ConcatRegExp *cexp=isConcat(reg);	
	if (cexp!=NULL){
		
		/*multiple sons
		for(i=0;i<cexp->sonsNb;i++){
			exp=Reg2Sharp(cexp->sons[i]);
			temp=exp->sharp_height() ;
			if (temp>maxs) {maxs=temp;maxexp=exp;}
		}
		*/
		
		ExtendedExpression *exp1=Reg2Sharp(cexp->left);
		ExtendedExpression *exp2=Reg2Sharp(cexp->right);
		ConcatExpr *res=new ConcatExpr(exp1,exp2);
		return res;	
		
	}
	const StarRegExp *sexp=isStar(reg);
	if (sexp!=NULL){
		SharpedExpr *res=new SharpedExpr(Reg2Sharp(sexp->base));
		return res;
	}
	return NULL;
}
