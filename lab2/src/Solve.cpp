#include "../include/Solve.h"

Solver::Solver() : initialized(false), bestSolution(Graph(), 0) {}

bool Solver::initialize(const string &filename) {
    if (!graph.createFromFile(filename)) {
        return false;
    }
    dangerHeuristic = std::make_unique<DangerHeuristic>(graph);
    bounds = std::make_unique<Bounds>(graph);
    bnb = std::make_unique<BranchAndBound>(graph, *bounds, *dangerHeuristic);
    bestSolution = ColoringState(graph, graph.getVertexCount());
    initialized = true;
    return true;
}

void Solver::solve() {
    if (!initialized) {
        throw std::runtime_error("Solver not initialized");
    }

    auto start = std::chrono::steady_clock::now();

    try {
        bnb->solve(bestSolution);
    } catch (const std::exception &e) {
        std::cerr << "Error during solving: " << e.what() << std::endl;
        throw;
    }

    auto end = std::chrono::steady_clock::now();
    executionTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

void Solver::printSolution() const {
    if (!initialized) {
        std::cout << "No solution available" << std::endl;
        return;
    }

    // Imprimir asignación de colores
    for (int v = 0; v < graph.getVertexCount(); v++) {
        std::cout << v << " " << bestSolution.getColor(v) << std::endl;
    }

    std::cout << "Total colores: " << bestSolution.getNumColors() << std::endl;
    std::cout << "Tiempo: " << executionTime.count() << " [ms]" << std::endl;

    if (!bestSolution.isLegal()) {
        std::cout << "WARNING: La solución tiene "
                  << bestSolution.getNumConflicts() << " conflictos!"
                  << std::endl;
    }
}
