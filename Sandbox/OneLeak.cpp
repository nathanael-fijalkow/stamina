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

//	while (true)
	{
		int stateNb = 3;
		UnstableMarkovMonoid monoid(stateNb);
		
		ExplicitMatrix * a = ExplicitMatrix::random(stateNb);
		ExplicitMatrix * b = ExplicitMatrix::random(stateNb);
		//ExplicitMatrix * c = ExplicitMatrix::random(stateNb);

		for (int i = 0; i < stateNb * stateNb; i++)
			a->coefficients[i] = b->coefficients[i] = 0;
		
		a->coefficients[0] = 2;
		a->coefficients[1] = 2;
		a->coefficients[4] = 2;
		a->coefficients[8] = 2;

		b->coefficients[2] = 2;
		b->coefficients[3] = 2;
		b->coefficients[8] = 2;

		monoid.addLetter('a', *a);
		monoid.addLetter('b', *b);
//		monoid.addLetter('c', *c);

		monoid.ComputeMarkovMonoid(&monoid);

		cout << monoid.expr_to_mat.size() << " elements and " << monoid.rewriteRules.size() << " rewrite rules and " ;

		monoid.print();
		int ln = monoid.maxLeakNb();
		cout << " max leak number " << ln << endl;

		delete a;
		delete b;
//		delete c;
	}
}
