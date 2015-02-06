#include "ExplicitAutomaton.hpp"
#include <stdlib.h>

ExplicitAutomaton::ExplicitAutomaton(int size,int alphabet_length) {
  this->size = size;
  this->finalStates = (int*) malloc(sizeof(int)*size);
  memset(this->finalStates,-1,sizeof(int)*size);
  this->matrices = (ExplicitMatrix **) malloc(sizeof(ExplicitMatrix*)*alphabet_length);
}
ExplicitAutomaton::~ExplicitAutomaton() {
  delete this->finalStates;
  delete this->matrices;
}
