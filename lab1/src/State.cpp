#include "../include/State.h"

using namespace std;
// Constructor vacio
State::State() {
    this->size = 0;
    this->jugs = nullptr;
    this->depth = 0;
    this->weight = 0;
    this->parent = nullptr;
    this->heuristic_calculated = false;
}
// Constructor con size, arreglo de jarras, profunidad, peso y padre
// forma estandar de iniciar un estado
State::State(unsigned int size, unsigned int *jugs, unsigned int depth,
             unsigned int weight, State *parent) {
    this->size = size;
    this->depth = depth;
    this->weight = weight;
    this->parent = parent;
    this->heuristic_calculated = false;
    this->jugs = new unsigned int[size];
    memcpy(this->jugs, jugs, size * sizeof(unsigned int));
}

State::~State() { delete[] jugs; }

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
        for (unsigned int i = 0; i < size; i++) {
            // Pattern database matches, exceso o igualdad se les da
            // bonificacion
            if (jugs[i] == target_state.jugs[i]) {
                pattern_value += 25;
            } else if (jugs[i] > target_state.jugs[i]) {
                pattern_value += 12;
                if (target_state.jugs[i] > 0) {
                    pattern_value += 5;
                }
            }

            // diferencia total y deficit
            int diff = target_state.jugs[i] - jugs[i];
            total_diff += static_cast<unsigned int>(abs(diff));

            // Heuristica de transferencia necesarias
            if (diff != 0) {
                // Peso basado en capacidad
                unsigned int capacity_weight =
                    target_state.jugs[i] > 1 ? target_state.jugs[i] : 1;

                // Cantidad de operaciones
                unsigned int operations;
                if (diff > 0) {
                    // llenar
                    operations = static_cast<unsigned int>(
                        ceil(static_cast<float>(diff) / capacity_weight));
                } else {
                    // vaciar
                    operations = static_cast<unsigned int>(
                        ceil(static_cast<float>(-diff) / capacity_weight));
                }

                transfer_value += operations * capacity_weight;
            }
        }

        // normalizar
        unsigned int pattern_max = size * 25;
        pattern_value =
            pattern_max > pattern_value ? pattern_max - pattern_value : 0;
        // peso ponderado luego de normalizar, 0.65 da el ajuste perfecto
        transfer_value = static_cast<unsigned int>(transfer_value * 0.65f);

        // Peso segun profundidad, 0.4 a 0.8
        // max(0.4, 0.8 - (profundidad * 0.002)
        float pattern_weight =
            (0.8f - (depth * 0.002f) > 0.4f) ? 0.8f - (depth * 0.002f) : 0.4f;

        // Penalizar por profundidad, evitar irse por caminos largos
        float base_penalty = 0.1f;
        float depth_ratio = static_cast<float>(depth) / (size * 10);
        float depth_penalty = base_penalty * (1.0f + depth_ratio);
        depth_penalty = depth_penalty > 0.25f ? 0.25f : depth_penalty;
        // ponderado estatico para la transferencia
        //  se agrega ademas una penalizacion por profunidad general
        weight = static_cast<unsigned int>(transfer_value * 3.0f +
                                           pattern_value * pattern_weight +
                                           depth * depth_penalty);
        // agregamos ambos pesos para ver estadisticas despues
        HeuristicMetrics::recordChoice(transfer_value, pattern_value);
        heuristic_calculated = true; // caching
    }
}
State **State::generateSuccessors(const unsigned int *capacities,
                                  unsigned int &num_successors) const {
    unsigned int max_successors = size * ((size - 1) + 2);
    State **successors = new State *[max_successors];
    num_successors = 0;
    unsigned int *new_jugs = new unsigned int[size];
    memcpy(new_jugs, jugs, size * sizeof(unsigned int));

    for (unsigned int i = 0; i < size; i++) {
        unsigned int original_i = new_jugs[i];

        // jarra i a distintas jarras j
        for (unsigned int j = 0; j < size; j++) {
            if (i == j || new_jugs[i] == 0 || new_jugs[j] == capacities[j])
                continue;

            unsigned int transfer_amount =
                (new_jugs[i] < capacities[j] - new_jugs[j])
                    ? new_jugs[i]
                    : capacities[j] - new_jugs[j];
            if (transfer_amount > 0) {
                new_jugs[i] -= transfer_amount;
                new_jugs[j] += transfer_amount;

                successors[num_successors++] = new State(
                    size, new_jugs, depth + 1, 0, const_cast<State *>(this));

                // siguiente iteracion
                new_jugs[i] = original_i;
                new_jugs[j] = jugs[j];
            }
        }

        // llenar jarras
        if (new_jugs[i] < capacities[i]) {
            new_jugs[i] = capacities[i];
            successors[num_successors++] = new State(
                size, new_jugs, depth + 1, 0, const_cast<State *>(this));
            new_jugs[i] = original_i;
        }

        // vaciar jarras
        if (new_jugs[i] > 0) {
            new_jugs[i] = 0;
            successors[num_successors++] = new State(
                size, new_jugs, depth + 1, 0, const_cast<State *>(this));
            new_jugs[i] = original_i;
        }
    }

    delete[] new_jugs;
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
        cerr << "Error: puntero para el State invalido." << endl;
        return false;
    }

    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error, no se pudo abrir el archivo" << fileName << endl;
        return false;
    }

    string line;
    if (!getline(file, line)) {
        cerr << "Error: el archivo esta vacio" << endl;
        return false;
    }
    // contar jarras
    int count = 1;
    for (char c : line) {
        if (c == ' ')
            count++;
    }

    // iniciar states
    max_state->size = count;
    target_state->size = count;
    max_state->jugs = new unsigned int[count];
    target_state->jugs = new unsigned int[count];

    // obtener maximos
    istringstream ss(line);
    bool valid_capacities = true;
    for (int i = 0; i < count; i++) {
        if (!(ss >> max_state->jugs[i]) || max_state->jugs[i] == 0) {
            valid_capacities = false;
            break;
        }
    }

    if (!valid_capacities) {
        cerr << "Error: capacidades maximas invalidas" << endl;
        delete[] max_state->jugs;
        delete[] target_state->jugs;
        return false;
    }

    // target state
    for (unsigned int i = 0; i < target_state->size; i++) {
        target_state->jugs[i] = 0;
    }

    // agregar info a target state
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
            cerr << "Error: target value se sobrepasa en: " << i << endl;
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
