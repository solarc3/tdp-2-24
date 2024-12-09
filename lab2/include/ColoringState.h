// ColoringState.h
#pragma once
#include "Graph.h"
#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;

class ColoringState {
    private:
    using AdjList = cc_hash_table<int, null_type, hash<int>>;

    const Graph &graph;
    vector<int> colors;
    int numColors;
    int numConflicts;
    vector<AdjList> colorClass;
    AdjList uncoloredVertices;

    void updateConflicts();
    bool isConflicting(int vertex) const;

    public:
    ColoringState(const Graph &g, int initialColors);
    int getNumColors() const { return numColors; }
    int getNumConflicts() const { return numConflicts; }
    int getColor(int vertex) const { return colors[vertex]; }
    const vector<int> &getColors() const { return colors; }
    void assignColor(int vertex, int color);
    bool isValidAssignment(int vertex, int color) const;
    vector<int> getAvailableColors(int vertex) const;
    bool isComplete() const { return uncoloredVertices.empty(); }
    bool isLegal() const { return numConflicts == 0; }
    void print() const;
    vector<pair<int, int>> getConflictingPairs() const;
    int getDeltaConflicts(int vertex, int newColor) const;
    vector<int> getVerticesWithColor(int color) const;
    int getMaxUsedColor() const;
    ColoringState &operator=(const ColoringState &other) {
        if (this != &other) {
            colors = other.colors;
            numColors = other.numColors;
            numConflicts = other.numConflicts;
            colorClass.clear();
            colorClass.resize(other.colorClass.size());
            for (size_t i = 0; i < other.colorClass.size(); ++i) {
                colorClass[i] = other.colorClass[i];
            }
            uncoloredVertices = other.uncoloredVertices;
        }
        return *this;
    }
    void unassignColor(int vertex);
};
