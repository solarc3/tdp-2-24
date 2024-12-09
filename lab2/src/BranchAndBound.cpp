#include "../include/BranchAndBound.h"

BranchAndBound::color_set
BranchAndBound::getAvailableColorsSet(const ColoringState &state,
                                      int vertex) const {
    color_set availableColors;
    auto colorVector = state.getAvailableColors(vertex);

    for (int color : colorVector) {
        availableColors.insert(color);
    }

    return availableColors;
}
void BranchAndBound::solve(ColoringState &solution) {
    int currentK = bounds.getLowerBound();
    int bestK = bounds.getUpperBound();

    std::cout << "Comenzando Branch and Bound con k=" << currentK << std::endl;

    ColoringState initialState =
        dangerHeuristic.generateInitialColoring(currentK);
    solution = initialState;

    while (currentK <= bounds.getUpperBound() && currentK > 0) {
        std::cout << "Intentando colorear con " << currentK << " colores..."
                  << std::endl;

        ColoringState currentState = initialState;
        pruned_vertices.clear();

        if (branchAndBoundRecursive(currentState, currentK, 0, 0)) {
            std::cout << "¡Solución encontrada con " << currentK << " colores!"
                      << std::endl;
            solution = currentState;
            bestK = currentK;
            bounds.updateUpperBound(currentK);

            if (currentK > 1) {
                currentK--;
                initialState = currentState;
            } else {
                break;
            }
        } else {
            bounds.updateLowerBound(currentK + 1);
            if (currentK + 1 <= bounds.getUpperBound()) {
                currentK++;
                initialState =
                    dangerHeuristic.generateInitialColoring(currentK);
            } else {
                break;
            }
        }

        if (currentK <= 0 || currentK > graph.getVertexCount()) {
            std::cout << "Error: K fuera de rango válido. Usando mejor "
                         "solución encontrada."
                      << std::endl;
            currentK = bestK;
            break;
        }
    }

    if (solution.getNumColors() <= 0 ||
        solution.getNumColors() > graph.getVertexCount()) {
        std::cout << "Restaurando a la mejor solución encontrada con " << bestK
                  << " colores." << std::endl;
        solution = dangerHeuristic.generateInitialColoring(bestK);
    }
}
bool BranchAndBound::shouldPrune(const ColoringState &state, int vertex,
                                 int color) const {
    // Solo podar cuando hay conflictos reales
    if (state.getDeltaConflicts(vertex, color) > 0) {
        return true;
    }

    size_t uncoloredNeighbors = 0;
    color_set availableColors; // Asegúrate de usar el alias correcto
    int maxColor = state.getMaxUsedColor();

    for (int neighbor : graph.getNeighbors(vertex)) {
        if (state.getColor(neighbor) == -1) {
            uncoloredNeighbors++;
            auto neighborColors = state.getAvailableColors(neighbor);
            for (int c : neighborColors) {
                availableColors.insert(c); // Corrección aquí
            }
        }
    }

    if (uncoloredNeighbors > availableColors.size()) {
        return true;
    }

    int saturation = 0;
    color_set neighborColors; // Asegúrate de usar el alias correcto
    for (int neighbor : graph.getNeighbors(vertex)) {
        int neighborColor = state.getColor(neighbor);
        if (neighborColor != -1) {
            neighborColors.insert(neighborColor); // Corrección aquí
        }
    }
    saturation = neighborColors.size();
    const double saturationThreshold = 0.95; // Aumentar el umbral
    return saturation > (maxColor * saturationThreshold);
}
int BranchAndBound::selectBestVertex(const ColoringState &state) const {
    int selectedVertex = -1;
    double maxDanger = -std::numeric_limits<double>::max();

    // Primero creamos un conjunto de vértices candidatos
    std::vector<int> candidates;
    for (int v = 0; v < graph.getVertexCount(); ++v) {
        if (state.getColor(v) == -1 &&
            pruned_vertices.find(v) == pruned_vertices.end()) {

            auto availableColors = state.getAvailableColors(v);
            if (availableColors.empty()) {
                pruned_vertices.insert(v);
                continue;
            }
            candidates.push_back(v);
        }
    }

    if (candidates.empty()) {
        return -1;
    }

    // Calcular estadísticas de danger para normalización
    double sumDanger = 0;
    double maxDangerValue = -std::numeric_limits<double>::max();
    double minDangerValue = std::numeric_limits<double>::max();
    std::vector<double> dangers(candidates.size());

    for (size_t i = 0; i < candidates.size(); ++i) {
        int v = candidates[i];
        double danger = dangerHeuristic.calculateVertexDanger(state, v);
        dangers[i] = danger;
        sumDanger += danger;
        maxDangerValue = std::max(maxDangerValue, danger);
        minDangerValue = std::min(minDangerValue, danger);
    }

    double avgDanger = sumDanger / candidates.size();
    double dangerRange = maxDangerValue - minDangerValue;

    // Evaluar cada vértice considerando múltiples factores
    for (size_t i = 0; i < candidates.size(); ++i) {
        int v = candidates[i];

        // Normalizar el danger score
        double normalizedDanger =
            dangerRange > 0 ? (dangers[i] - minDangerValue) / dangerRange : 0;

        // Factores adicionales normalizados
        double saturationFactor =
            static_cast<double>(
                dangerHeuristic.getDifferentColoredNeighbors(state, v)) /
            state.getMaxUsedColor();

        double uncoloredFactor =
            static_cast<double>(
                dangerHeuristic.getUncoloredNeighbors(state, v)) /
            graph.getDegree(v);

        double shareRatio = dangerHeuristic.getColorShareRatio(state, v);

        // Combinar factores con pesos
        double totalScore =
            normalizedDanger * 0.4 + // Peso del danger
            saturationFactor * 0.3 + // Peso de la saturación
            uncoloredFactor * 0.2 +  // Peso de vecinos sin colorear
            shareRatio * 0.1;        // Peso del ratio de compartición

        // Añadir un factor de variación para evitar empates
        if (dangers[i] > avgDanger) {
            totalScore *=
                1.1; // Bonus para vértices más peligrosos que el promedio
        }

        if (totalScore > maxDanger) {
            maxDanger = totalScore;
            selectedVertex = v;
        }
    }

    return selectedVertex;
}
bool BranchAndBound::branchAndBoundRecursive(ColoringState &state,
                                             int targetColors,
                                             int maxBacktrackDepth,
                                             int currentDepth) {
    static int stagnationCount = 0;
    const int MAX_STAGNATION = graph.getVertexCount() * 2;

    if (state.isComplete()) {
        stagnationCount = 0;
        return state.isLegal();
    }

    if (stagnationCount > MAX_STAGNATION) {
        perturb(state, targetColors);
        stagnationCount = 0;
        dangerHeuristic.clearCaches();
    }

    int vertex = selectBestVertex(state);

    if (vertex == -1) {
        stagnationCount++;
        return false;
    }

    auto availableColors = state.getAvailableColors(vertex);
    if (availableColors.empty()) {
        stagnationCount++;
        return false;
    }

    typedef tree<double, int, std::less<double>, rb_tree_tag,
                 tree_order_statistics_node_update>
        OrderedColorSet;
    OrderedColorSet colorSet;

    for (int color : availableColors) {
        if (color >= targetColors)
            continue;
        if (state.isValidAssignment(vertex, color) &&
            !shouldPrune(state, vertex, color)) {
            double danger =
                dangerHeuristic.calculateColorDanger(state, vertex, color);
            colorSet.insert({danger, color});
        }
    }

    for (const auto &entry : colorSet) {
        int color = entry.second;
        state.assignColor(vertex, color);

        if (branchAndBoundRecursive(state, targetColors, maxBacktrackDepth,
                                    currentDepth + 1)) {
            return true;
        }

        state.unassignColor(vertex);
        stagnationCount++;
    }

    return false;
}

void BranchAndBound::perturb(ColoringState &state, int targetColors) {
    std::uniform_int_distribution<> vertexDist(0, graph.getVertexCount() - 1);
    std::uniform_real_distribution<> probDist(0.0, 1.0);

    const double UNCOLOR_PROB = 0.3; // Probabilidad de descolorear un vértice
    const int NUM_PERTURBATIONS =
        graph.getVertexCount() / 4; // Número de perturbaciones

    for (int i = 0; i < NUM_PERTURBATIONS; i++) {
        int vertex = vertexDist(gen);

        if (probDist(gen) < UNCOLOR_PROB || state.getColor(vertex) == -1) {
            // Descolorear vértice
            state.unassignColor(vertex);
        } else {
            // Intentar cambiar el color
            auto availableColors = state.getAvailableColors(vertex);
            if (!availableColors.empty()) {
                std::uniform_int_distribution<> colorDist(
                    0, availableColors.size() - 1);
                int newColorIdx = colorDist(gen);
                int newColor = availableColors[newColorIdx];

                if (newColor < targetColors) {
                    state.assignColor(vertex, newColor);
                }
            }
        }
    }
}
