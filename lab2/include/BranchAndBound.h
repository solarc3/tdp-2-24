// BranchAndBound.h
#pragma once
#include "../include/BranchAndBound.h"
#include "Bounds.h"
#include "ColoringState.h"
#include "DangerHeuristic.h"
#include "Graph.h"
#include <cmath> // Para std::pow
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/priority_queue.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <functional> // Para std::less
#include <limits>
#include <random>
#include <vector>

using namespace __gnu_pbds;

class BranchAndBound {
    struct VertexInfo {
        int vertex;
        size_t availableColors;
        double danger;
        bool operator<(const VertexInfo &other) const {
            if (availableColors != other.availableColors)
                return availableColors > other.availableColors;

            return danger < other.danger;
        }
    };
    typedef __gnu_pbds::priority_queue<VertexInfo> VertexQueue;
    VertexQueue vertexQueue;
    const Graph &graph;
    Bounds &bounds;
    DangerHeuristic &dangerHeuristic;
    mutable cc_hash_table<int, null_type, hash<int>> pruned_vertices;
    mutable std::mt19937 gen;
    template <typename T>
    using ordered_set = tree<T, null_type, std::less<T>, rb_tree_tag,
                             tree_order_statistics_node_update>;

    template <typename T> using gp_hash_table = gp_hash_table<T, null_type>;

    using color_set = cc_hash_table<int, null_type, hash<int>>;

    // Métodos para d*
    double calculateDStar(int currentK) const;
    int getMaxIndependentSetSize() const;
    int findBacktrackingDepth(int currentK) const;
    bool branchAndBoundRecursive(ColoringState &state, int targetColors,
                                 int maxBacktrackDepth, int depth);
    bool shouldPrune(const ColoringState &state, int vertex, int color) const;
    int selectBestVertex(const ColoringState &state) const;
    color_set getAvailableColorsSet(const ColoringState &state,
                                    int vertex) const;

    public:
    BranchAndBound(const Graph &g, Bounds &b, DangerHeuristic &d)
        : graph(g), bounds(b), dangerHeuristic(d), gen(std::random_device{}()) {
    }
    void solve(ColoringState &solution);
};
