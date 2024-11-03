#pragma once
#include "State.h"

class HashTable {
    public:
    static constexpr unsigned int INITIAL_SIZE = 1u << 16;
    static constexpr float MAX_LOAD_FACTOR = 0.7f;
    static constexpr unsigned int PRIME1 = 0x7FEB352D;
    static constexpr unsigned int PRIME2 = 0x846CA68B;
    static constexpr unsigned int PRIME3 = 0x4B1BD1B5;

    struct Bucket {
        State *state;
        unsigned int psl;
        bool occupied;
        Bucket();
    };

    HashTable();
    ~HashTable();

    bool insert(State *state);
    bool contains(const State *state) const;
    void cleanup();
    void removeState(State *state);

    // Getters pueden quedarse inline
    unsigned int size() const { return size_; }
    unsigned int capacity() const { return capacity_; }

    private:
    Bucket *buckets_;
    unsigned int size_;
    unsigned int capacity_;

    unsigned int computeHash(const State *state) const;
    bool shouldResize() const;
    void resize();
};
