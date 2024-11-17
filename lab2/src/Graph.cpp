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

bool Graph::createFromFile(const string &fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "No se pudo abrir el archivo " << fileName << endl;
        return false;
    }
    vpii edges;
    int v, w;
    while (file >> v >> w) {
        edges.push_back({v, w});
    }
    int maxVertex = 0;
    for (const auto &edge : edges) {
        maxVertex = std::max(maxVertex, std::max(edge.first, edge.second));
    }
    this->vertexCount = maxVertex;
    this->adj = vector<AdjList>(maxVertex);
    this->edgeCount = 0;
    // formato DIMACS no dirigido
    for (const auto &edge : edges) {
        this->addEdge(edge.first - 1, edge.second - 1);
        this->addEdge(edge.second - 1, edge.first - 1);
    }

    std::cout << "total de aristas: " << edges.size() << std::endl;
    std::cout << "total de vertices: " << maxVertex << std::endl;
    this->printGraph();

    return true;
}

void Graph::printGraph() {
    for (int i = 0; i < vertexCount; i++) {
        cout << i << ": ";
        for (const auto &w : adj[i]) {
            cout << w << " ";
        }
        cout << endl;
    }
}
