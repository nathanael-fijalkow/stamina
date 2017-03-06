
#include "ExplicitAutomaton.hpp"
#include <stdlib.h>

ExplicitAutomaton::ExplicitAutomaton(int size,int alphabet_length) {
  this->size = size;
    for(int letter = 0 ; letter < alphabet_length; letter++) {
        matrices.push_back(size);
        alphabet.push_back('a'+letter);
    }
    for(int letter = 0 ; letter < alphabet_length; letter++) {
        matrices[letter].clear(0);
    }
}
ExplicitAutomaton::~ExplicitAutomaton() {
}
