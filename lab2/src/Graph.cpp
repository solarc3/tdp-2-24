#include "../include/Graph.h"

Graph::Graph(int vertices) {
    this->vertexCount = vertices;
    this->adj = vector<AdjList>(vertices);
    this->edgeCount = 0;
}

void Graph::addEdge(int v, int w) {
    if (!hasEdge(v, w)) {
        adj[v].insert(w);
        edgeCount++;
    }
}

bool Graph::hasEdge(int v, int w) const {
    return adj[v].find(w) != adj[v].end();
}

vector<int> Graph::getNeighbors(int v) const {
    vector<int> neighbors;
    neighbors.reserve(adj[v].size()); // Preallocate para eficiencia
    for (const auto &w : adj[v]) {
        neighbors.push_back(w);
    }
    return neighbors;
}

void Graph::removeEdge(int v, int w) {
    if (hasEdge(v, w)) {
        adj[v].erase(w);
        edgeCount--;
    }
}

int Graph::getVertexCount() const { return vertexCount; }

int Graph::getDegree(int v) const { return adj[v].size(); }

bool Graph::isEmpty() const { return edgeCount == 0; }

void Graph::clear() {
    for (auto &adjList : adj) {
        adjList.clear();
    }
    edgeCount = 0;
}
