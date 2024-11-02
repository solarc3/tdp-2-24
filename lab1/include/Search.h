#pragma once
#include "../include/HeuristicMetrics.h"
#include "../include/TracyMacros.h"
#include "HashTable.h"
#include "Heap.h"
#include <iostream>

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
    void randomDFS(State *start_state, unsigned int max_states,
                   unsigned int max_depth_diff, HashTable &visited_states);
    const unsigned int *capacities;
    State *initial_state;
    State *target_state;
    Heap open_list;
    HashTable closed_list;
    Path reconstructPath(State *final_state);
    void reversePath(Path &path) const;
};
