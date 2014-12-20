#include <iostream>

#include "Expressions.hpp"
#include "Matrix.hpp"
#include "Monoid.hpp"

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

	while (true)
	{
		int stateNb = 5;
		UnstableMonoid monoid(stateNb);
		
		ExplicitMatrix * a = ExplicitMatrix::random(stateNb);
		ExplicitMatrix * b = ExplicitMatrix::random(stateNb);
		//ExplicitMatrix * c = ExplicitMatrix::random(stateNb);

		/*
		for (int i = 0; i < stateNb * stateNb; i++)
			a->coefficients[i] = b->coefficients[i] = 0;
		
		a->coefficients[0] = 2;
		a->coefficients[1] = 2;
		a->coefficients[4] = 2;
		a->coefficients[8] = 2;

		b->coefficients[2] = 2;
		b->coefficients[3] = 2;
		b->coefficients[8] = 2;
		*/

		monoid.addLetter('a', *a);
		monoid.addLetter('b', *b);
//		monoid.addLetter('c', *c);

		monoid.ComputeMonoid();



		int ln = monoid.maxLeakNb();

		cout << endl << stateNb << " states and " <<  monoid.expr_to_mat.size() << " elements and " << monoid.rewriteRules.size() << " rewrite rules and ";
		cout << " max leak number " << ln << endl;
		if (ln > 0)
		{
//			monoid.print();
		}

		delete a;
		delete b;
//		delete c;
	}
}
