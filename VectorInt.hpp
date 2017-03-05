
#ifndef VECTORR_INT_HPP
#define VECTORR_INT_HPP

#include <unordered_set>
#include <vector>
#include <set>

class VectorInt;

//The Min Heuristic consists in recording the minimal element in each vector and using it
//for speeding up vector scalr poduct:
//if the minimum is reached during the product then the loop stops
#define USE_MIN_HEURISTIC 0


// Class of vectors
class VectorInt
{
public:
    
	//set Vector size
    static void SetSize(uint size);

	static uint GetStateNb()
	{
		return entriesNb;
	}


	// Entries is an array containing integers whose first entriesNb bits encode the entries
	// thus the array size is the smallest integer larger than entriesNb / (sizeof(uint) * 8) 
	unsigned char * coefs;

#if USE_MIN_HEURISTIC
    //the minimum
    unsigned char min;
#endif
    
//	bool contains(size_t n) const { return  ( bits[n / (sizeof(uint) * 8)] & (1 << (n % (sizeof(uint) * 8)) ) ) != 0; };

	// Second constructor: copy constructor
	VectorInt(const VectorInt & other);

	// Third constructor
	VectorInt(std::vector<char> data);

    //copy the data
	VectorInt(unsigned char * data);

	// Function returning the hash
	size_t Hash() const { return _hash; };

	// Equality operator
	bool operator == (const VectorInt & vec) const;

	// Free a useless vector
	~VectorInt();

	// Print
	void print(std::ostream& os) const;
  


protected:
    // Construct new vector, size is fixed by set_size
    VectorInt();


	// Number of entries
	static uint entriesNb;

	// Hash
	size_t _hash;

	static std::hash <std::vector <bool> > hash_val;

	// Function computing the hash
	void update_hash(){
		_hash = 0;
		for (unsigned char * index = coefs; index != coefs + entriesNb; index++)
			_hash ^= std::hash_value(*index) + 0x9e3779b9 + (_hash << 6) + (_hash >> 2);
	};

	// Initialization of content
	void init();

private:

	// Affecttaion operator
	VectorInt & operator=(const VectorInt & other);

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

/* the set of all vectors */
extern std::unordered_set<VectorInt> int_vectors;

#endif
