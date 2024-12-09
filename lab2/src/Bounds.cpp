#include "../include/Bounds.h"

Bounds::Bounds(const Graph &g) : graph(g) {
    maxClique = findMaximalClique();
    lowerBound = maxClique.size();
    upperBound = calculateDSaturBound();
}

vector<int> Bounds::findMaximalClique() {
    vector<int> clique;
    vector<bool> inClique(graph.getVertexCount(), false);

    // Ordenar vértices por grado
    vector<pair<int, int>> vertices;
    for (int v = 0; v < graph.getVertexCount(); v++) {
        vertices.push_back({graph.getDegree(v), v});
    }
    sort(vertices.rbegin(), vertices.rend());

    // Construir clique
    for (const auto &[degree, v] : vertices) {
        bool canAdd = true;
        for (int u : clique) {
            if (!graph.hasEdge(v, u)) {
                canAdd = false;
                break;
            }
        }
        if (canAdd) {
            clique.push_back(v);
            inClique[v] = true;
        }
    }

    return clique;
}

int Bounds::calculateDSaturBound() {
    vector<int> colors(graph.getVertexCount(), -1);
    vector<set<int>> saturation(
        graph.getVertexCount()); // Colores usados por vecinos

    // Comenzar con el vértice de mayor grado
    int maxDegree = -1;
    int firstVertex = 0;
    for (int v = 0; v < graph.getVertexCount(); v++) {
        int degree = graph.getDegree(v);
        if (degree > maxDegree) {
            maxDegree = degree;
            firstVertex = v;
        }
    }

    colors[firstVertex] = 0;
    int maxColorUsed = 0;

    // Actualizar saturación de vecinos
    for (int u : graph.getNeighbors(firstVertex)) {
        saturation[u].insert(0);
    }

    // Colorear resto de vértices
    for (int i = 1; i < graph.getVertexCount(); i++) {
        // Seleccionar vértice con mayor saturación
        int maxSat = -1;
        int maxDeg = -1;
        int nextVertex = -1;

        for (int v = 0; v < graph.getVertexCount(); v++) {
            if (colors[v] != -1)
                continue;

            int sat = saturation[v].size();
            int deg = graph.getDegree(v);

            if (sat > maxSat || (sat == maxSat && deg > maxDeg)) {
                maxSat = sat;
                maxDeg = deg;
                nextVertex = v;
            }
        }

        // Encontrar el menor color disponible
        int color = 0;
        while (saturation[nextVertex].count(color))
            color++;

        colors[nextVertex] = color;
        maxColorUsed = max(maxColorUsed, color);

        // Actualizar saturación de vecinos
        for (int u : graph.getNeighbors(nextVertex)) {
            if (colors[u] == -1) {
                saturation[u].insert(color);
            }
        }
    }

    return maxColorUsed + 1;
}

int Bounds::getLowerBound() const { return lowerBound; }

int Bounds::getUpperBound() const { return upperBound; }

const vector<int> &Bounds::getMaxClique() const { return maxClique; }

void Bounds::updateLowerBound(int newBound) {
    if (newBound > lowerBound) {
        lowerBound = newBound;
    }
}

void Bounds::updateUpperBound(int newBound) {
    if (newBound < upperBound) {
        upperBound = newBound;
    }
}
