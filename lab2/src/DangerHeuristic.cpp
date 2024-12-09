#include "../include/DangerHeuristic.h"
#include <queue>
DangerHeuristic::DangerHeuristic(const Graph &g)
    : graph(g), rng(std::random_device{}()) {}

double DangerHeuristic::calculateVertexDanger(const ColoringState &state,
                                              int vertex) const {
    // Verifica si el valor ya está en la caché
    auto it = vertexDangerCache.find(vertex);
    if (it != vertexDangerCache.end()) {
        return it->second;
    }

    double maxColor = state.getMaxUsedColor() + 1;
    double differentColored = getDifferentColoredNeighbors(state, vertex);

    // Evita división por cero
    if (maxColor - differentColored == 0) {
        vertexDangerCache[vertex] = std::numeric_limits<double>::max();
        return vertexDangerCache[vertex];
    }

    // Cálculo del peligro
    double F = C / std::pow(maxColor - differentColored, k);
    double uncoloredTerm = ku * getUncoloredNeighbors(state, vertex);
    double shareRatioTerm = ka * getColorShareRatio(state, vertex);

    double danger = F + uncoloredTerm + shareRatioTerm;

    // Almacena el valor en la caché
    vertexDangerCache[vertex] = danger;
    return danger;
}
void DangerHeuristic::clearCaches() {
    vertexDangerCache.clear();
    colorDangerCache.clear();
}
double DangerHeuristic::calculateColorDanger(const ColoringState &state,
                                             int vertex, int color) const {
    std::pair<int, int> key = {vertex, color};
    auto it = colorDangerCache.find(key);
    if (it != colorDangerCache.end()) {
        return it->second;
    }
    int maxDiffNeighbors = 0;
    int nc = vertex; // Initialize with the current vertex

    for (int v : graph.getNeighbors(vertex)) {
        if (state.getColor(v) == -1 && state.isValidAssignment(v, color)) {
            int diffNeighbors = getDifferentColoredNeighbors(state, v);
            if (diffNeighbors > maxDiffNeighbors) {
                maxDiffNeighbors = diffNeighbors;
                nc = v;
            }
        }
    }

    double maxColor = state.getMaxUsedColor() + 1;

    // Prevent division by zero
    if (maxColor - maxDiffNeighbors == 0) {
        return std::numeric_limits<double>::max();
    }

    // Calculate the danger of the color according to the formula
    double diffNeighborsTerm = k1 * std::pow(maxColor - maxDiffNeighbors, k2);
    double uncoloredTerm = k3 * getUncoloredNeighbors(state, nc);
    double frequencyTerm = k4 * state.getVerticesWithColor(color).size();

    double danger = diffNeighborsTerm + uncoloredTerm - frequencyTerm;
    colorDangerCache[key] = danger;
    return danger;
}

ColoringState DangerHeuristic::generateInitialColoring(int maxColors) {
    std::cout << "Generando coloreo inicial con máximo " << maxColors
              << " colores" << std::endl;

    ColoringState state(graph, maxColors);

    auto cmp = [](const std::pair<double, int> &left,
                  const std::pair<double, int> &right) {
        return left.first < right.first;
    };
    std::priority_queue<std::pair<double, int>,
                        std::vector<std::pair<double, int>>, decltype(cmp)>
        vertexQueue(cmp);
    for (int v = 0; v < graph.getVertexCount(); ++v) {
        double danger = calculateVertexDanger(state, v);
        vertexQueue.push({danger, v});
    }

    int verticesColored = 0;
    while (!vertexQueue.empty()) {
        auto [danger, vertex] = vertexQueue.top();
        vertexQueue.pop();

        if (state.getColor(vertex) != -1)
            continue;

        int color = selectColor(state, vertex);
        state.assignColor(vertex, color);
        verticesColored++;
    }

    std::cout << "Coloreo inicial completado. Total de vértices coloreados: "
              << verticesColored << std::endl;

    return state;
}

int DangerHeuristic::selectColor(const ColoringState &state, int vertex) const {
    auto availableColors = state.getAvailableColors(vertex);
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
    ordered_set<int> usedColors;
    for (int neighbor : graph.getNeighbors(vertex)) {
        int color = state.getColor(neighbor);
        if (color != -1) {
            usedColors.insert(color);
        }
    }
    return usedColors.size();
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
    auto availableColors = state.getAvailableColors(vertex);
    int availableColorsCount = availableColors.size();
    int sharedColors = 0;

    for (int color : availableColors) {
        // Check if any uncolored neighbor can use this color
        bool shared = false;
        for (int neighbor : graph.getNeighbors(vertex)) {
            if (state.getColor(neighbor) == -1 &&
                state.isValidAssignment(neighbor, color)) {
                shared = true;
                break;
            }
        }
        if (shared)
            sharedColors++;
    }

    return availableColorsCount > 0
               ? static_cast<double>(sharedColors) / availableColorsCount
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

std::vector<int> DangerHeuristic::getInitialOrderedVertices() const {
    std::vector<std::pair<int, int>> vertices;
    for (int v = 0; v < graph.getVertexCount(); v++) {
        vertices.emplace_back(v, graph.getDegree(v));
    }

    std::sort(vertices.begin(), vertices.end(),
              [](const auto &a, const auto &b) { return a.second > b.second; });

    std::vector<int> ordered;
    ordered.reserve(vertices.size());
    for (const auto &[v, d] : vertices) {
        ordered.push_back(v);
    }

    return ordered;
}
