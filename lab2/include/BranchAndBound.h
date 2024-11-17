#pragma once
#include "Bounds.h"
#include "ColoringState.h"
#include "DangerHeuristic.h"
#include "Graph.h"
#include <chrono>
#include <vector>

class BranchAndBound {
    private:
    const Graph &graph;
    Bounds &bounds;
    DangerHeuristic &dangerHeuristic;
    int maxIterations;
    std::chrono::milliseconds timeLimit;

    // Método principal recursivo de B&B
    bool branchAndBoundRecursive(ColoringState &state, int targetColors);

    public:
    BranchAndBound(const Graph &g, Bounds &b, DangerHeuristic &d,
                   int maxIter = 1000000,
                   std::chrono::milliseconds tLimit = std::chrono::hours(1));

    // Método principal que inicia la búsqueda
    void solve(ColoringState &solution);
};
