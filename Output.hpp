
#ifndef OUTPUT_HPP
#define OUTPUT_HPP
#include "ExplicitAutomaton.hpp"
#include "Monoid.hpp"
#include "MarkovMonoid.hpp"
#include "MultiMonoid.hpp"

class Dot
{
public:
  static string toDot(const ExplicitAutomaton*,Monoid*,int);
private:
  static int SH;
  static int size;
  static int initialState;
  static const int* finalStates;
  
  static string giveStyles();
  static string toDot(const ExplicitAutomaton* a);
  static string toDot(UnstableMultiMonoid* m, const ExtendedExpression* e, const MultiCounterMatrix* mat);
  static string toDot(UnstableMarkovMonoid* m,const ExtendedExpression* e,const ProbMatrix* mat);
  static string toDot(Monoid* m);

  static bool not_final(int s);
  static bool test_witness(const ProbMatrix* m); 

  Dot() {}
};
#endif
