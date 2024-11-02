#include "../include/Solver.h"
#include "../include/TracyMacros.h"

int main() {
    TRACE_SCOPE;
    Solver solver;
    std::string fileName;
    int option;

    do {
        std::cout << "\n--------MENU--------\n";
        std::cout << "1. Read file\n";
        std::cout << "2. Solve\n";
        std::cout << "3. Exit\n";
        std::cout << "Option: ";

        // Robust input handling
        while (!(std::cin >> option)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number (1-3): ";
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
                return 0;
            }

            default: {
                TRACE_SCOPE;
                std::cout << "Invalid option. Please select 1, 2, or 3.\n";
                break;
            }
        }
    } while (option != 3);

    return 0;
}
