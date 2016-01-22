#include <Expressions.hpp>
#include <Vector.hpp>
#include <string.h>
#include <iostream>

using namespace std;

// Class Vector
// First constructor
Vector::Vector()
{
	init();
}

void Vector::init()
{
	bits = (size_t *)malloc(bitsNb * sizeof(size_t));
	memset(bits, (char)0, bitsNb * sizeof(size_t));
}


// Second constructor
Vector::Vector(const Vector & other)
{
	init();
	memcpy(bits, other.bits, bitsNb * sizeof(size_t));
	_hash = other.Hash();
}

// Third constructor
Vector::Vector(vector<bool> data)
{
	init();
	for (int i = data.size() - 1; i >= 0; i--)
		bits[i / (8 * sizeof(uint))] = (bits[i / (8 * sizeof(uint))] << 1) | (data[i] ? 1 : 0);
	update_hash();
}

// Fourth constructor
Vector::Vector(size_t * data, bool copy)
{
	if (copy)
	{
		init();
		memcpy(bits, data, bitsNb * sizeof(size_t));
	}
	else
	{
		bits = data;
	}
	update_hash();
};

void Vector::print(ostream & os) const
{
	for (uint i = 0; i < entriesNb; i++)
		os << (contains(i) ? "1" : "0");
	os << endl;
}

// Number of entries
uint Vector::entriesNb = 0;

// size of the bits array
uint Vector::bitsNb = 0;

Vector::~Vector()
{
	free(bits);
	bits = NULL;
}

bool Vector::operator==(const Vector & vec) const
{
	if (entriesNb != vec.entriesNb) return false;
	for (uint i = 0; i < bitsNb; i++)
		if (bits[i] != vec.bits[i]) return false;
	return true;
}
