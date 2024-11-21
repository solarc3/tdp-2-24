// DangerHeuristic.h
#pragma once
#include "ColoringState.h"
#include "Graph.h"
#include <cmath>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <memory>
#include <random>

using namespace __gnu_pbds;
// Añadir esto antes de la clase DangerHeuristic
namespace std {
template <> struct hash<pair<int, int>> {
    size_t operator()(const pair<int, int> &p) const {
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
    }
};
} // namespace std

// También agrega esto para tr1 si sigues teniendo problemas
namespace std {
namespace tr1 {
template <> struct hash<pair<int, int>> {
    size_t operator()(const pair<int, int> &p) const {
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
    }
};
} // namespace tr1
} // namespace std
class DangerHeuristic {
    private:
    using vertex_tree = tree<std::pair<double, int>, null_type,
                             std::less<std::pair<double, int>>, rb_tree_tag,
                             tree_order_statistics_node_update>;

    // Hash table para tracking de vértices y colores
    using color_set = cc_hash_table<int, null_type, hash<int>>;
    using vertex_set = cc_hash_table<int, null_type, hash<int>>;

    const Graph &graph;
    vertex_tree vertex_scores;
    mutable vertex_set active_vertices;

    // Parámetros DANGER según el paper
    struct DangerParams {
        double C = 1.0;    // Constante para función de criticidad
        double k = 1.0;    // Exponente para criticidad
        double ku = 0.025; // Peso para vértices no coloreados
        double ka = 0.33;  // Peso para colores compartidos
        double k1 = 1.0;   // Peso para diferencia de vecinos
        double k2 = 1.0;   // Exponente para color danger
        double k3 = 0.5;   // Peso para vecinos no coloreados
        double k4 = 0.025; // Peso para frecuencia de uso
    } params;

    // Cache para evitar recálculos frecuentes
    mutable gp_hash_table<int, double> vertex_danger_cache;
    mutable gp_hash_table<std::pair<int, int>, double> color_danger_cache;

    // Generador de números aleatorios para desempates
    mutable std::mt19937 rng;

    // Métodos privados de cálculo
    int getDifferentColoredNeighbors(const ColoringState &state,
                                     int vertex) const;
    int getUncoloredNeighbors(const ColoringState &state, int vertex) const;
    double getColorShareRatio(const ColoringState &state, int vertex) const;
    void updateVertexScores(const ColoringState &state);
    double calculateF(int diffColored, int maxColor) const;
    void clearCaches() {
        vertex_danger_cache.clear();
        color_danger_cache.clear();
    }

    public:
    explicit DangerHeuristic(const Graph &g);

    // Métodos principales
    ColoringState generateInitialColoring(int maxColors);
    int selectNextVertex(const ColoringState &state) const;
    int selectColor(const ColoringState &state, int vertex) const;
    double calculateVertexDanger(const ColoringState &state, int vertex) const;
    double calculateColorDanger(const ColoringState &state, int vertex,
                                int color) const;

    // Métodos de soporte para B&B
    bool isVertexCCDependent(const ColoringState &state, int vertex) const;
    vector<int> getInitialOrderedVertices() const;
};
