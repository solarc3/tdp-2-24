#pragma once
#include "Graph.h"
#include <set>
#include <vector>
using namespace std;

class Bounds {
    private:
    const Graph &graph;
    int lowerBound;
    int upperBound;

    // Métodos privados para calcular cotas
    int calculateUpperBound(); // Implementación RLF
    int calculateLowerBound(); // Algoritmo de clique máxima

    public:
    // Constructor
    explicit Bounds(const Graph &graph);

    // Getters para las cotas
    int getLowerBound() const;
    int getUpperBound() const;

    // Actualizadores de cotas
    void updateLowerBound(int newLowerBound);
    void updateUpperBound(int newUpperBound);
};
