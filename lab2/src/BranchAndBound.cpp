#include "../include/BranchAndBound.h"

<<<<<<< HEAD
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
=======
BranchAndBound::BranchAndBound(const Graph &g, Bounds &b, DangerHeuristic &d)
    : graph(g), bounds(b), dangerHeuristic(d) {}

void BranchAndBound::solve(ColoringState &solution) {
    int currentK = bounds.getUpperBound() - 1;
>>>>>>> d9d247d (no funciona deje la zorra)

    cout << "DEBUG: Iniciando solve con upperBound = " << bounds.getUpperBound()
         << " y lowerBound = " << bounds.getLowerBound() << endl;

    while (currentK >= bounds.getLowerBound()) {
<<<<<<< HEAD
        ColoringState newState(graph, currentK);
        if (branchAndBoundRecursive(newState, currentK)) {
            solution = newState;
            currentK--;
        } else {
=======
        cout << "\nDEBUG: Intentando colorear con k = " << currentK << endl;

        ColoringState currentState(graph, currentK);
        const auto &clique = bounds.getMaxClique();

        cout << "DEBUG: Tamaño del clique inicial: " << clique.size() << endl;
        cout << "DEBUG: Vertices del clique: ";
        for (auto v : clique)
            cout << v << " ";
        cout << endl;

        // Colorear el clique inicial
        try {
            for (size_t i = 0; i < clique.size(); i++) {
                cout << "DEBUG: Asignando color " << i << " al vertice "
                     << clique[i] << endl;
                currentState.assignColor(clique[i], i);
            }

            cout << "DEBUG: Clique coloreado exitosamente" << endl;

            if (branchAndBoundRecursive(currentState, currentK)) {
                cout << "DEBUG: Encontrada solución con " << currentK
                     << " colores" << endl;
                bounds.updateUpperBound(currentK, currentState);
                solution = currentState;
                currentK--;
            } else {
                cout << "DEBUG: No se encontró solución con " << currentK
                     << " colores" << endl;
                bounds.updateLowerBound(currentK + 1);
                break;
            }
        } catch (const std::exception &e) {
            cout << "DEBUG: Error coloreando clique: " << e.what() << endl;
>>>>>>> d9d247d (no funciona deje la zorra)
            break;
        }
    }
}
bool BranchAndBound::branchAndBoundRecursive(ColoringState &state,
                                             int targetColors) {
<<<<<<< HEAD
    if (state.isComplete() && state.isLegal())
=======
    static int recursionDepth = 0;
    recursionDepth++;
    string indent(recursionDepth * 2, ' ');

    cout << indent << "DEBUG: Profundidad recursión = " << recursionDepth
         << endl;

    if (state.isComplete() && state.isLegal()) {
        cout << indent << "DEBUG: Encontrada solución legal completa!" << endl;
        recursionDepth--;
>>>>>>> d9d247d (no funciona deje la zorra)
        return true;

<<<<<<< HEAD
    // Early infeasibility detection
    int vertex = selectBestVertex(state);
    if (vertex == -1 || isInfeasible(state, vertex))
=======
    // Imprimir estado actual
    cout << indent << "DEBUG: Estado actual - Vertices sin colorear: "
         << state.uncoloredVertices.size() << endl;
    cout << indent << "DEBUG: Conflictos actuales: " << state.getNumConflicts()
         << endl;

    int vertex = dangerHeuristic.selectNextVertex(state);

    if (vertex == -1) {
        cout << indent << "DEBUG: No se encontró vértice para colorear" << endl;
        recursionDepth--;
>>>>>>> d9d247d (no funciona deje la zorra)
        return false;
    }

    cout << indent << "DEBUG: Seleccionado vertice " << vertex << endl;

<<<<<<< HEAD
    // Get colors ordered by DANGER score
    auto availableColors = state.getAvailableColors(vertex);
    vector<pair<int, double>> orderedColors;

    for (int color : availableColors) {
=======
    vector<pair<int, double>> colorDangers;
    for (int color : state.getAvailableColors(vertex)) {
>>>>>>> d9d247d (no funciona deje la zorra)
        if (color >= targetColors)
            continue;
        if (state.isValidAssignment(vertex, color) &&
            !shouldPrune(state, vertex, color)) {
            double danger =
                dangerHeuristic.calculateColorDanger(state, vertex, color);
            orderedColors.emplace_back(color, danger);
        }
    }

<<<<<<< HEAD
    sort(orderedColors.begin(), orderedColors.end(),
         [](const auto &a, const auto &b) { return a.second < b.second; });

    for (const auto &[color, danger] : orderedColors) {
        state.assignColor(vertex, color);
        if (branchAndBoundRecursive(state, targetColors))
=======
    cout << indent << "DEBUG: Colores disponibles: " << colorDangers.size()
         << endl;

    sort(colorDangers.begin(), colorDangers.end(),
         [](const auto &a, const auto &b) { return a.second < b.second; });

    for (const auto &[color, danger] : colorDangers) {
        cout << indent << "DEBUG: Probando color " << color
             << " (danger=" << danger << ")" << endl;

        state.assignColor(vertex, color);

        if (branchAndBoundRecursive(state, targetColors)) {
            recursionDepth--;
>>>>>>> d9d247d (no funciona deje la zorra)
            return true;
    }

    cout << indent << "DEBUG: No se encontró color válido para vértice "
         << vertex << endl;
    recursionDepth--;
    return false;
}

<<<<<<< HEAD
bool BranchAndBound::shouldPrune(const ColoringState &state, int vertex,
                                 int color) const {
    // More aggressive pruning based on conflicts
    if (state.getDeltaConflicts(vertex, color) > 0) {
        return true;
=======
int BranchAndBound::calculateDStar(const ColoringState &state) const {
    int alpha = state.getMaxUsedColor() + 1;
    int maxIndependent = 0;
    for (int i = 0; i < alpha; i++) {
        maxIndependent =
            max(maxIndependent, (int)state.getVerticesWithColor(i).size());
    }

    return (maxIndependent * (alpha + 1) * (alpha + 1)) / (4 * alpha);
}

bool BranchAndBound::backtrackToEligibleNode(ColoringState &state, int fromPos,
                                             int toPos) {
    for (int pos = fromPos; pos >= toPos; pos--) {
        if (isEligibleBacktrackNode(state, pos)) {
            // Deshacer colores hasta pos
            for (int i = state.graph.getVertexCount() - 1; i >= pos; i--) {
                if (state.getColor(i) != -1) {
                    state.assignColor(i, -1);
                }
            }
            return true;
        }
>>>>>>> d9d247d (no funciona deje la zorra)
    }
    return false;
}

<<<<<<< HEAD
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
=======
bool BranchAndBound::isEligibleBacktrackNode(const ColoringState &state,
                                             int vertex) const {
    // Verificar si el vértice no era cc-dependent y tiene otros colores
    // disponibles
    if (state.getColor(vertex) == -1)
        return false;

    vector<int> available = state.getAvailableColors(vertex);
    return available.size() > 1;
>>>>>>> d9d247d (no funciona deje la zorra)
}
