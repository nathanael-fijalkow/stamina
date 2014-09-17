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

	unsigned int nb_samples = 50000;


	int max_state_nb = 10;

	stringstream filename;
	filename << "Experiment seed " << seed << " samples " << nb_samples << " maxstatenb " << max_state_nb;

	//filename << " densities 0 / 1 " << (int)(1000 * pzeros) << " / " << (int)(1000 * (1.0 - pzeros));

	ofstream file(filename.str() + ".csv");
	ofstream file2(filename.str() + " monoids.txt");

	file << "Size;Densitya;Densityb;ElementsNb;RewriteRulesNb;VectorNb;LeakNb" << endl;

	uint nb = 0;
	while (nb++ < nb_samples)
	{

		//seed = hash_value(seed << 6 | seed >> 3);
		//srand(seed);




		int n = 1 + (rand() % max_state_nb);

		int da = rand();
		float density_a = 1.0 - (da * 1.0) / RAND_MAX;
		int db = rand();
		float density_b = 1.0 - (db * 1.0) / RAND_MAX;

		file2 << endl << "******************************************************" << endl << endl;

		cout << endl << "#" << nb  << " size " << n << " 1-density " << density_a << " " << density_b << " seed " << seed <<  endl;
		file2 << endl << "#" << nb << " size " << n << " 1-density " << density_a << " " << density_b << " seed " << seed << endl;


		UnstableMarkovMonoid monoid(n);


		ExplicitMatrix m1(n), m2(n);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if (rand() < da)
					m1.coefficients[n*i + j] = 0;
				else
					m1.coefficients[n*i + j] = 2;
				if (rand() < db)
					m2.coefficients[n*i + j] = 0;
				else
					m2.coefficients[n*i + j] = 2;
			}
		}

		auto a = monoid.addLetter('a', m1);
		auto b = monoid.addLetter('b', m2);

		cout << "a" << *a << endl << "b" << endl << *b << endl;
		file2 << "a" << *a << endl << "b" << endl << *b << endl;

		try

		{
			monoid.ComputeMarkovMonoid();

			//monoid.print();

			int s = monoid.expr_to_mat.size();
			cout << s << " elements." << monoid.rewriteRules.size() << " rewrite rules " << flush;
			file2 << s << " elements." << monoid.rewriteRules.size() << " rewrite rules " << flush;

			auto l = monoid.maxLeakNb();
			if (l.first == 0)
			{
				cout << " leaktight " << endl;
				file2 << " leaktight " << endl;
			}
			else
			{
				cout << " maxleaknb " << l.first << " on expression " << endl << *(l.second) << endl;
				file2 << " maxleaknb " << l.first << " on expression " << endl << *(l.second) << endl;
			}
			
			//file << "Size;Proba;Seed;ElementsNb;RewriteRulesNb;VectorNb;LeakNb" << endl;

			file << n << ";" << da << ";" << db << "; " << monoid.expr_to_mat.size();
			file << "; " << monoid.rewriteRules.size() << "; " << Matrix::vectors.size() << "; " << l.first << endl;

			//		Sleep(10000);
		}
		catch (const runtime_error & err)
		{
			cout << "Failed to compute: " << err.what() << endl;
			file2 << "Failed to compute: " << err.what() << endl;
		}
	}

	file.close();
	file2.close();

	cout << "Experiment over " << endl;

}
