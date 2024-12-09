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
    // Start with the lower bound from the Bounds class
    int currentK = bounds.getLowerBound();

    std::cout << "Comenzando Branch and Bound con k=" << currentK << std::endl;

    // Initialize solution with the danger heuristic
    solution = dangerHeuristic.generateInitialColoring(currentK);

    while (currentK <= bounds.getUpperBound()) {
        std::cout << "Intentando colorear con " << currentK << " colores..."
                  << std::endl;

        // Create a working copy of the solution
        ColoringState currentState(graph, currentK);

        // Clear the pruned vertices set for this iteration
        pruned_vertices.clear();

        // Calculate appropriate backtracking depth for current k
        int maxBacktrackDepth = findBacktrackingDepth(currentK);

        // Try to find a solution with currentK colors
        if (branchAndBoundRecursive(currentState, currentK, maxBacktrackDepth,
                                    0)) {
            std::cout << "¡Solución encontrada con " << currentK << " colores!"
                      << std::endl;

            // Update the solution and upper bound
            solution = currentState;
            bounds.updateUpperBound(currentK);

            // Try with fewer colors
            currentK--;
        } else {
            std::cout << "No se encontró solución con " << currentK
                      << " colores." << std::endl;

            // Update lower bound and try with more colors
            bounds.updateLowerBound(currentK + 1);
            currentK++;
        }
    }
}
bool BranchAndBound::shouldPrune(const ColoringState &state, int vertex,
                                 int color) const {
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

    const double saturationThreshold = 0.8; // Ahora configurable
    return saturation > (maxColor * saturationThreshold);
}
int BranchAndBound::selectBestVertex(const ColoringState &state) const {
    int selectedVertex = -1;
    size_t minAvailableColors = std::numeric_limits<size_t>::max();
    double maxDanger = -1.0;

    for (int v = 0; v < graph.getVertexCount(); ++v) {
        if (state.getColor(v) == -1 &&
            pruned_vertices.find(v) == pruned_vertices.end()) {

            auto availableColors = state.getAvailableColors(v);

            if (availableColors.empty()) {
                pruned_vertices.insert(v);
                continue;
            }

            double danger = dangerHeuristic.calculateVertexDanger(state, v);

            if (availableColors.size() < minAvailableColors ||
                (availableColors.size() == minAvailableColors &&
                 danger > maxDanger)) {
                selectedVertex = v;
                minAvailableColors = availableColors.size();
                maxDanger = danger;
            }
        }
    }

    return selectedVertex;
}

bool BranchAndBound::branchAndBoundRecursive(ColoringState &state,
                                             int targetColors,
                                             int maxBacktrackDepth,
                                             int currentDepth) {
    if (state.isComplete()) {
        return state.isLegal();
    }

    if (currentDepth > maxBacktrackDepth) {
        return false;
    }

    int vertex = selectBestVertex(state);
    if (vertex == -1) {
        return false;
    }

    std::vector<std::pair<int, double>> orderedColors;
    auto availableColors = state.getAvailableColors(vertex);

    for (const auto &color : availableColors) {
        if (color >= targetColors) {
            continue;
        }

        if (state.isValidAssignment(vertex, color) &&
            !shouldPrune(state, vertex, color)) {
            double danger =
                dangerHeuristic.calculateColorDanger(state, vertex, color);
            orderedColors.emplace_back(color, danger);
        }
    }

    // Sort colors by danger (ascending) to prioritize less dangerous colors
    std::sort(orderedColors.begin(), orderedColors.end(),
              [](const auto &a, const auto &b) { return a.second < b.second; });

    constexpr size_t MAX_COLORS_TO_TRY = 5;
    if (orderedColors.size() > MAX_COLORS_TO_TRY) {
        orderedColors.resize(MAX_COLORS_TO_TRY);
    }

    for (const auto &[color, danger] : orderedColors) {
        state.assignColor(vertex, color);

        if (branchAndBoundRecursive(state, targetColors, maxBacktrackDepth,
                                    currentDepth + 1)) {
            return true;
        }

        // Backtracking
        state.unassignColor(vertex);
    }

    return false;
}
int BranchAndBound::findBacktrackingDepth(int currentK) const {
    // Base depth proportional to graph size
    int baseDepth = graph.getVertexCount() / 10;

    // Adjust depth based on how close we are to known bounds
    double boundRatio = static_cast<double>(currentK - bounds.getLowerBound()) /
                        (bounds.getUpperBound() - bounds.getLowerBound() + 1);

    // Increase depth when we're closer to known feasible solutions
    int depthAdjustment = static_cast<int>(baseDepth * boundRatio);

    // Add some randomness to avoid getting stuck in the same patterns
    std::uniform_int_distribution<> dis(-baseDepth / 4, baseDepth / 4);
    return std::max(baseDepth + depthAdjustment + dis(gen),
                    graph.getVertexCount() / 20);
}
