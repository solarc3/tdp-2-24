#include "../include/Search.h"

Search::Search(State *initial_state, State *target_state,
               const unsigned int *capacities) {
    TRACE_SCOPE;
    this->capacities = capacities;
    this->initial_state = initial_state;
    this->target_state = target_state;
    this->initial_state->calculateHeuristic(*target_state);
}

Search::~Search() {
    TRACE_SCOPE;
    while (!open_list.empty()) {
        State *state = open_list.pop();
        if (state != initial_state && state != target_state) {
            delete state;
        }
    }
    closed_list.cleanup();
}

Search::Path Search::findPath() {
    TRACE_SCOPE;

    open_list.push(initial_state);
    unsigned int steps = 0;
    unsigned int total_states_generated = 0;

    std::random_device rd;
    std::mt19937 rng(rd());

    StagnationParams stag(initial_state->size);

    while (!open_list.empty()) {
        State *current = open_list.pop();
        steps++;
        stag.steps_since_last_improvement++;
        stag.steps_since_last_random++;

        if (current->equals(target_state)) {
            std::cout << "\nEstadísticas de búsqueda:" << std::endl;
            std::cout << "Estados totales: " << total_states_generated
                      << std::endl;
            return reconstructPath(
                current,
                total_states_generated); // Pasar total_states_generated
        }

        if (!closed_list.contains(current)) {
            closed_list.insert(current);
            unsigned int num_successors;
            State **successors =
                current->generateSuccessors(capacities, num_successors);
            total_states_generated += num_successors;

            for (unsigned int i = 0; i < num_successors; i++) {
                if (!closed_list.contains(successors[i])) {
                    successors[i]->calculateHeuristic(*target_state);
                    open_list.push(successors[i]);
                } else {
                    delete successors[i];
                }
            }
            delete[] successors;

            if (stag.steps_since_last_random >= stag.random_check_interval) {
                generateRandomVariations(current, rng, total_states_generated,
                                         stag);
                stag.steps_since_last_random = 0;

                if (current->weight < stag.best_heuristic) {
                    stag.best_heuristic = current->weight;
                }
            }
        }
        TRACE_PLOT("Search/Lists/OpenSize",
                   static_cast<int64_t>(open_list.size_));
        TRACE_PLOT("Search/Lists/ClosedSize",
                   static_cast<int64_t>(closed_list.size_));

        TRACE_PLOT("Search/Progress/CurrentDepth",
                   static_cast<int64_t>(current->depth));
        TRACE_PLOT("Search/Progress/BestHeuristic",
                   static_cast<int64_t>(stag.best_heuristic));

        TRACE_PLOT("Search/Progress/StatesGenerated",
                   static_cast<int64_t>(total_states_generated));
        TRACE_PLOT("Search/Progress/Steps", static_cast<int64_t>(steps));
    }

    std::cout << "\nNo se encontró solución." << std::endl;
    std::cout << "Estados totales: " << total_states_generated << std::endl;
    return Path{nullptr, 0};
}

Search::Path Search::reconstructPath(State *final_state,
                                     unsigned int total_states) {
    TRACE_SCOPE;
    unsigned int length = 0;
    State *current = final_state;

    while (current != nullptr) {
        length++;
        current = current->parent;
    }

    State **path_states = new State *[length];

    current = final_state;
    int index = length - 1;
    while (current != nullptr) {
        path_states[index] = current;
        closed_list.removeState(current);
        current = current->parent;
        index--;
    }

    return {path_states, length};
}

void Search::freePath(Path &path) {
    TRACE_SCOPE;
    if (path.states != nullptr) {
        delete[] path.states;
        path.states = nullptr;
        path.length = 0;
    }
}

void Search::generateRandomVariations(State *current, std::mt19937 &rng,
                                      unsigned int &total_states_generated,
                                      StagnationParams &stag) {
    TRACE_SCOPE;

    // Si no hay mejora por mucho tiempo, aumentar la temperatura
    if (stag.steps_since_last_improvement >= stag.stagnation_threshold) {
        stag.temperature = std::min(stag.temperature * stag.REHEAT_FACTOR,
                                    stag.INITIAL_TEMPERATURE);
        stag.steps_since_last_improvement = 0;
    }

    unsigned int *new_jugs = new unsigned int[current->size];
    std::uniform_real_distribution<float> probability(0.0f, 1.0f);
    bool improved = false;

    // Generar varios estados aleatorios
    for (unsigned int i = 0; i < stag.random_states_per_check; i++) {
        std::memcpy(new_jugs, current->jugs,
                    current->size * sizeof(unsigned int));

        // Multi-operation mutations: aplicar múltiples operaciones aleatorias
        unsigned int num_operations = 1 + (rng() % 2); // 1 a 2 operaciones
        bool valid_sequence = false;

        for (unsigned int op = 0; op < num_operations; op++) {
            unsigned int op_type = rng() % 3;
            bool valid_operation = false;
            unsigned int attempts = 0;

            while (!valid_operation && attempts < current->size * 2) {
                switch (op_type) {
                    case 0: { // Transferir
                        unsigned int from = rng() % current->size;
                        unsigned int to = rng() % current->size;

                        if (from != to && new_jugs[from] > 0 &&
                            new_jugs[to] < capacities[to]) {
                            unsigned int space = capacities[to] - new_jugs[to];
                            unsigned int transfer =
                                std::min(new_jugs[from], space);
                            if (transfer > 0) {
                                new_jugs[from] -= transfer;
                                new_jugs[to] += transfer;
                                valid_operation = true;
                                valid_sequence = true;
                            }
                        }
                        break;
                    }
                    case 1: { // Llenar
                        unsigned int jug = rng() % current->size;
                        if (new_jugs[jug] < capacities[jug]) {
                            new_jugs[jug] = capacities[jug];
                            valid_operation = true;
                            valid_sequence = true;
                        }
                        break;
                    }
                    case 2: { // Vaciar
                        unsigned int jug = rng() % current->size;
                        if (new_jugs[jug] > 0) {
                            new_jugs[jug] = 0;
                            valid_operation = true;
                            valid_sequence = true;
                        }
                        break;
                    }
                }
                attempts++;
            }
        }

        if (valid_sequence) {
            State *new_state = new State(current->size, new_jugs,
                                         current->depth + 1, 0, current);
            new_state->calculateHeuristic(*target_state);

            // Criterio de aceptación basado en temperatura
            bool accept = false;
            if (new_state->weight < stag.best_heuristic) {
                accept = true;
                stag.best_heuristic = new_state->weight;
                stag.steps_since_last_improvement = 0;
                improved = true;
                State::adaptive_params.consecutive_improvements++;
                State::adaptive_params.plateaus = 0;
                State::adaptive_params.current_performance = std::min(
                    1.0f, State::adaptive_params.current_performance + 0.1f);
            } else {
                // Probabilidad de aceptar peores soluciones basada en
                // temperatura
                float delta =
                    static_cast<float>(new_state->weight - stag.best_heuristic);
                // Factor de escala dinámico basado en el tamaño del problema
                float scale_factor =
                    500.0f * (1.0f + static_cast<float>(current->size) / 20.0f);
                float acceptance_prob =
                    std::exp(-delta / (stag.temperature * scale_factor));
                accept = probability(rng) < acceptance_prob;

                if (!accept) {
                    State::adaptive_params.consecutive_improvements = 0;
                    if (stag.steps_since_last_improvement >
                        stag.stagnation_threshold / 2) {
                        State::adaptive_params.plateaus++;
                        State::adaptive_params.current_performance = std::max(
                            0.0f,
                            State::adaptive_params.current_performance - 0.05f);
                    }
                }
            }

            TRACE_PLOT("Search/Random/AcceptanceRate",
                       static_cast<int64_t>(acceptance_prob * 100));

            if (accept && (!closed_list.contains(new_state) ||
                           new_state->weight < stag.best_heuristic)) {
                TRACE_PLOT("Search/Random/StatesAccepted",
                           static_cast<int64_t>(1));
                open_list.push(new_state);
                total_states_generated++;
            } else {
                delete new_state;
            }
        }
    }

    // Actualizar temperatura con la nueva lógica
    stag.updateTemperature(improved);

    // Ajustar pesos adaptativos basados en temperatura
    float temp_factor = stag.temperature / stag.INITIAL_TEMPERATURE;
    State::adaptive_params.exploration_weight =
        std::min(0.6f, 0.4f + (temp_factor * 0.2f));
    State::adaptive_params.optimization_weight =
        std::max(0.2f, 0.4f - (temp_factor * 0.2f));
    State::adaptive_params.balance_weight =
        1.0f - (State::adaptive_params.exploration_weight +
                State::adaptive_params.optimization_weight);

    TRACE_PLOT("Search/Random/Temperature",
               static_cast<int64_t>(stag.temperature * 100));
    TRACE_PLOT("Search/Random/StatesGenerated", static_cast<int64_t>(1));
    TRACE_PLOT("Search/Random/StepsSinceImprovement",
               static_cast<int64_t>(stag.steps_since_last_improvement));

    delete[] new_jugs;
}
