#include "../include/State.h"
#include <cassert>

inline void testState() {
    // Prueba del constructor vacío
    State *s1 = new State();
    assert(s1->size == 0);
    assert(s1->jugs == nullptr);
    assert(s1->depth == 0);
    assert(s1->parent == nullptr);
    assert(s1->heuristic_calculated == false);

    // Prueba del constructor con parámetros
    unsigned int *test_jugs = new unsigned int[3]{4, 0, 0};
    State *s2 = new State(3, test_jugs, 0, 0, nullptr);
    assert(s2->size == 3);
    assert(s2->jugs != nullptr);
    assert(s2->jugs[0] == 4);
    assert(s2->jugs[1] == 0);
    assert(s2->jugs[2] == 0);
    assert(s2->depth == 0);
    assert(s2->parent == nullptr);
    assert(s2->heuristic_calculated == false);

    State *same_state = new State(3, test_jugs, 0, 0, nullptr);
    assert(s2->equals(same_state));

    unsigned int *test_jugs2 = new unsigned int[3]{3, 1, 0};
    State *different_state = new State(3, test_jugs2, 0, 0, nullptr);
    assert(!s2->equals(different_state));

    unsigned int capacities[3] = {4, 3, 2};
    unsigned int cant_succs;
    State **succs = s2->generateSuccessors(capacities, cant_succs);
    assert(cant_succs != 0);

    for (unsigned int i = 0; i < cant_succs; i++) {
        assert(succs[i] != nullptr);
        assert(succs[i]->size == 3);
        assert(succs[i]->jugs != nullptr);
        assert(succs[i]->depth == s2->depth + 1);
        assert(succs[i]->parent == s2);
    }

    for (unsigned int i = 0; i < cant_succs; i++) {
        delete succs[i];
    }
    delete[] succs;
    delete s1;
    delete s2;
    delete same_state;
    delete different_state;
    delete[] test_jugs;
    delete[] test_jugs2;
}
