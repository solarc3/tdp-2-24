#pragma once
#include <ext/pb_ds/assoc_container.hpp>
#include <fstream>
#include <iostream>
using namespace __gnu_pbds;
using namespace std;
#define vpii vector<pair<int, int>> // sobrenombre para no escribir tanto

class Graph {
    private:
    using AdjList = cc_hash_table<int, null_type, hash<int>>;
    int vertexCount;
    vector<AdjList> adj;
    size_t edgeCount;

    public:
    Graph();
    explicit Graph(int vertices);
    void addEdge(int v, int w);
    bool hasEdge(int v, int w) const;
    vector<int> getNeighbors(int v) const;
    void removeEdge(int v, int w);
    int getVertexCount() const;
    int getDegree(int v) const;
    bool createFromFile(const string &fileName);
    void printGraph();
    int getMaxDegree();
    bool areNeighbors(int a, int b) const;
};
