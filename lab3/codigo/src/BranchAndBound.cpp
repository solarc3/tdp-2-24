#include "../include/BranchAndBound.h"
BranchAndBound::BranchAndBound(const Graph &g, Bounds &b, DangerHeuristic &dh)
    : graph(g), dangerHeuristic(dh), bounds(b),
      bestValidSolution(g, g.getVertexCount()), bestK(g.getVertexCount()) {}

void BranchAndBound::solve(ColoringState &solution) {
    int currentK = bounds.getLowerBound();
    std::cout << "Comenzando Branch and Bound con k=" << currentK << std::endl;
    ColoringState currentState =
        dangerHeuristic.generateInitialColoring(currentK);

    while (currentK >= bounds.getLowerBound() && currentK > 0) {
        std::cout << "Intentando colorear con " << currentK << " colores..."
                  << std::endl;
        bool found = branchAndBoundRecursive(currentState, currentK);
        // mejor caso
        if (found && currentState.isLegal() && currentState.isComplete()) {
            std::cout << "¡Solucion legal encontrada con " << currentK
                      << " colores!" << std::endl;
            solution = currentState; // Guarda la solucion actual
            bounds.updateUpperBound(currentK);

            std::cout << "Actualizado Upper Bound a: " << bounds.getUpperBound()
                      << std::endl;

            if (currentK > bounds.getLowerBound()) {
                currentK--;
                currentState =
                    dangerHeuristic.generateInitialColoring(currentK);

                // legalidad
                if (!currentState.isLegal()) {
                    std::cout << "Advertencia: La coloracion inicial con k="
                              << currentK << " no es legal.\n";
                }
            } else {
                break;
            }
        } else {
            // sino, updatear bounds, partimos desde abajo agregando colores
            bounds.updateLowerBound(currentK + 1);
            std::cout << "Actualizado Lower Bound a: " << bounds.getLowerBound()
                      << std::endl;

            if (currentK + 1 <= bounds.getUpperBound()) {
                currentK++;
                currentState =
                    dangerHeuristic.generateInitialColoring(currentK);
                if (!currentState.isLegal()) {
                    std::cout << "Advertencia: La coloracion con k=" << currentK
                              << " no es legal.\n";
                }
            } else {
                break;
            }
        }
        //
        if (currentK <= 0 || currentK > graph.getVertexCount()) {
            std::cout << "Error: K fuera de rango valido." << std::endl;
            break;
        }
    }
    if (!solution.isLegal() || !solution.isComplete()) {
        std::cout << "No se encontro solucion legal completa." << std::endl;
    }
}

bool BranchAndBound::branchAndBoundRecursive(ColoringState &state,
                                             int targetColors) {
    // si todos los vertices tienen colores y es legal, tenemos solucion
    if (state.isComplete()) {
        if (state.isLegal()) {
            std::cout << "Solucion encontrada con " << targetColors
                      << " colores.\n";
            return true;
        }
        return false;
    }

    // seleccion por danger
    //
    int vertex = selectBestVertex(state);
    if (vertex == -1) {
        std::cout << "No quedan vertices por colorear.\n";
        return false;
    }

    // set de colores disponibles y se ordenan
    OrderedColorSet colorSet;
    auto availableColors = state.getAvailableColors(vertex, targetColors);

    std::cout << "Coloreando vertice " << vertex
              << " con colores disponibles: ";
    for (auto c : availableColors)
        std::cout << c << " ";
    std::cout << "\n";

    for (int color : availableColors) {
        if (color < targetColors) {
            if (isColorValid(state, vertex, color)) {
                double danger =
                    dangerHeuristic.calculateColorDanger(state, vertex, color);
                colorSet.insert({danger, color});
            }
        }
    }

    // si no hay, se hace backtrack
    if (colorSet.empty()) {
        std::cout << "No hay colores validos para el vertice " << vertex
                  << ". Retrocediendo.\n";
        return false;
    }

    // en orden de menor peligro
    for (const auto &entry : colorSet) {
        int color = entry.second;
        std::cout << "Intentando colorear vertice " << vertex << " con color "
                  << color << ".\n";
        state.assignColor(vertex, color, targetColors);

        if (branchAndBoundRecursive(state, targetColors)) {
            return true;
        }
        // sino, no se le asigna
        state.unassignColor(vertex);
        std::cout << "Desasignado color " << color << " del vertice " << vertex
                  << ".\n";
    }

    return false;
}

bool BranchAndBound::isColorValid(const ColoringState &state, int vertex,
                                  int color) const {
    return state.isValidAssignment(vertex, color);
}

int BranchAndBound::selectBestVertex(const ColoringState &state) const {
    double maxDanger = -std::numeric_limits<double>::max();
    int selectedVertex = -1;

    // iterar por todos y quedarse con el que tenga danger
    for (int v : state.getUncoloredVertices()) {
        double danger = dangerHeuristic.calculateVertexDanger(state, v);
        if (danger > maxDanger) {
            maxDanger = danger;
            selectedVertex = v;
        }
    }

    return selectedVertex;
}
