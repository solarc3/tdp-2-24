#include "../include/BranchAndBound.h"

void BranchAndBound::solve(ColoringState &solution) {
    // Obtener coloreo inicial usando la heurística DANGER
    solution = dangerHeuristic.generateInitialColoring(bounds.getUpperBound());

    // Si no hay solución inicial válida, crear una usando coloreo secuencial
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

    ColoringState bestSolution = solution;
    int currentK = solution.getNumColors() - 1;

    while (currentK >= bounds.getLowerBound()) {
        ColoringState newState(graph, currentK);

        for (int v = 0; v < graph.getVertexCount(); v++) {
            int color = bestSolution.getColor(v);
            if (color < currentK) {
                newState.assignColor(v, color);
            }
        }

        pruned_vertices.clear();

        if (branchAndBoundRecursive(newState, currentK)) {
            bounds.updateUpperBound(currentK);
            bestSolution = newState;
            currentK--;
        } else {
            bounds.updateLowerBound(currentK + 1);
            break;
        }
    }

    solution = bestSolution;
}

bool BranchAndBound::shouldPrune(const ColoringState &state, int vertex,
                                 int color) const {
    if (state.getDeltaConflicts(vertex, color) > 0) {
        return true;
    }

    size_t uncoloredNeighbors = 0;
    set<int> availableColors;

    for (int neighbor : graph.getNeighbors(vertex)) {
        if (state.getColor(neighbor) == -1) {
            uncoloredNeighbors++;
            for (int c = 0; c < color; c++) {
                if (state.isValidAssignment(neighbor, c)) {
                    availableColors.insert(c);
                }
            }
        }
    }

    return uncoloredNeighbors > availableColors.size();
}

bool BranchAndBound::branchAndBoundRecursive(ColoringState &state,
                                             int targetColors) {
    // Si el estado está completo y es legal, encontramos una solución válida
    if (state.isComplete()) {
        return state.isLegal();
    }

    // Seleccionar el siguiente vértice a colorear
    int vertex = selectBestVertex(state);
    if (vertex == -1) {
        return false; // No hay vértices disponibles para colorear
    }

    // Obtener y ordenar colores disponibles por su valor de DANGER
    vector<pair<int, double>> orderedColors;
    auto availableColors = state.getAvailableColors(vertex);

    for (int color : availableColors) {
        // Solo considerar colores dentro del límite objetivo
        if (color >= targetColors) {
            continue;
        }

        // Verificar si la asignación es válida y no debe ser podada
        if (state.isValidAssignment(vertex, color) &&
            !shouldPrune(state, vertex, color)) {
            double danger =
                dangerHeuristic.calculateColorDanger(state, vertex, color);
            orderedColors.emplace_back(color, danger);
        }
    }

    // Ordenar colores por valor de DANGER (menor a mayor)
    sort(orderedColors.begin(), orderedColors.end(),
         [](const auto &a, const auto &b) { return a.second < b.second; });

    // Probar cada color en orden
    for (const auto &[color, danger] : orderedColors) {
        // Hacer backup del estado actual
        int oldColor = state.getColor(vertex);

        // Asignar color y recursión
        state.assignColor(vertex, color);

        if (branchAndBoundRecursive(state, targetColors)) {
            return true; // Se encontró una solución válida
        }

        // Si esta rama no lleva a solución, deshacer el cambio
        if (oldColor == -1) {
            state.unassignColor(vertex);
        } else {
            state.assignColor(vertex, oldColor);
        }
    }

    return false;
}

bool BranchAndBound::isInfeasible(const ColoringState &state, int vertex,
                                  int targetColors) const {
    for (int c = 0; c < targetColors; c++) {
        if (state.isValidAssignment(vertex, c))
            return false;
    }
    return true;
}

int BranchAndBound::selectBestVertex(const ColoringState &state) const {
    int selectedVertex = -1;
    size_t minAvailableColors = std::numeric_limits<size_t>::max();

    for (int v = 0; v < graph.getVertexCount(); v++) {
        if (state.getColor(v) == -1 &&
            pruned_vertices.find(v) == pruned_vertices.end()) {
            auto availableColors = state.getAvailableColors(v);
            if (availableColors.empty()) {
                pruned_vertices.insert(v);
                return -1;
            }

            if (availableColors.size() < minAvailableColors) {
                dangerHeuristic.calculateVertexDanger(
                    state, v); // Mantener la llamada por consistencia
                selectedVertex = v;
                minAvailableColors = availableColors.size();
            }
        }
    }

    return selectedVertex;
}
