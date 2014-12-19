
#include <Expressions.hpp>
#include <Vector.hpp>
#include <string.h>
#include <iostream>

using namespace std;

// Class Vector
// First constructor
#if USE_SPARSE_MATRIX
Vector::Vector(uint size) : entriesNb(size)
{
	entries = (size_t *)malloc(entriesNb * sizeof(size_t));
}
#else
Vector::Vector(uint size) : entriesNb(size), bitsNb((entriesNb + 8 * sizeof(uint) - 1) / (8 * sizeof(uint)))
{
	allocate(size);
}
#endif

void Vector::allocate(int size)
{
#if USE_SPARSE_MATRIX
	entries = (size_t *)malloc(entriesNb * sizeof(size_t));
#else
	bits = (uint *)malloc(bitsNb * sizeof(uint));
	memset(bits, (char)0, bitsNb * sizeof(uint));
#endif
}


// Second constructor
Vector::Vector(const Vector & other) : entriesNb(other.entriesNb), bitsNb((entriesNb + 8 * sizeof(uint) - 1) / (8 * sizeof(uint)))
{
	allocate(other.entriesNb);

	_hash = other.Hash();
#if USE_SPARSE_MATRIX
	memcpy(entries, other.entries, entriesNb * sizeof(size_t));
#else
	memcpy(bits, other.bits, bitsNb * sizeof(size_t));
#endif
}

// Third constructor
#if USE_SPARSE_MATRIX
Vector::Vector(vector<size_t> data) : entriesNb(data.size()), bitsNb((entriesNb + 8 * sizeof(uint) - 1) / (8 * sizeof(uint)))
{
	entries = (size_t *)malloc(entriesNb * sizeof(size_t));
	size_t * p = entries;
	for (vector<size_t>::iterator it = data.begin(); it != data.end(); it++)
		*p++ = *it;
#else
Vector::Vector(vector<bool> data) : entriesNb(data.size()), bitsNb((entriesNb + 8 * sizeof(uint) - 1) / (8 * sizeof(uint)))
{
	allocate(entriesNb);
	for (int i = data.size() - 1; i >= 0; i--)
	{
		bits[i / (8 * sizeof(uint))] = (bits[i / (8 * sizeof(uint))] << 1) | (data[i] ? 1 : 0);
		//		cout << "i:" << i << " bool " << data[i] << " stored " << bits[i / (8 * sizeof(uint))] << endl;
	}
#endif
	update_hash();
}

// Fourth constructor
#if USE_SPARSE_MATRIX
Vector::Vector(size_t * data, size_t data_size, bool copy) : entriesNb(data_size), bitsNb((entriesNb + 8 * sizeof(uint) - 1) / (8 * sizeof(uint)))
{
	if (copy)
	{
		entries = (size_t *)malloc(entriesNb * sizeof(size_t));
		memcpy(entries, data, entriesNb * sizeof(size_t));
	}
	else
	{
		entries = data;
	}
	update_hash();
};
#else
Vector::Vector(uint * data, size_t entriesNb, bool copy) : entriesNb(entriesNb), bitsNb((entriesNb + 8 * sizeof(uint) - 1) / (8 * sizeof(uint)))
{
	if (copy)
	{
		bits = (size_t *)malloc(bitsNb * sizeof(uint));
		memcpy(bits, data, bitsNb * sizeof(size_t));
	}
	else
	{
		bits = data;
	}
	update_hash();
};
#endif

void Vector::print(ostream & os) const
{
#if USE_SPARSE_MATRIX
	for (size_t * v = entries; v != entries + entriesNb; v++)
		os << *v << " ";
	os << endl;
#else
	for (int i = 0; i < entriesNb; i++)
		os << (contains(i) ? "1" : "0");
	os << endl;
#endif
}

Vector::~Vector()
{
#if USE_SPARSE_MATRIX
	free(entries);
	entries = NULL;
#else
	free(bits);
	bits = NULL;
#endif
}

bool Vector::operator==(const Vector & vec) const
{
	if (entriesNb != vec.entriesNb) return false;
#if USE_SPARSE_MATRIX
	size_t * v1 = entries;
	size_t * v2 = vec.entries;

	for (; v1 != entries + entriesNb; v1++, v2++)
	{
		if (*v1 != *v2)
			return false;
	}
#else
	for (int i = 0; i < bitsNb; i++)
		if (bits[i] != vec.bits[i]) return false;
#endif
	return true;
}
