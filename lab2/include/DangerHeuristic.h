// DangerHeuristic.h
#pragma once
#include "ColoringState.h"
#include "Graph.h"
#include <cmath>
#include <ext/pb_ds/priority_queue.hpp>
#include <random>
class DangerHeuristic {
    struct pair_hash {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2> &pair) const {
            return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
        }
    };
    const Graph &graph;
    mutable std::unordered_map<int, double> vertexDangerCache;
    mutable std::unordered_map<std::pair<int, int>, double, pair_hash>
        colorDangerCache;
    using ColorSet = cc_hash_table<int, null_type, hash<int>>;

    using ScoredVertices =
        tree<pair<double, int>, null_type, less<pair<double, int>>, rb_tree_tag,
             tree_order_statistics_node_update>;
    template <typename T>
    using ordered_set = tree<T, null_type, std::less<T>, rb_tree_tag,
                             tree_order_statistics_node_update>;
    using priority_queue_t =
        __gnu_pbds::priority_queue<std::pair<double, int>,
                                   std::less<std::pair<double, int>>,
                                   pairing_heap_tag>;

    const double C = 1.0;
    const double k = 1.0;
    const double ku = 0.025;
    const double ka = 0.33;

    const double k1 = 1.0;
    const double k2 = 1.0;
    const double k3 = 0.5;
    const double k4 = 0.025;
    mutable ScoredVertices danger_scores;
    mutable std::mt19937 rng;

    public:
    bool isVertexCCDependent(const ColoringState &state, int vertex) const;
    vector<int> getInitialOrderedVertices() const;
    DangerHeuristic(const Graph &g);
    ColoringState generateInitialColoring(int maxColors);
    int selectNextVertex(const ColoringState &state) const;
    int selectColor(const ColoringState &state, int vertex) const;
    double calculateVertexDanger(const ColoringState &state, int vertex) const;
    double calculateColorDanger(const ColoringState &state, int vertex,
                                int color) const;

    int getDifferentColoredNeighbors(const ColoringState &state,
                                     int vertex) const;
    int getUncoloredNeighbors(const ColoringState &state, int vertex) const;
    double getColorShareRatio(const ColoringState &state, int vertex) const;
    void clearCaches();
};
