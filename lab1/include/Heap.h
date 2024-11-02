#pragma once
#include "State.h"
#include "hashTable.h"
#include <iostream>
#include <string>

using namespace std;

// cola de prioridad implementada como arbol binario
// en un arreglo.
// Hijo izquierdo de i: 2*i+1
// Hijo derecho de i: 2*i+2
// Padre de i: (i-1)/2

class Heap {
    public:
    State **arr;
    int capacity;
    int number;
    static const int INITIAL_CAPACITY = 4096; // Capacidad inicial por defecto

    Heap(int n);             // Crea Heap vacio de capacidad n
    Heap();                  // Crea Heap vacio con capacidad inicial
    ~Heap();                 // Destructor
    void push(State *x);     // Inserta un elemento en el Heap
    State *pop();            // Elimina y retorna el elemento de mayor prioridad
    void bubbleUp(int i);    // Mueve el elemento en la posicion i hacia arriba
    void bubbleDown(int i);  // Mueve el elemento en la posicion i hacia abajo
    void swap(int i, int j); // Intercambia elementos
    void resize(int new_capacity);
};
