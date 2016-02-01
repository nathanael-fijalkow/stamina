
#ifndef VECTORR_HPP
#define VECTORR_HPP

#include <unordered_set>
#include <vector>
#include <set>


// Class of vectors
class Vector
{
public:

	//set Vector size
	static void SetSize(uint size)
	{
		entriesNb = size;
		bitsNb = (entriesNb + 8 * sizeof(uint) - 1) / (8 * sizeof(uint));
	}

	static uint GetStateNb()
	{
		return entriesNb;
	}

	static uint GetBitSize()
	{
		return bitsNb;
	}

	// Entries is an array containing integers whose first entriesNb bits encode the entries
	// thus the array size is the smallest integer larger than entriesNb / (sizeof(uint) * 8) 
	size_t * bits;

	bool contains(size_t n) const { return  ( bits[n / (sizeof(uint) * 8)] & (1 << (n % (sizeof(uint) * 8)) ) ) != 0; };


	// Construct new vector, size is fixed by set_size
	Vector();

	// Copy constructor
	Vector(const Vector & other);

	// Third constructor
	Vector(std::vector <bool> data);

	Vector(size_t * data, bool copy = true);

	// Function returning the hash
	size_t Hash() const { return _hash; };

	// Equality operator
	bool operator == (const Vector & vec) const;

	// Free a useless vector
	~Vector();

	// Print
	void print(std::ostream& os) const;

protected:

	// Number of entries
	static uint entriesNb;

	// size of the bits array
	static uint bitsNb;

	// Hash
	size_t _hash;

	static std::hash <std::vector <bool> > hash_val;

	void init();

	// Function computing the hash
	void update_hash(){
		_hash = 0;
		for (size_t * index = bits; index != bits + entriesNb / (sizeof(uint) * 8); index++)
			_hash ^= std::hash_value(*index) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	};

    // Assignement operator
    Vector & operator = (const Vector & other);


};

/* Defines default hash for the class of Vector */
namespace std
{
	template <> struct hash < const Vector >
	{
		size_t operator()(const Vector & vec) const
		{
			return vec.Hash();
		}
	};

		template <> struct hash < Vector >
		{
			size_t operator()(const Vector & vec) const
			{
				return vec.Hash();
			}
		};
}

#endif
