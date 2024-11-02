#pragma once
#include "../include/TracyMacros.h"
#include "Search.h"
#include "State.h"
#include <chrono>
#include <iostream>
#include <string>
class Solver {
    public:
    Solver();
    ~Solver();
    bool initializeFromFile(const std::string &filename);
    void solve();
    bool isInitialized() const;
    void printCurrentStates() const;

    private:
    State *max_state;
    State *target_state;
    bool initialized;
    void cleanup();
};
