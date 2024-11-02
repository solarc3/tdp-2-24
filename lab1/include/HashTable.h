#pragma once
#include "../include/TracyMacros.h"
#include "State.h"
#include <cstring>
class HashTable {
    public:
    struct HashEntry {
        State *state;
        bool occupied;
        HashEntry() : state(nullptr), occupied(false) {}
    };

    // size 2^18 y factor de balance agresivo
    static constexpr unsigned int INITIAL_SIZE = 1u << 18;
    static constexpr unsigned int BALANCE_FACTOR = 35;

    // numeros primos para hashing
    static constexpr unsigned int PRIME1 = 2654435761u;
    static constexpr unsigned int PRIME2 = 2246822519u;
    static constexpr unsigned int PRIME3 = 3266489917u;

    HashEntry *table;
    unsigned int table_size;
    unsigned int num_elements;

    // stats para la tabla hash
    static unsigned int total_collisions;
    static unsigned int total_inserts;
    HashTable();
    ~HashTable();
    unsigned int computeHash(State *state) const;
    unsigned int computeStep(State *state) const;
    void resize();
    bool insert(State *state);
    bool contains(State *state) const;
    unsigned int size() const { return num_elements; }
    void cleanup();
    void removeState(State *state);
    unsigned int mixBits(unsigned int x) const;
};
