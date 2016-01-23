
#ifndef EXPLICITAUTOMATON_HPP
#define EXPLICITAUTOMATON_HPP

#include "Matrix.hpp"

#define PROB 0
#define CLASSICAL -1
class ExplicitAutomaton {
public:
  int type; // -1 classical, 0 probabilistic, n n-counter 
  int size;
  string alphabet;
  int initialState;
  int* finalStates;
  ExplicitMatrix** matrices; 
  ExplicitAutomaton(int size,int alphabet_length);
  ~ExplicitAutomaton();
};
#endif
