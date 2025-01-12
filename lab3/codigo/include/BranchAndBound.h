#pragma once
#include "Bounds.h"
#include "ColoringState.h"
#include "DangerHeuristic.h"
#include "Graph.h"
#include <chrono>
#include <memory>
#include <random>
#include <set>
#include <vector>
class BranchAndBound {
    private:
    const Graph &graph;
    DangerHeuristic &dangerHeuristic;
    Bounds &bounds;
    ColoringState bestValidSolution; // mejor solucion hasta ahora
    int bestK;                       // best number de colores hasta ahora
    typedef tree<double, int, std::less<double>, rb_tree_tag,
                 tree_order_statistics_node_update>
        OrderedColorSet; // gnu pbds stuff
    bool branchAndBoundRecursive(ColoringState &state, int targetColors);
    bool isColorValid(const ColoringState &state, int vertex, int color) const;
    int selectBestVertex(const ColoringState &state) const;

    public:
    BranchAndBound(const Graph &g, Bounds &b, DangerHeuristic &dh);

    void solve(ColoringState &solution);
};
