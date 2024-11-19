#include "../include/Graph.h"

Graph::Graph() {
    this->vertexCount = 0;
    this->edgeCount = 0;
}
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
    neighbors.reserve(adj[v].size());
    for (const auto &w : adj[v]) {
        neighbors.push_back(w);
    }
    return neighbors;
}

void Graph::removeEdge(int v, int w) { // TODO: NO SE UTILIZA
    if (hasEdge(v, w)) {
        adj[v].erase(w);
        edgeCount--;
    }
}

int Graph::getVertexCount() const { return vertexCount; }

int Graph::getDegree(int v) const { return adj[v].size(); }

bool Graph::isEmpty() const { return edgeCount == 0; }

void Graph::clear() { // TODO: NO SE UTILIZA
    for (auto &adjList : adj) {
        adjList.clear();
    }
    edgeCount = 0;
}

bool Graph::createFromFile(const string &fileName) {
    // First pass - find max vertex
    ifstream file(fileName);
    if (!file.is_open()) {
        return false;
    }

    int v, w;
    int maxVertex = 0;
    while (file >> v >> w) {
        maxVertex = max(maxVertex, max(v, w));
    }
    file.close();

    // Reserve space
    vertexCount = maxVertex;
    adj.resize(maxVertex);
    edgeCount = 0;

    file.open(fileName);
    while (file >> v >> w) {
        addEdge(v - 1, w - 1);
        addEdge(w - 1, v - 1);
    }

    return true;
}

void Graph::printGraph() { // TODO: NO SE UTILIZA
    for (int i = 0; i < vertexCount; i++) {
        cout << i << ": ";
        for (const auto &w : adj[i]) {
            cout << w << " ";
        }
        cout << endl;
    }
}

int Graph::getMaxDegree() { // TODO: NO SE UTILIZA
    int maxDegree = 0;
    for (int i = 0; i < vertexCount; i++) {
        maxDegree = std::max(maxDegree, (int)adj[i].size());
    }
    return maxDegree;
}
