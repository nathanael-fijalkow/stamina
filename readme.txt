ACME++: Automata with Counters, Monoids and Equivalence


ACME++ is a tool implementing algebraic techniques to solve decision problems from automata theory.
Most importantly, it is, to the best of our knowledge, the very first implementation of an algorithm solving the star-height problem!

It has been written in C++, by Nathanael Fijalkow, Hugo Gimbert, Edon Kelmendi and Denis Kuperberg.

The core generic algorithm takes as input an automaton and computes its stabilisation monoid,
which is a generalization of its transition monoid.

ACME++ is the successor of ACME, which also implements the transformation of an automaton into a stabilisation monoid. 
The point of ACME++ is that it is much much faster, and does a lot of space optimisations.
This allows to handle much bigger automata, which is required to solve the star-height problem.


