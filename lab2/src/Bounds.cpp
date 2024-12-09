#include "../include/Bounds.h"
#include <climits>
#include <functional>
#include <set>

Bounds::Bounds(const Graph &graph) : graph(graph) {
    upperBound = calculateUpperBound();
    lowerBound = calculateLowerBound();
}

int Bounds::calculateUpperBound() {
    std::cout << "Calculando cota superior..." << std::endl;
    int n = graph.getVertexCount();
    vector<int> color(n, -1);
    int numColors = 0;
    vector<int> uncolored(n);

    // Inicializar vector de vértices no coloreados
    for (int i = 0; i < n; i++) {
        uncolored[i] = i;
    }

    const int MAX_ITERATIONS =
        10000; // Límite de iteraciones para evitar bucles muy largos
    int iterations = 0;

    while (!uncolored.empty() && iterations++ < MAX_ITERATIONS) {
        // Encontrar vértice de mayor grado entre los no coloreados
        int maxDegree = -1;
        int startVertex = -1;
        int startVertexIndex = -1;

        for (size_t i = 0; i < uncolored.size(); i++) {
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

        // Construir conjunto independiente con límite de tiempo
        vector<bool> canAdd(n, true);
        for (int u : graph.getNeighbors(startVertex)) {
            canAdd[u] = false;
        }

        const int MAX_INDEPENDENT_SET_ITERATIONS = 1000;
        int indSetIterations = 0;

        for (int v : uncolored) {
            if (indSetIterations++ > MAX_INDEPENDENT_SET_ITERATIONS)
                break;

            if (!canAdd[v])
                continue;

            bool isIndependent = true;
            for (int u = 0; u < n; u++) {
                if (color[u] == numColors && graph.hasEdge(v, u)) {
                    isIndependent = false;
                    break;
                }
            }

            if (isIndependent) {
                color[v] = numColors;
                // Remover de uncolored
                for (size_t i = 0; i < uncolored.size(); i++) {
                    if (uncolored[i] == v) {
                        uncolored[i] = uncolored.back();
                        uncolored.pop_back();
                        break;
                    }
                }
                // Marcar vecinos como no disponibles
                for (int u : graph.getNeighbors(v)) {
                    canAdd[u] = false;
                }
            }
        }
    }
    std::cout << "Cota superior calculada: " << numColors << std::endl;
    return numColors;
}

int Bounds::calculateLowerBound() {
    std::cout << "Calculando cota inferior..." << std::endl;
    int n = graph.getVertexCount();

    // Usar el grado máximo como cota inferior inicial
    int lowerBound = 1;

    // Algoritmo voraz para encontrar una clique
    vector<bool> inClique(n, false);
    vector<int> candidates(n);
    for (int i = 0; i < n; i++)
        candidates[i] = i;

    // Ordenar vértices por grado descendente
    std::sort(candidates.begin(), candidates.end(), [this](int a, int b) {
        return graph.getDegree(a) > graph.getDegree(b);
    });

    // Construir clique vorazmente
    for (int i = 0; i < n && i < 1000;
         i++) { // Limitar iteraciones para grafos grandes
        int v = candidates[i];
        bool canAdd = true;

        // Verificar si v forma clique con todos los vértices ya seleccionados
        for (int u = 0; u < n; u++) {
            if (inClique[u]) {
                if (!graph.hasEdge(u, v)) {
                    canAdd = false;
                    break;
                }
            }
        }

        if (canAdd) {
            inClique[v] = true;
            lowerBound =
                std::max(lowerBound, (int)std::count(inClique.begin(),
                                                     inClique.end(), true));
        }
    }

    // Usar el grado mínimo como otra cota inferior
    int minDegree = n;
    for (int v = 0; v < n; v++) {
        minDegree = std::min(minDegree, graph.getDegree(v));
    }

    lowerBound = std::max(lowerBound, minDegree + 1);

    std::cout << "Cota inferior calculada: " << lowerBound << std::endl;
    return lowerBound;
}
int Bounds::getLowerBound() const { return lowerBound; }
int Bounds::getUpperBound() const { return upperBound; }
void Bounds::updateLowerBound(int newLowerBound) { lowerBound = newLowerBound; }
void Bounds::updateUpperBound(int newUpperBound) { upperBound = newUpperBound; }
