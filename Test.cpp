#include <iostream>

#include "Expressions.hpp"
#include "Matrix.hpp"
#include "MarkovMonoid.hpp"

#include <fstream>
#include <sstream>

#ifdef MSVC
#include <windows.h>
#include <time.h>
#endif

using namespace std;

int main(int argc, char **argv)
{
	cout << "Acme++ rules" << endl;

	unsigned int seed = time(NULL);

	unsigned int nb_samples = 1;

	float pzeros = 0.6f;
	float ppluses = 0.025f;

	int int0 = (int)(pzeros * RAND_MAX);
	int int1 = (int)((pzeros + ppluses) * RAND_MAX);

	int max_state_nb = 10;

	stringstream filename;
	filename << "Experiment seed " << seed << " samples " << nb_samples;
	filename << " densities 0 plus 1 " << (int)(1000 * pzeros) << "  " << (int)(1000 * ppluses) << " " << (int)(1000 * (1.0 - ppluses - pzeros));
	filename << ".csv";

	ofstream file(filename.str());

	file << "Size;Seed;ElementsNb;RewriteRulesNb;VectorNb" << endl;

	uint nb = 0;
	while (nb++ < nb_samples)
	{

		seed = hash_value( seed << 6 | seed >> 3);
		srand(seed);

		int n = 10 + (rand() % max_state_nb);

		cout << endl;
		cout << "************ " << nb << "/" << nb_samples << " SIZE " << n << " SEED " << seed << "  **********" << endl;

		srand(seed);

		UnstableMarkovMonoid monoid(n);


		ExplicitMatrix m1(n), m2(n);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				int r = rand();
				if (r < int0)
					m1.coefficients[n*i + j] = 0;
				else if (r > int1)
					m1.coefficients[n*i + j] = 2;
				else
					m1.coefficients[n*i + j] = 1;
				r = rand();
				if (r < int0)
					m2.coefficients[n*i + j] = 0;
				else if (r > int1)
					m2.coefficients[n*i + j] = 2;
				else
					m2.coefficients[n*i + j] = 1;
			}
		}

		cout << "Computing matrix " << endl;
		monoid.addLetter('a', m1);
		monoid.addLetter('b', m2);

		int sz = monoid.elements.size();
		cout << sz << " elements in the tab elements." << endl;
		cout << "Closing by product " << endl;

		monoid.CloseByProduct();

		int s = monoid.expr_to_mat.size();
		cout << s << " elements." << endl;
		cout << monoid.rewriteRules.size() << " rewrite rules." << endl;

		monoid.print();

		cout << "Closing by stabilization " << endl;

		monoid.CloseByStabilization();

		s = monoid.expr_to_mat.size();
		cout << s << " elements." << endl;
		cout << monoid.rewriteRules.size() << " rewrite rules." << endl;

		monoid.print();

	}

	file.close();

	cout << "Experiment over " << endl;

}
