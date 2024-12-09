#pragma once
#include "ColoringState.h"
#include "Graph.h"
#include <cmath>
#include <limits>
#include <set>

class DangerHeuristic {
    private:
    const Graph &graph;

    // Parámetros originales del paper para node_danger
    const double C = 1.0;
    const double k = 1.0;
    const double ku = 0.025;
    const double ka = 0.33;

    // Parámetros originales del paper para color_danger
    const double k1 = 1.0;
    const double k2 = 1.0;
    const double k3 = 0.5;
    const double k4 = 0.025;

    // Método interno para calcular la función F(y) del paper
    double calculateF(int diff_colored, int max_color) const;

    public:
    explicit DangerHeuristic(const Graph &g);

    // Calcula el danger de un vértice según la fórmula del paper
    double calculateVertexDanger(const ColoringState &state, int vertex) const;

    // Calcula el danger de un color para un vértice según la fórmula del paper
    double calculateColorDanger(const ColoringState &state, int vertex,
                                int color) const;

    // Selecciona el siguiente vértice a colorear
    int selectBestVertex(const ColoringState &state) const;

    // Selecciona el mejor color para un vértice
    int selectBestColor(const ColoringState &state, int vertex) const;
};
