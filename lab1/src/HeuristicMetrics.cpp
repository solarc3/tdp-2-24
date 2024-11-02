#include "../include/HeuristicMetrics.h"
// variables para medir cantidades
unsigned int HeuristicMetrics::total_steps = 0;
unsigned int HeuristicMetrics::chosen_transfers = 0;
unsigned int HeuristicMetrics::chosen_pattern = 0;

void HeuristicMetrics::recordChoice(unsigned int w_transfers,
                                    unsigned int w_pattern) {
    TRACE_SCOPE;

    unsigned int min_val = (w_transfers < w_pattern) ? w_transfers : w_pattern;

    TRACE_PLOT("transfers chosen", static_cast<int64_t>(chosen_transfers));
    TRACE_PLOT("Pattern chosen", static_cast<int64_t>(chosen_pattern));
    if (w_transfers == min_val) {
        chosen_transfers++;
        TRACE_PLOT("Chosen Heuristic", static_cast<int64_t>(1));
    }
    if (w_pattern == min_val) {
        chosen_pattern++;
        TRACE_PLOT("Chosen Heuristic", static_cast<int64_t>(2));
    }

    TRACE_PLOT("Total Heuristic Steps", static_cast<int64_t>(total_steps));
    TRACE_PLOT("transfers Value", static_cast<int64_t>(w_transfers));
    TRACE_PLOT("Pattern Value", static_cast<int64_t>(w_pattern));
    TRACE_PLOT("Values sum", static_cast<int64_t>(w_transfers + w_pattern));
}
