#pragma once
#include "State.h"

class HashTable {
    public:
    // Configuración
    static constexpr unsigned int INITIAL_SIZE = 1u << 16;
    static constexpr float MAX_LOAD_FACTOR = 0.7f;

    struct Bucket {
        State *state;
        unsigned int psl; // Probe sequence length
        bool occupied;

        Bucket();
    };

    HashTable();
    ~HashTable();

    bool insert(State *state);
    bool contains(const State *state) const;
    void cleanup();
    void removeState(State *state);
    unsigned int size() const { return size_; }
    unsigned int capacity() const { return capacity_; }

    private:
    Bucket *buckets_;
    unsigned int size_;
    unsigned int capacity_;

    // Constantes de hash
    static constexpr unsigned int PRIME1 = 0x7FEB352D;
    static constexpr unsigned int PRIME2 = 0x846CA68B;
    static constexpr unsigned int PRIME3 = 0x4B1BD1B5;

    unsigned int computeHash(const State *state) const;
    bool shouldResize() const;
    void resize();
};
