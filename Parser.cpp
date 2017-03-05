
#include "Parser.hpp"
#include "Automata.hpp"
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
    cerr << "Syntax error at line: " << linenumber <<  endl << "Exception: " << string(e.what()) << endl;
    exit(-1);
  }
  return res;
}

ExplicitAutomaton* Parser::parseFile(std::istream &file) 
{
  string line;
  
  getfline(file,line);
  int size = stoi(line);

  int type;
  getline(file,line);
  if(!line.compare("p"))
    type=PROB;
  else {
    if (!line.compare("c"))
      type=CLASSICAL;
    else
      type=stoi(line);
  }
  
  string alphabet;
  getfline(file,alphabet);

  ExplicitAutomaton* ret = new ExplicitAutomaton(size,alphabet.length());
  ret->type = type;
  ret->alphabet = alphabet;

  getfline(file,line);
  try
    {
  ret->initialState = stoi(line);
  //	  cout << "Initial state: " << stoi(line) << endl;
    }
    catch(const exception & exc){ throw runtime_error("Error while parsing initial states, could not parse '" + line + "' to integer"); }
  
  getfline(file,line);
  istringstream iss(line);
  int id=0;
  for(int i=0;i<size;i++){
    getfline(iss,line,' ');
    if(!line.empty())
      try
	{
	  ret->finalStates[id]=stoi(line);
	  //	  cout << "Final state: " << i << ":"  << stoi(line) << endl;
	  id++;
	}
      catch(const exception & exc){ throw runtime_error("Error while parsing final states, could not parse '" + line + "' to integer"); }
    i++;
  }

  for(int i=0;i<alphabet.length();i++) {
    ExplicitMatrix* mat = new ExplicitMatrix(size);
    string lt;
    getfline(file,lt);
    for(int j=0;j<size;j++) {
      getfline(file,line);
      istringstream iss2(line);
      for(int k=0;k<size;k++) {
	getfline(iss2,line,' ');
          if(type <= 0)
              mat->coefficients[j][k]=lttoi(line);
          else
              mat->coefficients[j][k]=MultiCounterAut::coef_to_char(line,type);
          
      }
    }
    ret->matrices[i]=mat;
  }

  return ret;
}
