
#include <Expressions.hpp>
#include <VectorInt.hpp>
#include <string.h>
#include <iostream>

using namespace std;

// Class Vector
//For constructors
void VectorInt::init(){
  	coefs = (char *)malloc(entriesNb * sizeof(char));
	memset(coefs, (char)0, entriesNb * sizeof(char));
	update_hash();
}

// First constructor
VectorInt::VectorInt()
{
  init();
}


// Second constructor
VectorInt::VectorInt(const VectorInt & other)
{
	memcpy(coefs, other.coefs, entriesNb * sizeof(char));
	_hash = other.Hash();
}

// Third constructor
VectorInt::VectorInt(vector<char> data)
{
        init();
	for (int i = data.size() - 1; i >= 0; i--)
		coefs[i] = data[i];
	update_hash();
}

// Fourth constructor
VectorInt::VectorInt(char * data, bool copy)
{
	if (copy)
	{
		coefs = (char *)malloc(entriesNb * sizeof(char));
		memcpy(coefs, data, entriesNb * sizeof(char));
	}
	else
	{
		coefs = data;
	}
	update_hash();
};

void VectorInt::print(ostream & os) const
{
  // throw runtime_error("Unimplemented");
	/*
	for (uint i = 0; i < entriesNb; i++)
		os << (contains(i) ? "1" : "0");
	os << endl;
	*/
}

// Number of entries
uint VectorInt::entriesNb = 0;

VectorInt::~VectorInt()
{
	free(coefs);
	coefs = NULL;
}

bool VectorInt::operator==(const VectorInt & vec) const
{
	if (entriesNb != vec.entriesNb) return false;
	for (uint i = 0; i < entriesNb; i++)
		if (coefs[i] != vec.coefs[i]) return false;
	return true;
}
