Stamina: STabilisation Monoids IN Automata theory

Stamina is a tool implementing algebraic techniques to solve decision problems from automata theory.
More specifically, it uses stabilisation monoids as an algorithmic back-end to solve two problems: the value 1 problem for probabilistic automata
and that boundedness problem for automata with counters.
Most importantly, it is, to the best of our knowledge, the very first implementation of an algorithm solving the star-height problem.

It has been written in C++, by Nathanael Fijalkow, Hugo Gimbert, Edon Kelmendi and Denis Kuperberg.

Stamina is the successor of ACME, which also implements the transformation of an automaton into a stabilisation monoid. 
The point of Stamina is that it is much much faster, and does a lot of space optimisations.
This allows to handle much bigger automata, which is required to solve the star-height problem.




