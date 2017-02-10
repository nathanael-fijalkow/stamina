
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
	// Check for (aa*)* = a*; (a*a)*=a*
	const ConcatRegExp* concat = isConcat(base);
	if(concat) {
	  const StarRegExp* lhs = isStar(concat->left);
	  const StarRegExp* rhs = isStar(concat->right);
	  if(lhs && !rhs && *(lhs->base) == rhs)
	    return (RegExp*) lhs;
	  if(!lhs && rhs && *(rhs->base) == lhs)
	    return (RegExp*) rhs;
	}

	return new StarRegExp(base);
}

// Printing functions
void LetterRegExp::print() const
{
	if(letter >= 0) cout << (char)('a' + letter);
	else cout << "1";
	// 1 is for epsilon
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

// Inspired from Charles' cpaut package:
// http://www.liafa.univ-paris-diderot.fr/~paperman/index.php?page=sage

// Returns a pointer to a regular expression that is equivalent with
// the automaton in, by eliminating the states in the order given. If
// it returns NULL, in has an empty language.

// There was a mistake in this algorithm
// some pairs were removed before they can be used for transitivity.
// this is fixed but the expressions produced seem more complicated.

RegExp *Aut2RegExp(ClassicAut *in, list<state> order)
{
	//to switch for debugging printing
	bool debug_aut2reg=false;
	map<pair<state,state>,RegExp*> toReg;
	list<pair<state,state>> toRemove;

	ClassicAut* aut = new ClassicAut(in->NbLetters, in->NbStates+2);
	state init = in->NbStates;
	state final = in->NbStates+1;

	aut->initialstate[init] = true ;
	aut->finalstate[final] = true ;
	
	bool eps_included = false ;
	for(state s = 0; s < in->NbStates; s++)	if(in->initialstate[s] && in->finalstate[s]) eps_included = true ;
	
	for(int let = 0; let < in->NbLetters; let++) 
	  for(state s = 0; s < in->NbStates; s++)
	    for(state t = 0; t < in->NbStates; t++) {
	      aut->trans[let][s][t] = in->trans[let][s][t];
	      if(in->initialstate[t] && in->trans[let][t][s]) aut->trans[let][init][s] = true;
	      if(in->finalstate[t] && in->trans[let][s][t]) aut->trans[let][s][final] = true;
	      //also add transitions directly from inital to final (added by denis)
		  //if(in->initialstate[s] && in->finalstate[t] && in->trans[let][s][t]) aut->trans[let][init][final] = true;
	    }

	if(debug_aut2reg) aut->print();

	// For every transition (let,s1,s2) add to toReg the element
	// with index (s1,s2) and the regular expression corresponding
	// to letter 'let'
			
	for(int let=0; let < aut->NbLetters; let++) {
	  LetterRegExp* letExp = new LetterRegExp(let);
	  for(state s1 = 0; s1 < aut->NbStates; s1++)
	    for(state s2 = 0; s2 < aut->NbStates; s2++) {
	      if(aut->trans[let][s1][s2]) {
			auto spair = make_pair(s1,s2);
			auto toAdd = make_pair(spair,letExp);
			auto res = toReg.insert(toAdd);
			if(debug_aut2reg) cout<<"Adding pair "<<s1<<"->"<<s2<<endl;
			if(!res.second)	toReg[spair] = add(toReg[spair],letExp);
		  }
	    }
	}

	int iter = 0 ;
	
	while (order.size() > 0) {
	 // debug printing
	 /* cout << "iteration : " << iter << endl;
	  for(state s1 = 0; s1 < aut->NbStates; s1++) {
	    for(state s2 = 0; s2 < aut->NbStates; s2++) {
			auto spair = make_pair(s1,s2);
			if(toReg.count(spair) != 0) {
				cout << s1 << "->" << s2 << " ";
				toReg[spair]->print();
				cout << endl;
			}
		}
	  }*/
	  
	  iter++;
	  
	  state toEliminate = *(order.begin());
	  if (debug_aut2reg) cout<< "Elimination of state "<<toEliminate<<endl;
	  order.pop_front();

		auto tt = make_pair(toEliminate,toEliminate);
		if(toReg.count(tt) != 0) {
			for(state s1 = 0; s1 < aut->NbStates; s1++) {
				auto s1t = make_pair(s1,toEliminate);
				if(s1 != toEliminate && toReg.count(s1t) != 0){
					for(state s2 = 0; s2 < aut->NbStates; s2++) {
						auto ts2 = make_pair(toEliminate,s2);
						auto spair = make_pair(s1,s2);
						if(s2 != toEliminate && toReg.count(ts2) != 0) {
							toReg[spair] = add(toReg[spair],concat(toReg[s1t],concat(star(toReg[tt]),toReg[ts2])));
							if (debug_aut2reg) cout << "Adding pair "<<s1<<"->"<<s2<<endl;				
						//  toReg.erase(ts2);
						//	if (debug_aut2reg) cout << "Removing pair "<<toEliminate<<"->"<<s2<<endl;
						}
					}
					//toReg.erase(s1t);
					//if (debug_aut2reg) cout << "Removing pair "<<s1<<"->"<<toEliminate<<endl;
				}
			}
			toReg.erase(tt);
		}
		else {
			for(state s1 = 0; s1 < aut->NbStates; s1++) {
				auto s1t = make_pair(s1,toEliminate);
				if(s1 != toEliminate && toReg.count(s1t) != 0) {
					for(state s2 = 0; s2 < aut->NbStates; s2++) {
						auto ts2 = make_pair(toEliminate,s2);
						auto spair = make_pair(s1,s2);
						if(s2 != toEliminate && toReg.count(ts2) != 0) {
							toReg[spair] = add(toReg[spair],concat(toReg[s1t],toReg[ts2]));
							if (debug_aut2reg) cout << "Adding pair "<<s1<<"->"<<s2<<endl;		
							//toReg.erase(ts2);
							//if (debug_aut2reg) cout << "Removing pair "<<toEliminate<<"->"<<s2<<endl;
						}
					}
					//toReg.erase(s1t);
					//if (debug_aut2reg) cout << "Removing pair "<<s1<<"->"<<toEliminate<<endl;
				}
			}
		}
		//Erase all pairs involving state to Eliminate	    
		//in the end instead of doing it on-the-fly
		for(state s = 0; s < aut->NbStates; s++) {
			auto st=make_pair(s,toEliminate);
			auto ts=make_pair(toEliminate,s);
			toReg.erase(st);
			toReg.erase(ts);
		}
	}

/*
	  for(state s1 = 0; s1 < aut->NbStates; s1++) {
	    for(state s2 = 0; s2 < aut->NbStates; s2++) {
			auto spair = make_pair(s1,s2);
			if(toReg.count(spair) != 0) {
				cout << s1 << "->" << s2 << " ";
				toReg[spair]->print();
				cout << endl;
			}
		}
	  }
*/

	auto i_f = make_pair(init,final);
	
	if(toReg.find(i_f) == toReg.end()) {
			// special case where init' did not link to the original initial state.
			//we assume here there is only one initial state.
			state sin;
			for (auto s=0; s<in->NbStates ; s++)
				{if (in->initialstate[s]) {sin=s;break;}}
			auto in_f=make_pair (sin, final);
			if(toReg.find(in_f) == toReg.end()) {
			//cout << "Empty language\n";
			return NULL; }
			else return toReg[in_f];
		}
	
/* we do not care about epsilon, does not change star height or help finding witness
	if(eps_included) {
		//cout << "Epsilon ignored in the expression\n";
		//LetterRegExp* ExpEpsilon = new LetterRegExp(-1);
		//RegExp* res = new UnionRegExp(ExpEpsilon,toReg[i_f]);
		//return res;
	}
*/
	return toReg[i_f];
}	


//We add a type of RegExp: Union given by a list instead of two sons. Useful for function RegExp-> SharpExprs
//we do it here because we'll only use it locally
struct UnionListRegExp : RegExp
{
	list<RegExp *> sons;
	UnionListRegExp(list<RegExp *> elist){
		sons=elist;
		}
	~UnionListRegExp(){
		for(RegExp *e: sons)
			delete e;
	}
	virtual void print() const {}
	virtual RegExp* clone() const {}
};

//dynamic cast
const UnionListRegExp * isUnionList(const RegExp *expr){ return dynamic_cast<const UnionListRegExp *>(expr); }

//auxiliary functions for regexp->sharpexp


//return the list of summand terms in an expression (singleton if not sum)
list<RegExp *> RegTerms(RegExp *reg){
	const UnionRegExp *uexp=isUnion(reg);
	if (uexp!=NULL){ 
		//binary union;		
		list<RegExp *> list1=RegTerms(uexp->left);
		list<RegExp *> list2=RegTerms(uexp->right);
		list1.insert(list1.end(),list2.begin(),list2.end());
		return list1;
	}
	const UnionListRegExp *lexp=isUnionList(reg);
	if (lexp!=NULL){ 
		//union list
		list<RegExp *> reslist;
		for(RegExp *e: lexp->sons){
			list<RegExp *> el=RegTerms(e);
			reslist.insert(reslist.end(),el.begin(),el.end());
		}
		return reslist;
	}
	//not a union, return the singleton expression
	list<RegExp *> reslist;
	reslist.push_back(reg);
	return reslist;
}

//expand subexpressions: a(b+c)->ab+ac
RegExp *ExpandReg(RegExp *reg){
	const LetterRegExp *lexp=isLetter(reg);
	if (lexp!=NULL) {
		return reg;
		}
	const UnionRegExp *uexp=isUnion(reg);
	if (uexp!=NULL){
		RegExp *exp1=ExpandReg(uexp->left);
		RegExp *exp2=ExpandReg(uexp->right);		
		UnionRegExp *res=new UnionRegExp(exp1,exp2);
		return res;
	}
	const UnionListRegExp *listexp=isUnionList(reg);
	if (listexp!=NULL){
		list <RegExp*> newsons;
		for(RegExp *e: listexp->sons){
			newsons.push_back(ExpandReg(e));
		}
		UnionListRegExp *res=new UnionListRegExp(newsons);
		return res;
	}
	const ConcatRegExp *cexp=isConcat(reg);	
	if (cexp!=NULL){				
		list<RegExp *> list1=RegTerms(ExpandReg(cexp->left));
		list<RegExp *> list2=RegTerms(ExpandReg(cexp->right));
		list<RegExp *> sonlist;
		//expand the product of two lists
		for(RegExp *e : list1){
				for(RegExp *f : list2){
						ConcatRegExp *ef=new ConcatRegExp(e,f);	
						sonlist.push_back(ef);
				}
			}
		UnionListRegExp *res=new UnionListRegExp(sonlist);
		return res;	
	}
	const StarRegExp *sexp=isStar(reg);
	if (sexp!=NULL){
		StarRegExp *res=new StarRegExp(ExpandReg(sexp->base));
		return res;
	}
	return NULL;
}

ExtendedExpression *Reg2Sharp(const RegExp *reg){
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
		if (s1>=s2) return exp1;
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
