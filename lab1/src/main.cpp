#include "../include/Solver.h"
#include "../include/TracyMacros.h"
#include "../test/test_HashTable.h"
#include "../test/test_Search.h"
#include "../test/test_Solver.h"
#include "../test/test_State.h"
#include <iostream>

int main() {
    TRACE_SCOPE;
    Solver solver;
    std::string fileName;
    int option;

    do {
        std::cout << "\n--------MENU--------\n";
        std::cout << "1. Read file\n";
        std::cout << "2. Solve\n";
        std::cout << "3. Run Tests\n";
        std::cout << "4. Exit\n";
        std::cout << "Option: ";

        while (!(std::cin >> option)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout
                << "Numero invalido, se debe seleccionar alguno entre (1-4): ";
        }

        switch (option) {
            case 1: {
                TRACE_SCOPE;
                std::cout << "\nEnter the filename: ";
                std::cin >> fileName;
                if (solver.initializeFromFile(fileName)) {
                    std::cout << "File read correctly\n";
                }
                break;
            }

            case 2: {
                TRACE_SCOPE;
                if (!solver.isInitialized()) {
                    std::cout << "Error: Must read a valid file first\n";
                    break;
                }
                TRACE_FRAME;
                solver.solve();
                TRACE_FRAME;
                break;
            }

            case 3: {
                TRACE_SCOPE;
                try {
                    std::cout << "Running tests....\n";
                    std::cout << "----------------------\n";

                    std::cout << "\033[1;31mProbando HashTable..\033[0m.\n";
                    testHashTable();
                    std::cout << "\033[32mHashTable tests passed!\033[0m.\n";

                    std::cout << "\033[1;31mTesting Search...\033[0m.\n";
                    testSearch();
                    std::cout << "\033[32mSearch tests passed!\033[0m.\n\n";

                    std::cout << "\033[1;31mTesting Solver...\033[0m.\n\n";
                    testSolver();
                    std::cout << "\033[32mSolver tests passed!\033[0m.\n\n";

                    std::cout << "\033[1;31mTesting State...\033[0m.\n\n";
                    testState();
                    std::cout << "\033[32mState tests passed!\033[0m.\n\n";
                    std::cout << "----------------------\n";
                    std::cout << "\033[32mResuelto todos los test con "
                                 "exito!\033[0m.\n\n";
                } catch (const std::exception &e) {
                    std::cerr << "Test execution failed: " << e.what()
                              << std::endl;
                    return 1;
                }
                break;
            }

            case 4: {
                TRACE_SCOPE;
                std::cout << "Exiting program...\n";
                return 0;
            }

            default: {
                TRACE_SCOPE;
                std::cout << "Invalid option. Please select 1-4.\n";
                break;
            }
        }
    } while (true);

    return 0;
}
