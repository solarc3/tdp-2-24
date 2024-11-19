#pragma once
#include "ColoringState.h"
#include "Graph.h"

class ColorDependency {
    private:
    const Graph &graph;
    vector<bool> isDependentVertex;
    vector<int> shrinkSequence;
    int maxColor;

    bool isColorDependent(int vertex, const ColoringState &state) {
        int potentialDifference = calculatePotentialDifference(vertex, state);
        return potentialDifference < maxColor;
    }

    int calculatePotentialDifference(int vertex, const ColoringState &state) {
        int differentColored = state.getDifferentColoredNeighbors(vertex);
        int uncolored = state.getUncoloredNeighbors(vertex);
        return differentColored + uncolored;
    }

    public:
    ColorDependency(const Graph &g, int maxColors)
        : graph(g), isDependentVertex(g.getVertexCount(), false),
          maxColor(maxColors) {}

    // Método principal para shrinking recursivo
    void recursiveShrink(ColoringState &state) {
        bool changed;
        do {
            changed = false;
            for (int v = 0; v < graph.getVertexCount(); v++) {
                if (!isDependentVertex[v] && isColorDependent(v, state)) {
                    isDependentVertex[v] = true;
                    shrinkSequence.push_back(v);
                    changed = true;
                }
            }
        } while (changed);
    }

    // Reconstruir solución expandiendo el grafo
    void expand(ColoringState &state) {
        for (int i = shrinkSequence.size() - 1; i >= 0; i--) {
            int v = shrinkSequence[i];
            // Colorear v con el primer color disponible
            vector<int> availableColors = state.getAvailableColors(v);
            if (!availableColors.empty()) {
                state.assignColor(v, availableColors[0]);
            }
        }
    }
};
