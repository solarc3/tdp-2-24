#pragma once
#include "ColoringState.h"
#include "Graph.h"
#include <cmath>
#include <limits>
#include <set>

class DangerHeuristic {
    private:
    const Graph &graph;
    int targetColors;
    // parametros para vertex danger
    const double C = 1.0;
    const double k = 1.0;
    const double ku = 0.025;
    const double ka = 0.33;

    // parametros para node danger
    const double k1 = 1.0;
    const double k2 = 1.0;
    const double k3 = 0.5;
    const double k4 = 0.025;

    double calculateF(int diff_colored, int max_color) const;

    public:
    explicit DangerHeuristic(const Graph &g);

    double calculateVertexDanger(const ColoringState &state, int vertex) const;
    double calculateColorDanger(const ColoringState &state, int vertex,
                                int color) const;
    int selectBestVertex(const ColoringState &state) const;
    int selectBestColor(const ColoringState &state, int vertex) const;
    int getDifferentColoredNeighbors(const ColoringState &state,
                                     int vertex) const;
    int getUncoloredNeighbors(const ColoringState &state, int vertex) const;
    double getColorShareRatio(const ColoringState &state, int vertex) const;
    ColoringState generateInitialColoring(int targetColors);
};
