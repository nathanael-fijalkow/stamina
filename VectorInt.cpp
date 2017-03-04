
#include <Expressions.hpp>
#include <VectorInt.hpp>
#include <string.h>
#include <iostream>

using namespace std;

// Class Vector
//For constructors
void VectorInt::init(){
  	coefs = (unsigned char *)malloc(entriesNb * sizeof(char));
	memset(coefs, (char)0, entriesNb * sizeof(char));
}

// First constructor
VectorInt::VectorInt()
{
  init();
  update_hash();
#if USE_MIN_HEURISTIC
    min  = 0;
#endif
}

//set Vector size
void VectorInt::SetSize(uint size)
{
    if(entriesNb != size) {
        int_vectors.clear();
        entriesNb = size;
    }
}


// Second constructor
VectorInt::VectorInt(const VectorInt & other)
{
	memcpy(coefs, other.coefs, entriesNb * sizeof(char));
	_hash = other.Hash();
#if USE_MIN_HEURISTIC
    min  = other.min;
#endif
}

// Third constructor
VectorInt::VectorInt(vector<char> data)
{
        init();
#if USE_MIN_HEURISTIC
    min = -1;
#endif
    for (int i = 0; i < data.size(); i++) {
		coefs[i] = data[i];
#if USE_MIN_HEURISTIC
        if(data[i] < min) min = data[i];
#endif
    }
    
	update_hash();
}

// Fourth constructor
VectorInt::VectorInt(unsigned char * data, bool copy)
{
	if (copy)
	{
		coefs = (unsigned char *)malloc(entriesNb * sizeof(char));
		memcpy(coefs, data, entriesNb * sizeof(char));
	}
	else
	{
		coefs = data;
	}
#if USE_MIN_HEURISTIC
    min = -1;
    for (int i = 0; i < entriesNb; i++)
        if(data[i] < min)
            min = data[i];
#endif
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

std::unordered_set<VectorInt> int_vectors;
