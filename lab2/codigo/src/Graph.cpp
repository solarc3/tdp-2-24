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
    if (v == w) {
        // self loop, invariante
        return;
    }
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
    neighbors.reserve(
        adj[v].size()); // prealloc para eficiencia, sino se queda pegado
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

bool Graph::createFromFile(const string &fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "No se pudo abrir el archivo " << fileName << endl;
        return false;
    }
    int maxVertex = 0;
    vector<pair<int, int>> edges;
    int v, w;

    while (file >> v >> w) {
        maxVertex = max(maxVertex, max(v, w));
        edges.push_back({v, w});
    }

    // iniciar con size correcto luego de agregar los vpii
    this->vertexCount = maxVertex;
    this->adj = vector<AdjList>(vertexCount);
    this->edgeCount = 0;

    // segunda pasada, cambio de base y grafo no dirigido
    for (const auto &edge : edges) {
        addEdge(edge.first - 1, edge.second - 1);
        addEdge(edge.second - 1, edge.first - 1);
    }

    cout << "Grafo cargado exitosamente:" << endl;
    cout << "Vertices: " << vertexCount << endl;
    cout << "Aristas: " << edgeCount / 2 << endl;

    file.close();
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

int Graph::getMaxDegree() {
    int maxDegree = 0;
    for (int i = 0; i < vertexCount; i++) {
        maxDegree = std::max(maxDegree, (int)adj[i].size());
    }
    return maxDegree;
}

bool Graph::areNeighbors(int a, int b) const {
    return adj[a].find(b) != adj[a].end();
}
