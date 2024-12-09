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
    vector<int> maxClique;

    // Métodos privados para calcular cotas
    vector<int> findMaximalClique();
    int calculateDSaturBound();

    public:
    // Constructor
    explicit Bounds(const Graph &g);

    // Getters
    int getLowerBound() const;
    int getUpperBound() const;
    const vector<int> &getMaxClique() const;

    // Métodos para actualizar cotas
    void updateLowerBound(int newBound);
    void updateUpperBound(int newBound);
};
