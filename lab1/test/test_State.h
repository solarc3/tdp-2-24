#include "../include/State.h"
#include <cassert>

inline void testState() {
    State *s1 = nullptr;
    State *s2 = nullptr;
    State *same_state = nullptr;
    State *different_state = nullptr;
    unsigned int *test_jugs = nullptr;
    unsigned int *test_jugs2 = nullptr;
    State **succs = nullptr;

    try {
        // Test empty constructor
        s1 = new State();
        assert(s1->size == 0);
        assert(s1->jugs == nullptr);
        assert(s1->depth == 0);
        assert(s1->parent == nullptr);
        assert(s1->heuristic_calculated == false);

        // Test parameterized constructor
        test_jugs = new unsigned int[3]{4, 0, 0};
        s2 = new State(3, test_jugs, 0, 0, nullptr);
        assert(s2->size == 3);
        assert(s2->jugs != nullptr);
        assert(s2->jugs[0] == 4);
        assert(s2->jugs[1] == 0);
        assert(s2->jugs[2] == 0);

        same_state = new State(3, test_jugs, 0, 0, nullptr);
        assert(s2->equals(same_state));

        test_jugs2 = new unsigned int[3]{3, 1, 0};
        different_state = new State(3, test_jugs2, 0, 0, nullptr);
        assert(!s2->equals(different_state));

        // Test successor generation
        unsigned int capacities[3] = {4, 3, 2};
        unsigned int num_succs = 0;

        succs = s2->generateSuccessors(capacities, num_succs);
        assert(num_succs > 0);

        // Verify successors
        for (unsigned int i = 0; i < num_succs; i++) {
            assert(succs[i] != nullptr);
            assert(succs[i]->size == 3);
            assert(succs[i]->jugs != nullptr);
            assert(succs[i]->depth == s2->depth + 1);
            assert(succs[i]->parent == s2);
        }

        // Clean up successors
        for (unsigned int i = 0; i < num_succs; i++) {
            delete succs[i];
        }
        delete[] succs;

        // Clean up everything else
        delete s1;
        delete s2;
        delete same_state;
        delete different_state;
        delete[] test_jugs;
        delete[] test_jugs2;

    } catch (...) {

        delete s1;
        delete s2;
        delete same_state;
        delete different_state;
        delete[] test_jugs;
        delete[] test_jugs2;
        throw;
    }
}
