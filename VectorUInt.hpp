
#ifndef VECTOR_UINT_HPP
#define VECTOR_UINT_HPP

#include <unordered_set>
#include <vector>
#include <set>


// Class of vectors
class VectorUInt
{
public:

	//set Vector size
	static void SetSize(uint size)
	{
		entriesNb = size;
	}

	static uint GetStateNb()
	{
		return entriesNb;
	}


	// Entries is an array containing integers whose first entriesNb bits encode the entries
	// thus the array size is the smallest integer larger than entriesNb / (sizeof(uint) * 8) 
	uint * coefs;
	
	uint id;//identifier for later use, not used for equality test.
	
//	bool contains(size_t n) const { return  ( bits[n / (sizeof(uint) * 8)] & (1 << (n % (sizeof(uint) * 8)) ) ) != 0; };


	// Construct new vector, size is fixed by set_size
	VectorUInt();

	// Second constructor
	VectorUInt(const VectorUInt & other);

	// Third constructor
	VectorUInt(std::vector<uint> data,uint k);

	VectorUInt(uint * data,uint k, bool copy = true);

	// Function returning the hash
	size_t Hash() const { return _hash; };

	// Equality operator
	bool operator == (const VectorUInt & vec) const;

	// Free a useless vector
	~VectorUInt();

	// Print
	void print(std::ostream& os) const;
  


protected:

	// Number of entries
	static uint entriesNb;

	// Hash
	size_t _hash;

	static std::hash <std::vector <bool> > hash_val;

	// Function computing the hash
	void update_hash(){
		_hash = 0;
		for (uint * index = coefs; index != coefs + entriesNb; index++)
			_hash ^= std::hash_value(*index) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	};

	// Initialization of content
	void init();

private:

	// Affecttaion operator
	VectorUInt & operator = (const VectorUInt & other);

};

/* Defines default hash for the class of Vector */
namespace std
{
	template <> struct hash < const VectorUInt >
	{
		size_t operator()(const VectorUInt & vec) const
		{
			return vec.Hash();
		}
	};

		template <> struct hash < VectorUInt >
		{
			size_t operator()(const VectorUInt & vec) const
			{
				return vec.Hash();
			}
		};
}

#endif
