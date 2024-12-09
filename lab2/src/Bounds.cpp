#include "../include/Bounds.h"
#include <algorithm>

Bounds::Bounds(const Graph &graph) : graph(graph) {
    upperBound = calculateUpperBound();
    lowerBound = calculateLowerBound();
}

struct VertexComparator {
    const Graph &graph;
    bool operator()(int a, int b) const {
        int degreeA = graph.getDegree(a);
        int degreeB = graph.getDegree(b);
        if (degreeA != degreeB)
            return degreeA > degreeB;
        return a < b;
    }
    VertexComparator(const Graph &g) : graph(g) {}
};

int Bounds::calculateUpperBound() {
    std::cout << "Calculando cota superior..." << std::endl;
    int n = graph.getVertexCount();
    std::vector<int> color(n, -1);
    int numColors = 0;
    typedef tree<int, null_type, VertexComparator, rb_tree_tag,
                 tree_order_statistics_node_update>
        OrderedSet;
    OrderedSet uncolored((VertexComparator(graph)));

    for (int i = 0; i < n; i++) {
        uncolored.insert(i);
    }

    while (!uncolored.empty()) {
        // Siempre tomamos el vértice con mayor grado
        int startVertex = *uncolored.begin();
        uncolored.erase(uncolored.begin());

        numColors++;
        color[startVertex] = numColors;

        // Mantener un conjunto de vértices que pueden ser coloreados con el
        // color actual
        std::vector<int> toRemove;

        for (int v : uncolored) {
            bool canAssign = true;
            for (int neighbor : graph.getNeighbors(v)) {
                if (color[neighbor] == numColors) {
                    canAssign = false;
                    break;
                }
            }
            if (canAssign) {
                color[v] = numColors;
                toRemove.push_back(v);
            }
        }

        // Eliminar los vértices coloreados en esta iteración
        for (int v : toRemove) {
            uncolored.erase(v);
        }
    }
    std::cout << "Cota superior calculada: " << numColors << std::endl;
    return numColors;
}

// Nueva implementación de calculateLowerBound usando una heurística más rápida
int Bounds::calculateLowerBound() {
    std::cout << "Calculando cota inferior (heurística)..." << std::endl;
    int n = graph.getVertexCount();
    if (n == 0) {
        std::cout << "Cota inferior calculada: 0" << std::endl;
        return 0;
    }

    // Ordenar los vértices por grado descendente
    std::vector<int> vertices(n);
    for (int i = 0; i < n; ++i) {
        vertices[i] = i;
    }
    std::sort(vertices.begin(), vertices.end(), [&](int a, int b) {
        return graph.getDegree(a) > graph.getDegree(b);
    });

    std::vector<int> clique;

    for (int v : vertices) {
        bool isConnected = true;
        for (int u : clique) {
            if (!graph.areNeighbors(v, u)) {
                isConnected = false;
                break;
            }
        }
        if (isConnected) {
            clique.push_back(v);
        }
    }

    int lowerBound = clique.size();
    std::cout << "Cota inferior calculada (heurística): " << lowerBound
              << std::endl;
    return lowerBound;
}

int Bounds::getLowerBound() const { return lowerBound; }
int Bounds::getUpperBound() const { return upperBound; }
void Bounds::updateLowerBound(int newLowerBound) { lowerBound = newLowerBound; }
void Bounds::updateUpperBound(int newUpperBound) { upperBound = newUpperBound; }
