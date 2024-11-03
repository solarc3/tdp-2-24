#include "../include/HashTable.h"

HashTable::Bucket::Bucket() {
    this->state = nullptr;
    this->psl = 0;
    this->occupied = false;
}

HashTable::HashTable() : size_(0), capacity_(INITIAL_SIZE) {
    buckets_ = new Bucket[capacity_];
}

HashTable::~HashTable() {
    cleanup();
    delete[] buckets_;
}

bool HashTable::insert(State *state) {
    if (shouldResize()) {
        resize();
    }

    unsigned int hash = computeHash(state);
    unsigned int pos = hash & (capacity_ - 1);
    unsigned int psl = 0;

    while (true) {
        if (!buckets_[pos].occupied) {
            buckets_[pos].state = state;
            buckets_[pos].psl = psl;
            buckets_[pos].occupied = true;
            size_++;
            return true;
        }

        // Si encontramos un elemento existente igual, no insertamos
        if (buckets_[pos].state->equals(state)) {
            return false;
        }

        // Robin Hood - robar al rico
        if (psl > buckets_[pos].psl) {
            // swap
            State *temp_state = buckets_[pos].state;
            unsigned int temp_psl = buckets_[pos].psl;

            buckets_[pos].state = state;
            buckets_[pos].psl = psl;

            state = temp_state;
            psl = temp_psl;
        }

        pos = (pos + 1) & (capacity_ - 1);
        psl++;

        // Si llegamos muy lejos, redimensionar
        if (psl >= 32) {
            resize();
            return insert(state);
        }
    }
}

bool HashTable::contains(const State *state) const {
    unsigned int hash = computeHash(state);
    unsigned int pos = hash & (capacity_ - 1);
    unsigned int psl = 0;

    while (true) {
        if (!buckets_[pos].occupied) {
            return false;
        }

        if (buckets_[pos].state->equals(state)) {
            return true;
        }

        if (psl > buckets_[pos].psl) {
            return false;
        }

        pos = (pos + 1) & (capacity_ - 1);
        psl++;

        if (psl >= capacity_) {
            return false;
        }
    }
}

void HashTable::cleanup() {
    for (unsigned int i = 0; i < capacity_; i++) {
        if (buckets_[i].occupied && buckets_[i].state) {
            delete buckets_[i].state;
            buckets_[i].state = nullptr;
            buckets_[i].occupied = false;
            buckets_[i].psl = 0;
        }
    }
    size_ = 0;
}

void HashTable::removeState(State *state) {
    unsigned int hash = computeHash(state);
    unsigned int pos = hash & (capacity_ - 1);
    unsigned int psl = 0;

    while (true) {
        if (!buckets_[pos].occupied) {
            return;
        }

        if (buckets_[pos].state->equals(state)) {
            // Encontrado - backward-shift deletion
            unsigned int current = pos;
            unsigned int next = (current + 1) & (capacity_ - 1);

            while (buckets_[next].occupied && buckets_[next].psl > 0) {
                buckets_[current] = buckets_[next];
                buckets_[current].psl--;
                current = next;
                next = (current + 1) & (capacity_ - 1);
            }

            buckets_[current].occupied = false;
            buckets_[current].state = nullptr;
            buckets_[current].psl = 0;
            size_--;
            return;
        }

        if (psl > buckets_[pos].psl) {
            return;
        }

        pos = (pos + 1) & (capacity_ - 1);
        psl++;
    }
}

unsigned int HashTable::computeHash(const State *state) const {
    unsigned int h = PRIME1;

    for (unsigned int i = 0; i < state->size; i++) {
        unsigned int k = state->jugs[i];

        // bit mixing
        k *= PRIME2;
        k = (k << 23) | (k >> 9); // rotate
        k *= PRIME3;

        h ^= k;
        h = (h << 17) | (h >> 15); // rotate
        h += h << 3;
    }

    // Finalizer
    h ^= h >> 16;
    h *= PRIME2;
    h ^= h >> 13;
    h *= PRIME3;
    h ^= h >> 16;

    return h;
}

bool HashTable::shouldResize() const {
    return (static_cast<float>(size_) / capacity_) >= MAX_LOAD_FACTOR;
}

void HashTable::resize() {
    unsigned int old_capacity = capacity_;
    Bucket *old_buckets = buckets_;

    // Duplicar capacidad
    capacity_ *= 2;
    buckets_ = new Bucket[capacity_];
    size_ = 0;

    // Rehash
    for (unsigned int i = 0; i < old_capacity; i++) {
        if (old_buckets[i].occupied && old_buckets[i].state) {
            insert(old_buckets[i].state);
        }
    }

    delete[] old_buckets;
}
