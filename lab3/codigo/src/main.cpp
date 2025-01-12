#include "../include/Solve.h"
#include "../test/test.h"
int main() {
    string fileName;
    int option;
    Solver solver;

    do {
        std::cout << "\n--------MENU--------\n";
        std::cout << "1. Leer archivo\n";
        std::cout << "2. Resolver\n";
        std::cout << "3. Ejecutar Test\n";
        std::cout << "4. Salir\n";
        std::cout << "Opcion: ";

        while (!(std::cin >> option)) {
            std::cin.clear();
            std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
            std::cout << "numero invalido, debe seleccionar uno entre (1-4): ";
        }

        switch (option) {
            case 1: {
                std::cout << "\nIngrese el nombre del archivo: ";
                std::cin >> fileName;
                if (solver.initialize(fileName)) {
                    std::cout << "Archivo leido correctamente.\n";
                }
                break;
            }

            case 2: {
                if (!solver.isInitialized()) {
                    std::cout
                        << "Error: Debe leer un archivo valido primero.\n";
                    break;
                }
                try {
                    solver.solve();
                    solver.printSolution();
                } catch (const std::exception &e) {
                    std::cerr << "Error al resolver: " << e.what() << std::endl;
                }
                break;
            }

            case 3: {
                runAllTests();
                break;
            }

            case 4: {
                std::cout << "Saliendo del programa...\n";
                return 0;
            }

            default: {
                std::cout << "opcion invalida. Por favor, seleccione 1-4.\n";
                break;
            }
        }
    } while (true);

    return 0;
}
