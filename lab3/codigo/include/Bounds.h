#pragma once
#include "Graph.h"
#include <set>
#include <unordered_set>
#include <vector>
using namespace std;
#include <algorithm>
#include <climits>

class Bounds {
    private:
    const Graph &graph;
    int lowerBound;
    int upperBound;
    int calculateUpperBound(); // RLF
    int calculateLowerBound(); // Max clique

    public:
    explicit Bounds(const Graph &graph);

    int getLowerBound() const;
    int getUpperBound() const;

    void updateLowerBound(int newLowerBound);
    void updateUpperBound(int newUpperBound);
};
