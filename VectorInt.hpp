#ifndef VECTORR_INT_HPP
#define VECTORR_INT_HPP

#include <unordered_set>
#include <vector>
#include <set>


// Class of vectors
class VectorInt
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
	char * coefs;

//	bool contains(size_t n) const { return  ( bits[n / (sizeof(uint) * 8)] & (1 << (n % (sizeof(uint) * 8)) ) ) != 0; };


	// Construct new vector, size is fixed by set_size
	VectorInt();

	// Second constructor
	VectorInt(const VectorInt & other);

	// Third constructor
	VectorInt(std::vector<char> data);

	VectorInt(char * data, bool copy = true);

	// Function returning the hash
	size_t Hash() const { return _hash; };

	// Equality operator
	bool operator == (const VectorInt & vec) const;

	// Free a useless vector
	~VectorInt();

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
		for (char * index = coefs; index != coefs + entriesNb; index++)
			_hash ^= std::hash_value(*index) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	};

private:

	// Affecttaion operator
	VectorInt & operator = (const VectorInt & other);

};

/* Defines default hash for the class of Vector */
namespace std
{
	template <> struct hash < const VectorInt >
	{
		size_t operator()(const VectorInt & vec) const
		{
			return vec.Hash();
		}
	};

		template <> struct hash < VectorInt >
		{
			size_t operator()(const VectorInt & vec) const
			{
				return vec.Hash();
			}
		};
}

#endif
