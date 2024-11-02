#pragma once
#include <Tracy/tracy/Tracy.hpp>

class HeuristicMetrics {
    public:
    static unsigned int total_steps;
    static unsigned int chosen_manhattan;
    static unsigned int chosen_pattern;

    static void recordChoice(unsigned int w_manhattan, unsigned int w_pattern);
};
