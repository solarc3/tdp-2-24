#include "../include/State.h"
#include "../include/TracyMacros.h"
#include <algorithm>
#include <climits>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
using std::min;

using namespace std;

State::State() {
    this->size = 0;
    this->jugs = nullptr;
    this->depth = 0;
    this->weight = 0;
    this->parent = nullptr;
    this->heuristic_calculated = false;
}

State::State(unsigned int size, unsigned int *jugs, unsigned int depth,
             unsigned int weight, State *parent) {
    this->size = size;
    this->depth = depth;
    this->weight = 0;
    this->parent = parent;
    this->heuristic_calculated = false;
    this->jugs = new unsigned int[size];
    memcpy(this->jugs, jugs, size * sizeof(unsigned int));
}

// 2. Modify State destructor to handle parent pointer:
State::~State() { delete[] jugs; }
// TODO: quiza reemplazar eso

bool State::equals(const State *other) const {
    TRACE_SCOPE;
    return memcmp(jugs, other->jugs, size * sizeof(unsigned int)) == 0;
}
void State::calculateHeuristic(const State &target_state) {
    TRACE_SCOPE;
    if (!heuristic_calculated) {
        unsigned int pattern_value = 0;
        unsigned int transfer_value = 0;
        unsigned int total_diff = 0;

        // Un solo loop para ambas heurísticas
        for (unsigned int i = 0; i < size; i++) {
            // Pattern database - mantener como está
            if (jugs[i] == target_state.jugs[i]) {
                pattern_value += 25;
            } else if (jugs[i] > target_state.jugs[i]) {
                pattern_value += 12;
                if (target_state.jugs[i] > 0) {
                    pattern_value += 5;
                }
            }

            // Calcular diferencia total y exceso/déficit
            int diff = target_state.jugs[i] - jugs[i];
            total_diff += static_cast<unsigned int>(abs(diff));

            // Estimar transferencias necesarias
            if (diff != 0) {
                // Peso basado en capacidad
                unsigned int capacity_weight =
                    target_state.jugs[i] > 1 ? target_state.jugs[i] : 1;

                // Estimar número de operaciones
                unsigned int operations;
                if (diff > 0) {
                    // Necesitamos llenar
                    operations = static_cast<unsigned int>(
                        ceil(static_cast<float>(diff) / capacity_weight));
                } else {
                    // Necesitamos vaciar
                    operations = static_cast<unsigned int>(
                        ceil(static_cast<float>(-diff) / capacity_weight));
                }

                transfer_value += operations * capacity_weight;
            }
        }

        // Normalización
        unsigned int pattern_max = size * 25;
        pattern_value =
            pattern_max > pattern_value ? pattern_max - pattern_value : 0;

        transfer_value = static_cast<unsigned int>(transfer_value * 0.65f);

        // Ajustar peso de pattern según profundidad
        float pattern_weight = std::max(0.4f, 0.8f - (depth * 0.002f));

        // Penalización por profundidad
        float base_penalty = 0.1f;
        float depth_ratio = static_cast<float>(depth) / (size * 10);
        float depth_penalty = base_penalty * (1.0f + depth_ratio);
        depth_penalty = std::min(0.25f, depth_penalty);

        weight = static_cast<unsigned int>(transfer_value * 1.5f +
                                           pattern_value * pattern_weight +
                                           depth * depth_penalty);

        HeuristicMetrics::recordChoice(transfer_value, pattern_value);
        heuristic_calculated = true;
    }
}
State **State::generateSuccessors(const unsigned int *capacities,
                                  unsigned int &num_successors) const {
    unsigned int max_successors = size * ((size - 1) + 2);
    State **successors = new State *[max_successors];
    num_successors = 0;

    // Reutilizar arreglo temporal para almacenar los valores
    unsigned int *new_jugs = new unsigned int[size];
    memcpy(new_jugs, jugs, size * sizeof(unsigned int));

    for (unsigned int i = 0; i < size; i++) {
        unsigned int original_i = new_jugs[i];

        // Transferir agua de jarra i a todas las demás jarras j
        for (unsigned int j = 0; j < size; j++) {
            if (i == j || new_jugs[i] == 0 || new_jugs[j] == capacities[j])
                continue;

            unsigned int transfer_amount =
                min(new_jugs[i], capacities[j] - new_jugs[j]);
            if (transfer_amount > 0) {
                new_jugs[i] -= transfer_amount;
                new_jugs[j] += transfer_amount;

                successors[num_successors++] = new State(
                    size, new_jugs, depth + 1, 0, const_cast<State *>(this));

                // Restaurar valores para la siguiente iteración
                new_jugs[i] = original_i;
                new_jugs[j] = jugs[j];
            }
        }

        // Operación de llenar
        if (new_jugs[i] < capacities[i]) {
            new_jugs[i] = capacities[i];
            successors[num_successors++] = new State(
                size, new_jugs, depth + 1, 0, const_cast<State *>(this));
            new_jugs[i] = original_i;
        }

        // Operación de vaciar
        if (new_jugs[i] > 0) {
            new_jugs[i] = 0;
            successors[num_successors++] = new State(
                size, new_jugs, depth + 1, 0, const_cast<State *>(this));
            new_jugs[i] = original_i;
        }
    }

    delete[] new_jugs; // Liberar arreglo temporal
    return successors;
}

void State::printState(const char *label) {

    cout << label << ": ";
    if (this->size == 0 || this->jugs == nullptr) {
        cout << "No inicializado\n";
        return;
    }

    for (unsigned int i = 0; i < this->size; i++) {
        cout << this->jugs[i] << " ";
    }
    cout << "\n";
}

bool State::readStatesFromFile(const string &fileName, State *max_state,
                               State *target_state) {
    TRACE_SCOPE;
    if (!max_state || !target_state) {
        cerr << "Error: Invalid state pointers" << endl;
        return false;
    }

    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << fileName << endl;
        return false;
    }

    string line;
    // Read maximum capacities
    if (!getline(file, line)) {
        cerr << "Error: File is empty" << endl;
        return false;
    }

    // Count number of jugs and validate
    int count = 1;
    for (char c : line) {
        if (c == ' ')
            count++;
    }

    if (count <= 0 || count > 64) {
        cerr << "Error: Invalid number of jugs (must be between 1 and 64)"
             << endl;
        return false;
    }

    // Initialize states with validated size
    max_state->size = count;
    target_state->size = count;
    max_state->jugs = new unsigned int[count];
    target_state->jugs = new unsigned int[count];

    // Parse maximum capacities
    istringstream ss(line);
    bool valid_capacities = true;
    for (int i = 0; i < count; i++) {
        if (!(ss >> max_state->jugs[i]) || max_state->jugs[i] == 0) {
            valid_capacities = false;
            break;
        }
    }

    if (!valid_capacities) {
        cerr << "Error: Invalid capacity values" << endl;
        delete[] max_state->jugs;
        delete[] target_state->jugs;
        return false;
    }

    // Initialize target state
    for (unsigned int i = 0; i < target_state->size; i++) {
        target_state->jugs[i] = 0;
    }

    // Read target state
    if (!getline(file, line)) {
        cerr << "Error: Missing target state" << endl;
        delete[] max_state->jugs;
        delete[] target_state->jugs;
        return false;
    }

    istringstream ts(line);
    bool valid_targets = true;
    for (unsigned int i = 0; i < target_state->size; i++) {
        unsigned int value;
        if (!(ts >> value)) {
            valid_targets = false;
            break;
        }
        if (value > max_state->jugs[i]) {
            cerr << "Error: Target value exceeds capacity for jug " << i
                 << endl;
            valid_targets = false;
            break;
        }
        target_state->jugs[i] = value;
    }

    if (!valid_targets) {
        delete[] max_state->jugs;
        delete[] target_state->jugs;
        return false;
    }

    return true;
}
