#ifndef PARSER_HPP
#define PARSER_HPP
#include "ExplicitAutomaton.hpp"
#include <istream>

class Parser {
public: 
  static ExplicitAutomaton* parseFile(std::istream& file);
  static int linenumber;
};
#endif
