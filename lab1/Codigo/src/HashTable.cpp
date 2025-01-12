#include "../include/HashTable.h"
#include <cassert>

HashTable::Bucket::Bucket() {
    this->state = nullptr;
    this->psl = 0;
    this->occupied = false;
}

HashTable::HashTable() {
    this->size = 0;
    this->capacity = INITIAL_SIZE;
    this->buckets = new Bucket[capacity]();
}

HashTable::~HashTable() {
    if (buckets) {
        cleanup();
        delete[] buckets;
        buckets = nullptr;
    }
}

bool HashTable::insert(State *state) {
    if (!state || !buckets)
        return false;

    if (shouldResize()) {
        resize();
    }

    unsigned int hash = computeHash(state);
    unsigned int pos = hash & (capacity - 1);
    unsigned int psl = 0;

    State *current_state = state;
    unsigned int current_psl = psl;

    while (true) {
        if (!buckets[pos].occupied) {
            buckets[pos].state = current_state;
            buckets[pos].psl = current_psl;
            buckets[pos].occupied = true;
            size++;
            return true;
        }
        if (buckets[pos].state && buckets[pos].state->equals(state)) {
            return false;
        }

        // Robin Hood Hashing
        if (current_psl > buckets[pos].psl) {
            std::swap(current_state, buckets[pos].state);
            std::swap(current_psl, buckets[pos].psl);
        }

        pos = (pos + 1) & (capacity - 1);
        current_psl++;

        if (current_psl >= 8) {
            resize();
            return insert(current_state);
        }
    }
}

bool HashTable::contains(const State *state) const {
    if (!state || !buckets)
        return false;

    unsigned int hash = computeHash(state);
    unsigned int pos = hash & (capacity - 1);
    unsigned int psl = 0;

    while (true) {
        if (!buckets[pos].occupied) {
            return false;
        }

        if (buckets[pos].state && buckets[pos].state->equals(state)) {
            return true;
        }

        if (psl > buckets[pos].psl) {
            return false;
        }

        pos = (pos + 1) & (capacity - 1);
        psl++;

        if (psl >= capacity) {
            return false;
        }
    }
}

void HashTable::cleanup() {
    if (!buckets)
        return;

    for (unsigned int i = 0; i < capacity; i++) {
        if (buckets[i].occupied && buckets[i].state) {
            delete buckets[i].state;
            buckets[i].state = nullptr;
            buckets[i].occupied = false;
            buckets[i].psl = 0;
        }
    }
    size = 0;
}

void HashTable::removeState(State *state) {
    if (!state || !buckets)
        return;

    unsigned int hash = computeHash(state);
    unsigned int pos = hash & (capacity - 1);
    unsigned int psl = 0;

    while (true) {
        if (!buckets[pos].occupied) {
            return;
        }

        if (buckets[pos].state && buckets[pos].state->equals(state)) {
            // Found the state - perform backward-shift deletion
            unsigned int current = pos;
            unsigned int next = (current + 1) & (capacity - 1);

            while (buckets[next].occupied && buckets[next].psl > 0) {
                buckets[current] = buckets[next];
                buckets[current].psl--;
                current = next;
                next = (current + 1) & (capacity - 1);
            }

            buckets[current].occupied = false;
            buckets[current].state = nullptr;
            buckets[current].psl = 0;
            size--;
            return;
        }

        if (psl > buckets[pos].psl) {
            return;
        }

        pos = (pos + 1) & (capacity - 1);
        psl++;

        if (psl >= capacity) {
            return;
        }
    }
}

unsigned int HashTable::computeHash(const State *state) const {
    if (!state || !state->jugs)
        return 0;

    unsigned int h = PRIME1;

    for (unsigned int i = 0; i < state->size; i++) {
        unsigned int k = state->jugs[i];

        k *= PRIME2;
        k = (k << 23) | (k >> 9);
        k *= PRIME3;

        h ^= k;
        h = (h << 17) | (h >> 15);
        h += h << 3;
    }

    h ^= h >> 16;
    h *= PRIME2;
    h ^= h >> 13;
    h *= PRIME3;
    h ^= h >> 16;

    return h;
}

bool HashTable::shouldResize() const {
    return (static_cast<float>(size) / capacity) >= MAX_LOAD_FACTOR;
}

void HashTable::resize() {
    if (!buckets)
        return;

    unsigned int old_capacity = capacity;
    Bucket *old_buckets = buckets;
    capacity *= 2;
    buckets = new Bucket[capacity]();
    size = 0;
    for (unsigned int i = 0; i < old_capacity; i++) {
        if (old_buckets[i].occupied && old_buckets[i].state) {
            insert(old_buckets[i].state);
        }
    }

    delete[] old_buckets;
}
