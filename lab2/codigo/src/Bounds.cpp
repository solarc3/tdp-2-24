#include "../include/Bounds.h"

Bounds::Bounds(const Graph &graph) : graph(graph) {
    upperBound = std::min(calculateUpperBound(), graph.getVertexCount());
    lowerBound = std::max(1, calculateLowerBound());
}

int Bounds::calculateUpperBound() {
    std::cout << "Calculando cota superior..." << std::endl;
    int n = graph.getVertexCount();
    std::vector<int> color(n, -1);
    int numColors = 0;
    // sort por grado
    std::vector<int> vertices(n);
    for (int i = 0; i < n; ++i) {
        vertices[i] = i;
    }
    // inline func funcional
    std::sort(vertices.begin(), vertices.end(), [&](int a, int b) {
        return graph.getDegree(a) > graph.getDegree(b);
    });

    for (int vertex : vertices) {
        // primer color que no usen los vecinos
        std::vector<bool> usedColors(numColors, false);
        for (int neighbor : graph.getNeighbors(vertex)) {
            if (color[neighbor] != -1) {
                usedColors[color[neighbor]] = true;
            }
        }

        // primer color disponible
        int assignedColor = -1;
        for (int c = 0; c < numColors; ++c) {
            if (!usedColors[c]) {
                assignedColor = c;
                break;
            }
        }

        // si no hay, me doy uno
        if (assignedColor == -1) {
            assignedColor = numColors;
            numColors++;
        }

        color[vertex] = assignedColor;
    }

    upperBound = numColors;
    std::cout << "Cota superior calculada: " << upperBound << std::endl;
    return upperBound;
}

int Bounds::calculateLowerBound() {
    std::cout << "Calculando cota inferior..." << std::endl;
    int n = graph.getVertexCount();
    if (n == 0) {
        std::cout << "Cota inferior calculada: 0" << std::endl;
        return 0;
    }

    // sort grado
    std::vector<int> vertices(n);
    for (int i = 0; i < n; ++i) {
        vertices[i] = i;
    }
    // inline function, ordenar por el retorn
    std::sort(vertices.begin(), vertices.end(), [&](int a, int b) {
        return graph.getDegree(a) > graph.getDegree(b);
    });

    std::vector<int> clique;
    // calculo de clique
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
    std::cout << "Cota inferior calculada (heuristica): " << lowerBound
              << std::endl;
    return lowerBound;
}

int Bounds::getLowerBound() const { return lowerBound; }
int Bounds::getUpperBound() const { return upperBound; }
void Bounds::updateLowerBound(int newLowerBound) {
    if (newLowerBound > lowerBound && newLowerBound <= graph.getVertexCount()) {
        lowerBound = newLowerBound;
        std::cout << "Lower Bound actualizado a: " << lowerBound << std::endl;
    }
}

void Bounds::updateUpperBound(int newUpperBound) {
    if (newUpperBound < upperBound && newUpperBound > 0) {
        upperBound = newUpperBound;
        std::cout << "Upper Bound actualizado a: " << upperBound << std::endl;
    }
}
