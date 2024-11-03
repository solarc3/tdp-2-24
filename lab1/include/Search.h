#pragma once
#include "../include/TracyMacros.h"
#include "HashTable.h"
#include "Heap.h"
#include <climits>
#include <iostream>
#include <random>

class Search {
    public:
    struct Path {
        State **states;
        unsigned int length;
        unsigned int total_states;
    };

    struct StagnationParams {
        unsigned int steps_since_last_improvement;
        unsigned int steps_since_last_random;
        unsigned int random_check_interval;
        unsigned int random_states_per_check;
        unsigned int best_heuristic;
        unsigned int stagnation_threshold;
        float temperature;

        static const unsigned int DEFAULT_INTERVAL = 800;
        static const unsigned int DEFAULT_RANDOM_STATES = 20;
        static const unsigned int STAGNATION_LIMIT = 300;
        constexpr static const float INITIAL_TEMPERATURE = 5.0f;
        constexpr static const float COOLING_RATE = 0.97f;
        constexpr static const float MIN_TEMPERATURE = 0.01f;
        constexpr static const float REHEAT_FACTOR = 1.5f;

        StagnationParams(unsigned int problem_size);
        void updateAdaptiveParams(bool improved, float current_temp);
        void updateTemperature(bool improved);
    };

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

    private:
    // Funciones auxiliares
    Path reconstructPath(State *final_state, unsigned int total_states);
    void generateRandomVariations(State *current, std::mt19937 &rng,
                                  unsigned int &total_states_generated,
                                  StagnationParams &stag);
};
