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
    mutable bool heuristic_calculated; // mutabilidad para cachear el valor
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

        AdaptiveParams()
            : exploration_weight(0.4f), balance_weight(0.4f),
              optimization_weight(0.2f), current_performance(0.0f),
              consecutive_improvements(0), plateaus(0) {}
    };
    static AdaptiveParams adaptive_params;

    // constantes multiplicativas de hash, exportadas de la implementacion de
    // murmur32u (https://github.com/aappleby/smhasher/tree/master/src)
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
