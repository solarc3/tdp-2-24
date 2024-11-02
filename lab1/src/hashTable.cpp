#include "../include/hashTable.h"
#include "../include/TracyMacros.h"
#include <cstring>

unsigned int hashTable::total_collisions = 0;
unsigned int hashTable::total_inserts = 0;

hashTable::hashTable()
    : table(new HashEntry[INITIAL_SIZE]), table_size(INITIAL_SIZE),
      num_elements(0) {}

hashTable::~hashTable() { delete[] table; }
/*
unsigned int hashTable::murmur3_32(const void *key, int len,
                                   unsigned int seed) const {
    TRACE_PLOT;
    const unsigned char *data = (const unsigned char *)key;
    const int nblocks = len >> 2;
    unsigned int h1 = seed;

    // Body
    const unsigned int *blocks = (const unsigned int *)(data);
    for (int i = 0; i < nblocks; i++) {
        unsigned int k1 = blocks[i];

        k1 *= C1;
        k1 = (k1 << R1) | (k1 >> (32 - R1));
        k1 *= C2;

        h1 ^= k1;
        h1 = (h1 << R2) | (h1 >> (32 - R2));
        h1 = h1 * M + N;
    }

    // Tail
    const unsigned char *tail = data + (nblocks << 2);
    unsigned int k1 = 0;

    switch (len & 3) {
        case 3:
            k1 ^= tail[2] << 16;
            [[fallthrough]];
        case 2:
            k1 ^= tail[1] << 8;
            [[fallthrough]];
        case 1:
            k1 ^= tail[0];
            k1 *= C1;
            k1 = (k1 << R1) | (k1 >> (32 - R1));
            k1 *= C2;
            h1 ^= k1;
    }

    // Finalization
    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}

*/
unsigned int hashTable::computeHash(State *state) const {
    TRACE_SCOPE;
    unsigned int hash = PRIME1;

    // Mejor mezcla de bits usando rotaciones y XOR
    for (unsigned int i = 0; i < state->size; i++) {
        unsigned int k = state->jugs[i];
        k *= PRIME2;
        k = (k << 13) | (k >> 19); // rotar bits
        k *= PRIME3;

        hash ^= k;
        hash = (hash << 13) | (hash >> 19);
        hash += hash << 3; // multiplicación rápida por 8
    }

    return hash & (table_size - 1);
}
// https://gcc.gnu.org/onlinedocs/libstdc++/ext/pb_ds/hash_based_containers.html
// tabla con size 2^n, se hace el AND y -1 para obtener el modulo, mas
// eficiente
// https://www.cs.cornell.edu/courses/JavaAndDS/files/hashing_collisions.pdf
// https://www.cs.cornell.edu/courses/JavaAndDS/files/CachingAffectsHashing.pdf

unsigned int hashTable::computeStep(State *state) const {
    TRACE_SCOPE;
    unsigned int hash = PRIME2;

    for (unsigned int i = 0; i < state->size; i++) {
        unsigned int k = state->jugs[i];
        k *= PRIME3;
        k = (k << 11) | (k >> 21); // rotación diferente
        k *= PRIME1;

        hash ^= k;
        hash = (hash << 11) | (hash >> 21);
        hash += hash << 2; // multiplicación rápida por 4
    }

    // Asegurar que el paso sea impar y menor que table_size
    return ((hash | 1) & (table_size - 1)) | 1;
}

bool hashTable::contains(State *state) const {
    TRACE_SCOPE;
    unsigned int hash = computeHash(state);
    unsigned int step = computeStep(state);
    unsigned int pos = hash;
    unsigned int start_pos = pos;
    const unsigned int mask = table_size - 1;
    // prueba de colisiones
    do {
        pos &= mask;
        if (!table[pos].occupied) {
            return false;
        }
        if (table[pos].state && table[pos].state->equals(state)) {
            return true;
        }
        // cout << "Colision en busqueda, posicion " << pos << endl;
        total_collisions++;
        pos += step;
    } while ((pos & mask) != start_pos);
    return false;
}

bool hashTable::insert(State *state) {
    TRACE_SCOPE;
    total_inserts++;

    // Redimensionar con un factor de carga más bajo
    if ((num_elements << 7) >= (table_size * BALANCE_FACTOR)) {
        resize();
    }

    unsigned int pos = computeHash(state);
    unsigned int step = computeStep(state);
    const unsigned int start_pos = pos;
    const unsigned int mask = table_size - 1;

    unsigned int probe_count = 0;
    const unsigned int MAX_PROBES = 16; // Limitar número de sondeos

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

        // Si hay muchas colisiones en secuencia, redimensionar
        if (probe_count >= MAX_PROBES) {
            resize();
            return insert(state);
        }

    } while ((pos & mask) != start_pos);

    resize();
    return insert(state);
}
void hashTable::resize() {
    TRACE_SCOPE;
    const unsigned int old_size = table_size;
    HashEntry *old_table = table;

    // Duplicar tamaño
    table_size <<= 1;
    table = new HashEntry[table_size];
    num_elements = 0;

    // Rehasher los elementos existentes
    for (unsigned int i = 0; i < old_size; i++) {
        if (old_table[i].occupied && old_table[i].state) {
            insert(old_table[i].state);
        }
    }

    delete[] old_table;
}

void hashTable::cleanup() {
    for (unsigned int i = 0; i < table_size; i++) {
        if (table[i].occupied && table[i].state) {
            delete table[i].state;
            table[i].state = nullptr;
            table[i].occupied = false;
        }
    }
}

void hashTable::removeState(State *state) {
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
