#pragma once
#include "../include/HeuristicMetrics.h"
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
    };

    struct StagnationParams {
        unsigned int steps_since_last_improvement;
        unsigned int steps_since_last_random;
        unsigned int random_check_interval;
        unsigned int random_states_per_check;
        unsigned int best_heuristic;
        unsigned int stagnation_threshold;
        float temperature;

        static const unsigned int DEFAULT_INTERVAL =
            800; // Revisar más frecuentemente
        static const unsigned int DEFAULT_RANDOM_STATES =
            20; // Generar más estados aleatorios
        static const unsigned int STAGNATION_LIMIT =
            300; // Umbral de estancamiento
        constexpr static const float INITIAL_TEMPERATURE = 1.0f;
        constexpr static const float COOLING_RATE = 0.995f;

        StagnationParams(unsigned int problem_size) {
            steps_since_last_improvement = 0;
            steps_since_last_random = 0;
            random_check_interval = DEFAULT_INTERVAL;
            random_states_per_check = DEFAULT_RANDOM_STATES;
            best_heuristic = UINT_MAX;
            stagnation_threshold = STAGNATION_LIMIT;
            temperature = INITIAL_TEMPERATURE;
        }
    };

    // Constructor y destructor
    Search(State *initial_state, State *target_state,
           const unsigned int *capacities);
    ~Search();

    // Funciones principales
    Path findPath();
    static void freePath(Path &path);

    private:
    // Miembros de clase
    const unsigned int *capacities;
    State *initial_state;
    State *target_state;
    PairingHeap open_list;
    HashTable closed_list;

    // Funciones auxiliares
    Path reconstructPath(State *final_state);
    void generateRandomVariations(State *current, std::mt19937 &rng,
                                  unsigned int &total_states_generated,
                                  StagnationParams &stag);
};
