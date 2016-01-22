#include <Expressions.hpp>
#include <VectorUInt.hpp>
#include <string.h>
#include <iostream>

using namespace std;

// Class VectorUInt
//For constructors
void VectorUInt::init(){
  	coefs = (uint *)malloc(entriesNb * sizeof(uint));
	memset(coefs, (uint)0, entriesNb * sizeof(uint));
	update_hash();
}

// First constructor
VectorUInt::VectorUInt()
{
  init();
}


// Second constructor
VectorUInt::VectorUInt(const VectorUInt & other)
{
	memcpy(coefs, other.coefs, entriesNb * sizeof(uint));
	_hash = other.Hash();
	id=other.id;
}

// Third constructor
VectorUInt::VectorUInt(vector<uint> data,uint k)
{
        init();
        id=k;
	for (int i = data.size() - 1; i >= 0; i--)
		coefs[i] = data[i];
	update_hash();
}

// Fourth constructor
VectorUInt::VectorUInt(uint * data, uint k,bool copy)
{
	id=k;
	if (copy)
	{
		coefs = (uint *)malloc(entriesNb * sizeof(uint));
		memcpy(coefs, data, entriesNb * sizeof(uint));
	}
	else
	{
		coefs = data;
	}
	update_hash();
};

void VectorUInt::print(ostream & os) const
{
  // throw runtime_error("Unimplemented");
	/*
	for (uint i = 0; i < entriesNb; i++)
		os << (contains(i) ? "1" : "0");
	os << endl;
	*/
}

// Number of entries
uint VectorUInt::entriesNb = 0;

VectorUInt::~VectorUInt()
{
	free(coefs);
	coefs = NULL;
}

bool VectorUInt::operator==(const VectorUInt & vec) const
{
	if (entriesNb != vec.entriesNb) return false;
	for (uint i = 0; i < entriesNb; i++)
		if (coefs[i] != vec.coefs[i]) return false;
	return true;
}
