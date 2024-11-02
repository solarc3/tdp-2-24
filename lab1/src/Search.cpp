#include "../include/Search.h"
#include <climits>
#include <random>

Search::Search(State *initial_state, State *target_state,
               const unsigned int *capacities) {
    this->capacities = capacities;
    this->initial_state = initial_state;
    this->target_state = target_state;
    this->initial_state->calculateHeuristic(*target_state);
}

Search::~Search() {
    TRACE_SCOPE;
    while (open_list.number > 0) {
        State *state = open_list.pop();
        if (state != initial_state && state != target_state) {
            delete state;
        }
    }
    closed_list.cleanup();
}
void Search::randomDFS(State *start_state, unsigned int max_states,
                       unsigned int max_depth_diff, HashTable &visited_states) {
    TRACE_SCOPE;

    // Stack implementado como array dinámico
    const unsigned int STACK_INITIAL_SIZE = 1024;
    unsigned int stack_capacity = STACK_INITIAL_SIZE;
    unsigned int stack_size = 0;
    State **stack = new State *[stack_capacity];

    // Lista temporal para mantener estados que serán agregados a la lista
    // abierta
    const unsigned int STATES_ARRAY_SIZE = max_states;
    State **states_to_add = new State *[STATES_ARRAY_SIZE];
    unsigned int num_states = 0;

    // Inicializar el generador de números aleatorios
    std::random_device rd;
    auto rng = std::mt19937();
    rng.seed(rd());

    // Agregar estado inicial al stack
    stack[stack_size++] = start_state;

    while (stack_size > 0 && num_states < max_states) {
        State *current = stack[--stack_size];

        // Si es un estado válido dentro del límite de profundidad
        if (current->depth - start_state->depth <= max_depth_diff) {
            // Si no es el estado inicial y no ha sido visitado
            if (current != start_state && !visited_states.contains(current)) {
                visited_states.insert(current);
                current->calculateHeuristic(*target_state);
                states_to_add[num_states++] = current;

                TRACE_PLOT("Random DFS states found",
                           static_cast<int64_t>(num_states));
                TRACE_PLOT(
                    "Current depth difference",
                    static_cast<int64_t>(current->depth - start_state->depth));

                // Generar sucesores
                unsigned int num_successors;
                State **successors =
                    current->generateSuccessors(capacities, num_successors);

                // Mezclar aleatoriamente los sucesores
                for (unsigned int i = num_successors - 1; i > 0; i--) {
                    std::uniform_int_distribution<unsigned int> dist(0, i);
                    unsigned int j = dist(rng);
                    if (i != j) {
                        std::swap(successors[i], successors[j]);
                    }
                }

                // Redimensionar stack si es necesario
                if (stack_size + num_successors > stack_capacity) {
                    unsigned int new_capacity = stack_capacity * 2;
                    while (new_capacity < stack_size + num_successors) {
                        new_capacity *= 2;
                    }

                    State **new_stack = new State *[new_capacity];
                    for (unsigned int i = 0; i < stack_size; i++) {
                        new_stack[i] = stack[i];
                    }
                    delete[] stack;
                    stack = new_stack;
                    stack_capacity = new_capacity;
                }

                // Agregar sucesores al stack
                for (unsigned int i = 0; i < num_successors; i++) {
                    stack[stack_size++] = successors[i];
                }

                delete[] successors;
            }
            // Si es un estado visitado o es el inicial, ignorarlo
            else if (current != start_state) {
                delete current;
            }
        }
        // Si excede la profundidad máxima, eliminarlo
        else if (current != start_state) {
            delete current;
        }
    }

    // Limpiar estados restantes en el stack
    while (stack_size > 0) {
        State *state = stack[--stack_size];
        if (state != start_state) {
            delete state;
        }
    }

    // Agregar los estados encontrados a la open_list
    for (unsigned int i = 0; i < num_states; i++) {
        if (states_to_add[i] != nullptr) {
            open_list.push(states_to_add[i]);
        }
    }

    // Liberar memoria
    delete[] states_to_add;
    delete[] stack;
}
Search::Path Search::findPath() {
    TRACE_SCOPE;
    open_list.push(initial_state);
    unsigned int steps = 0;
    unsigned int total_states_generated = 0;

    // stagnation randomizer
    struct {
        unsigned int counter = 0;
        unsigned int consecutive_fails = 0;
        unsigned int best_heuristic = UINT_MAX;
        unsigned int last_improvement_depth = 0;
        // parametrizado para soluciones cortas
        unsigned int base_stagnation_limit = 350;
        unsigned int current_stagnation_limit = 350;
        unsigned int mutation_rate = 42;
        unsigned int num_jumps = 3;

        // Rangos ajustados
        const unsigned int MIN_STAGNATION = 250;
        const unsigned int MAX_STAGNATION = 600;
        const unsigned int MIN_MUTATION = 35;
        const unsigned int MAX_MUTATION = 45;
        const unsigned int MIN_JUMPS = 2;
        const unsigned int MAX_JUMPS = 4;
    } stag;
    unsigned int states_from_base = 0;
    unsigned int states_from_jumps = 0;
    unsigned int best_solution_cost = UINT_MAX;

    // RNG
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<unsigned int> mutation_chance(0, 99);

    while (open_list.number > 0) {
        TRACE_SCOPE_NAMED("SearchIteration");
        State *current = open_list.pop();
        steps++;

        // Verificar solución
        if (current->equals(target_state)) {
            unsigned int solution_cost = current->depth;

            // Si encontramos una mejor solución, actualizar parámetros
            if (solution_cost < best_solution_cost) {
                best_solution_cost = solution_cost;
                // Reducir límites si la estrategia actual fue exitosa
                if (states_from_jumps > states_from_base) {
                    stag.mutation_rate =
                        std::min(stag.mutation_rate + 5, stag.MAX_MUTATION);
                    stag.num_jumps =
                        std::min(stag.num_jumps + 1, stag.MAX_JUMPS);
                } else {
                    stag.mutation_rate =
                        std::max(stag.mutation_rate - 5, stag.MIN_MUTATION);
                    stag.num_jumps =
                        std::max(stag.num_jumps - 1, stag.MIN_JUMPS);
                }
            }

            // Reportar estadísticas
            std::cout << "\nEstadísticas de búsqueda:" << std::endl;
            std::cout << "Estados totales: " << total_states_generated
                      << std::endl;
            std::cout << "Estados base: " << states_from_base << " ("
                      << (states_from_base * 100.0 / total_states_generated)
                      << "%)" << std::endl;
            std::cout << "Estados saltos: " << states_from_jumps << " ("
                      << (states_from_jumps * 100.0 / total_states_generated)
                      << "%)" << std::endl;

            Path path = reconstructPath(current);
            return path;
        }

        // Actualizar métricas de estancamiento
        if (current->weight < stag.best_heuristic) {
            stag.best_heuristic = current->weight;
            stag.counter = 0;
            stag.consecutive_fails = 0;
            stag.last_improvement_depth = current->depth;

            // Ajustar límite de estancamiento basado en el progreso
            float progress_ratio = static_cast<float>(current->depth) / steps;
            if (progress_ratio > 0.1f) {
                stag.current_stagnation_limit =
                    std::max(stag.MIN_STAGNATION,
                             static_cast<unsigned int>(
                                 stag.base_stagnation_limit * 0.8f));
            } else {
                stag.current_stagnation_limit =
                    std::min(stag.MAX_STAGNATION,
                             static_cast<unsigned int>(
                                 stag.base_stagnation_limit * 1.2f));
            }
        } else {
            stag.counter++;
        }

        // Estrategia de saltos cuando hay estancamiento
        if (stag.counter > stag.current_stagnation_limit) {
            stag.consecutive_fails++;

            // Ajustar parámetros basados en fallos consecutivos
            if (stag.consecutive_fails > 3) {
                stag.mutation_rate =
                    std::min(stag.mutation_rate + 5, stag.MAX_MUTATION);
                stag.num_jumps = std::min(stag.num_jumps + 1, stag.MAX_JUMPS);
            }

            // Generar saltos adaptativos
            for (unsigned int jump = 0; jump < stag.num_jumps; jump++) {
                unsigned int *new_jugs = new unsigned int[current->size];
                memcpy(new_jugs, current->jugs,
                       current->size * sizeof(unsigned int));

                bool made_changes = false;
                // Modificar jarras usando mutation_rate adaptativo
                for (unsigned int i = 0; i < current->size; i++) {
                    if (mutation_chance(rng) < stag.mutation_rate) {
                        // Calcular valor basado en el target y actual
                        unsigned int target_val = target_state->jugs[i];
                        unsigned int current_val = current->jugs[i];

                        // Favorecer movimientos hacia el target
                        if (current_val != target_val) {
                            std::uniform_int_distribution<unsigned int>
                                val_dist(std::min(current_val, target_val),
                                         std::max(current_val, target_val));
                            new_jugs[i] = val_dist(rng);
                            made_changes = true;
                        }
                    }
                }

                if (made_changes) {
                    State *random_state =
                        new State(current->size, new_jugs, current->depth + 1,
                                  0, current);
                    random_state->calculateHeuristic(*target_state);

                    if (!closed_list.contains(random_state)) {
                        open_list.push(random_state);
                        states_from_jumps++;
                        total_states_generated++;
                    } else {
                        delete random_state;
                    }
                }

                delete[] new_jugs;
            }

            stag.counter = 0;
            continue;
        }

        // Búsqueda base normal
        {
            TRACE_SCOPE_NAMED("StateProcessing");
            if (closed_list.contains(current)) {
                delete current;
                continue;
            }

            closed_list.insert(current);
            unsigned int num_successors;
            State **successors =
                current->generateSuccessors(capacities, num_successors);
            total_states_generated += num_successors;
            states_from_base += num_successors;

            for (unsigned int i = 0; i < num_successors; i++) {
                if (closed_list.contains(successors[i])) {
                    delete successors[i];
                    continue;
                }

                successors[i]->calculateHeuristic(*target_state);
                open_list.push(successors[i]);
            }

            delete[] successors;
        }

        // Métricas de progreso
        TRACE_PLOT("Current step", static_cast<int64_t>(steps));
        TRACE_PLOT("States from base", static_cast<int64_t>(states_from_base));
        TRACE_PLOT("States from jumps",
                   static_cast<int64_t>(states_from_jumps));
        TRACE_PLOT("Stagnation counter", static_cast<int64_t>(stag.counter));
        TRACE_PLOT("Current mutation rate",
                   static_cast<int64_t>(stag.mutation_rate));
        TRACE_PLOT("Current num jumps", static_cast<int64_t>(stag.num_jumps));
    }

    // No se encontró solución
    std::cout << "\nNo se encontró solución." << std::endl;
    std::cout << "Estados totales: " << total_states_generated << std::endl;
    std::cout << "Estados base: " << states_from_base << " ("
              << (states_from_base * 100.0 / total_states_generated) << "%)"
              << std::endl;
    std::cout << "Estados saltos: " << states_from_jumps << " ("
              << (states_from_jumps * 100.0 / total_states_generated) << "%)"
              << std::endl;

    return Path{nullptr, 0};
}
Search::Path Search::reconstructPath(State *final_state) {
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
