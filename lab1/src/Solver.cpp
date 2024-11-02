#include "../include/Solver.h"

Solver::Solver() {
    initialized = false;
    max_state = new State();
    target_state = new State();
}

Solver::~Solver() { cleanup(); }

void Solver::cleanup() {
    delete max_state;
    delete target_state;
    max_state = nullptr;
    target_state = nullptr;
}

bool Solver::initializeFromFile(const std::string &filename) {
    TRACE_SCOPE;
    cleanup();
    max_state = new State();
    target_state = new State();

    if (!State::readStatesFromFile(filename, max_state, target_state)) {
        std::cout << "Error en la lectura del archivo\n";
        initialized = false;
        return false;
    }

    std::cout << "Archivo leido con exito\n";
    std::cout << "Estados leidos:\n";
    max_state->printState("Maximum capacities");
    target_state->printState("Target state     ");

    initialized = true;
    return true;
}

void Solver::solve() {
    TRACE_SCOPE;

    if (!initialized) {
        std::cout << "Error: Se debe iniciar con un archivo primero\n";
        return;
    }

    std::cout << "\nStates actuales antes de resolver:\n";
    printCurrentStates();

    // Create initial state
    State *start_state = new State();
    start_state->size = max_state->size;
    start_state->jugs = new unsigned int[max_state->size];
    for (unsigned int i = 0; i < max_state->size; i++) {
        start_state->jugs[i] = 0;
    }

    // Create search and solve
    Search search(start_state, target_state, max_state->jugs);

    auto start_time = std::chrono::high_resolution_clock::now();
    Search::Path solution = search.findPath();
    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                        end_time - start_time)
                        .count();

    // Handle solution results
    if (solution.length == 0) {
        std::cout << "No se encontro solucion\n";
    } else {
        cout << "\nSecuencia de estados:\n";
        for (unsigned int i = 0; i < solution.length; i++) {
            if (!solution.states[i])
                continue; // Skip null states
            for (unsigned int j = 0; j < solution.states[i]->size; j++) {
                cout << solution.states[i]->jugs[j] << " ";
            }
            cout << "\n";
        }
        TRACE_PLOT("Solution time (ms)", duration / 1000.0);
        std::cout << "Solution found in " << solution.length - 1 << " steps\n";
        std::cout << "Execution time: " << duration / 1000.0
                  << " milliseconds\n";
    }

    Search::freePath(solution);
}

void Solver::printCurrentStates() const {
    if (!initialized) {
        std::cout << "Estados no inicializados\n";
        return;
    }

    max_state->printState("Maximum capacities");
    target_state->printState("Target state     ");
}

bool Solver::isInitialized() const { return initialized; }
