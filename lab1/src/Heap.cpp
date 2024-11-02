#include "../include/Heap.h"
#include "../include/TracyMacros.h"
#include <iostream>

Heap::Heap(int n) {
    TRACE_SCOPE;
    capacity = n > 0 ? n : INITIAL_CAPACITY;
    number = 0;
    arr = new State *[capacity];
}

Heap::Heap() : Heap(INITIAL_CAPACITY) {}

Heap::~Heap() {
    TRACE_SCOPE;
    delete[] arr;
}

void Heap::push(State *x) {
    TRACE_SCOPE;
    if (number == capacity) {
        resize(capacity * 2);
    }
    arr[number++] = x;
    bubbleUp(number - 1);
    TRACE_PLOT("Heap size", static_cast<int64_t>(number));
    TRACE_PLOT("Heap capacity", static_cast<int64_t>(capacity));
}

State *Heap::pop() {
    TRACE_SCOPE;
    if (number == 0)
        return nullptr;

    State *x = arr[0];
    arr[0] = arr[--number];
    if (number > 0) {
        bubbleDown(0);
    }

    TRACE_PLOT("Heap size", static_cast<int64_t>(number));
    return x;
}

void Heap::resize(int new_capacity) {
    TRACE_SCOPE;
    std::cout << " heap resized to: " << new_capacity << std::endl;

    State **new_arr = new State *[new_capacity];
    {
        TRACE_SCOPE_NAMED("HeapCopy");
        for (int i = 0; i < number; ++i) {
            new_arr[i] = arr[i];
        }
    }
    delete[] arr;
    arr = new_arr;
    capacity = new_capacity;

    TRACE_PLOT("Heap capacity", static_cast<int64_t>(capacity));
}

void Heap::bubbleUp(int index) {
    TRACE_SCOPE;
    if (index <= 0)
        return;

    State *temp = arr[index];
    const unsigned int weight = temp->weight;
    int parent;
    int swaps = 0;

    while (index > 0) {
        parent = (index - 1) >> 1; // Divide by 2
        if (arr[parent]->weight <= weight) {
            break;
        }
        arr[index] = arr[parent];
        index = parent;
        swaps++;
    }
    arr[index] = temp;

    TRACE_PLOT("BubbleUp swaps", static_cast<int64_t>(swaps));
}

void Heap::bubbleDown(int index) {
    TRACE_SCOPE;
    const int half = number >> 1; // Dividir por 2
    State *temp = arr[index];
    const unsigned int weight = temp->weight;
    int swaps = 0;

    while (index < half) {
        int child = (index << 1) + 1; // 2*index + 1
        int right = child + 1;

        // Obtener el hijo con menor weight
        if (right < number && arr[right]->weight < arr[child]->weight) {
            child = right;
        }

        if (arr[child]->weight >= weight) {
            break;
        }

        arr[index] = arr[child];
        index = child;
        swaps++;
    }
    arr[index] = temp;

    TRACE_PLOT("BubbleDown swaps", static_cast<int64_t>(swaps));
}
