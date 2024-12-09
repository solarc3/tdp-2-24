#include "../include/Bounds.h"
#include <climits>
#include <functional>
#include <set>

Bounds::Bounds(const Graph &graph) : graph(graph) {
    upperBound = calculateUpperBound();
    lowerBound = calculateLowerBound();
}

int Bounds::calculateUpperBound() {
    int n = graph.getVertexCount();
    vector<int> color(n, -1);
    int numColors = 0;
    vector<int> uncolored(n);

    // Inicializar vector de vértices no coloreados
    for (int i = 0; i < n; i++) {
        uncolored[i] = i;
    }

    while (!uncolored.empty()) {
        // Encontrar vértice de mayor grado entre los no coloreados
        int maxDegree = -1;
        int startVertex = -1;
        int startVertexIndex = -1;

        for (int i = 0; i < (int)uncolored.size(); i++) {
            int v = uncolored[i];
            if (color[v] == -1) {
                int degree = graph.getDegree(v);
                if (degree > maxDegree) {
                    maxDegree = degree;
                    startVertex = v;
                    startVertexIndex = i;
                }
            }
        }

        if (startVertex == -1)
            break;

        // Remover el vértice seleccionado de los no coloreados
        if (startVertexIndex != -1) {
            uncolored[startVertexIndex] = uncolored.back();
            uncolored.pop_back();
        }

        numColors++;
        color[startVertex] = numColors;

        // Construir conjunto independiente
        vector<int> independentSet;
        vector<bool> canAdd(n, true);

        // Marcar vecinos del vértice inicial como no disponibles
        for (int u : graph.getNeighbors(startVertex)) {
            canAdd[u] = false;
        }

        // Construir conjunto independiente maximial
        for (int v : uncolored) {
            if (!canAdd[v])
                continue;

            bool isIndependent = true;
            for (int u : independentSet) {
                if (graph.hasEdge(v, u)) {
                    isIndependent = false;
                    break;
                }
            }

            if (isIndependent) {
                independentSet.push_back(v);
                // Marcar vecinos como no disponibles
                for (int u : graph.getNeighbors(v)) {
                    canAdd[u] = false;
                }
            }
        }

        // Colorear el conjunto independiente
        for (int v : independentSet) {
            color[v] = numColors;
            // Remover de uncolored
            for (size_t i = 0; i < uncolored.size(); i++) {
                if (uncolored[i] == v) {
                    uncolored[i] = uncolored.back();
                    uncolored.pop_back();
                    break;
                }
            }
        }
    }

    return numColors;
}

int Bounds::calculateLowerBound() {
    int n = graph.getVertexCount();
    int maxCliqueSize = 0;
    vector<int> currentClique;
    function<void(int)> dfs = [&](int start) {
        for (int v = start; v < n; ++v) {
            bool canAdd = true;
            for (int u : currentClique) {
                if (!graph.hasEdge(u, v)) {
                    canAdd = false;
                    break;
                }
            }
            if (canAdd) {
                currentClique.push_back(v);
                if ((int)currentClique.size() > maxCliqueSize) {
                    maxCliqueSize = currentClique.size();
                }
                dfs(v + 1);
                currentClique.pop_back();
            }
        }
    };
    dfs(0);
    return maxCliqueSize;
}

int Bounds::getLowerBound() const { return lowerBound; }
int Bounds::getUpperBound() const { return upperBound; }
void Bounds::updateLowerBound(int newLowerBound) { lowerBound = newLowerBound; }
void Bounds::updateUpperBound(int newUpperBound) { upperBound = newUpperBound; }
