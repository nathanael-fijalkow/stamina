#include "Parser.hpp"
#include "MarkovMonoid.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <exception>
using namespace std;
int linenumber=0;
istream& getfline(istream& is,string& str, char delim='\n')
{
  do {
    is.peek();
    getline(is,str,delim);
    if(delim!=' ')
      linenumber++;
  } while(str.empty()||str[0]=='%');

  return is;
}

int lttoi(string s)
{
  if(!s.compare("_"))
    return 0;
  int res; 
  try {
    res = stoi(s)+1;
  }
  catch(exception& e) {
    cerr << "Syntax error at line: " << linenumber << endl;
    exit(-1);
  }
}

Monoid* Parser::parseFile(std::istream &file) 
{
  string line;
  
  getfline(file,line);
  int size = stoi(line);

  int type;
  getline(file,line);
  if(!line.compare("p"))
    type=0;
  else
    type=stoi(line);
  
  string alphabet;
  getfline(file,alphabet);

  int initialState;
  getfline(file,line);
  initialState = stoi(line);

  int finalStates[size];
  getfline(file,line);
  istringstream iss(line);
  for(int i=0;i<size;i++){
    getfline(iss,line,' ');
    if(!line.empty())
      finalStates[i]=stoi(line);
    i++;
  }

  ExplicitMatrix mat(size);
  UnstableMarkovMonoid* monoid = new UnstableMarkovMonoid(size);
  for(int i=0;i<alphabet.length();i++) {
    string lt;
    getfline(file,lt);
    for(int j=0;j<size;j++) {
      getfline(file,line);
      istringstream iss2(line);
      for(int k=0;k<size;k++) {
	getfline(iss2,line,' ');
	mat.coefficients[j*size+k]=lttoi(line);
      }
    }
    monoid->addLetter(lt[0],mat);
  }
  return monoid;
}
