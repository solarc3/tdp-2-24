// DangerHeuristic.h
#pragma once
#include "ColoringState.h"
#include "Graph.h"
#include <cmath>
#include <random>

class DangerHeuristic {
    private:
    const Graph &graph;

    // Parámetros de la heurística (según los papers)
    const double C = 1.0;    // Constante para función de criticidad
    const double k = 1.0;    // Exponente para función de criticidad
    const double ku = 0.025; // Peso para vértices no coloreados
    const double ka = 0.33;  // Peso para colores compartidos

    // Parámetros para selección de color
    const double k1 = 1.0;   // Peso para neighbors diferentes
    const double k2 = 1.0;   // Exponente para función de color
    const double k3 = 0.5;   // Peso para vértices no coloreados
    const double k4 = 0.025; // Peso para frecuencia de uso del color

    // Generador de números aleatorios
    mutable std::mt19937 rng;

    public:
    DangerHeuristic(const Graph &g);
    ColoringState generateInitialColoring(int maxColors);
    int selectNextVertex(const ColoringState &state) const;
    int selectColor(const ColoringState &state, int vertex) const;
    double calculateVertexDanger(const ColoringState &state, int vertex) const;
    double calculateColorDanger(const ColoringState &state, int vertex,
                                int color) const;
    int selectRandomFromTop(const vector<pair<int, double>> &scores,
                            int topK) const;
    int getDifferentColoredNeighbors(const ColoringState &state,
                                     int vertex) const;
    int getUncoloredNeighbors(const ColoringState &state, int vertex) const;
    double getColorShareRatio(const ColoringState &state, int vertex) const;
};
