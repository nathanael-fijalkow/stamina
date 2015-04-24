#include "RegExp.hpp"
#include <map>
#include <utility>

typedef uint state; 

using namespace std;

// Dynamic casts
const LetterRegExp * isLetter(const RegExp * expr) { return dynamic_cast<const LetterRegExp *>(expr); }
const ConcatRegExp * isConcat(const RegExp * expr){ return dynamic_cast<const ConcatRegExp *>(expr); }
const UnionRegExp * isUnion(const RegExp * expr){ return dynamic_cast<const UnionRegExp *>(expr); }
const StarRegExp * isStar(const RegExp *expr){ return dynamic_cast<const StarRegExp *>(expr); }


RegExp* concat(RegExp* lhs, RegExp* rhs)
{
	StarRegExp* stl = (StarRegExp*) isStar(lhs);
	StarRegExp* str = (StarRegExp*) isStar(rhs);

	if(stl && str && (*stl)==rhs)
		return stl;
	
	return new ConcatRegExp(lhs,rhs);
}
RegExp* add(RegExp* lhs, RegExp* rhs)
{
	if(!lhs)
		return rhs->clone();
	if(!rhs)
		return lhs->clone();
	if(*lhs == rhs)
		return lhs;
	
	StarRegExp* stl = (StarRegExp*) isStar(lhs);
	StarRegExp* str = (StarRegExp*) isStar(rhs);
	if(stl && !str && (*stl->base) == rhs)
		return stl;
	if(!stl && str && (*str->base) == lhs)
		return str;

	return new UnionRegExp(lhs,rhs);
}
RegExp* star(RegExp* base)
{
	if(isStar(base))
		return base; 
	return new StarRegExp(base);
}

// Printing functions
void LetterRegExp::print() const
{
	cout << (char)('a' + letter);
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
	left->print();
	cout<<"+";
	right->print();
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

RegExp* LetterRegExp::clone() const
{
	auto ret = new LetterRegExp(letter);
	ret->flat = flat;
	return ret;
}

RegExp* ConcatRegExp::clone() const
{
	auto ret = new ConcatRegExp(left->clone(),right->clone());
	ret->flat = flat;
	return ret;
}

RegExp* UnionRegExp::clone() const
{
	auto ret = new UnionRegExp(left->clone(),right->clone());
	ret->flat = flat;
	return ret;
}

RegExp* StarRegExp::clone() const
{
	auto ret = new StarRegExp(base->clone());
	ret->flat = flat;
	return ret;
}

LetterRegExp::LetterRegExp(char a)
{
	letter = a;
	flat = string(1,'a' + a);
}

ConcatRegExp::ConcatRegExp(RegExp *e1, RegExp *e2)
{
	left = e1;
	right = e2;
	flat = (e1->flat) + (e2->flat);
}

ConcatRegExp::~ConcatRegExp() 
{
	delete left; 
	delete right;
}
UnionRegExp::UnionRegExp(RegExp *e1, RegExp *e2)
{
	left = e1;
	right = e2;
	flat = "(" + e1->flat + "+" + e2->flat + ")";
}
UnionRegExp::~UnionRegExp() 
{
	delete left;
	delete right;
}

StarRegExp::StarRegExp(RegExp *e1)
{
	base = e1;
	if(isLetter(e1))
		flat = e1->flat + "*";
	else
		flat = "(" + e1->flat + ")*";
}

StarRegExp::~StarRegExp() 
{
	delete base; 
}

bool RegExp::operator ==(const RegExp* r) const
{
	if(!r) return false;
	return(flat == r->flat);
}

template<class T> bool contains(list<T> list, T element)
{
	for(T i : list)
		if(i == element)
			return true; 
	return false; 
}

void eliminateTrans(pair<state,state> t1, pair<state,state> t2,
		    state toEliminate, list<pair<state,state>> &toRemove,
		    map<pair<state,state>,RegExp*> &toReg) 
{
	if((t1.second == toEliminate) && (t2.first == toEliminate)) {
		toRemove.push_front(t1);
		toRemove.push_front(t2);
		auto pr = make_pair(t1.first,t2.second);
		auto tt = make_pair(toEliminate, toEliminate);

		if(toReg.find(tt) == toReg.end()) {
			if(toReg.find(pr) != toReg.end())
				toReg[pr] = add(toReg[pr],concat(toReg[t1],toReg[t2]));
			else 
				toReg[pr] = concat(toReg[t1],toReg[t2]);
		} 
		else {
			if(toReg.find(pr) != toReg.end()) 
				toReg[pr] = add(toReg[pr],
						concat(toReg[t1],
						       concat(star(toReg[tt]),toReg[t2])));
			else
				toReg[pr] = concat(toReg[t1],
						   concat(star(toReg[tt]),toReg[t2]));
									
		}
	}
}


// Borrowed from Charles' cpaut package:
// http://www.liafa.univ-paris-diderot.fr/~paperman/index.php?page=sage

// Returns a pointer to a regular expression that is equivalent with
// the automaton in, by eliminating the states in the order given. If
// it returns NULL, in has an empty language.
RegExp *Aut2RegExp(ClassicAut *in, list<state> order)
{
	map<pair<state,state>,RegExp*> toReg;
	list<pair<state,state>> toRemove;
	RegExp* ret = NULL;

	ClassicAut* aut = new ClassicAut(in->NbLetters, in->NbStates+2);
	state init = in->NbStates;
	state final = in->NbStates+1;

	for(int let = 0; let < in->NbLetters; let++) 
	  for(state s = 0; s < in->NbStates; s++)
	    for(state t = 0; t < in->NbStates; t++) {
	      aut->trans[let][s][t] = in->trans[let][s][t];
	      if(in->initialstate[t] && in->trans[let][t][s])
		aut->trans[let][init][s]=true;
	      if(in->finalstate[t] && in->trans[let][s][t])
		aut->trans[let][s][final]=true;
	      if(aut->trans[let][init][s] && aut->trans[let][s][final])
		aut->trans[let][init][final]=true;
	    }

	// For every transition (let,s1,s2) add to toReg the element
	// with index (s1,s2) and the regular expression corresponding
	// to letter 'let'
			
	for(int let=0; let < aut->NbLetters; let++) {
	  LetterRegExp* letExp = new LetterRegExp(let);
	  for(state s1=0; s1<aut->NbStates; s1++)
	    for(state s2=0; s2<aut->NbStates; s2++) {
	      if(aut->trans[let][s1][s2]) {
		auto spair = make_pair(s1,s2);
		auto toAdd = make_pair(spair,letExp);
		auto res = toReg.insert(toAdd);
		if(!res.second)
		  toReg[spair] = add(toReg[spair],letExp);
	      }
	    }
	}
	while (order.size() > 0) {
	  // Is this the good order?
	  state toEliminate = *(order.begin());
	  order.pop_front();
				
	  map<pair<state,state>,RegExp*> tempToReg(toReg);
	  toRemove.clear();
	  for(auto t1 : tempToReg)
	    for(auto t2 : tempToReg)
	      eliminateTrans(t1.first, t2.first, toEliminate,
			     toRemove, toReg);
	  
	  for (auto t : toRemove) 
	    toReg.erase(t);
	}
	auto i_i = make_pair(init,init);
	auto f_f = make_pair(final,final);
	auto i_f = make_pair(init,final);
	auto f_i = make_pair(final,init);
	if(toReg.find(i_f) == toReg.end())
	  return NULL; 
	if(init == final)
	  ret = add(ret,(star(toReg[i_i]))->clone());

	if(toReg.find(f_f) != toReg.end())
	  toReg[i_f] = concat(toReg[i_f],star(toReg[f_f]));
	if(toReg.find(i_i) != toReg.end())
	  toReg[i_f] = concat(star(toReg[i_i]),toReg[i_f]);
	if(toReg.find(f_i) != toReg.end())
	  ret = add(ret,
		    (concat(star(concat(toReg[i_f],toReg[f_i]))
			    ,toReg[i_f]))->clone());
	else
	  ret = add(ret, (toReg[i_f])->clone());
	return ret;
}	


ExtendedExpression *Reg2Sharp(const RegExp *reg){
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
		ExtendedExpression *exp1=Reg2Sharp(uexp->left);
		ExtendedExpression *exp2=Reg2Sharp(uexp->right);		
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
