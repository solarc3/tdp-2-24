// Bounds.cpp
#include "../include/Bounds.h"

Bounds::Bounds(const Graph &g)
    : graph(g), lowerBound(1), upperBound(g.getVertexCount()),
      bestColoring(nullptr) {

    maxClique = findMaximalClique();
    lowerBound = maxClique.size();
    upperBound = calculateUpperBound();
}

vector<int> Bounds::findMaximalClique() const {
    vector<int> clique;

    // Ordenar vértices por grado descendente
    auto vertices = getVerticesByDegree();
    sort(vertices.begin(), vertices.end(),
         [](const auto &a, const auto &b) { return a.second > b.second; });

    // Construir un clique maximal de forma greedy empezando por los vértices de
    // mayor grado
    for (const auto &[vertex, degree] : vertices) {
        // Verificar si el vértice puede agregarse al clique
        bool canAdd = true;
        for (int v : clique) {
            if (!graph.hasEdge(vertex, v)) {
                canAdd = false;
                break;
            }
        }
        if (canAdd) {
            clique.push_back(vertex);
        }
    }

    return clique;
}

vector<pair<int, int>> Bounds::getVerticesByDegree() const {
    vector<pair<int, int>> vertices;
    vertices.reserve(graph.getVertexCount());

    // Obtener los 3 vértices de mayor grado
    for (int v = 0; v < graph.getVertexCount(); v++) {
        vertices.emplace_back(v, graph.getDegree(v));
    }

    return vertices;
}

int Bounds::calculateUpperBound() {
    DangerHeuristic danger(graph);
    // Crear un nuevo ColoringState
    auto coloring = danger.generateInitialColoring(graph.getVertexCount());

    // Si es la primera solución o es mejor que la actual
    if (!bestColoring ||
        coloring.getNumColors() < bestColoring->getNumColors()) {
        bestColoring =
            std::make_unique<ColoringState>(graph, coloring.getNumColors());
        // Copiar los colores del nuevo coloreo
        for (int v = 0; v < graph.getVertexCount(); v++) {
            bestColoring->assignColor(v, coloring.getColor(v));
        }
    }

    return coloring.getNumColors();
}

void Bounds::updateLowerBound(int newBound) {
    if (newBound > lowerBound) {
        lowerBound = newBound;
    }
}

void Bounds::updateUpperBound(int newBound, const ColoringState &coloring) {
    if (newBound < upperBound) {
        upperBound = newBound;
        bestColoring =
            std::make_unique<ColoringState>(graph, coloring.getNumColors());

        // Copiar configuración del nuevo mejor coloreo
        for (int v = 0; v < graph.getVertexCount(); v++) {
            bestColoring->assignColor(v, coloring.getColor(v));
        }
    }
}
