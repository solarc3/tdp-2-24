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
    // Clear the open list without deleting initial or target states
    while (!open_list.empty()) {
        State *state = open_list.pop();
        if (state != initial_state && state != target_state) {
            delete state;
        }
    }
    HashTable::Bucket *buckets = closed_list.buckets;
    for (unsigned int i = 0; i < closed_list.capacity; i++) {
        if (buckets[i].occupied && buckets[i].state) {
            if (buckets[i].state != initial_state &&
                buckets[i].state != target_state) {
                delete buckets[i].state;
            }
            buckets[i].state = nullptr;
            buckets[i].occupied = false;
            buckets[i].psl = 0;
        }
    }
    closed_list.size = 0;
}

Search::Path Search::findPath() {
    TRACE_SCOPE;
    open_list.push(initial_state);
    unsigned int steps = 0;
    unsigned int total_states_generated = 0;

    std::random_device rd;
    std::knuth_b rng(rd());
    StagnationParams stag(initial_state->size);
    State *best_state = initial_state;

    while (!open_list.empty()) {
        State *current = open_list.pop();
        steps++;
        stag.steps_since_last_improvement++;
        stag.steps_since_last_random++;

        if (current->equals(target_state)) {
            std::cout << "\nEstadísticas de búsqueda:" << std::endl;
            std::cout << "Estados totales: " << total_states_generated
                      << std::endl;
            return reconstructPath(current, total_states_generated);
        }

        if (!closed_list.contains(current)) {
            closed_list.insert(current);

            if (current->weight < stag.best_heuristic) {
                stag.best_heuristic = current->weight;
                best_state = current;
                stag.steps_since_last_improvement = 0;
            }

            unsigned int num_successors;
            State **successors =
                current->generateSuccessors(capacities, num_successors);
            total_states_generated += num_successors;

            for (unsigned int i = 0; i < num_successors; i++) {
                if (!closed_list.contains(successors[i])) {
                    successors[i]->calculateHeuristic(*target_state);

                    // Solo aplicar annealing cuando está activo
                    bool accept =
                        !stag.annealing_active ||
                        successors[i]->weight <= current->weight ||
                        (std::rand() % 100) < (stag.temperature * 100);

                    if (accept) {
                        open_list.push(successors[i]);
                    } else {
                        delete successors[i];
                    }
                } else {
                    delete successors[i];
                }
            }
            delete[] successors;

            // Generación periódica de variaciones aleatorias
            if (stag.steps_since_last_random >= stag.random_check_interval) {
                generateRandomVariations(current, rng, total_states_generated,
                                         stag);
                stag.steps_since_last_random = 0;
            }

            // Actualizar parámetros adaptativos
            stag.updateAdaptiveParams(
                current->weight < stag.best_heuristic, stag.temperature,
                static_cast<float>(current->size) / 30.0f);
        }

        // Activar annealing si hay estancamiento
        stag.annealing_active =
            stag.steps_since_last_improvement > (stag.stagnation_threshold / 2);

        TRACE_PLOT("Search/Lists/OpenSize",
                   static_cast<int64_t>(open_list.size()));
        TRACE_PLOT("Search/Lists/ClosedSize",
                   static_cast<int64_t>(closed_list.size()));
        TRACE_PLOT("Search/Progress/CurrentDepth",
                   static_cast<int64_t>(current->depth));
        TRACE_PLOT("Search/Progress/BestHeuristic",
                   static_cast<int64_t>(stag.best_heuristic));
        TRACE_PLOT("Search/Progress/StatesGenerated",
                   static_cast<int64_t>(total_states_generated));
        TRACE_PLOT("Search/Progress/Steps", static_cast<int64_t>(steps));
    }

    return reconstructPath(best_state, total_states_generated);
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
        if (current->parent != nullptr)
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

void Search::generateRandomVariations(State *current, std::knuth_b &rng,
                                      unsigned int &total_states_generated,
                                      StagnationParams &stag) {
    TRACE_SCOPE;

    unsigned int states_to_generate = static_cast<unsigned int>(
        stag.random_states_per_check *
        (stag.temperature / stag.INITIAL_TEMPERATURE));
    states_to_generate = std::max(
        3u, std::min(states_to_generate, stag.random_states_per_check));

    // Información de estancamiento

    unsigned int *new_jugs = new unsigned int[current->size];
    std::uniform_real_distribution<float> probability(0.0f, 1.0f);
    std::uniform_int_distribution<unsigned int> op_type(0, 2);
    bool improved = false;

    for (unsigned int i = 0; i < states_to_generate; i++) {
        std::memcpy(new_jugs, current->jugs,
                    current->size * sizeof(unsigned int));

        unsigned int attempts = 0;
        bool valid_sequence = false;

        while (!valid_sequence && attempts < 3) {
            unsigned int operation = op_type(rng);

            switch (operation) {
                case 0: { // Transferir
                    unsigned int from = rng() % current->size;
                    unsigned int to = rng() % current->size;

                    if (from != to && new_jugs[from] > 0 &&
                        new_jugs[to] < capacities[to]) {
                        unsigned int space = capacities[to] - new_jugs[to];
                        unsigned int transfer = std::min(new_jugs[from], space);

                        if (transfer > 0) {
                            new_jugs[from] -= transfer;
                            new_jugs[to] += transfer;
                            valid_sequence = true;
                        }
                    }
                    break;
                }
                case 1: { // Llenar
                    unsigned int jug = rng() % current->size;
                    if (new_jugs[jug] < capacities[jug]) {
                        new_jugs[jug] = capacities[jug];
                        valid_sequence = true;
                    }
                    break;
                }
                case 2: { // Vaciar
                    unsigned int jug = rng() % current->size;
                    if (new_jugs[jug] > 0) {
                        new_jugs[jug] = 0;
                        valid_sequence = true;
                    }
                    break;
                }
            }
            attempts++;
        }

        if (valid_sequence) {
            State *new_state = new State(current->size, new_jugs,
                                         current->depth + 1, 0, current);
            new_state->calculateHeuristic(*target_state);

            bool accept = false;
            if (new_state->weight < stag.best_heuristic) {
                accept = true;
                stag.best_heuristic = new_state->weight;
                stag.steps_since_last_improvement = 0;
                improved = true;
                std::cout << "¡MEJORA! " << new_state->weight << " < "
                          << stag.best_heuristic << "\n";
            } else {
                float delta =
                    static_cast<float>(new_state->weight - current->weight);
                float relative_delta = delta / current->weight;

                // Aquí es donde intentamos escapar de mínimos locales
                if (relative_delta < 0.2f) {
                    float acceptance_prob =
                        std::exp(-relative_delta / (stag.temperature * 0.1f));
                    accept = probability(rng) < acceptance_prob;
                }
            }

            if (accept && !closed_list.contains(new_state)) {
                open_list.push(new_state);
                total_states_generated++;
            } else {
                delete new_state;
            }
        }
    }
    stag.updateAdaptiveParams(improved, stag.temperature, 1.0f);
    delete[] new_jugs;
}
Search::StagnationParams::StagnationParams(unsigned int problem_size) {
    steps_since_last_improvement = 0;
    steps_since_last_random = 0;
    random_check_interval = 50;
    random_states_per_check = std::min(problem_size * 2, 10u);
    best_heuristic = UINT_MAX;
    stagnation_threshold = 500;
    temperature = 1.0f;
    annealing_active = false;
}

void Search::StagnationParams::updateTemperature(bool improved) {
    if (improved) {
        temperature *= 0.93f;
    } else if (steps_since_last_improvement > stagnation_threshold) {
        temperature = std::min(temperature * 1.5f, INITIAL_TEMPERATURE);
    } else {
        temperature *= 0.97f;
    }
    temperature = std::max(temperature, 0.1f);
}

void Search::StagnationParams::updateAdaptiveParams(bool improved,
                                                    float current_temp,
                                                    float size_factor) {
    if (improved) {
        temperature *= 0.95f;
        State::adaptive_params.consecutive_improvements++;
        State::adaptive_params.plateaus = 0;
    } else {
        if (steps_since_last_improvement > stagnation_threshold) {
            temperature = std::min(temperature * 1.3f, INITIAL_TEMPERATURE);
            State::adaptive_params.plateaus++;
        } else {
            temperature *= 0.97f;
        }
        State::adaptive_params.consecutive_improvements = 0;
    }

    temperature = std::max(temperature, 0.05f);
    updateWeights();
}

void Search::StagnationParams::updateWeights() {
    float temp_factor = temperature / INITIAL_TEMPERATURE;
    State::adaptive_params.exploration_weight =
        std::min(0.6f, 0.4f + (temp_factor * 0.2f));
    State::adaptive_params.optimization_weight =
        std::max(0.2f, 0.4f - (temp_factor * 0.2f));
    State::adaptive_params.balance_weight =
        1.0f - (State::adaptive_params.exploration_weight +
                State::adaptive_params.optimization_weight);
}
