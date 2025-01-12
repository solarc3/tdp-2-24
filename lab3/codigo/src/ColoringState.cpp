#include "../include/ColoringState.h"

ColoringState::ColoringState(const Graph &g, int initialColors)
    : graph(g), colors(g.getVertexCount(), -1), numColors(0), numConflicts(0),
      colorClass(initialColors) {
    for (int v = 0; v < graph.getVertexCount(); ++v) {
        uncoloredVertices.insert(v);
    }
}

void ColoringState::assignColor(int vertex, int color, int targetColors) {
    if (vertex < 0 || vertex >= graph.getVertexCount()) {
        std::cerr << "Error: indice invalido " << vertex << std::endl;
        return;
    }
    int oldColor = colors[vertex];

    if (oldColor != -1) {
        colorClass[oldColor].erase(vertex);
        // menos conflictos con el vecino que se tiene
        for (int neighbor : graph.getNeighbors(vertex)) {
            if (colors[neighbor] == oldColor) {
                numConflicts--;
            }
        }
    } else {
        uncoloredVertices.erase(vertex);
    }
    if (color >= static_cast<int>(colorClass.size())) {
        if (color < targetColors) {
            colorClass.resize(color + 1);
        } else {
            // color range por los bounds, se mantiene entre eso, no puede
            // decidir eso esta funcion no es su responsabilidad
            std::cerr << "Error: Color " << color << " exceeds targetColors "
                      << targetColors << std::endl;
            return;
        }
    }

    colors[vertex] = color;
    colorClass[color].insert(vertex);

    if (color + 1 > numColors && color < targetColors) {
        numColors = color + 1;
    }

    for (int neighbor : graph.getNeighbors(vertex)) {
        if (colors[neighbor] == color) {
            numConflicts++;
        }
    }
}

bool ColoringState::isValidAssignment(int vertex, int color) const {
    for (int neighbor : graph.getNeighbors(vertex)) {
        if (colors[neighbor] == color) {
            return false;
        }
    }
    return true;
}

std::vector<int> ColoringState::getAvailableColors(int vertex,
                                                   int targetColors) const {
    std::vector<bool> usedColors(targetColors, false);
    if (vertex < 0 || vertex >= graph.getVertexCount()) {
        std::cerr << "Error: Indice de vertice invalido en getAvailableColors: "
                  << vertex << std::endl;
        return {};
    }

    for (int neighbor : graph.getNeighbors(vertex)) {
        int neighborColor = colors[neighbor];
        if (neighborColor != -1 && neighborColor < targetColors) {
            usedColors[neighborColor] = true;
        }
    }

    std::vector<int> available;
    for (int c = 0; c < targetColors; ++c) { // c < Targer colors si o si
        if (!usedColors[c]) {
            available.push_back(c);
        }
    }
    return available;
}

bool ColoringState::isConflicting(int vertex) const {
    int vertexColor = colors[vertex];
    if (vertexColor == -1)
        return false;
    // hay conflicto si se tiene el color con un vecino
    for (int neighbor : graph.getNeighbors(vertex)) {
        if (colors[neighbor] == vertexColor) {
            return true;
        }
    }
    return false;
}

std::vector<int> ColoringState::getVerticesWithColor(int color) const {
    if (color >= 0 && color < static_cast<int>(colorClass.size())) {
        return std::vector<int>(colorClass[color].begin(),
                                colorClass[color].end());
    }
    return {};
}

int ColoringState::getMaxUsedColor() const { return numColors - 1; }

int ColoringState::getDeltaConflicts(int vertex, int newColor) const {
    int delta = 0;
    int currentColor = colors[vertex];
    // delta dice la diferencia
    for (int neighbor : graph.getNeighbors(vertex)) {
        int neighborColor = colors[neighbor];
        if (neighborColor == currentColor && currentColor != -1) {
            delta--; // sacar conflicto
        }
        if (neighborColor == newColor && newColor != -1) {
            delta++; // agregar conflicto
        }
    }
    return delta;
}

void ColoringState::unassignColor(int vertex) {
    int color = colors[vertex];
    if (color != -1) {
        colorClass[color].erase(vertex);
        colors[vertex] = -1;
        uncoloredVertices.insert(vertex);

        // se quitan conflictos si se elimina ese
        for (int neighbor : graph.getNeighbors(vertex)) {
            if (colors[neighbor] == color) {
                numConflicts--;
            }
        }
    }
}

void ColoringState::print() const {
    std::cout << "Current coloring state:" << std::endl;
    for (int v = 0; v < graph.getVertexCount(); ++v) {
        std::cout << "Vertex " << v << ": Color " << colors[v] << std::endl;
    }
    std::cout << "Number of colors used: " << numColors << std::endl;
    std::cout << "Number of conflicts: " << numConflicts << std::endl;
    std::cout << "Uncolored vertices: " << uncoloredVertices.size()
              << std::endl;
}
