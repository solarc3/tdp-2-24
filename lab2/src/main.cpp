#include <iostream>
#include <limits>
using namespace std;
#include "../include/Solve.h"

int main() {
    string fileName;
    int option;
    Solver solver;

    do {
        std::cout << "\n--------MENU--------\n";
        std::cout << "1. Read file\n";
        std::cout << "2. Solve\n";
        std::cout << "3. Run Tests\n";
        std::cout << "4. Exit\n";
        std::cout << "Option: ";

        while (!(std::cin >> option)) {
            std::cin.clear();
            std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
            std::cout
                << "Numero invalido, se debe seleccionar alguno entre (1-4): ";
        }

        switch (option) {
            case 1: {
                std::cout << "\nEnter the filename: ";
                std::cin >> fileName;
                if (solver.initialize(fileName)) {
                    std::cout << "File read correctly\n";
                }
                break;
            }

            case 2: {
                if (!solver.isInitialized()) {
                    std::cout << "Error: Must read a valid file first\n";
                    break;
                }
                try {
                    solver.solve();
                    solver.printSolution();
                } catch (const std::exception &e) {
                    std::cerr << "Error solving: " << e.what() << std::endl;
                }
                break;
            }

            case 3: {
                try {
                    // Ejemplo basico de test
                    Solver testSolver;
                    if (testSolver.initialize("test_graph.txt")) {
                        testSolver.solve();
                        std::cout << "Test passed successfully\n";
                        testSolver.printSolution();
                    } else {
                        std::cout << "Test failed: Could not load test file\n";
                    }
                } catch (const std::exception &e) {
                    std::cerr << "Test execution failed: " << e.what()
                              << std::endl;
                    return 1;
                }
                break;
            }

            case 4: {
                std::cout << "Exiting program...\n";
                return 0;
            }

            default: {
                std::cout << "Invalid option. Please select 1-4.\n";
                break;
            }
        }
    } while (true);

    return 0;
}
