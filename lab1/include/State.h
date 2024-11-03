#pragma once
#include "../include/TracyMacros.h"
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class State {
    public:
    State *parent;
    mutable bool heuristic_calculated;
    unsigned int size;
    unsigned int *jugs;
    unsigned int depth;
    unsigned int weight;

    struct AdaptiveParams {
        float exploration_weight;
        float balance_weight;
        float optimization_weight;
        float current_performance;
        unsigned int consecutive_improvements;
        unsigned int plateaus;

        AdaptiveParams();
    };

    static AdaptiveParams adaptive_params;
    static constexpr unsigned int C1 = 0xcc9e2d51;
    static constexpr unsigned int C2 = 0x1b873593;

    State();
    State(unsigned int size, unsigned int *buckets, unsigned int depth,
          unsigned int weight, State *parent);
    ~State();

    bool equals(const State *other) const;
    void calculateHeuristic(const State &target_state);
    State **generateSuccessors(const unsigned int *capacities,
                               unsigned int &num_successors) const;
    void printState(const char *label);
    static bool readStatesFromFile(const std::string &fileName,
                                   State *max_state, State *target_state);
};
