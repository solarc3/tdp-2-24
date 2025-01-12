#pragma once
#include "Bounds.h"
#include "BranchAndBound.h"
#include "DangerHeuristic.h"
#include "Graph.h"
#include <chrono>
#include <iostream>
#include <limits>
using namespace std;

class Solver {
    private:
    Graph graph;
    bool initialized;
    ColoringState bestSolution;
    std::unique_ptr<Bounds> bounds;
    std::unique_ptr<DangerHeuristic> dangerHeuristic;
    std::unique_ptr<BranchAndBound> bnb;
    std::chrono::milliseconds executionTime;

    public:
    Solver();
    bool initialize(const string &filename);
    void solve();
    void printSolution() const;
    bool isInitialized() const { return initialized; }
    const ColoringState &getSolution() const { return bestSolution; }
    std::chrono::milliseconds getExecutionTime() const { return executionTime; }
};
