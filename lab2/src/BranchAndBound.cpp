#include "../include/BranchAndBound.h"

void BranchAndBound::solve(ColoringState &solution) {
    int currentK = bounds.getLowerBound();
    bool foundBetter = false;
    int previousK = bounds.getUpperBound();

    std::cout << "Iniciando Branch and Bound con k=" << currentK << std::endl;

    while (currentK <= bounds.getUpperBound()) {
        std::cout << "Intentando colorear con " << currentK << " colores..."
                  << std::endl;

        ColoringState currentState(graph, currentK);

        if (branchAndBoundRecursive(currentState, currentK, 0)) {
            std::cout << "¡Solución encontrada con " << currentK << " colores!"
                      << std::endl;
            solution = currentState;

            if (currentK < previousK) {
                foundBetter = true;
                previousK = currentK;
                bounds.updateUpperBound(currentK);
                // Solo intentamos con menos colores si no estamos en el límite
                // inferior
                if (currentK > bounds.getLowerBound()) {
                    currentK--;
                } else {
                    break; // Hemos alcanzado el límite inferior
                }
            } else {
                break; // No mejoramos la solución anterior
            }
        } else {
            std::cout << "No se encontró solución con " << currentK
                      << " colores" << std::endl;
            bounds.updateLowerBound(currentK + 1);

            // Si no encontramos solución y ya teníamos una mejor, terminamos
            if (foundBetter) {
                break;
            }

            // Si no podemos colorear con k colores, tampoco podremos con menos
            if (currentK == bounds.getLowerBound()) {
                break;
            }

            currentK++;
        }
    }
}

bool BranchAndBound::branchAndBoundRecursive(ColoringState &state,
                                             int targetColors, int vertex) {
    // Si hemos coloreado todos los vértices, verificar si la solución es válida
    if (vertex >= graph.getVertexCount()) {
        return isValidColoring(state);
    }

    // Verificar si el estado actual es prometedor
    if (!isPromising(state, targetColors)) {
        return false;
    }

    // Seleccionar el siguiente vértice usando DANGER
    int nextVertex =
        (vertex == 0) ? 0 : dangerHeuristic.selectBestVertex(state);
    if (nextVertex == -1)
        nextVertex = vertex;

    // Obtener y ordenar colores usando DANGER
    std::vector<int> orderedColors;
    auto availableColors = state.getAvailableColors(nextVertex);
    for (int color : availableColors) {
        if (color < targetColors) {
            orderedColors.push_back(color);
        }
    }

    std::sort(orderedColors.begin(), orderedColors.end(), [&](int c1, int c2) {
        return dangerHeuristic.calculateColorDanger(state, nextVertex, c1) <
               dangerHeuristic.calculateColorDanger(state, nextVertex, c2);
    });

    // Probar cada color disponible
    for (int color : orderedColors) {
        if (isColorValid(state, nextVertex, color)) {
            state.assignColor(nextVertex, color);
            if (branchAndBoundRecursive(state, targetColors, vertex + 1)) {
                return true;
            }
            state.unassignColor(nextVertex);
        }
    }

    return false;
}

bool BranchAndBound::isColorValid(const ColoringState &state, int vertex,
                                  int color) const {
    for (int neighbor : graph.getNeighbors(vertex)) {
        if (state.getColor(neighbor) == color) {
            return false;
        }
    }
    return true;
}

bool BranchAndBound::isValidColoring(const ColoringState &state) const {
    return state.isComplete() && state.isLegal();
}

bool BranchAndBound::isPromising(const ColoringState &state,
                                 int targetColors) const {
    if (state.getMaxUsedColor() >= targetColors) {
        return false;
    }

    for (int v = 0; v < graph.getVertexCount(); v++) {
        if (state.getColor(v) == -1) {
            auto availableColors = state.getAvailableColors(v);
            if (availableColors.empty()) {
                return false;
            }

            int validColors = 0;
            for (int c = 0; c < targetColors; c++) {
                if (std::find(availableColors.begin(), availableColors.end(),
                              c) != availableColors.end()) {
                    validColors++;
                }
            }

            if (validColors == 0) {
                return false;
            }
        }
    }
    return true;
}
