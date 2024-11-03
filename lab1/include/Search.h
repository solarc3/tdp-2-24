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
        unsigned int total_states; // Agregar este campo
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
        constexpr static const float INITIAL_TEMPERATURE =
            5.0f; // Aumentado de 1.0f
        constexpr static const float COOLING_RATE =
            0.97f; // Más agresivo que 0.995f
        constexpr static const float MIN_TEMPERATURE =
            0.01f; // Nuevo: temperatura mínima
        constexpr static const float REHEAT_FACTOR =
            1.5f; // Nuevo: factor de recalentamiento

        StagnationParams(unsigned int problem_size) {
            steps_since_last_improvement = 0;
            steps_since_last_random = 0;
            random_check_interval = DEFAULT_INTERVAL;
            random_states_per_check = DEFAULT_RANDOM_STATES;
            best_heuristic = UINT_MAX;
            stagnation_threshold = STAGNATION_LIMIT;
            temperature = INITIAL_TEMPERATURE;
        }
        void updateAdaptiveParams(bool improved, float current_temp) {
            if (improved) {
                State::adaptive_params.consecutive_improvements++;
                State::adaptive_params.plateaus = 0;
                State::adaptive_params.current_performance = std::min(
                    1.0f, State::adaptive_params.current_performance + 0.1f);
            } else {
                State::adaptive_params.consecutive_improvements = 0;
                if (steps_since_last_improvement > stagnation_threshold / 2) {
                    State::adaptive_params.plateaus++;
                    State::adaptive_params.current_performance = std::max(
                        0.0f,
                        State::adaptive_params.current_performance - 0.05f);
                }
            }

            // Ajustar pesos basados en temperatura
            float temp_factor = current_temp / INITIAL_TEMPERATURE;
            State::adaptive_params.exploration_weight =
                std::min(0.6f, 0.4f + (temp_factor * 0.2f));
            State::adaptive_params.optimization_weight =
                std::max(0.2f, 0.4f - (temp_factor * 0.2f));
        }
        void updateTemperature(bool improved) {
            if (improved) {
                // Si hay mejora, enfriar normalmente
                temperature *= COOLING_RATE;
            } else if (steps_since_last_improvement > stagnation_threshold) {
                // Si estamos estancados, recalentar
                temperature =
                    std::min(temperature * REHEAT_FACTOR, INITIAL_TEMPERATURE);
            } else {
                // Enfriar más lentamente cuando estamos cerca del mínimo
                float current_cooling =
                    COOLING_RATE + (1.0f - COOLING_RATE) *
                                       (temperature - MIN_TEMPERATURE) /
                                       INITIAL_TEMPERATURE;
                temperature *= current_cooling;
            }

            // Asegurar que no bajamos del mínimo
            temperature = std::max(temperature, MIN_TEMPERATURE);
        }
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

    // Funciones auxiliares
    Path reconstructPath(State *final_state, unsigned int total_states);
    void generateRandomVariations(State *current, std::mt19937 &rng,
                                  unsigned int &total_states_generated,
                                  StagnationParams &stag);
};
