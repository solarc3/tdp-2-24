#include "../include/Search.h"
#include "../include/HeuristicMetrics.h"
#include "../include/TracyMacros.h"
#include <iostream>

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

Search::Path Search::findPath() {
    TRACE_SCOPE;
    open_list.push(initial_state);
    unsigned int steps = 0;
    unsigned int total_states_generated = 0;

    // Reset metrics
    HeuristicMetrics::total_steps = 0;
    HeuristicMetrics::chosen_manhattan = 0;
    HeuristicMetrics::chosen_pattern = 0;

    while (open_list.number > 0) {
        TRACE_SCOPE_NAMED("SearchIteration");
        State *current = open_list.pop();
        steps++;

        if (current->equals(target_state)) {
            TRACE_PLOT("Steps explored", static_cast<int64_t>(steps));
            TRACE_PLOT("Total states generated",
                       static_cast<int64_t>(total_states_generated));
            TRACE_PLOT("Open list size",
                       static_cast<int64_t>(open_list.number));
            TRACE_PLOT("Closed list size",
                       static_cast<int64_t>(closed_list.num_elements));

            // Record final heuristic statistics - with safety checks
            if (HeuristicMetrics::total_steps > 0) {
                TRACE_PLOT(
                    "Final Manhattan Ratio",
                    static_cast<int64_t>(HeuristicMetrics::chosen_manhattan *
                                         100 / HeuristicMetrics::total_steps));
                TRACE_PLOT(
                    "Final Pattern Ratio",
                    static_cast<int64_t>(HeuristicMetrics::chosen_pattern *
                                         100 / HeuristicMetrics::total_steps));
            }

            TRACE_FRAME_NAMED("Solution Found");

            // Output statistics
            std::cout << "\nSolution found!" << std::endl;
            std::cout << "Steps explored: " << steps << std::endl;
            std::cout << "Total states generated: " << total_states_generated
                      << std::endl;
            std::cout << "States in open list: " << open_list.number
                      << std::endl;
            std::cout << "States in closed list: " << closed_list.num_elements
                      << std::endl;

            // Output heuristic metrics with safety checks
            std::cout << "\nHeuristic Selection Stats:" << std::endl;
            std::cout << "Total heuristic calculations: "
                      << HeuristicMetrics::total_steps << std::endl;

            if (HeuristicMetrics::total_steps > 0) {
                std::cout << "Manhattan chosen: "
                          << HeuristicMetrics::chosen_manhattan << " ("
                          << (HeuristicMetrics::chosen_manhattan * 100.0 /
                              HeuristicMetrics::total_steps)
                          << "%)" << std::endl;
                std::cout << "Pattern chosen: "
                          << HeuristicMetrics::chosen_pattern << " ("
                          << (HeuristicMetrics::chosen_pattern * 100.0 /
                              HeuristicMetrics::total_steps)
                          << "%)" << std::endl;
            } else {
                std::cout << "Manhattan chosen: "
                          << HeuristicMetrics::chosen_manhattan << " (0%)"
                          << std::endl;
                std::cout << "Pattern chosen: "
                          << HeuristicMetrics::chosen_pattern << " (0%)"
                          << std::endl;
            }

            // Calculate and output average depths with safety checks
            if (HeuristicMetrics::chosen_manhattan > 0) {
                std::cout << "Average depth when Manhattan chosen: "
                          << current->depth * 1.0 /
                                 HeuristicMetrics::chosen_manhattan
                          << std::endl;
            }
            if (HeuristicMetrics::chosen_pattern > 0) {
                std::cout << "Average depth when Pattern chosen: "
                          << current->depth * 1.0 /
                                 HeuristicMetrics::chosen_pattern
                          << std::endl;
            }

            Path path = reconstructPath(current);
            std::cout << "Solution length: " << path.length - 1 << " steps\n"
                      << std::endl;
            return path;
        }

        if (!closed_list.contains(current)) {
            closed_list.insert(current);
            unsigned int num_successors;

            {
                TRACE_SCOPE_NAMED("GenerateSuccessors");
                State **successors =
                    current->generateSuccessors(capacities, num_successors);
                total_states_generated += num_successors;

                {
                    TRACE_SCOPE_NAMED("SuccessorsProcessing");
                    for (unsigned int i = 0; i < num_successors; i++) {
                        if (!closed_list.contains(successors[i])) {
                            {
                                TRACE_SCOPE_NAMED("HeuristicCalculation");
                                successors[i]->calculateHeuristic(
                                    *target_state);
                            }
                            open_list.push(successors[i]);
                        } else {
                            delete successors[i];
                        }
                    }
                }
                delete[] successors;
            }
        } else {
            delete current;
        }

        // Update progress metrics
        TRACE_PLOT("Current step", static_cast<int64_t>(steps));
        TRACE_PLOT("States generated",
                   static_cast<int64_t>(total_states_generated));
        TRACE_PLOT("Open list size", static_cast<int64_t>(open_list.number));
        TRACE_PLOT("Closed list size",
                   static_cast<int64_t>(closed_list.num_elements));

        // Update heuristic selection rates periodically with safety check
        if (steps % 1000 == 0 && HeuristicMetrics::total_steps > 0) {
            TRACE_PLOT(
                "Manhattan Selection Rate",
                static_cast<int64_t>(HeuristicMetrics::chosen_manhattan * 100 /
                                     HeuristicMetrics::total_steps));
            TRACE_PLOT(
                "Pattern Selection Rate",
                static_cast<int64_t>(HeuristicMetrics::chosen_pattern * 100 /
                                     HeuristicMetrics::total_steps));
        }
    }

    // No solution found - record failure metrics
    TRACE_PLOT("Final Steps", static_cast<int64_t>(steps));
    TRACE_PLOT("Final States Generated",
               static_cast<int64_t>(total_states_generated));
    TRACE_FRAME_NAMED("No Solution");

    std::cout << "\nNo solution found!" << std::endl;
    std::cout << "Steps explored: " << steps << std::endl;
    std::cout << "Total states generated: " << total_states_generated
              << std::endl;
    std::cout << "States in closed list: " << closed_list.num_elements << "\n"
              << std::endl;

    // Output final heuristic stats with safety checks
    std::cout << "\nFinal Heuristic Stats:" << std::endl;
    std::cout << "Total calculations: " << HeuristicMetrics::total_steps
              << std::endl;

    if (HeuristicMetrics::total_steps > 0) {
        std::cout << "Manhattan chosen: " << HeuristicMetrics::chosen_manhattan
                  << " ("
                  << (HeuristicMetrics::chosen_manhattan * 100.0 /
                      HeuristicMetrics::total_steps)
                  << "%)" << std::endl;
        std::cout << "Pattern chosen: " << HeuristicMetrics::chosen_pattern
                  << " ("
                  << (HeuristicMetrics::chosen_pattern * 100.0 /
                      HeuristicMetrics::total_steps)
                  << "%)" << std::endl;
    } else {
        std::cout << "Manhattan chosen: " << HeuristicMetrics::chosen_manhattan
                  << " (0%)" << std::endl;
        std::cout << "Pattern chosen: " << HeuristicMetrics::chosen_pattern
                  << " (0%)" << std::endl;
    }

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
