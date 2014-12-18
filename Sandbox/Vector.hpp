#ifndef VECTORR_HPP
#define VECTORR_HPP

#include <unordered_set>
#include <vector>
#include <set>

/*
if 0 vectors are stored as an array of integers containing indices of non zero entries
if 1 they are stored as bits of an array of uint*/
#define USE_SPARSE_MATRIX 0

// Class of vectors
class Vector
{
public:
#if USE_SPARSE_MATRIX
	// Number of non-zero entries
	const uint entriesNb;
	// Entries is a C-style array of length entriesNb containing all the non-zero values in increasing order
	size_t * entries;

	bool contains(size_t n) const { for (int i = 0; i < entriesNb; i++) if (entries[i] == n) return true; return false; };

	// end() points right after the last entry
	const size_t * end() const { return entries + entriesNb; };

#else
	// Number of entries
	const uint entriesNb;

	// Entries is an array containing integers whose first entriesNb bits encode the entries
	// thus the array size is the smallest integer larger than entriesNb / (sizeof(uint) * 8) 
	uint * bits;

	bool contains(size_t n) const { return bits[n / (sizeof(uint) * 8)] & (1 << (n % (sizeof(uint) * 8))); };

	// size of th ebits array
	const uint bitsNb;

	Vector() : bits(NULL), bitsNb(0), entriesNb(0) {};

#endif

	// First constructor
	Vector(uint size);

	// Second constructor
	Vector(const Vector & other);

#if USE_SPARSE_MATRIX
	// Third constructor
	Vector(vector <size_t> data);

	// Fourth constructor, data is copied by default
	Vector(size_t * data, size_t data_size, bool copy = true);
#else
	// Third constructor
	Vector(std::vector <bool> data);

	Vector(uint * data, size_t data_size, bool copy = true);
#endif

	// Function returning the hash
	size_t Hash() const { return _hash; };

	// Equality operator
	bool operator == (const Vector & vec) const;

	// Free a useless vector
	~Vector();

	// Print
	void print(std::ostream& os) const;


protected:

	//alocates memory
	void allocate(int size);

	// Hash
	size_t _hash;

	static std::hash <std::vector <bool> > hash_val;

#if USE_SPARSE_MATRIX
	// Function computing the hash
	void update_hash(){
		_hash = 0;
		for (size_t * index = entries; index != entries + entriesNb; index++)
			_hash ^= hash_value(*index) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	};
#else
	// Function computing the hash
	void update_hash(){
		_hash = 0;
		for (size_t * index = bits; index != bits + entriesNb / (sizeof(uint) * 8); index++)
			_hash ^= std::hash_value(*index) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	};
#endif

private:

	// Equality operator
	Vector & operator = (const Vector & other);

};

/* Defines default hash for the class of Vector */
namespace std
{
	template <> struct hash < Vector >
	{
		size_t operator()(const Vector & vec) const
		{
			return vec.Hash();
		}
	};
}

#endif