#pragma once
#include "Search.h"
#include "State.h"
#include <string>

class Solver {
    public:
    Solver();
    ~Solver();

    // Initialize the solver with a file
    bool initializeFromFile(const std::string &filename);

    // Solve the water jugs problem
    void solve();

    // Get current state information
    bool isInitialized() const { return initialized; }
    void printCurrentStates() const;

    private:
    State *max_state;
    State *target_state;
    bool initialized;
    void cleanup();
};
