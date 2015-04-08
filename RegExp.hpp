#ifndef REGEXP_HPP
#define REGEXP_HPP

#include "Automata.hpp"
#include <map>
#include <utility>
#include <list>

//#include "match.hpp"       

#define VERBOSE_AUTOMATA_COMPUTATION 0
#define LOG_COMPUTATION_TO_FILE 1

using namespace std;

struct RegExp
{
    virtual ~RegExp() {}
};

struct LetterRegExp :RegExp
{
	LetterRegExp(char a) : letter(a) {};
	char letter;
};

struct ConcatRegExp : RegExp
{
    ConcatRegExp(RegExp *e1, RegExp *e2) : left(e1), right(e2) {}
    RegExp *left;
    RegExp *right;
};

struct UnionRegExp : RegExp
{
    UnionRegExp(RegExp *e1, RegExp *e2) : one(e1), two(e2) {}
    RegExp *one;
    RegExp *two;
};

struct SharpRegExp : RegExp
{
    SharpRegExp(RegExp *e1) : base(e1) {}
    RegExp *base;
};


RegExp Aut2RegExp(ClassicAut *Aut, list<uint> order);

#endif
