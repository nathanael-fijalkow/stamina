#ifndef OUTPUT_HPP
#define OUTPUT_HPP
#include "ExplicitAutomaton.hpp"
#include "Monoid.hpp"
#include "MarkovMonoid.hpp"

class Dot
{
public:
  static string toDot(const ExplicitAutomaton*,Monoid*);
private:
  static int size;
  static int initialState;
  static const int* finalStates;
  
  static string giveStyles();
  static string toDot(const ExplicitAutomaton* a);
  static string toDot(UnstableMarkovMonoid* m,const ExtendedExpression* e,const ProbMatrix* mat);
  static string toDot(Monoid* m);

  static bool not_final(int s);
  static bool test_witness(const ProbMatrix* m); 

  Dot() {}
};
#endif
