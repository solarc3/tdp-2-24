// BranchAndBound.h
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
    private:
    const Graph &graph;
    Bounds &bounds;
    DangerHeuristic &dangerHeuristic;
    mutable cc_hash_table<int, null_type, hash<int>> pruned_vertices;

    // Para usar el set ordenado para los colores disponibles
    using ordered_set = tree<int, null_type, std::less<int>, rb_tree_tag,
                             tree_order_statistics_node_update>;
    using color_set = cc_hash_table<int, null_type, hash<int>>;

    // Métodos para d*
    double calculateDStar(int currentK) const;
    int getMaxIndependentSetSize() const;
    int findBacktrackingDepth(int currentK) const;
    bool branchAndBoundRecursive(ColoringState &state, int targetColors,
                                 int maxBacktrackDepth);
    bool shouldPrune(const ColoringState &state, int vertex, int color) const;
    int selectBestVertex(const ColoringState &state) const;
    color_set getAvailableColorsSet(const ColoringState &state,
                                    int vertex) const;

    public:
    BranchAndBound(const Graph &g, Bounds &b, DangerHeuristic &d)
        : graph(g), bounds(b), dangerHeuristic(d) {}
    void solve(ColoringState &solution);
};
