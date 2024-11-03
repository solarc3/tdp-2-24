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

        // Segmentación (mantenido igual que el original)
        const unsigned int SEGMENT_SIZE = 8;
        unsigned int num_segments = (size + SEGMENT_SIZE - 1) / SEGMENT_SIZE;
        unsigned int *segment_matches = new unsigned int[num_segments]();
        unsigned int *segment_max = new unsigned int[num_segments]();

        // Primera pasada: análisis por segmento (igual que original)
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

        // Momentum global (mantenido igual)
        float global_momentum =
            1.0f - (static_cast<float>(matching_jugs) / size);

        // Calcular pesos para las tres estrategias basados en la profundidad
        float depth_ratio = std::min(1.0f, static_cast<float>(depth) /
                                               60.0f); // Duplicar escala
        // Pesos base para cada estrategia
        float strategy_weights[3];
        // Exploración: mantenerla más tiempo
        strategy_weights[0] = std::max(
            0.4f, 1.0f - (depth_ratio * 0.5f)); // Decaimiento más lento

        // Balance: hacerla más amplia y centrada más tarde
        strategy_weights[1] =
            1.0f - std::pow((depth_ratio - 0.6f), 2); // Mover pico a 60%

        // Optimización: retrasarla y hacerla más gradual
        strategy_weights[2] = std::max(
            0.0f,                         // Empezar desde 0
            std::min(0.4f,                // Máximo más bajo
                     depth_ratio < 0.4f ? // Retrasar inicio
                         0.0f
                                        : // No optimización al inicio
                         (depth_ratio - 0.4f) * 0.6f)); // Subida más lenta
        // Normalizar pesos
        float sum =
            strategy_weights[0] + strategy_weights[1] + strategy_weights[2];
        for (int i = 0; i < 3; i++) {
            strategy_weights[i] /= sum;
        }

        TRACE_PLOT("State/Weights/Exploration",
                   static_cast<int64_t>(strategy_weights[0] * 100));
        TRACE_PLOT("State/Weights/Balance",
                   static_cast<int64_t>(strategy_weights[1] * 100));
        TRACE_PLOT("State/Weights/Optimization",
                   static_cast<int64_t>(strategy_weights[2] * 100));

        // Segunda pasada: cálculo de valores con las tres estrategias
        for (unsigned int i = 0; i < size; i++) {
            unsigned int segment = i / SEGMENT_SIZE;
            float segment_momentum =
                1.0f - (static_cast<float>(segment_matches[segment]) /
                        std::min(SEGMENT_SIZE, size - segment * SEGMENT_SIZE));

            float combined_momentum =
                (global_momentum * 0.7f + segment_momentum * 0.3f);

            if (jugs[i] == target_state.jugs[i]) {
                float position_factor = 1.0f - (static_cast<float>(i) / size);

                // Pattern value combinado de las tres estrategias
                float pattern_boost =
                    strategy_weights[0] *
                        35.0f + // Aumentar valor base exploración (era 30)
                    strategy_weights[1] * 25.0f + // Mantener balance igual
                    strategy_weights[2] *
                        15.0f; // Reducir optimización (era 20)
                pattern_value += static_cast<unsigned int>(
                    pattern_boost * (1.0f + position_factor * 0.5f));
            } else {
                int diff = static_cast<int>(target_state.jugs[i]) -
                           static_cast<int>(jugs[i]);

                unsigned int local_max = segment_max[segment];
                unsigned int operations = static_cast<unsigned int>(
                    ceil(static_cast<float>(abs(diff)) / local_max));
                float transfer_factor =
                    strategy_weights[0] *
                        (12.0f - (6.0f * combined_momentum)) + // Más permisivo
                    strategy_weights[1] *
                        (20.0f - (12.0f * combined_momentum)) + // Igual
                    strategy_weights[2] *
                        (28.0f - (18.0f * combined_momentum)); // Más estricto

                transfer_value +=
                    static_cast<unsigned int>(operations * transfer_factor);
            }
        }

        delete[] segment_matches;
        delete[] segment_max;

        // Normalización con bonus (mantenido del original)
        unsigned int pattern_max = static_cast<unsigned int>(size * 25 * 1.5f);
        pattern_value =
            pattern_max > pattern_value ? pattern_max - pattern_value : 0;

        // Penalización por profundidad adaptativa
        float size_factor = std::min(1.0f, static_cast<float>(size) / 30.0f);
        float base_penalty = 0.1f + (0.1f * size_factor);

        // Penalización combinada de las tres estrategias
        float depth_penalty = std::min(
            base_penalty +
                (depth / (size * (2.5f + global_momentum *
                                             3.5f))) * // Aumentar denominadores
                    (strategy_weights[0] *
                         0.6f + // Reducir penalización exploración (era 0.8)
                     strategy_weights[1] * 1.0f + // Mantener balance
                     strategy_weights[2] *
                         1.4f), // Aumentar penalización optimización (era 1.2)
            0.5f);

        // Peso final con las tres estrategias integradas
        weight = static_cast<unsigned int>(
            transfer_value * (1.5f + global_momentum * 1.5f) +
            pattern_value * (1.0f - depth_penalty * 0.7f) +
            depth * depth_penalty *
                (5.0f +
                 25.0f * global_momentum *
                     (strategy_weights[0] *
                          0.8f + // Exploración: menor influencia de profundidad
                      strategy_weights[1] * 1.0f + // Balance: influencia normal
                      strategy_weights[2] *
                          1.2f))); // Optimización: mayor influencia

        // Tracing (mantenido del original)
        TRACE_PLOT("State/Heuristic/PatternValue",
                   static_cast<int64_t>(pattern_value));
        TRACE_PLOT("State/Heuristic/TransferValue",
                   static_cast<int64_t>(transfer_value));
        TRACE_PLOT("State/Heuristic/DepthPenalty",
                   static_cast<int64_t>(depth_penalty * 100));
        TRACE_PLOT("State/Heuristic/GlobalMomentum",
                   static_cast<int64_t>(global_momentum * 100));
        TRACE_PLOT("State/Heuristic/Weight", static_cast<int64_t>(weight));
        TRACE_PLOT("State/Stats/MatchingJugs",
                   static_cast<int64_t>(matching_jugs));

        heuristic_calculated = true;
    }
}
State **State::generateSuccessors(const unsigned int *capacities,
                                  unsigned int &num_successors) const {
    TRACE_SCOPE;
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
    TRACE_PLOT("State/Successors/Count", static_cast<int64_t>(num_successors));
    TRACE_PLOT("State/Successors/MaxPossible",
               static_cast<int64_t>(max_successors));
    TRACE_PLOT("State/Operations/Transfers", static_cast<int64_t>(1));
    TRACE_PLOT("State/Operations/Fills", static_cast<int64_t>(1));
    TRACE_PLOT("State/Operations/Empties", static_cast<int64_t>(1));
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
