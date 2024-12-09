#pragma once
#include "Bounds.h"
#include "ColoringState.h"
#include "DangerHeuristic.h"
#include "Graph.h"
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <vector>
using namespace __gnu_pbds;

class BranchAndBound {
    using hash_set = cc_hash_table<int, null_type, hash<int>>;

    private:
    const Graph &graph;
    Bounds &bounds;
    DangerHeuristic &dangerHeuristic;
    mutable hash_set pruned_vertices;

    // Método principal recursivo de B&B
    bool branchAndBoundRecursive(ColoringState &state, int targetColors);

    public:
    BranchAndBound(const Graph &g, Bounds &b, DangerHeuristic &d)
        : graph(g), bounds(b), dangerHeuristic(d) {}

    void solve(ColoringState &solution);
    bool shouldPrune(const ColoringState &state, int vertex, int color) const;
    bool isInfeasible(const ColoringState &state, int vertex, int target) const;
    int selectBestVertex(const ColoringState &state) const;
};
