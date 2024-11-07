#include "../include/Search.h"
#include <cassert>

inline void testSearch() {
    // Create arrays for the test scenario
    unsigned int *initial_jugs = nullptr;
    unsigned int *target_jugs = nullptr;
    unsigned int *max_capacities = nullptr;
    State *initial_state = nullptr;
    State *target_state = nullptr;
    Search *search = nullptr;

    try {
        initial_jugs = new unsigned int[3]{0, 0, 0};
        target_jugs = new unsigned int[3]{0, 0, 6};
        max_capacities = new unsigned int[3]{3, 5, 7};

        // Create initial and target states
        initial_state = new State(3, initial_jugs, 0, 0, nullptr);
        target_state = new State(3, target_jugs, 0, 0, nullptr);

        // Create and execute search
        search = new Search(initial_state, target_state, max_capacities);
        Search::Path path = search->findPath();

        // Verify solution
        assert(path.length > 0);
        assert(path.states != nullptr);

        // Clean up path
        Search::freePath(path);

        // Clean up everything else
        delete search;
        delete initial_state;
        delete target_state;
        delete[] initial_jugs;
        delete[] target_jugs;
        delete[] max_capacities;

    } catch (...) {
        // Clean up everything in case of exception
        delete search;
        delete initial_state;
        delete target_state;
        delete[] initial_jugs;
        delete[] target_jugs;
        delete[] max_capacities;
        throw;
    }
}
