
#include <iostream>
#include "Expressions.hpp"
#include "Matrix.hpp"
#include "MarkovMonoid.hpp"
#include "StabilisationMonoid.hpp"
#include "StarHeight.hpp"

#include <fstream>
#include <sstream>

#ifdef MSVC
#include <windows.h>
#include <time.h>
#endif

using namespace std;


int main(int argc, char **argv)
{

	cout << "Stamina rules" << endl;

	unsigned int seed = time(NULL);

    unsigned int nb_samples = 100;
    
    int max_state_nb = 15;
    
    stringstream filename;
    filename << "Experiment seed " << seed << " samples " << nb_samples << " maxstatenb " << max_state_nb;
    
    ofstream file("Exp/" + filename.str() + ".csv");
    ofstream file2("Exp/" + filename.str() + " monoids.txt");
    
    file << "#;Size;Densitya;Densityb;ElementsNb;RewriteRulesNb;VectorNb;LeakNb;SharpHeight" << endl;
    
    uint nb = 0;

    while(nb++ < nb_samples) {
        ClassicAut aut(*expa);
        if(!aut.isdet()) {
            cout << "Only deterministic automata are handled" << endl;
            return 0;
        }
        UnstableMultiMonoid * monoid = NULL;
        const ExtendedExpression * witness = NULL;
        
        auto h = computeStarHeight(aut, monoid, witness, true, true);
        cout << "RESULT: the star height is " << h << "." << endl;
        
        if(toOut) {
            ofstream ofs(outputFilename + ".dot");
            ofs << Dot::toDot(expa,monoid, -1);
        }
        
        delete expa;
        delete monoid;
    }
    
	while (nb++ < nb_samples)
	{
		int n = 5 + (rand() % max_state_nb);

		int da = rand();
		float density_a = 1.0 - (da * 1.0) / RAND_MAX;
		int db = rand();
		float density_b = 1.0 - (db * 1.0) / RAND_MAX;

		UnstableStabMonoid monoid(n);

		ExplicitMatrix m1(n), m2(n);
		//int max_tries = 100;

			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < n; j++)
				{
					if (rand() < da / 2)
						m1.coefficients[i][j] = BOT;
					else if (rand() < da)
						m1.coefficients[i][j] = INC;
					else if (rand() < 2.5 * da)
						m1.coefficients[i][j] = EPS;
					else
						m1.coefficients[i][j] = RESET;

					if (rand() < db / 2)
						m2.coefficients[i][j] = BOT;
					else if (rand() < db)
						m2.coefficients[i][j] = INC;
					else if (rand() < 2.5 * db)
						m2.coefficients[i][j] = EPS;
					else
						m2.coefficients[i][j] = RESET;

				}
				m1.coefficients[i][rand() % n] = 2;
				m2.coefficients[i][rand() % n] = 2;
			}

			auto a = monoid.addLetter('a', m1);
			auto b = monoid.addLetter('b', m2);

		try
		{
			monoid.ComputeMonoid();

			//monoid.print();

			int s = monoid.expr_to_mat.size();
			if(s > 100){
				file2 << endl << "******************************************************" << endl << endl;

				cout << endl << "#" << nb  << " size " << n << " 1-density " << density_a << " " << density_b << " seed " << seed <<  endl;
				file2 << endl << "#" << nb << " size " << n << " 1-density " << density_a << " " << density_b << " seed " << seed << endl;

				cout << "a" << endl << *a << endl << "b" << endl << *b << endl;
				file2 << "a" << endl << *a << endl << "b" << endl << *b << endl;

				cout << s << " elements.\n" << monoid.rewriteRules.size() << " rewrite rules " << endl;
				file2 << s << " elements.\n" << monoid.rewriteRules.size() << " rewrite rules " << endl;

				cout << "Sharpheight " << monoid.sharp_height() << endl;
				file2 << "Sharpheight " << monoid.sharp_height() << endl;

			/*
			auto l = monoid.maxLeakNb();
			if (l.first == 0)
			{
				cout << "Leaktight " << endl;
				file2 << "Leaktight " << endl;
			}
			else
			{
				cout << "Maxleaknb " << l.first << " on expression " << endl << *(l.second) << endl << "and matrix " << endl << *(monoid.expr_to_mat[l.second]) << endl;
				file2 << "Maxleaknb " << l.first << " on expression " << endl << *(l.second) << endl << "and matrix " << endl << *(monoid.expr_to_mat[l.second]) << endl;
			}
			*/
			//file << "Size;Proba;Seed;ElementsNb;RewriteRulesNb;VectorNb;LeakNb" << endl;

				file << nb << ";" << n << ";" << density_a << ";" << density_b << ";" << monoid.expr_to_mat.size();
				file << ";" << monoid.rewriteRules.size() << ";" << Matrix::vectors.size() << ";" << /* l.first << ";" << monoid.sharp_height() << */endl;
			}
			//		Sleep(10000);
		}
		catch (const runtime_error & err)
		{
			cout << "Failed to compute: " << err.what() << endl;
			file2 << " > " << monoid.expr_to_mat.size() << " elements > " << monoid.rewriteRules.size() << " rewrite rules " << endl;
			file2 << "Failed to compute: " << err.what() << endl;
			file << nb << ";" << n << ";" << density_a << ";" << density_b << ";>>" << monoid.expr_to_mat.size();
			file << ";>>" << monoid.rewriteRules.size() << ";>>" << Matrix::vectors.size() << ";?;>" << monoid.sharp_height() << endl;
		}
	}

	file.close();
	file2.close();

	cout << "Experiment over " << endl;

}
