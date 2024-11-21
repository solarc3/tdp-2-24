// DangerHeuristic.cpp
#include "../include/DangerHeuristic.h"

DangerHeuristic::DangerHeuristic(const Graph &g)
    : graph(g), rng(std::random_device{}()) {}

ColoringState DangerHeuristic::generateInitialColoring(int maxColors) {
    ColoringState state(graph, maxColors);
    active_vertices.clear();
    clearCaches();

    // Inicializar conjunto de vértices activos
    for (int v = 0; v < graph.getVertexCount(); v++) {
        active_vertices.insert(v);
    }

    // Colorear vértices
    while (!active_vertices.empty()) {
        updateVertexScores(state);

        int vertex = selectNextVertex(state);
        if (vertex == -1)
            break;

        int color = selectColor(state, vertex);
        state.assignColor(vertex, color);

        // Remover vértice coloreado
        active_vertices.erase(vertex);
    }

    return state;
}

void DangerHeuristic::updateVertexScores(const ColoringState &state) {
    vertex_scores.clear();

    for (const auto &v : active_vertices) {
        double danger = calculateVertexDanger(state, v);
        vertex_scores.insert({danger, v});
    }
}

double DangerHeuristic::calculateVertexDanger(const ColoringState &state,
                                              int vertex) const {
    // Verificar cache
    if (vertex_danger_cache.find(vertex) != vertex_danger_cache.end()) {
        return vertex_danger_cache[vertex];
    }

    int diffColored = getDifferentColoredNeighbors(state, vertex);
    int maxColor = state.getMaxUsedColor() + 1;

    // Calcular componentes de danger
    double F = calculateF(diffColored, maxColor);
    double uncoloredTerm = params.ku * getUncoloredNeighbors(state, vertex);
    double shareRatioTerm = params.ka * getColorShareRatio(state, vertex);

    double danger = F + uncoloredTerm + shareRatioTerm;
    vertex_danger_cache[vertex] = danger;

    return danger;
}

double DangerHeuristic::calculateF(int diffColored, int maxColor) const {
    if (maxColor <= diffColored)
        return std::numeric_limits<double>::max();
    return params.C / std::pow(maxColor - diffColored, params.k);
}

int DangerHeuristic::selectNextVertex(const ColoringState &state) const {
    if (vertex_scores.empty())
        return -1;

    // Seleccionar entre los top 3 vértices con mayor danger
    size_t size = vertex_scores.size();
    size_t candidates = std::min(size_t(3), size);

    std::uniform_int_distribution<> dist(0, candidates - 1);
    auto it = vertex_scores.end();
    std::advance(it, -1 - dist(rng));

    return it->second;
}

double DangerHeuristic::calculateColorDanger(const ColoringState &state,
                                             int vertex, int color) const {
    auto key = std::make_pair(vertex, color);
    if (color_danger_cache.find(key) != color_danger_cache.end()) {
        return color_danger_cache[key];
    }

    // Encontrar vecino con máximo different_colored
    int maxDiffNeighbors = 0;
    int nc = vertex; // vértice con max diffNeighbors

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
    double diffNeighborsTerm =
        params.k1 * std::pow(maxColor - maxDiffNeighbors, params.k2);
    double uncoloredTerm = params.k3 * getUncoloredNeighbors(state, nc);
    double frequencyTerm = params.k4 * state.getVerticesWithColor(color).size();

    double danger = diffNeighborsTerm + uncoloredTerm - frequencyTerm;
    color_danger_cache[key] = danger;

    return danger;
}

int DangerHeuristic::selectColor(const ColoringState &state, int vertex) const {
    auto colors = state.getAvailableColors(vertex);
    if (colors.empty())
        return 0;

    // Seleccionar color con menor danger
    double minDanger = std::numeric_limits<double>::max();
    int selectedColor = colors[0];

    for (int color : colors) {
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
    color_set usedColors;
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
        if (state.getColor(neighbor) == -1)
            count++;
    }
    return count;
}

double DangerHeuristic::getColorShareRatio(const ColoringState &state,
                                           int vertex) const {
    vector<bool> colorAvailable(state.getNumColors() + 1, true);
    int availableCount = 0;
    int sharedCount = 0;

    // Marcar colores no disponibles por vecinos
    for (int neighbor : graph.getNeighbors(vertex)) {
        int color = state.getColor(neighbor);
        if (color != -1)
            colorAvailable[color] = false;
    }

    // Contar colores disponibles y compartidos
    for (size_t color = 0; color < colorAvailable.size(); color++) {
        if (colorAvailable[color]) {
            availableCount++;
            // Verificar si algún vecino no coloreado puede usar este color
            bool shared = false;
            for (int neighbor : graph.getNeighbors(vertex)) {
                if (state.getColor(neighbor) == -1 &&
                    state.isValidAssignment(neighbor, color)) {
                    shared = true;
                    break;
                }
            }
            if (shared)
                sharedCount++;
        }
    }

    return availableCount > 0
               ? static_cast<double>(sharedCount) / availableCount
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
    vertices.reserve(graph.getVertexCount());

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
