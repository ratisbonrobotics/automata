#include "nondeterministic_finite_automaton.h"
#include <assert.h>

static set consumeLetter(nondeterministic_finite_automaton nfa, set states, letter let)
{
    nfa_delta_function delta = getObjectByIndex(nfa, 2);
    set new_states = Set();
    for (unsigned int i = 0; i < getCardinality(states); i++)
    {
        word state = drawFromSet(states);
        set to = getNFADeltaFunctionValue(delta, state, let);
        if (to != NULL)
            new_states = unionSet(new_states, to);
    }
    return new_states;
}

static set epsilonClosure(nondeterministic_finite_automaton nfa, set states)
{
    if (getCardinality(states) == 0)
        return states;

    nfa_delta_function delta = getObjectByIndex(nfa, 2);
    set new_states = Set();
    for (unsigned int i = 0; i < getCardinality(states); i++)
    {
        word state = drawFromSet(states);
        set to = getNFADeltaFunctionValue(delta, state, letter_epsilon);
        if (to != NULL)
            new_states = unionSet(new_states, to);
    }

    new_states = unionSet(new_states, epsilonClosure(nfa, new_states));

    return unionSet(states, new_states);
}

void printNFA(nondeterministic_finite_automaton nfa)
{
    set states = getObjectByIndex(nfa, 0);
    print(L"Q = {");
    for (unsigned int i = 0; i < getCardinality(states); i++)
    {
        word state = drawFromSet(states);
        print(L"%lw", state);
        if (i < getCardinality(states) - 1)
            print(L", ");
    }
    print(L"}\n");

    set alphabet = getObjectByIndex(nfa, 1);
    print(L"Σ = {");
    for (unsigned int i = 0; i < getCardinality(alphabet); i++)
    {
        letter let = drawFromSet(alphabet);
        print(L"%ll", let);
        if (i < getCardinality(alphabet) - 1)
            print(L", ");
    }
    print(L"}\n");

    nfa_delta_function delta = getObjectByIndex(nfa, 2);
    print(L"δ = {\n");
    for (unsigned int i = 0; i < getCardinality(delta); i++)
    {
        n_tuple tuple = drawFromSet(delta);
        set from = getObjectByIndex(tuple, 0);
        word state = getWordFromNFADeltaFunctionDomainElement(from);
        letter let = getLetterFromNFADeltaFunctionDomainElement(from);
        set to = getObjectByIndex(tuple, 1);

        print(L"    ({%lw, %ll}, {", state, let);
        for (unsigned int j = 0; j < getCardinality(to); j++)
        {
            word next_state = drawFromSet(to);
            print(L"%lw", next_state);
            if (j < getCardinality(to) - 1)
                print(L", ");
        }
        print(L"})");
        if (i < getCardinality(delta) - 1)
            print(L",\n");
    }

    print(L"\n}\n");

    word start = getObjectByIndex(nfa, 3);
    print(L"q = %lw\n", start);

    set final_states = getObjectByIndex(nfa, 4);
    print(L"F = {");
    for (unsigned int i = 0; i < getCardinality(final_states); i++)
    {
        word state = drawFromSet(final_states);
        print(L"%lw", state);
        if (i < getCardinality(final_states) - 1)
            print(L", ");
    }
    print(L"}\n");
}

bool runNFA(nondeterministic_finite_automaton nfa, void *inp)
{
    word start = getObjectByIndex(nfa, 3);
    set states = epsilonClosure(nfa, addToSet(Set(), start));

    if (inp != NULL)
    {
        if (isObjectAnOrderedPair(inp))
        {
            for (unsigned int i = 0; i < getLength(inp); i++)
            {
                letter let = getLetterByIndex(inp, i);
                states = consumeLetter(nfa, states, let);
                states = epsilonClosure(nfa, states);
            }
        }
        else
        {
            states = consumeLetter(nfa, states, inp);
            states = epsilonClosure(nfa, states);
        }
    }

    set final_states = getObjectByIndex(nfa, 4);
    for (unsigned int i = 0; i < getCardinality(final_states); i++)
    {
        word state = drawFromSet(final_states);
        if (isElementOf(states, state))
            return true;
    }
    return false;
}

nondeterministic_finite_automaton NondeterministicFiniteAutomaton(set states, set alphabet, nfa_delta_function delta, word start, set final_states)
{
    return NTuple(5, states, alphabet, delta, start, final_states);
}

nondeterministic_finite_automaton letterNFA(letter let)
{
    // Q
    word q0 = Word(2, letter_q, letter_0);
    word q1 = Word(2, letter_q, letter_1);

    set states = addToSet(Set(), q0);
    states = addToSet(states, q1);

    // Σ
    set alphabet = addToSet(Set(), let);

    // δ
    nfa_delta_function delta = NFADeltaFunction();
    delta = addToNFADeltaFunction(delta, q0, let, addToSet(Set(), q1));

    // F
    set final_states = addToSet(Set(), q1);

    return NondeterministicFiniteAutomaton(states, alphabet, delta, q0, final_states);
}

nondeterministic_finite_automaton concatinationNFA(nondeterministic_finite_automaton nfa_left, nondeterministic_finite_automaton nfa_right)
{
    if (nfa_left == NULL && nfa_right == NULL)
        return NULL;
    else if (nfa_left == NULL)
        return nfa_right;
    else if (nfa_right == NULL)
        return nfa_left;

    set states_left = getObjectByIndex(nfa_left, 0);
    set alphabet_left = getObjectByIndex(nfa_left, 1);
    nfa_delta_function delta_left = getObjectByIndex(nfa_left, 2);
    word start_left = getObjectByIndex(nfa_left, 3);
    set final_states_left = getObjectByIndex(nfa_left, 4);

    set states_right = getObjectByIndex(nfa_right, 0);
    set alphabet_right = getObjectByIndex(nfa_right, 1);
    nfa_delta_function delta_right = getObjectByIndex(nfa_right, 2);
    word start_right = getObjectByIndex(nfa_right, 3);
    set final_states_right = getObjectByIndex(nfa_right, 4);

    set states = Set();
    for (unsigned int i = 0; i < getCardinality(states_left); i++)
    {
        word state = drawFromSet(states_left);
        state = Word(2, state, letter_l);
        states = addToSet(states, state);
    }
    for (unsigned int i = 0; i < getCardinality(states_right); i++)
    {
        word state = drawFromSet(states_right);
        state = Word(2, state, letter_r);
        states = addToSet(states, state);
    }

    set alphabet = Set();
    for (unsigned int i = 0; i < getCardinality(alphabet_left); i++)
    {
        letter let = drawFromSet(alphabet_left);
        alphabet = addToSet(alphabet, let);
    }
    for (unsigned int i = 0; i < getCardinality(alphabet_right); i++)
    {
        letter let = drawFromSet(alphabet_right);
        alphabet = addToSet(alphabet, let);
    }

    relation delta_relation = Relation();
    for (unsigned int i = 0; i < getCardinality(delta_left); i++)
    {
        n_tuple tuple = drawFromSet(delta_left);
        set from = getObjectByIndex(tuple, 0);
        set to = getObjectByIndex(tuple, 1);

        word state = getWordFromNFADeltaFunctionDomainElement(from);
        state = Word(2, state, letter_l);
        letter let = getLetterFromNFADeltaFunctionDomainElement(from);

        for (unsigned int j = 0; j < getCardinality(to); j++)
        {
            word next_state = drawFromSet(to);
            next_state = Word(2, next_state, letter_l);
            delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), state), let), addToSet(Set(), next_state));
        }
    }
    for (unsigned int i = 0; i < getCardinality(delta_right); i++)
    {
        n_tuple tuple = drawFromSet(delta_right);
        set from = getObjectByIndex(tuple, 0);
        set to = getObjectByIndex(tuple, 1);

        word state = getWordFromNFADeltaFunctionDomainElement(from);
        state = Word(2, state, letter_r);
        letter let = getLetterFromNFADeltaFunctionDomainElement(from);

        for (unsigned int j = 0; j < getCardinality(to); j++)
        {
            word next_state = drawFromSet(to);
            next_state = Word(2, next_state, letter_r);
            delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), state), let), addToSet(Set(), next_state));
        }
    }

    word start = Word(2, letter_q, letter_0);
    word final_state = Word(2, letter_q, letter_1);
    states = addToSet(states, start);
    states = addToSet(states, final_state);

    delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), start), letter_epsilon), addToSet(Set(), Word(2, start_left, letter_l)));

    for (unsigned int i = 0; i < getCardinality(final_states_left); i++)
    {
        word state = drawFromSet(final_states_left);
        state = Word(2, state, letter_l);
        delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), state), letter_epsilon), addToSet(Set(), Word(2, start_right, letter_r)));
    }

    for (unsigned int i = 0; i < getCardinality(final_states_right); i++)
    {
        word state = drawFromSet(final_states_right);
        state = Word(2, state, letter_r);
        delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), state), letter_epsilon), addToSet(Set(), final_state));
    }

    function delta = relationToNFADeltaFunction(delta_relation);

    return NondeterministicFiniteAutomaton(states, alphabet, delta, start, addToSet(Set(), final_state));
}

nondeterministic_finite_automaton unionNFA(nondeterministic_finite_automaton nfa_left, nondeterministic_finite_automaton nfa_right)
{
    if (nfa_left == NULL && nfa_right == NULL)
        return NULL;
    else if (nfa_left == NULL)
        return nfa_right;
    else if (nfa_right == NULL)
        return nfa_left;

    set states_left = getObjectByIndex(nfa_left, 0);
    set alphabet_left = getObjectByIndex(nfa_left, 1);
    function delta_left = getObjectByIndex(nfa_left, 2);
    word start_left = getObjectByIndex(nfa_left, 3);
    set final_states_left = getObjectByIndex(nfa_left, 4);

    set states_right = getObjectByIndex(nfa_right, 0);
    set alphabet_right = getObjectByIndex(nfa_right, 1);
    function delta_right = getObjectByIndex(nfa_right, 2);
    word start_right = getObjectByIndex(nfa_right, 3);
    set final_states_right = getObjectByIndex(nfa_right, 4);

    set states = Set();
    for (unsigned int i = 0; i < getCardinality(states_left); i++)
    {
        word state = drawFromSet(states_left);
        state = Word(2, state, letter_l);
        states = addToSet(states, state);
    }
    for (unsigned int i = 0; i < getCardinality(states_right); i++)
    {
        word state = drawFromSet(states_right);
        state = Word(2, state, letter_r);
        states = addToSet(states, state);
    }

    set alphabet = Set();
    for (unsigned int i = 0; i < getCardinality(alphabet_left); i++)
    {
        letter let = drawFromSet(alphabet_left);
        alphabet = addToSet(alphabet, let);
    }
    for (unsigned int i = 0; i < getCardinality(alphabet_right); i++)
    {
        letter let = drawFromSet(alphabet_right);
        alphabet = addToSet(alphabet, let);
    }

    relation delta_relation = Relation();
    for (unsigned int i = 0; i < getCardinality(delta_left); i++)
    {
        n_tuple tuple = drawFromSet(delta_left);
        set from = getObjectByIndex(tuple, 0);
        set to = getObjectByIndex(tuple, 1);

        word state = getWordFromNFADeltaFunctionDomainElement(from);
        state = Word(2, state, letter_l);
        letter let = getLetterFromNFADeltaFunctionDomainElement(from);

        for (unsigned int j = 0; j < getCardinality(to); j++)
        {
            word next_state = drawFromSet(to);
            next_state = Word(2, next_state, letter_l);

            delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), state), let), addToSet(Set(), next_state));
        }
    }
    for (unsigned int i = 0; i < getCardinality(delta_right); i++)
    {
        n_tuple tuple = drawFromSet(delta_right);
        set from = getObjectByIndex(tuple, 0);
        set to = getObjectByIndex(tuple, 1);

        word state = getWordFromNFADeltaFunctionDomainElement(from);
        state = Word(2, state, letter_r);
        letter let = getLetterFromNFADeltaFunctionDomainElement(from);

        for (unsigned int j = 0; j < getCardinality(to); j++)
        {
            word next_state = drawFromSet(to);
            next_state = Word(2, next_state, letter_r);

            delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), state), let), addToSet(Set(), next_state));
        }
    }

    word start = Word(2, letter_q, letter_0);
    word final_state = Word(2, letter_q, letter_1);
    states = addToSet(states, start);
    states = addToSet(states, final_state);

    delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), start), letter_epsilon), addToSet(Set(), Word(2, start_left, letter_l)));
    delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), start), letter_epsilon), addToSet(Set(), Word(2, start_right, letter_r)));

    for (unsigned int i = 0; i < getCardinality(final_states_left); i++)
    {
        word state = drawFromSet(final_states_left);
        state = Word(2, state, letter_l);
        delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), state), letter_epsilon), addToSet(Set(), final_state));
    }

    for (unsigned int i = 0; i < getCardinality(final_states_right); i++)
    {
        word state = drawFromSet(final_states_right);
        state = Word(2, state, letter_r);
        delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), state), letter_epsilon), addToSet(Set(), final_state));
    }

    function delta = relationToNFADeltaFunction(delta_relation);

    return NondeterministicFiniteAutomaton(states, alphabet, delta, start, addToSet(Set(), final_state));
}

nondeterministic_finite_automaton iterationNFA(nondeterministic_finite_automaton nfa_iter)
{
    if (nfa_iter == NULL)
        return NULL;

    set states_iter = getObjectByIndex(nfa_iter, 0);
    set alphabet_iter = getObjectByIndex(nfa_iter, 1);
    nfa_delta_function delta_iter = getObjectByIndex(nfa_iter, 2);
    word start_iter = getObjectByIndex(nfa_iter, 3);
    set final_states_iter = getObjectByIndex(nfa_iter, 4);

    set states = Set();
    for (unsigned int i = 0; i < getCardinality(states_iter); i++)
    {
        word state = drawFromSet(states_iter);
        state = Word(2, state, letter_i);
        states = addToSet(states, state);
    }

    set alphabet = Set();
    for (unsigned int i = 0; i < getCardinality(alphabet_iter); i++)
    {
        letter let = drawFromSet(alphabet_iter);
        alphabet = addToSet(alphabet, let);
    }

    relation delta_relation = Relation();
    for (unsigned int i = 0; i < getCardinality(delta_iter); i++)
    {
        n_tuple tuple = drawFromSet(delta_iter);
        set from = getObjectByIndex(tuple, 0);
        set to = getObjectByIndex(tuple, 1);

        word state = getWordFromNFADeltaFunctionDomainElement(from);
        state = Word(2, state, letter_i);
        letter let = getLetterFromNFADeltaFunctionDomainElement(from);

        for (unsigned int j = 0; j < getCardinality(to); j++)
        {
            word next_state = drawFromSet(to);
            next_state = Word(2, next_state, letter_i);

            delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), state), let), addToSet(Set(), next_state));
        }
    }

    word start = Word(2, letter_q, letter_0);
    word final_state = Word(2, letter_q, letter_1);
    states = addToSet(states, start);
    states = addToSet(states, final_state);

    delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), start), letter_epsilon), addToSet(Set(), Word(2, start_iter, letter_i)));
    delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), start), letter_epsilon), addToSet(Set(), final_state));

    for (unsigned int i = 0; i < getCardinality(final_states_iter); i++)
    {
        word state = drawFromSet(final_states_iter);
        state = Word(2, state, letter_i);
        delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), state), letter_epsilon), addToSet(Set(), Word(2, start_iter, letter_i)));
        delta_relation = addToRelation(delta_relation, addToSet(addToSet(Set(), state), letter_epsilon), addToSet(Set(), final_state));
    }

    function delta = relationToNFADeltaFunction(delta_relation);

    return NondeterministicFiniteAutomaton(states, alphabet, delta, start, addToSet(Set(), final_state));
}

nondeterministic_finite_automaton regexNFA(word regex)
{
    static unsigned int end = 0;
    nondeterministic_finite_automaton nfa1 = NULL;
    nondeterministic_finite_automaton nfa2 = NULL;
    nondeterministic_finite_automaton nfa3 = NULL;

    for (unsigned int i = 0; i < getLength(regex); i++)
    {
        letter let = getLetterByIndex(regex, i);

        if (let == letter_bracket_closed)
        {
            end = i;
            break;
        }
        else if (let == letter_bracket_open)
        {
            nfa2 = concatinationNFA(nfa2, nfa1);
            word subregex = getSubword(regex, i + 1, getLength(regex));
            nfa1 = regexNFA(subregex);
            i += end + 1;
        }
        else if (let == letter_star)
        {
            nfa1 = iterationNFA(nfa1);
        }
        else if (let == letter_bar)
        {
            nfa2 = concatinationNFA(nfa2, nfa1);
            nfa3 = unionNFA(nfa3, nfa2);
            nfa1 = NULL;
            nfa2 = NULL;
        }
        else
        {
            nfa2 = concatinationNFA(nfa2, nfa1);
            nfa1 = letterNFA(let);
        }
    }

    nfa2 = concatinationNFA(nfa2, nfa1);
    nfa3 = unionNFA(nfa3, nfa2);
    return nfa3;
}
