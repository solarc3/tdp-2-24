#pragma once
#include "Bounds.h"
#include "BranchAndBound.h"
#include "DangerHeuristic.h"
#include "Graph.h"
#include <chrono>

class Solver {
    private:
    Graph graph;                                      // 1
    bool initialized;                                 // 2
    ColoringState bestSolution;                       // 3
    std::unique_ptr<Bounds> bounds;                   // 4
    std::unique_ptr<DangerHeuristic> dangerHeuristic; // 5
    std::unique_ptr<BranchAndBound> bnb;              // 6
    std::chrono::milliseconds executionTime;          // 7

    public:
    Solver();
    bool initialize(const string &filename);
    void solve();
    void printSolution() const;
    bool isInitialized() const { return initialized; }
    const ColoringState &getSolution() const { return bestSolution; }
    std::chrono::milliseconds getExecutionTime() const { return executionTime; }
};
