#include "../include/DangerHeuristic.h"

DangerHeuristic::DangerHeuristic(const Graph &g)
    : graph(g), rng(std::random_device{}()) {}

double DangerHeuristic::calculateVertexDanger(const ColoringState &state,
                                              int vertex) const {
    double differentColored = getDifferentColoredNeighbors(state, vertex);
    double maxColor = state.getMaxUsedColor() + 1;

    // F(y) = C/(maxColor - y)^k donde y es differentColored
    double F = C / pow(maxColor - differentColored, k);
    double uncoloredTerm = ku * getUncoloredNeighbors(state, vertex);
    double shareRatioTerm = ka * getColorShareRatio(state, vertex);

    return F + uncoloredTerm + shareRatioTerm;
}

double DangerHeuristic::calculateColorDanger(const ColoringState &state,
                                             int vertex, int color) const {
    // Encontrar el máximo número de vecinos con diferentes colores
    int maxDiffNeighbors = 0;
    int nc = 0; // vértice con máximo diffNeighbors

    for (int v : graph.getNeighbors(vertex)) {
        if (state.getColor(v) == -1 && state.isValidAssignment(v, color)) {
            int diffNeighbors = getDifferentColoredNeighbors(state, v);
            if (diffNeighbors > maxDiffNeighbors) {
                maxDiffNeighbors = diffNeighbors;
                nc = v;
            }
        }
    }

    // Calcular el danger del color según la fórmula del paper
    double maxColor = state.getMaxUsedColor() + 1;
    double diffNeighborsTerm = k1 * pow(maxColor - maxDiffNeighbors, k2);
    double uncoloredTerm = k3 * getUncoloredNeighbors(state, nc);
    double frequencyTerm = k4 * state.getVerticesWithColor(color).size();

    return diffNeighborsTerm + uncoloredTerm - frequencyTerm;
}

ColoringState DangerHeuristic::generateInitialColoring(int maxColors) {
    ColoringState state(graph, maxColors);

    // Get initial vertex ordering
    auto orderedVertices = getInitialOrderedVertices();

    for (int vertex : orderedVertices) {
        // Skip CC-dependent vertices
        if (isVertexCCDependent(state, vertex))
            continue;

        int color = selectColor(state, vertex);
        state.assignColor(vertex, color);
    }

    // Color CC-dependent vertices last
    for (int v = 0; v < graph.getVertexCount(); v++) {
        if (state.getColor(v) == -1) {
            int color = selectColor(state, v);
            state.assignColor(v, color);
        }
    }

    return state;
}

int DangerHeuristic::selectNextVertex(const ColoringState &state) const {
    danger_scores.clear();
    for (int v = 0; v < graph.getVertexCount(); v++) {
        if (state.getColor(v) == -1) {
            double danger = calculateVertexDanger(state, v);
            danger_scores.insert({danger, v});
        }
    }

    // Simply return the vertex with highest danger score
    if (!danger_scores.empty()) {
        return danger_scores.rbegin()->second;
    }
    return -1;
}

int DangerHeuristic::selectColor(const ColoringState &state, int vertex) const {
    vector<int> availableColors = state.getAvailableColors(vertex);
    if (availableColors.empty())
        return 0;

    double minDanger = std::numeric_limits<double>::max();
    int selectedColor = availableColors[0];

    for (int color : availableColors) {
        double danger = calculateColorDanger(state, vertex, color);
        if (danger < minDanger) {
            minDanger = danger;
            selectedColor = color;
        }
    }

    return selectedColor;
}

int DangerHeuristic::getDifferentColoredNeighbors(const ColoringState &state,
                                                  int vertex) const {
    vector<bool> usedColors(state.getMaxUsedColor() + 1, false);
    int count = 0;

    for (int neighbor : graph.getNeighbors(vertex)) {
        int color = state.getColor(neighbor);
        if (color != -1 && !usedColors[color]) {
            usedColors[color] = true;
            count++;
        }
    }

    return count;
}

int DangerHeuristic::getUncoloredNeighbors(const ColoringState &state,
                                           int vertex) const {
    int count = 0;
    for (int neighbor : graph.getNeighbors(vertex)) {
        if (state.getColor(neighbor) == -1) {
            count++;
        }
    }
    return count;
}

double DangerHeuristic::getColorShareRatio(const ColoringState &state,
                                           int vertex) const {
    int availableColors = 0;
    int sharedColors = 0;
    vector<bool> colorAvailable(state.getMaxUsedColor() + 1, true);

    // Marcar colores no disponibles debido a vecinos
    for (int neighbor : graph.getNeighbors(vertex)) {
        int color = state.getColor(neighbor);
        if (color != -1) {
            colorAvailable[color] = false;
        }
    }

    // Contar colores disponibles y compartidos
    for (size_t c = 0; c < colorAvailable.size(); c++) {
        if (colorAvailable[c]) {
            availableColors++;
            // Verificar si algún vecino no coloreado también puede usar este
            // color
            bool shared = false;
            for (int neighbor : graph.getNeighbors(vertex)) {
                if (state.getColor(neighbor) == -1 &&
                    state.isValidAssignment(neighbor, c)) {
                    shared = true;
                    break;
                }
            }
            if (shared)
                sharedColors++;
        }
    }

    return availableColors > 0
               ? static_cast<double>(sharedColors) / availableColors
               : 1.0;
}

bool DangerHeuristic::isVertexCCDependent(const ColoringState &state,
                                          int vertex) const {
    int maxColor = state.getMaxUsedColor();
    if (maxColor < 0)
        return false;

    int potentialDiff = getDifferentColoredNeighbors(state, vertex) +
                        getUncoloredNeighbors(state, vertex);

    return potentialDiff < maxColor;
}

vector<int> DangerHeuristic::getInitialOrderedVertices() const {
    vector<pair<int, int>> vertices;
    for (int v = 0; v < graph.getVertexCount(); v++) {
        vertices.emplace_back(v, graph.getDegree(v));
    }

    sort(vertices.begin(), vertices.end(),
         [](const auto &a, const auto &b) { return a.second > b.second; });

    vector<int> ordered;
    ordered.reserve(vertices.size());
    for (const auto &[v, d] : vertices) {
        ordered.push_back(v);
    }

    return ordered;
}
