import libacme as acme
from sage import all
import sage.combinat.finite_state_machine

ONE_TRANS = 2
ZERO_TRANS = 0

def matrix_to_vvchar(matrix):
    ret = acme.VVChar()
    for i in range(0,matrix.nrows()):
        v = acme.VChar()
        for j in range(0,matrix.ncols()):
            if matrix[i][j]==1:
                v.append(chr(ONE_TRANS))
            else:
                v.append(chr(ZERO_TRANS))
        ret.append(v)
    return ret

def to_monoid(automaton):
    dimension = len(automaton.states())
    n_of_letters = len(automaton.input_alphabet)
    monoid = acme.Monoid(dimension,n_of_letters)
    for letter in automaton.input_alphabet:
        m = acme.MyMatrix(dimension)
        m.coefficients = matrix_to_vvchar(aut.adjacency_matrix(letter))
        monoid.add_letter(letter,m)
    for s in automaton.initial_states():
        monoid.add_initial_state(automaton.states().index(s))
    for s in automaton.final_states():
        monoid.add_final_state(automaton.states().index(s))
    return monoid
