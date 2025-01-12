#pragma once
#include "../include/TracyMacros.h"
#include "HashTable.h"
#include "Heap.h"
#include <iostream>
#include <random>

class Search {
    public:
    struct Path {
        State **states;
        unsigned int length;
    };

    struct StagnationParams {
        unsigned int steps_since_last_improvement;
        unsigned int steps_since_last_random;
        unsigned int random_check_interval;
        unsigned int random_states_per_check;
        unsigned int best_heuristic;
        unsigned int stagnation_threshold;
        float temperature;
        bool annealing_active;

        static constexpr unsigned int DEFAULT_INTERVAL = 100;
        static constexpr unsigned int STAGNATION_LIMIT = 1000;
        static constexpr float INITIAL_TEMPERATURE = 1.0f;

        StagnationParams(unsigned int problem_size);
        void updateAdaptiveParams(bool improved, float current_temp,
                                  float size_factor);
        void updateTemperature(bool improved);
        void updateWeights();
    };
    void generateRandomVariations(State *current, std::knuth_b &rng,
                                  unsigned int &total_states_generated,
                                  StagnationParams &stag);
    // Constructor y destructor
    Search(State *initial_state, State *target_state,
           const unsigned int *capacities);
    ~Search();

    // Funciones principales
    Path findPath();
    static void freePath(Path &path);
    // Miembros de clase
    const unsigned int *capacities;
    State *initial_state;
    State *target_state;
    PairingHeap open_list;
    HashTable closed_list;
    void cleanupOldStates(unsigned int current_depth);
    void cleanupSuccessors(State **successors, unsigned int num_successors);
    Path reconstructPath(State *final_state, unsigned int total_states);
    void cleanUpStates();
    void cleanUpState(State *state);
    bool isSpecialState(State *state) const;
};
