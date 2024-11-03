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
        unsigned int max_jug_size = 0;
        unsigned int matching_jugs = 0;

        // Segmentar el problema
        const unsigned int SEGMENT_SIZE = 8; // Analizar en grupos de 8 jarras
        unsigned int num_segments = (size + SEGMENT_SIZE - 1) / SEGMENT_SIZE;
        unsigned int *segment_matches = new unsigned int[num_segments]();
        unsigned int *segment_max = new unsigned int[num_segments]();

        // Primera pasada: análisis por segmentos
        for (unsigned int i = 0; i < size; i++) {
            unsigned int segment = i / SEGMENT_SIZE;

            if (target_state.jugs[i] > segment_max[segment]) {
                segment_max[segment] = target_state.jugs[i];
            }
            if (target_state.jugs[i] > max_jug_size) {
                max_jug_size = target_state.jugs[i];
            }
            if (jugs[i] == target_state.jugs[i]) {
                matching_jugs++;
                segment_matches[segment]++;
            }
        }

        // Calcular momentum por segmento y global
        float global_momentum =
            1.0f - (static_cast<float>(matching_jugs) / size);

        // Segunda pasada: cálculo de valores
        for (unsigned int i = 0; i < size; i++) {
            unsigned int segment = i / SEGMENT_SIZE;
            float segment_momentum =
                1.0f - (static_cast<float>(segment_matches[segment]) /
                        std::min(SEGMENT_SIZE, size - segment * SEGMENT_SIZE));

            // Combinar momentum global y local
            float combined_momentum =
                (global_momentum * 0.7f + segment_momentum * 0.3f);

            if (jugs[i] == target_state.jugs[i]) {
                // Bonus por match basado en posición
                float position_factor = 1.0f - (static_cast<float>(i) / size);
                pattern_value += static_cast<unsigned int>(
                    25.0f * (1.0f + position_factor * 0.5f));
            } else {
                int diff = static_cast<int>(target_state.jugs[i]) -
                           static_cast<int>(jugs[i]);

                // Usar el máximo del segmento para calcular operaciones
                unsigned int local_max = segment_max[segment];
                unsigned int operations = static_cast<unsigned int>(
                    ceil(static_cast<float>(abs(diff)) / local_max));

                // Transfer value reducido por momentum combinado
                float transfer_factor = 20.0f - (12.0f * combined_momentum);
                transfer_value +=
                    static_cast<unsigned int>(operations * transfer_factor);
            }
        }

        delete[] segment_matches;
        delete[] segment_max;

        // Normalización adaptativa
        unsigned int pattern_max =
            static_cast<unsigned int>(size * 25 * 1.5f); // Ajustado por bonus
        pattern_value =
            pattern_max > pattern_value ? pattern_max - pattern_value : 0;

        // Penalización por profundidad más agresiva en casos grandes
        float size_factor = std::min(1.0f, static_cast<float>(size) / 30.0f);
        float base_penalty = 0.1f + (0.1f * size_factor);
        float depth_penalty = std::min(
            base_penalty + (depth / (size * (2.0f + global_momentum * 3.0f))),
            0.5f);

        weight = static_cast<unsigned int>(
            transfer_value *
                (1.5f + global_momentum * 1.5f) + // Más variación con momentum
            pattern_value *
                (1.0f - depth_penalty * 0.7f) + // Pattern más estable
            depth * depth_penalty *
                (5.0f + 25.0f * global_momentum)); // Profundidad más flexible

        HeuristicMetrics::recordChoice(transfer_value, pattern_value);
        heuristic_calculated = true;
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
