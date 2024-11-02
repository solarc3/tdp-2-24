#pragma once
#include "State.h"

class hashTable {
    public:
    struct HashEntry {
        State *state;
        bool occupied;
        HashEntry() : state(nullptr), occupied(false) {}
    };

    // Aumentar tamaño inicial y reducir factor de carga para menos colisiones
    static constexpr unsigned int INITIAL_SIZE = 1u << 18;
    static constexpr unsigned int BALANCE_FACTOR = 35;

    // Números primos grandes para mejor distribución
    static constexpr unsigned int PRIME1 = 2654435761u;
    static constexpr unsigned int PRIME2 = 2246822519u;
    static constexpr unsigned int PRIME3 = 3266489917u;

    HashEntry *table;
    unsigned int table_size;
    unsigned int num_elements;

    // Estadísticas
    static unsigned int total_collisions;
    static unsigned int total_inserts;
    /*
    private:
    unsigned int murmur3_32(const void *key, int len, unsigned int seed)
   const;
    */
    hashTable();
    ~hashTable();
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
