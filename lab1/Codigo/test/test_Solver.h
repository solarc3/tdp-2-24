#include "../include/Solver.h"
#include <cassert>

inline std::string createTestFile() {
    std::string filename = "test_input.txt";
    std::ofstream file(filename);
    file << "3 5\n";
    file << "0 4\n";
    file.close();
    return filename;
}

inline void testSolver() {
    Solver *solver = nullptr;
    std::string test_filename;

    try {
        solver = new Solver();
        test_filename = createTestFile();

        assert(solver->initializeFromFile(test_filename));
        assert(solver->isInitialized());

        solver->solve();

        delete solver;
        std::remove(test_filename.c_str());

    } catch (...) {
        delete solver;
        if (!test_filename.empty()) {
            std::remove(test_filename.c_str());
        }
        throw;
    }
}
