#include "../include/BranchAndBound.h"

BranchAndBound::BranchAndBound(const Graph &g, Bounds &b, DangerHeuristic &d,
                               double alpha, int maxIter,
                               std::chrono::milliseconds tLimit)
    : graph(g), bounds(b), dangerHeuristic(d), alpha(0.1),
      maxIterations(maxIter), timeLimit(tLimit) {}
void BranchAndBound::solve(ColoringState &solution) {
    auto startTime = std::chrono::steady_clock::now();

    // Generar coloreo inicial
    solution = dangerHeuristic.generateInitialColoring(bounds.getUpperBound());

    // Si no hay solución inicial válida, intentar con coloreo secuencial
    if (solution.getNumColors() == 0) {
        ColoringState initialState(graph, graph.getVertexCount());
        for (int v = 0; v < graph.getVertexCount(); v++) {
            for (int c = 0; c < graph.getVertexCount(); c++) {
                if (initialState.isValidAssignment(v, c)) {
                    initialState.assignColor(v, c);
                    break;
                }
            }
        }
        solution = initialState;
    }

    int currentK = solution.getNumColors() - 1;

    while (currentK >= bounds.getLowerBound()) {
        ColoringState newState(graph, currentK);
        if (branchAndBoundRecursive(newState, currentK)) {
            solution = newState;
            currentK--;
        } else {
            break;
        }
    }
}

bool BranchAndBound::branchAndBoundRecursive(ColoringState &state,
                                             int targetColors) {
    if (state.isComplete() && state.isLegal())
        return true;

    // Early infeasibility detection
    int vertex = selectBestVertex(state);
    if (vertex == -1 || isInfeasible(state, vertex))
        return false;

    // Get colors ordered by DANGER score
    auto availableColors = state.getAvailableColors(vertex);
    vector<pair<int, double>> orderedColors;

    for (int color : availableColors) {
        if (color >= targetColors)
            continue;
        if (state.isValidAssignment(vertex, color) &&
            !shouldPrune(state, vertex, color)) {
            double danger =
                dangerHeuristic.calculateColorDanger(state, vertex, color);
            orderedColors.emplace_back(color, danger);
        }
    }

    sort(orderedColors.begin(), orderedColors.end(),
         [](const auto &a, const auto &b) { return a.second < b.second; });

    for (const auto &[color, danger] : orderedColors) {
        state.assignColor(vertex, color);
        if (branchAndBoundRecursive(state, targetColors))
            return true;
    }

    return false;
}

bool BranchAndBound::shouldPrune(const ColoringState &state, int vertex,
                                 int color) const {
    // More aggressive pruning based on conflicts
    if (state.getDeltaConflicts(vertex, color) > 0) {
        return true;
    }
    return false;
}

bool BranchAndBound::isInfeasible(const ColoringState &state,
                                  int vertex) const {
    // Check if vertex has no valid colors available
    for (int c = 0; c < state.getNumColors(); c++) {
        if (state.isValidAssignment(vertex, c))
            return false;
    }
    return true;
}

int BranchAndBound::selectBestVertex(const ColoringState &state) const {
    int selectedVertex = -1;
    double maxDanger = -1;
    int minAvailableColors = std::numeric_limits<int>::max();

    for (int v = 0; v < graph.getVertexCount(); v++) {
        if (state.getColor(v) == -1 &&
            pruned_vertices.find(v) == pruned_vertices.end()) {
            auto availableColors = state.getAvailableColors(v);
            if (availableColors.empty()) {
                pruned_vertices.insert(v);
                return -1;
            }

            if (availableColors.size() < minAvailableColors) {
                double danger = dangerHeuristic.calculateVertexDanger(state, v);
                selectedVertex = v;
                maxDanger = danger;
                minAvailableColors = availableColors.size();
            }
        }
    }

    return selectedVertex;
}
