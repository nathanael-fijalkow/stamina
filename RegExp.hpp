#ifndef REGEXP_HPP
#define REGEXP_HPP

#include "Automata.hpp"
#include "Expressions.hpp"
#include <map>
#include <utility>
#include <list>

#define VERBOSE_AUTOMATA_COMPUTATION 0
#define LOG_COMPUTATION_TO_FILE 1


using namespace std;


struct RegExp
{
    virtual ~RegExp() {}
    virtual void print() const=0;
};

struct LetterRegExp :RegExp
{
	LetterRegExp(char a) : letter(a) {};
	char letter;
	virtual void print() const;
};

struct ConcatRegExp : RegExp
{
    ConcatRegExp(RegExp *e1, RegExp *e2) : left(e1), right(e2) {}
    RegExp *left;
    RegExp *right;
    virtual void print() const;
};

struct UnionRegExp : RegExp
{
    UnionRegExp(RegExp *e1, RegExp *e2) : one(e1), two(e2) {}
    RegExp *one;
    RegExp *two;
    virtual void print() const;
};

struct StarRegExp : RegExp
{
    StarRegExp(RegExp *e1) : base(e1) {}
    RegExp *base;
    virtual void print() const;
};



// Dynamic casts to test the type of a regular expression
const LetterRegExp * isLetter(const RegExp * expr);
const ConcatRegExp * isConcat(const RegExp * expr);
const UnionRegExp * isUnion(const RegExp * expr);
const StarRegExp * isStar(const RegExp *expr);


RegExp* Aut2RegExp(ClassicAut *Aut, list<uint> order);

ExtendedExpression* Reg2Sharp(RegExp *reg);

#endif
