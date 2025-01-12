#include "../include/Solve.h"

Solver::Solver() : initialized(false), bestSolution(Graph(), 0) {}

bool Solver::initialize(const string &filename) {
    std::cout << "Iniciando carga del grafo..." << std::endl;

    if (!graph.createFromFile(filename)) {
        return false;
    }

    std::cout << "Grafo cargado. Inicializando componentes..." << std::endl;

    std::cout << "Creando DangerHeuristic..." << std::endl;
    dangerHeuristic = std::make_unique<DangerHeuristic>(graph);

    std::cout << "Creando Bounds..." << std::endl;
    bounds = std::make_unique<Bounds>(graph);
    std::cout << "Bounds iniciales - Lower: " << bounds->getLowerBound()
              << ", Upper: " << bounds->getUpperBound() << std::endl;

    std::cout << "Creando Branch and Bound..." << std::endl;
    bnb = std::make_unique<BranchAndBound>(graph, *bounds, *dangerHeuristic);

    std::cout << "Inicializando estado inicial..." << std::endl;
    bestSolution = ColoringState(graph, bounds->getUpperBound());

    initialized = true;
    std::cout << "Inicialización completada" << std::endl;
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

    // nodo -> color
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
