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

    // Colorear vértices hasta que todos estén coloreados
    while (!state.isComplete()) {
        int vertex = selectNextVertex(state);
        if (vertex == -1)
            break; // No hay más vértices para colorear

        int color = selectColor(state, vertex);
        state.assignColor(vertex, color);
    }

    return state;
}

int DangerHeuristic::selectNextVertex(const ColoringState &state) const {
    vector<pair<int, double>> vertexDangers;

    // Calcular danger para vértices no coloreados
    for (int v = 0; v < graph.getVertexCount(); v++) {
        if (state.getColor(v) == -1) {
            double danger = calculateVertexDanger(state, v);
            vertexDangers.emplace_back(v, danger);
        }
    }

    if (vertexDangers.empty())
        return -1;

    // Ordenar por danger descendente
    sort(vertexDangers.begin(), vertexDangers.end(),
         [](const auto &a, const auto &b) { return a.second > b.second; });

    // Seleccionar aleatoriamente entre los top 3 vértices más peligrosos
    return selectRandomFromTop(vertexDangers, 3);
}

int DangerHeuristic::selectColor(const ColoringState &state, int vertex) const {
    vector<int> availableColors = state.getAvailableColors(vertex);
    if (availableColors.empty())
        return 0; // Retornar color 0 si no hay colores disponibles

    vector<pair<int, double>> colorDangers;
    for (int color : availableColors) {
        double danger = calculateColorDanger(state, vertex, color);
        colorDangers.emplace_back(color, danger);
    }

    // Seleccionar el color con menor danger
    auto minDanger = min_element(
        colorDangers.begin(), colorDangers.end(),
        [](const auto &a, const auto &b) { return a.second < b.second; });

    return minDanger->first;
}

int DangerHeuristic::selectRandomFromTop(
    const vector<pair<int, double>> &scores, int topK) const {
    int size = min(topK, (int)scores.size());
    std::uniform_int_distribution<> dist(0, size - 1);
    return scores[dist(rng)].first;
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
