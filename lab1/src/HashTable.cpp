#include "../include/HashTable.h"

unsigned int HashTable::total_collisions = 0;
unsigned int HashTable::total_inserts = 0;

HashTable::HashTable() {
    table = new HashEntry[INITIAL_SIZE];
    table_size = INITIAL_SIZE;
    num_elements = 0;
}

HashTable::~HashTable() { delete[] table; }
// https://gcc.gnu.org/onlinedocs/libstdc++/ext/pb_ds/hash_based_containers.html
// tabla con size 2^n, se hace el AND y -1 para obtener el modulo, mas
// eficiente
// https://www.cs.cornell.edu/courses/JavaAndDS/files/hashing_collisions.pdf
// https://www.cs.cornell.edu/courses/JavaAndDS/files/CachingAffectsHashing.pdf
unsigned int HashTable::computeHash(State *state) const {
    TRACE_SCOPE;
    unsigned int hash = PRIME1;
    // bit mix con XOR y fast mul
    // da buen balance entre velocidad y distribucion
    for (unsigned int i = 0; i < state->size; i++) {
        unsigned int k = state->jugs[i];
        k *= PRIME2;
        k = (k << 13) | (k >> 19); // rotar bits
        k *= PRIME3;

        hash ^= k;
        hash = (hash << 13) | (hash >> 19);
        hash += hash << 3; // fast mul 8
    }

    return hash & (table_size - 1);
}

unsigned int HashTable::computeStep(State *state) const {
    TRACE_SCOPE;
    unsigned int hash = PRIME2;

    for (unsigned int i = 0; i < state->size; i++) {
        unsigned int k = state->jugs[i];
        k *= PRIME3;
        k = (k << 11) | (k >> 21); // mix distinto
        k *= PRIME1;

        hash ^= k;
        hash = (hash << 11) | (hash >> 21);
        hash += hash << 2; // fast mul 4
    }

    // impar y menor que el table size
    return ((hash | 1) & (table_size - 1)) | 1;
}

bool HashTable::contains(State *state) const {
    TRACE_SCOPE;
    unsigned int hash = computeHash(state);
    unsigned int step = computeStep(state);
    unsigned int pos = hash;
    unsigned int start_pos = pos;
    const unsigned int mask = table_size - 1;
    // colision check
    do {
        pos &= mask;
        if (!table[pos].occupied) {
            return false;
        }
        if (table[pos].state && table[pos].state->equals(state)) {
            return true;
        }
        total_collisions++;
        pos += step;
    } while ((pos & mask) != start_pos);
    return false;
}

bool HashTable::insert(State *state) {
    TRACE_SCOPE;
    total_inserts++;

    // redimension en base al factor de balance
    if ((num_elements << 7) >= (table_size * BALANCE_FACTOR)) {
        resize();
    }

    unsigned int pos = computeHash(state);
    unsigned int step = computeStep(state);
    const unsigned int start_pos = pos;
    const unsigned int mask = table_size - 1;

    unsigned int probe_count = 0;
    const unsigned int MAX_PROBES =
        16; // si son muchas veces las que se intenta encontrar, quiza es tiempo
            // de aumentar el size

    do {
        pos &= mask;
        if (!table[pos].occupied) {
            table[pos].state = state;
            table[pos].occupied = true;
            num_elements++;
            return true;
        }

        if (table[pos].state && table[pos].state->equals(state)) {
            return false;
        }

        total_collisions++;
        pos += step;
        probe_count++;

        // muchas colisiones? redimensionar
        if (probe_count >= MAX_PROBES) {
            resize();
            return insert(state);
        }

    } while ((pos & mask) != start_pos);

    resize();
    return insert(state);
}
void HashTable::resize() {
    TRACE_SCOPE;
    const unsigned int old_size = table_size;
    HashEntry *old_table = table;

    // duplicar potencia de 2
    table_size <<= 1;
    table = new HashEntry[table_size];
    num_elements = 0;

    // rehashear, no ingresar a la misma posicion, asi se evita tener las mismas
    // posiciones y se distribuyen mejor
    for (unsigned int i = 0; i < old_size; i++) {
        if (old_table[i].occupied && old_table[i].state) {
            insert(old_table[i].state);
        }
    }

    delete[] old_table;
}

void HashTable::cleanup() {
    for (unsigned int i = 0; i < table_size; i++) {
        if (table[i].occupied && table[i].state) {
            delete table[i].state;
            table[i].state = nullptr;
            table[i].occupied = false;
        }
    }
}

void HashTable::removeState(State *state) {
    unsigned int hash = computeHash(state);
    unsigned int step = computeStep(state);
    unsigned int pos = hash;
    const unsigned int start_pos = pos;
    const unsigned int mask = table_size - 1;

    do {
        pos &= mask;
        if (table[pos].occupied && table[pos].state &&
            table[pos].state->equals(state)) {
            table[pos].occupied = false;
            table[pos].state = nullptr;
            return;
        }
        pos += step;
    } while ((pos & mask) != start_pos);
}
