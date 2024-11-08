#include "../include/State.h"
using namespace std;
State::AdaptiveParams State::adaptive_params;
State::State() {
    this->size = 0;
    this->jugs = nullptr;
    this->depth = 0;
    this->weight = 0;
    this->parent = nullptr;
    this->heuristic_calculated = false;
}

State::AdaptiveParams::AdaptiveParams() {
    this->exploration_weight = 0.4f;
    this->balance_weight = 0.4f;
    this->optimization_weight = 0.2f;
    this->current_performance = 0.0f;
    this->consecutive_improvements = 0;
    this->plateaus = 0;
}

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
// Calculo de heuristicas ponderado por profunidad momentum, tamano y peso
// Se deciden ademas 2 heuristicas ponderadas por cada una de las 3 estrategias
// distintas, para cambiar como actuan dentro del algoritmo Se calcula el
// momentum global, y se pondera por segmento Se calcula el peso de cada
// estrategia, y se pondera por el tamano del problema Se ajustan los pesos en
// base a la performance del algoritmo Se asegura que almenos una estrategia
// tenga un peso Se normalizan los pesos y se pondera todo como un total
void State::calculateHeuristic(const State &target_state) {
    TRACE_SCOPE;
    if (!heuristic_calculated) {
        unsigned int pattern_value = 0;
        unsigned int transfer_value = 0;
        unsigned int max_jug_size = 0;
        unsigned int matching_jugs = 0;

        // segmentacion
        const unsigned int SEGMENT_SIZE = 8;
        unsigned int num_segments = (size + SEGMENT_SIZE - 1) / SEGMENT_SIZE;
        unsigned int *segment_matches = new unsigned int[num_segments]();
        unsigned int *segment_max = new unsigned int[num_segments]();

        // analisis por segment
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

        // Momentum global
        // en base a los estados
        float global_momentum =
            1.0f - (static_cast<float>(matching_jugs) / size);

        // calculo de pesos, transiciones lineales, mediano es constante,
        // siempre queremos balancear
        float depth_ratio = std::min(1.0f, static_cast<float>(depth) / 60.0f);
        float strategy_weights[3];

        // Exploracion:
        strategy_weights[0] = std::max(0.25f, 0.6f * (1.0f - depth_ratio));

        // Balance: constante, siempre queremos algo de balance
        strategy_weights[1] = 0.3f;

        // Optimizacion: crece linealmente
        strategy_weights[2] = std::min(0.6f, 0.2f + (depth_ratio * 0.4f));

        // Normalizar pesos
        float sum =
            strategy_weights[0] + strategy_weights[1] + strategy_weights[2];
        for (int i = 0; i < 3; i++) {
            strategy_weights[i] /= sum;
        }

        // Ponderaror en base a size del problema
        float size_factor = std::min(1.0f, static_cast<float>(size) / 30.0f);

        if (adaptive_params.consecutive_improvements > 3) {
            // Aumentar optimizaciom
            float adjustment = 0.1f * size_factor;
            strategy_weights[2] += adjustment;
            strategy_weights[1] -= adjustment * 0.5f;
            strategy_weights[0] -= adjustment * 0.5f;
        } else if (adaptive_params.plateaus > 2) {
            // Aumentar exploracion
            float adjustment = 0.1f * size_factor;
            strategy_weights[0] += adjustment;
            strategy_weights[1] -= adjustment * 0.5f;
            strategy_weights[2] -= adjustment * 0.5f;
        }

        // Asegurar almenos un peso
        strategy_weights[0] = std::max(0.2f, strategy_weights[0]);
        strategy_weights[1] = std::max(0.2f, strategy_weights[1]);
        strategy_weights[2] = std::max(0.2f, strategy_weights[2]);
        // ponderacion
        sum = strategy_weights[0] + strategy_weights[1] + strategy_weights[2];
        for (int i = 0; i < 3; i++) {
            strategy_weights[i] /= sum;
        }

        TRACE_PLOT("State/Weights/Exploration",
                   static_cast<int64_t>(strategy_weights[0] * 100));
        TRACE_PLOT("State/Weights/Balance",
                   static_cast<int64_t>(strategy_weights[1] * 100));
        TRACE_PLOT("State/Weights/Optimization",
                   static_cast<int64_t>(strategy_weights[2] * 100));

        // Calculo momentum por segmento
        for (unsigned int i = 0; i < size; i++) {
            unsigned int segment = i / SEGMENT_SIZE;
            float segment_momentum =
                1.0f - (static_cast<float>(segment_matches[segment]) /
                        std::min(SEGMENT_SIZE, size - segment * SEGMENT_SIZE));

            float combined_momentum =
                (global_momentum * 0.7f + segment_momentum * 0.3f);

            if (jugs[i] == target_state.jugs[i]) {
                // mayor prioridad a las que estan a la izquierda
                //  los ejemplos todos funcionan en ese orden, si se cambia esto
                //  no sirve
                float position_factor = 1.0f - (static_cast<float>(i) / size);

                float pattern_boost =
                    strategy_weights[0] * 30.0f + // Exploracion
                    strategy_weights[1] * 25.0f + // Balance
                    strategy_weights[2] * 20.0f;  // Optimizacion
                // bonificacion por estar en la posicion correcta
                pattern_value += static_cast<unsigned int>(
                    pattern_boost * (1.0f + position_factor * 0.5f));
            } else {
                // diferencia por cada posicion
                int diff = static_cast<int>(target_state.jugs[i]) -
                           static_cast<int>(jugs[i]);
                // max de cada segmeneto
                unsigned int local_max = segment_max[segment];
                unsigned int operations = static_cast<unsigned int>(
                    std::ceil(static_cast<float>(std::abs(diff)) / local_max));
                // factor de la heuristica, para cada una de las 3 estrategias
                // considerando el momentum, se prefiere la expacion para los
                // transfers
                float transfer_factor =
                    strategy_weights[0] *
                        (10.0f - (5.0f * combined_momentum)) + // Exploracion
                    strategy_weights[1] *
                        (20.0f - (10.0f * combined_momentum)) + // Balance
                    strategy_weights[2] *
                        (30.0f - (15.0f * combined_momentum)); // Optimizacion

                transfer_value +=
                    static_cast<unsigned int>(operations * transfer_factor);
            }
        }

        delete[] segment_matches;
        delete[] segment_max;

        // Normalizado por un maximo
        unsigned int pattern_max = static_cast<unsigned int>(size * 25);
        pattern_value =
            pattern_max > pattern_value ? pattern_max - pattern_value : 0;

        // Penalizar por profundidad
        float depth_penalty = std::min(
            0.1f + (depth / (size * 3.0f)) * (0.8f + global_momentum), 0.3f);
        // Peso final, ponderado cada heuristica considerando el momentum y la
        // profundidad transfers pq pierde precision al final y no se prefiere
        weight = static_cast<unsigned int>(
            transfer_value * (1.5f + global_momentum) +
            pattern_value * (1.0f - depth_penalty) +
            depth * depth_penalty * (10.0f + 20.0f * global_momentum));

        // Tracing
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
        TRACE_PLOT(
            "State/Adaptive/Performance",
            static_cast<int64_t>(adaptive_params.current_performance * 100));

        heuristic_calculated = true;
    }
}
// generacion de suceros sin ningun filtro, se generan todos los posibles y se
// agregan
State **State::generateSuccessors(const unsigned int *capacities,
                                  unsigned int &num_successors) const {
    TRACE_SCOPE;
    unsigned int max_successors = size * ((size - 1) + 2);
    State **successors = nullptr;
    unsigned int *new_jugs = nullptr;

    try {
        successors = new State *[max_successors]();
        new_jugs = new unsigned int[size];

        memcpy(new_jugs, jugs, size * sizeof(unsigned int));

        num_successors = 0;

        for (unsigned int i = 0; i < size; i++) {
            unsigned int original_i = new_jugs[i];

            // Transfer
            for (unsigned int j = 0; j < size; j++) {
                if (i == j || new_jugs[i] == 0 ||
                    new_jugs[j] == capacities[j]) {
                    continue;
                }

                unsigned int space_available = capacities[j] - new_jugs[j];
                unsigned int transfer_amount = (new_jugs[i] < space_available)
                                                   ? new_jugs[i]
                                                   : space_available;

                if (transfer_amount > 0) {
                    new_jugs[i] -= transfer_amount;
                    new_jugs[j] += transfer_amount;
                    successors[num_successors] =
                        new State(size, new_jugs, depth + 1, 0,
                                  const_cast<State *>(this));
                    num_successors++;

                    new_jugs[i] = original_i;
                    new_jugs[j] = jugs[j];
                }
            }

            // Fill
            if (new_jugs[i] < capacities[i]) {
                new_jugs[i] = capacities[i];
                successors[num_successors] = new State(
                    size, new_jugs, depth + 1, 0, const_cast<State *>(this));
                num_successors++;
                new_jugs[i] = original_i;
            }

            // Empty
            if (new_jugs[i] > 0) {
                new_jugs[i] = 0;

                successors[num_successors] = new State(
                    size, new_jugs, depth + 1, 0, const_cast<State *>(this));
                num_successors++;

                new_jugs[i] = original_i;
            }
        }
        delete[] new_jugs;
        new_jugs = nullptr;
        return successors;

    } catch (...) {
        // en caso de error, liberar todo para evitar leaks en la misma
        // ejecucion
        if (successors) {
            for (unsigned int i = 0; i < num_successors; i++) {
                delete successors[i];
            }
            delete[] successors;
            successors = nullptr;
        }
        if (new_jugs) {
            delete[] new_jugs;
            new_jugs = nullptr;
        }
        throw;
    }
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
// lectura de archivos 2 lineas, falta el verificador de que si no es posible
// solucionar un estado, no se deberia cargar
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
