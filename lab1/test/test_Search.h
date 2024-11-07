#include "../include/Search.h"
#include <cassert>

inline void testSearch() {
    // Create arrays for the test scenario
    unsigned int *initial_jugs = new unsigned int[3]{0, 0, 0};
    unsigned int *target_jugs = new unsigned int[3]{0, 0, 6};
    unsigned int *max_capacities = new unsigned int[3]{3, 5, 7};

    // Create initial and target states
    State *initial_state = new State(3, initial_jugs, 0, 0, nullptr);
    State *target_state = new State(3, target_jugs, 0, 0, nullptr);

    // Create and execute search
    Search *search = new Search(initial_state, target_state, max_capacities);
    Search::Path path = search->findPath();

    // Verify that a solution exists
    assert(path.length > 0);
    assert(path.states != nullptr);

    unsigned int *final_jugs = new unsigned int[3];
    std::memcpy(final_jugs, path.states[path.length - 1]->jugs,
                3 * sizeof(unsigned int));

    for (unsigned int i = 0; i < path.length - 1; i++) {
        bool valid_move = false;
        unsigned int num_successors;
        State **successors =
            path.states[i]->generateSuccessors(max_capacities, num_successors);

        for (unsigned int j = 0; j < num_successors; j++) {
            if (successors[j]->equals(path.states[i + 1])) {
                valid_move = true;
                break;
            }
        }

        for (unsigned int j = 0; j < num_successors; j++) {
            delete successors[j];
        }
        delete[] successors;

        assert(valid_move && "Invalid move in solution path");
    }

    assert(final_jugs[0] == target_jugs[0] && final_jugs[1] == target_jugs[1] &&
           final_jugs[2] == target_jugs[2]);

    Search::freePath(path);
    delete search;

    delete[] final_jugs;
    delete[] initial_jugs;
    delete[] target_jugs;
    delete[] max_capacities;
}
