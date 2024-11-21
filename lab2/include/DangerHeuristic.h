// DangerHeuristic.h
#pragma once
#include "ColoringState.h"
#include "Graph.h"
#include <cmath>
#include <random>

class DangerHeuristic {
    using ColorSet = cc_hash_table<int, null_type, hash<int>>;

    // Use tree for ordered scoring
    using ScoredVertices =
        tree<pair<double, int>, null_type, less<pair<double, int>>, rb_tree_tag,
             tree_order_statistics_node_update>;

    private:
    const Graph &graph;

    // Parámetros de la heurística (según los papers)
    const double C = 1.0;    // Constante para función de criticidad
    const double k = 1.0;    // Exponente para función de criticidad
    const double ku = 0.025; // Peso para vértices no coloreados
    const double ka = 0.33;  // Peso para colores compartidos

    const double k1 = 1.0;
    const double k2 = 1.0;
    const double k3 = 0.5;
    const double k4 = 0.025;
    mutable ColorSet colored_vertices;
    mutable ScoredVertices danger_scores;

    // Generador de números aleatorios
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
    int selectRandomFromTop(const vector<int> &candidates, int topK) const;
    int getDifferentColoredNeighbors(const ColoringState &state,
                                     int vertex) const;
    int getUncoloredNeighbors(const ColoringState &state, int vertex) const;
    double getColorShareRatio(const ColoringState &state, int vertex) const;
};
