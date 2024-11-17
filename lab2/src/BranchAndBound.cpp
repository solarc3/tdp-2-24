#include "../include/BranchAndBound.h"

BranchAndBound::BranchAndBound(const Graph &g, Bounds &b, DangerHeuristic &d,
                               int maxIter, std::chrono::milliseconds tLimit)
    : graph(g), bounds(b), dangerHeuristic(d), maxIterations(maxIter),
      timeLimit(tLimit) {}

void BranchAndBound::solve(ColoringState &solution) {
    auto startTime = std::chrono::steady_clock::now();
    int currentK = bounds.getUpperBound() - 1;

    while (currentK >= bounds.getLowerBound()) {
        ColoringState currentState(graph, currentK);

        // Colorear el clique inicial
        const auto &clique = bounds.getMaxClique();
        for (size_t i = 0; i < clique.size(); i++) {
            currentState.assignColor(clique[i], i);
        }

        if (branchAndBoundRecursive(currentState, currentK)) {
            bounds.updateUpperBound(currentK, currentState);
            solution = currentState;
            currentK--;

        } else {
            bounds.updateLowerBound(currentK + 1);
            break;
        }
        auto currentTime = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
                currentTime - startTime) > timeLimit) {
            break;
        }
    }
}

bool BranchAndBound::branchAndBoundRecursive(ColoringState &state,
                                             int targetColors) {
    // Si el coloreo es completo y legal, hemos encontrado una solución
    if (state.isComplete() && state.isLegal()) {
        return true;
    }

    // Reemplazar selectVertexDSatur por selectNextVertex de DANGER
    int vertex = dangerHeuristic.selectNextVertex(state);
    if (vertex == -1)
        return false;

    // Obtener orden de colores usando DANGER en lugar de getColorOrder
    vector<int> availableColors = state.getAvailableColors(vertex);
    vector<pair<int, double>> colorDangers;

    for (int color : availableColors) {
        if (color >= targetColors)
            continue;
        if (state.isValidAssignment(vertex, color)) {
            double danger =
                dangerHeuristic.calculateColorDanger(state, vertex, color);
            colorDangers.emplace_back(color, danger);
        }
    }

    // Ordenar colores por danger ascendente (menor danger primero)
    sort(colorDangers.begin(), colorDangers.end(),
         [](const auto &a, const auto &b) { return a.second < b.second; });

    // Probar colores en orden de menor a mayor danger
    for (const auto &[color, danger] : colorDangers) {
        state.assignColor(vertex, color);

        if (branchAndBoundRecursive(state, targetColors)) {
            return true;
        }
    }

    return false;
}
