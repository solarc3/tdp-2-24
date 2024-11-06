#include "../include/HashTable.h"
#include <cassert>

inline void testHashTable() {
    // constructor

    HashTable *ht = new HashTable();

    assert(ht->capacity() == HashTable::INITIAL_SIZE);
    assert(ht->buckets_->occupied == false);
    assert(ht->buckets_->state == nullptr);
    assert(ht->buckets_->psl == 0);

    unsigned int *test_jugs = new unsigned int[3]{4, 0, 0};
    State *test_state = new State(3, test_jugs, 0, 0, nullptr);

    // computar hash
    unsigned int stateHash = ht->computeHash(test_state);

    assert(stateHash != 0);

    bool insertar = ht->insert(test_state);
    assert(insertar);

    // duplicado
    bool insertar2 = ht->insert(test_state);
    assert(!insertar2);

    // remover de la tabla
    ht->removeState(test_state);
    bool contains = ht->contains(test_state);
    assert(!contains);

    // destructor
    delete ht;
    delete test_state;
    delete[] test_jugs;
}
