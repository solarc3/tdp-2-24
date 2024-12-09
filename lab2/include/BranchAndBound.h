#pragma once
#include "Bounds.h"
#include "ColoringState.h"
#include "DangerHeuristic.h"
#include "Graph.h"
#include <memory>
#include <vector>

class BranchAndBound {
    private:
    const Graph &graph;
    DangerHeuristic &dangerHeuristic;
    Bounds &bounds;

    // Métodos auxiliares para el branch and bound
    bool branchAndBoundRecursive(ColoringState &state, int targetColors,
                                 int vertex);
    bool isValidColoring(const ColoringState &state) const;

    // Verifica si un color es válido para un vértice
    bool isColorValid(const ColoringState &state, int vertex, int color) const;

    // Verifica si el estado actual puede llevar a una solución mejor
    bool isPromising(const ColoringState &state, int targetColors) const;

    public:
    BranchAndBound(const Graph &g, Bounds &b, DangerHeuristic &dh)
        : graph(g), bounds(b), dangerHeuristic(dh) {}

    // Método principal que intenta encontrar una coloración con targetColors
    // colores
    void solve(ColoringState &solution);
};
