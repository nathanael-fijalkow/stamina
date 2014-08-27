#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <iostream>
#include <cmath>
#include <time.h>
#include <fstream>
#include <sstream>

#include "AcmeTypes.cpp"


using namespace std;

int main(int argc, char **argv)
{
	cout << "Acme++ rules" << endl;

	ExplicitMatrix mata(2);
	mata.coefficients[0] = 2;
	mata.coefficients[1] = 1;
	mata.coefficients[2] = 1;
	mata.coefficients[3] = 2;
/*	mata.coefficients[4] = 2;
	mata.coefficients[5] = 2;
	mata.coefficients[6] = 0;
	mata.coefficients[7] = 2;
    mata.coefficients[8] = 2;
    */

	ExplicitMatrix matb(2);
	matb.coefficients[0] = 2;
	matb.coefficients[1] = 2;
	matb.coefficients[2] = 0;
	matb.coefficients[3] = 2;


	Matrix a(mata);
    Matrix b(matb);
//	Matrix b=Matrix::stab(mata);

	Matrix ab(a, b);
	Matrix c=Matrix::stab(ab);
    a.print();

    cout<<endl;
	b.print();
	cout << "" << endl;
    ab.print() ;
    cout << "" << endl;
    c.print() ;

/*
	unsigned int seed = time(NULL);

	unsigned int nb_samples = 60;
	unsigned int max_monoid_size = 1200000;


	float pzeros = 0.95f;
	float ppluses = 0.025f;

	int int0 = (int)(pzeros * RAND_MAX);
	int int1 = (int)((pzeros + ppluses) * RAND_MAX);

	int max_state_nb = 15;


	stringstream filename;
	filename << "Experiment seed " << seed << " samples " << nb_samples << " max_state_nb " << max_state_nb;
	filename << " densities 0 plus 1 " << (int)(1000 * pzeros) << "  " << (int)(1000 * ppluses) << " " << (int)(1000 * (1.0 - ppluses - pzeros));
	filename << " max_monoid_size " << max_monoid_size << ".csv";

	ofstream file(filename.str());

		if (!file)
		throw runtime_error("Failed to open file with filename " + filename.str());

	file << ";;;;;" << filename.str() << endl;

	file << "Size;Seed;ElementsNb;RewriteRulesNb;VectorNb;ComputationTime (mus);Comment" << endl;

	int nb = 0;
	while (nb++ < nb_samples)
	{

		seed = hash_value( seed << 6 | seed >> 3);
		srand(seed);

		int n = 1 + max_state_nb * rand() / RAND_MAX;


		cout << endl;
		cout << "************ " << nb << "/" << nb_samples << " SIZE " << n << " SEED " << seed << "  **********" << endl;

		// We reseed in order that seeds are consistent when working with fixed or random size
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

		cout << "Closing by product " << endl;

		LARGE_INTEGER start, end, frequency;
		QueryPerformanceFrequency(&frequency);

		QueryPerformanceCounter(&start);

		string error = "";
		try
		{
			monoid.CloseByProduct();
		}
		catch (exception e)
		{
			error = e.what();
		}

		QueryPerformanceCounter(&end);

		int duration = 1000000 * (end.QuadPart - start.QuadPart) / frequency.QuadPart;

		int s = monoid.expr_to_mat.size();
		cout << s << " elements." << endl;
		cout << monoid.rewriteRules.size() << " rewrite rules." << endl;
				cout << "Computation time " << duration << "mus" << endl;


		//file << n << ";" << seed << ";" << s << "; ";
		//file << monoid.rewriteRules.size() << ";" << Matrix::vectors.size();
			//	file << ";" << duration << ";" << error << endl;
			file << monoid.rewriteRules.size() << ";" << Matrix::vectors.size()<<endl;

		//if (s < 10)
			//monoid.print();
		//Sleep(100000000);

	}

	file.close();

	cout << "Experiment over " << endl;

	ExplicitMatrix mat(2);
	mat.coefficients[0] = 2;
	mat.coefficients[1] = 2;
	mat.coefficients[2] = 0;
	mat.coefficients[3] = 2;

	ExplicitMatrix matb(2);
	matb.coefficients[0] = 0;
	matb.coefficients[1] = 2;
	matb.coefficients[2] = 0;
	matb.coefficients[3] = 2;


	ExplicitMatrix matc(3);
	matc.coefficients[0] = 0;
	matc.coefficients[1] = 2;
	matc.coefficients[2] = 0;
	matc.coefficients[3] = 0;
	matc.coefficients[4] = 0;
	matc.coefficients[5] = 1;
	matc.coefficients[6] = 1;
	matc.coefficients[7] = 0;
	matc.coefficients[8] = 0;

	Matrix a(mat);
	Matrix b(matb);
	Matrix c(matc);

	cout << endl << "c" << endl;
	c.print();


	Matrix c2 = Matrix::prod(c, c);

	cout << endl << "cc" << endl;
	c2.print();

	Matrix c4 = Matrix::prod(c2, c2);
	Matrix c8 = Matrix::prod(c4, c4);

	Matrix ab = Matrix::prod(a, b);

	cout << endl << "a" << endl;
	a.print();

	cout << endl << "b" << endl;
	b.print();

	cout << endl << "ab" << endl;
	ab.print();


	cout << endl << "cccc" << endl;
	c4.print();

	cout << endl << "cccccccc" << endl;
	c8.print();


	monoid.addLetter('a', mat);
	monoid.addLetter('b', matb);

	monoid.CloseByProduct();

	monoid.print();


	Sleep(10000000);
	*/
}
