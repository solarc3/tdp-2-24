#include "../include/HeuristicMetrics.h"
#include "../include/TracyMacros.h"
#include <algorithm>

// Define static members
unsigned int HeuristicMetrics::total_steps = 0;
unsigned int HeuristicMetrics::chosen_manhattan = 0;
unsigned int HeuristicMetrics::chosen_pattern = 0;

void HeuristicMetrics::recordChoice(unsigned int w_manhattan,
                                    unsigned int w_pattern) {
    TRACE_SCOPE;

    unsigned int min_val = std::min({w_manhattan, w_pattern});

    TRACE_PLOT("Manhattan chosen", static_cast<int64_t>(chosen_manhattan));
    TRACE_PLOT("Pattern chosen", static_cast<int64_t>(chosen_pattern));
    if (w_manhattan == min_val) {
        chosen_manhattan++;
        TRACE_PLOT("Chosen Heuristic", static_cast<int64_t>(1));
    }
    if (w_pattern == min_val) {
        chosen_pattern++;
        TRACE_PLOT("Chosen Heuristic", static_cast<int64_t>(2));
    }

    TRACE_PLOT("Total Heuristic Steps", static_cast<int64_t>(total_steps));
    TRACE_PLOT("Manhattan Value", static_cast<int64_t>(w_manhattan));
    TRACE_PLOT("Pattern Value", static_cast<int64_t>(w_pattern));
    TRACE_PLOT("Values sum", static_cast<int64_t>(w_manhattan + w_pattern));
}
