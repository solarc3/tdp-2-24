#pragma once
#include "../include/TracyMacros.h"
#include "HashTable.h"
#include "State.h"
#include <iostream>
#include <string>

using namespace std;
class Heap {
    public:
    State **arr;
    int capacity;
    int number;
    static const int INITIAL_CAPACITY = 4096; // default

    Heap(int n);
    Heap();
    ~Heap();
    void push(State *x);
    State *pop();
    void bubbleUp(int i);
    void bubbleDown(int i);
    void swap(int i, int j);
    void resize(int new_capacity);
};
