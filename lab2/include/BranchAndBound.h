#pragma once
#include "Bounds.h"
#include "ColoringState.h"
#include "DangerHeuristic.h"
#include "Graph.h"
#include <chrono>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <vector>
using namespace __gnu_pbds;

class BranchAndBound {
    using vertex_set = tree<pair<double, int>, // Par {danger,vertex}
                            null_type, less<pair<double, int>>, rb_tree_tag,
                            tree_order_statistics_node_update>;
    using hash_set = cc_hash_table<int, null_type, hash<int>>;

    private:
    int upBnd = 1; // destructivo I
    int loBnd = 3; // destructivo II
    int maxBacktrackDepth;
    const Graph &graph;
    Bounds &bounds;
    DangerHeuristic &dangerHeuristic;
    int maxIterations;
    std::chrono::milliseconds timeLimit;
    double alpha; // Factor for marginal cost pruning
    mutable hash_set pruned_vertices;
    mutable vertex_set vertex_order;

    // Metodo principal recursivo de B&B
    bool branchAndBoundRecursive(ColoringState &state, int targetColors);

    public:
<<<<<<< HEAD
    BranchAndBound(const Graph &g, Bounds &b, DangerHeuristic &d,
                   double alpha = 0.1, int maxIter = 1000000,
                   std::chrono::milliseconds tLimit = std::chrono::hours(1));

    void solve(ColoringState &solution);
    bool shouldPrune(const ColoringState &state, int vertex, int color) const;
    bool isInfeasible(const ColoringState &state, int vertex) const;
    int selectBestVertex(const ColoringState &state) const;
=======
    BranchAndBound(const Graph &g, Bounds &b, DangerHeuristic &d);

    void solve(ColoringState &solution);
    int calculateDStar(const ColoringState &state) const;
    bool isEligibleBacktrackNode(const ColoringState &state, int vertex) const;

    bool backtrackToEligibleNode(ColoringState &state, int fromPos, int toPos);
>>>>>>> d9d247d (no funciona deje la zorra)
};
