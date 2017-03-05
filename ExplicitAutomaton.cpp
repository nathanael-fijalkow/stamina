
#include "ExplicitAutomaton.hpp"
#include <stdlib.h>

ExplicitAutomaton::ExplicitAutomaton(int size,int alphabet_length) {
  this->size = size;
  this->matrices = (ExplicitMatrix **) malloc(sizeof(ExplicitMatrix*)*alphabet_length);
}
ExplicitAutomaton::~ExplicitAutomaton() {
  delete this->matrices;
}
