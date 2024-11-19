#pragma once
#include "ColorDependency.h"
#include "ColoringState.h"
#include "Graph.h"
#include <cmath>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <random>
using namespace __gnu_pbds;

class DangerHeuristic {
    using ColorSet = cc_hash_table<int, null_type, hash<int>>;

    // Use tree for ordered scoring
    using ScoredVertices =
        tree<pair<double, int>, null_type, less<pair<double, int>>, rb_tree_tag,
             tree_order_statistics_node_update>;

    private:
    const Graph &graph;

    // Parametros adaptativos
    struct DangerParameters {
        double C = 1.0;    // Constante base
        double k = 1.0;    // Exponente criticidad
        double ku = 0.025; // Peso vertices no coloreados
        double ka = 0.33;  // Peso ratio compartido
        // Nuevos parametros adaptativos
        double kd = 0.5; // Peso para grado dinamico
        double ks = 0.4; // Peso para estabilidad
        double kp = 0.3; // Peso para historial previo

<<<<<<< HEAD
    const double k1 = 1.0;
    const double k2 = 1.0;
    const double k3 = 0.5;
    const double k4 = 0.025;
    mutable ColorSet colored_vertices;
    mutable ScoredVertices danger_scores;
=======
        // Parametros para color danger
        double k1 = 1.0;
        double k2 = 1.0;
        double k3 = 0.5;
        double k4 = 0.025;

        double criticalityBase = 1.0;
        double criticalityExponent = 2.0;

        // Parámetros para dependency shrinking
        double dependencyThreshold = 0.8;
        double shrinkingRatio = 0.2;
    } params;

    // Usar GNU PBDS para ordenamiento eficiente
    using ColorSet = tree<int, null_type, less<int>, rb_tree_tag,
                          tree_order_statistics_node_update>;

    using OrderedVertices =
        tree<pair<double, int>, null_type, less<pair<double, int>>, rb_tree_tag,
             tree_order_statistics_node_update>;

    mutable vector<ColorSet> colorHistory; // Historial de colores por vertice
    mutable vector<int> stableCount;       // Contador estabilidad
    mutable OrderedVertices vertexQueue;   // Cola de vertices por danger

    // Grados dinamicos
    struct DynamicDegree {
        vector<int> degree;
        vector<int> effectiveDegree;
        vector<double> saturation;

        DynamicDegree(int n) : degree(n), effectiveDegree(n), saturation(n) {}

        void update(int vertex, const ColoringState &state) {
            degree[vertex] = 0;
            effectiveDegree[vertex] = 0;
            saturation[vertex] = 0;

            ColorSet usedColors;
            for (int u : state.graph.getNeighbors(vertex)) {
                degree[vertex]++;
                if (state.getColor(u) != -1) {
                    effectiveDegree[vertex]++;
                    usedColors.insert(state.getColor(u));
                }
            }
            saturation[vertex] = usedColors.size();
        }
    };
    double calculateCriticalityFactor(int differentColors,
                                      int maxColors) const {
        double v = maxColors - differentColors;
        return params.criticalityBase / pow(v, params.criticalityExponent);
    }
    DynamicDegree dynamicDegree;

    // Metodos auxiliares nuevos
    double calculateSaturationDegree(const ColoringState &state,
                                     int vertex) const;
    double calculateNeighborImpact(const ColoringState &state,
                                   int vertex) const;
    double calculateStability(int vertex) const;
    double calculateConflictPenalty(const ColoringState &state,
                                    int vertex) const;
    double calculateColorBalance(const ColoringState &state, int color) const;
    bool stagnationDetected() const;
>>>>>>> d9d247d (no funciona deje la zorra)

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
<<<<<<< HEAD
    int selectRandomFromTop(const vector<int> &candidates, int topK) const;
=======
    void updateParameters(const ColoringState &state);
    int selectRandomFromTop(const vpii &scores, int topK) const;
>>>>>>> d9d247d (no funciona deje la zorra)
    int getDifferentColoredNeighbors(const ColoringState &state,
                                     int vertex) const;
    int getUncoloredNeighbors(const ColoringState &state, int vertex) const;
    double getColorShareRatio(const ColoringState &state, int vertex) const;
};
