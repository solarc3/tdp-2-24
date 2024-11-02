#pragma once
#include "HashTable.h"
#include "Heap.h"
#include <climits>
#include <random>

class Search {
    public:
    struct Path {
        State **states;
        unsigned int length;
    };

    // Constructor
    Search(State *initial_state, State *target_state,
           const unsigned int *capacities);

    ~Search();
    Path findPath();
    static void freePath(Path &path);

    private:
    // Miembros en orden de inicialización
    const unsigned int *capacities;
    State *initial_state;
    State *target_state;
    Heap open_list;
    HashTable closed_list;
    Path reconstructPath(State *final_state);
    void reversePath(Path &path) const;
};
