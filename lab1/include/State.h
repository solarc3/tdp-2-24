#pragma once
#include "HeuristicMetrics.h"
#include <bitset>
using namespace std;

class State {
    public:
    // TODO: quiza cambiar esto, como son funciones constantes
    // Lazy evaluation para algo constante
    State *parent;
    mutable bool heuristic_calculated;
    unsigned int size;
    unsigned int *jugs;
    unsigned int depth;
    unsigned int weight;
    unsigned int state_hash; // Almacena el hash precalculado
    static constexpr unsigned int C1 = 0xcc9e2d51;
    static constexpr unsigned int C2 = 0x1b873593;
    State();
    State(unsigned int size, unsigned int *buckets, unsigned int depth,
          unsigned int weight, State *parent);
    ~State();
    bool equals(const State *other) const;
    void calculateHeuristic(const State &target_state);
    State **generateSuccessors(const unsigned int *capacities,
                               unsigned int &num_successors) const;
    void printState(const char *label);
    static bool readStatesFromFile(const string &fileName, State *max_state,
                                   State *target_state);
};
