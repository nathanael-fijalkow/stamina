#ifndef PARSER_HPP
#define PARSER_HPP
#include "Monoid.hpp"
#include <istream>
class Parser {
public: 
  static Monoid* parseFile(std::istream& file);
  static int linenumber;
};
#endif
