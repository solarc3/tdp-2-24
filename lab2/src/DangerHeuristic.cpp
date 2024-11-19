// DangerHeuristic.cpp
#include "../include/DangerHeuristic.h"

DangerHeuristic::DangerHeuristic(const Graph &g)
    : graph(g), colorHistory(g.getVertexCount()),
      stableCount(g.getVertexCount()), dynamicDegree(g.getVertexCount()),
      rng(std::random_device{}()) {}

double DangerHeuristic::calculateVertexDanger(const ColoringState &state,
                                              int vertex) const {
    double diffColored = getDifferentColoredNeighbors(state, vertex);
    double maxColor = state.getMaxUsedColor() + 1;
    // double F = params.C / pow(maxColor - diffColored, params.k);
    double F = calculateCriticalityFactor(diffColored, maxColor);
    // usar DSATUR como complemento para danger
    double satDegree = calculateSaturationDegree(state, vertex);
    double satFactor = params.kd * (satDegree / maxColor);

    double neighborImpact = calculateNeighborImpact(state, vertex);
    double stability = calculateStability(vertex);
    double conflictPenalty = calculateConflictPenalty(state, vertex);

    return F + params.ku * getUncoloredNeighbors(state, vertex) +
           params.ka * getColorShareRatio(state, vertex) + satFactor +
           params.kp * neighborImpact + params.ks * stability + conflictPenalty;
}

double DangerHeuristic::calculateSaturationDegree(const ColoringState &state,
                                                  int vertex) const {
    ColorSet usedColors;
    double totalWeight = 0.0;

    for (int neighbor : graph.getNeighbors(vertex)) {
        int color = state.getColor(neighbor);
        if (color != -1) {
            if (usedColors.find(color) == usedColors.end()) {
                usedColors.insert(color);
                totalWeight +=
                    1.0 / (dynamicDegree.effectiveDegree[neighbor] + 1);
            }
        }
    }

    return totalWeight * usedColors.size();
}

double DangerHeuristic::calculateNeighborImpact(const ColoringState &state,
                                                int vertex) const {
    double impact = 0.0;

    for (int neighbor : graph.getNeighbors(vertex)) {
        if (state.getColor(neighbor) == -1) {
            vector<int> availableColors = state.getAvailableColors(neighbor);
            impact +=
                1.0 - (double)availableColors.size() / state.getNumColors();
        }
    }

    return impact / (graph.getDegree(vertex) + 1);
}

int DangerHeuristic::selectNextVertex(const ColoringState &state) const {
    int selectedVertex = -1;
    double maxDanger = -1;
    for (int v = 0; v < graph.getVertexCount(); v++) {
        if (state.getColor(v) == -1) { // Si no está coloreado
            double danger = calculateVertexDanger(state, v);
            if (danger > maxDanger) {
                maxDanger = danger;
                selectedVertex = v;
            }
        }
    }

    return selectedVertex;
}

int DangerHeuristic::selectColor(const ColoringState &state, int vertex) const {
    vpii colorDangers;
    vector<int> availableColors = state.getAvailableColors(vertex);

    for (int color : availableColors) {
        double colorDanger = calculateColorDanger(state, vertex, color);
        colorDangers.emplace_back(color, colorDanger);
    }

    // Ordenar por danger ascendente
    sort(colorDangers.begin(), colorDangers.end(),
         [](const auto &a, const auto &b) { return a.second < b.second; });

    // Seleccion probabilistica entre los mejores colores
    double totalWeight = 0.0;
    vector<double> weights;

    for (size_t i = 0; i < std::min(size_t(3), colorDangers.size()); i++) {
        double weight = 1.0 / (colorDangers[i].second + 1);
        weights.push_back(weight);
        totalWeight += weight;
    }

    // Seleccion ruleta
    double r = std::uniform_real_distribution<>(0, totalWeight)(rng);
    double accum = 0.0;

    for (size_t i = 0; i < weights.size(); i++) {
        accum += weights[i];
        if (r <= accum) {
            // Actualizar historial
            colorHistory[vertex].insert(colorDangers[i].first);
            return colorDangers[i].first;
        }
    }

    return colorDangers[0].first;
}

void DangerHeuristic::updateParameters(
    const ColoringState &state) { // TODO: NO SE UTILIZA MODIFICAR PARA QUE SI
    double progressRatio =
        (double)state.getNumColors() / graph.getVertexCount();

    if (progressRatio < 0.2) {
        params.k *= 1.1;
        params.ku *= 0.9;
    } else if (stagnationDetected()) {
        params.k *= 0.9;
        params.ku *= 1.1;
    }

    // Actualizar contadores de estabilidad
    for (int v = 0; v < graph.getVertexCount(); v++) {
        if (state.getColor(v) != -1) {
            stableCount[v]++;
        } else {
            stableCount[v] = 0;
        }
        dynamicDegree.update(v, state);
    }
}

double DangerHeuristic::calculateStability(int vertex) const {
    return 1.0 / (stableCount[vertex] + 1);
}

double DangerHeuristic::calculateConflictPenalty(const ColoringState &state,
                                                 int vertex) const {
    int conflicts = 0;
    int color = state.getColor(vertex);
    if (color != -1) {
        for (int neighbor : graph.getNeighbors(vertex)) {
            if (state.getColor(neighbor) == color) {
                conflicts++;
            }
        }
    }
    return conflicts * 0.5; // Factor de penalizacion
}

bool DangerHeuristic::stagnationDetected()
    const { // TODO: NO SE UTILIZA PERO QUIZA CAMBIAR PARA QUE SI SE USE
    // Implementacion simple de deteccion de estancamiento
    int stagnatedVertices = 0;
    for (int count : stableCount) {
        if (count > 10)
            stagnatedVertices++;
    }
    return stagnatedVertices > graph.getVertexCount() / 2;
}
ColoringState DangerHeuristic::generateInitialColoring(int maxColors) {
    ColoringState state(graph, maxColors);
    ColorDependency dependency(graph, maxColors);

    dependency.recursiveShrink(state);

    while (!state.isComplete()) {
        int vertex = selectNextVertex(state);
        if (vertex == -1)
            break;

        int color = selectColor(state, vertex);
        state.assignColor(vertex, color);
    }

    dependency.expand(state);

    return state;
}
double DangerHeuristic::calculateColorDanger(const ColoringState &state,
                                             int vertex, int color) const {
    // Encontrar el maximo numero de vecinos con diferentes colores
    int maxDiffNeighbors = 0;
    int nc = 0; // vertice con maximo diffNeighbors

    for (int v : graph.getNeighbors(vertex)) {
        if (state.getColor(v) == -1 && state.isValidAssignment(v, color)) {
            int diffNeighbors = getDifferentColoredNeighbors(state, v);
            if (diffNeighbors > maxDiffNeighbors) {
                maxDiffNeighbors = diffNeighbors;
                nc = v;
            }
        }
    }

    // Calcular el danger del color segun la formula del paper
    double maxColor = state.getMaxUsedColor() + 1;
    double diffNeighborsTerm =
        params.k1 * pow(maxColor - maxDiffNeighbors, params.k2);
    double uncoloredTerm = params.k3 * getUncoloredNeighbors(state, nc);
    double frequencyTerm = params.k4 * state.getVerticesWithColor(color).size();

    return diffNeighborsTerm + uncoloredTerm - frequencyTerm;
}
<<<<<<< HEAD

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

=======
>>>>>>> d9d247d (no funciona deje la zorra)
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
            // Verificar si algun vecino no coloreado tambien puede usar este
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
<<<<<<< HEAD

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

    // Sort by degree descending
    sort(vertices.begin(), vertices.end(),
         [](const auto &a, const auto &b) { return a.second > b.second; });

    vector<int> ordered;
    ordered.reserve(vertices.size());
    for (const auto &[v, d] : vertices) {
        ordered.push_back(v);
    }

    return ordered;
=======
int DangerHeuristic::selectRandomFromTop(const vpii &scores, int topK) const {
    int size = min(topK, (int)scores.size());
    std::uniform_int_distribution<> dist(0, size - 1);
    return scores[dist(rng)].first;
>>>>>>> d9d247d (no funciona deje la zorra)
}
