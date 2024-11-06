#include "../include/Search.h"
#include <cassert>

inline void testSearch() {
    // Crear los arreglos de jugadas
    unsigned int *initial_jugs = new unsigned int[3]{0, 0, 0};
    unsigned int *target_jugs = new unsigned int[3]{0, 0, 6};
    unsigned int *max_capacities = new unsigned int[3]{3, 5, 7};

    // Crear estados inicial y final
    State *initial_state = new State(3, initial_jugs, 0, 0, nullptr);
    State *target_state = new State(3, target_jugs, 0, 0, nullptr);

    // Crear y ejecutar la búsqueda
    Search *search = new Search(initial_state, target_state, max_capacities);
    Search::Path path = search->findPath();

    // Verificar que existe solución
    assert(path.length > 0);
    assert(path.states != nullptr);
    assert(path.states[path.length - 1]->equals(target_state));

    // Verificar la validez del camino
    for (unsigned int i = 0; i < path.length - 1; i++) {
        bool valid_move = false;
        unsigned int num_successors;
        State **successors =
            path.states[i]->generateSuccessors(max_capacities, num_successors);

        // Verificar que cada paso es válido
        for (unsigned int j = 0; j < num_successors; j++) {
            if (successors[j]->equals(path.states[i + 1])) {
                valid_move = true;
                delete successors[j];
                break;
            }
            delete successors[j];
        }
        delete[] successors;
        assert(valid_move && "Invalid move in solution path");
    }

    // Cleanup en orden correcto
    // 1. Primero liberar el path (pero no los estados dentro de él)
    delete[] path.states;

    // 2. Eliminar el objeto search (que limpiará open_list y closed_list)
    delete search;

    // 3. Ahora sí podemos eliminar los estados inicial y final
    delete initial_state;
    delete target_state;

    // 4. Finalmente, eliminar los arreglos de configuración
    delete[] initial_jugs;
    delete[] target_jugs;
    delete[] max_capacities;
}
