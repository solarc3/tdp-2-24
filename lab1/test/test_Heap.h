#include "../include/Heap.h"
#include <cassert>

inline void testHeap() {
    PairingHeap *heap = new PairingHeap();
    assert(heap->empty());
    assert(heap->size() == PairingHeap::INITIAL_CAPACITY);

    // agregar elementos con distinto peso para ver si se ordenan por prioridad
    // al sacarlos
    State *s1 = new State(3, new unsigned int[3]{4, 0, 0}, 0, 10, nullptr);
    State *s2 = new State(3, new unsigned int[3]{0, 4, 0}, 0, 5, nullptr);
    State *s3 = new State(3, new unsigned int[3]{0, 0, 4}, 0, 15, nullptr);
    heap->push(s1);
    heap->push(s2);
    heap->push(s3);
    // ver size y si s2 quedo como punta
    assert(heap->size() == 3);
    assert(heap->peek() == s2);
    // eliminar en orden de peso
    assert(heap->pop() == s2);
    assert(heap->pop() == s1);
    assert(heap->pop() == s3);

    assert(heap->empty());

    delete heap;
    delete s1;
    delete s2;
    delete s3;
}
