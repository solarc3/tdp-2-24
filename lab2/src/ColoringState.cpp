// ColoringState.cpp
#include "../include/ColoringState.h"

ColoringState::ColoringState(const Graph &g, int initialColors)
    : graph(g), colors(g.getVertexCount(), -1), numColors(0), numConflicts(0),
      colorClass(initialColors) {
    for (int v = 0; v < graph.getVertexCount(); v++) {
        uncoloredVertices.insert(v);
    }
}

void ColoringState::assignColor(int vertex, int color) {
    if (vertex < 0 || vertex >= graph.getVertexCount() || color < 0) {
        throw std::invalid_argument("Invalid vertex or color");
    }

    int oldColor = colors[vertex];

    // Si el vértice ya tenía un color
    if (oldColor != -1) {
        // Asegurar que oldColor es válido antes de acceder
        if (oldColor < (int)colorClass.size()) {
            colorClass[oldColor].erase(vertex);
        }
    } else {
        uncoloredVertices.erase(vertex);
    }

    // Asegurar que el nuevo color tenga espacio en colorClass
    if (color >= (int)colorClass.size()) {
        colorClass.resize(color + 1);
    }

    colors[vertex] = color;
    colorClass[color].insert(vertex);

    // Actualizar numColors
    if (color >= numColors) {
        numColors = color + 1;
    }

    updateConflicts();
}

bool ColoringState::isValidAssignment(int vertex, int color) const {
    if (vertex < 0 || vertex >= graph.getVertexCount() || color < 0) {
        return false;
    }

    // Revisar si algún vecino tiene el mismo color
    for (int neighbor : graph.getNeighbors(vertex)) {
        if (colors[neighbor] == color) {
            return false;
        }
    }
    return true;
}

vector<int> ColoringState::getAvailableColors(int vertex) const {
    vector<bool> usedColors(numColors + 1, false);
    // Marcar colores usados por los vecinos
    for (int neighbor : graph.getNeighbors(vertex)) {
        if (colors[neighbor] != -1) {
            usedColors[colors[neighbor]] = true;
        }
    }

    // Recolectar colores disponibles
    vector<int> available;
    for (int c = 0; c < numColors + 1; c++) {
        if (!usedColors[c]) {
            available.push_back(c);
        }
    }
    return available;
}

void ColoringState::updateConflicts() {
    numConflicts = 0;
    for (int v = 0; v < graph.getVertexCount(); v++) {
        if (isConflicting(v)) {
            numConflicts++;
        }
    }
}

bool ColoringState::isConflicting(int vertex) const {
    if (colors[vertex] == -1)
        return false;

    for (int neighbor : graph.getNeighbors(vertex)) {
        if (colors[neighbor] == colors[vertex]) {
            return true;
        }
    }
    return false;
}

void ColoringState::updateColorClass(int vertex, int oldColor, int newColor) {
    // Eliminar vértice de la clase de color anterior
    colorClass[oldColor].erase(vertex);
}

vector<int> ColoringState::getVerticesWithColor(int color) const {
    vector<int> vertices;
    if (color >= 0 && color < (int)colorClass.size()) {
        vertices.reserve(colorClass[color].size());
        for (const auto &vertex : colorClass[color]) {
            vertices.push_back(vertex);
        }
    }
    return vertices;
}

int ColoringState::getMaxUsedColor() const { return numColors - 1; }

int ColoringState::getDeltaConflicts(int vertex, int newColor) const {
    if (colors[vertex] == newColor)
        return 0;

    int delta = 0;
    // Restar conflictos actuales
    for (int neighbor : graph.getNeighbors(vertex)) {
        if (colors[neighbor] == colors[vertex]) {
            delta--;
        }
    }

    // Sumar nuevos conflictos potenciales
    for (int neighbor : graph.getNeighbors(vertex)) {
        if (colors[neighbor] == newColor) {
            delta++;
        }
    }

    return delta;
}

vector<pair<int, int>> ColoringState::getConflictingPairs() const {
    vector<pair<int, int>> conflicts;
    for (int v = 0; v < graph.getVertexCount(); v++) {
        if (colors[v] == -1)
            continue;

        for (int u : graph.getNeighbors(v)) {
            if (u > v && colors[u] == colors[v]) {
                conflicts.emplace_back(v, u);
            }
        }
    }
    return conflicts;
}

void ColoringState::print() const {
    cout << "Current coloring state:" << endl;
    for (int v = 0; v < graph.getVertexCount(); v++) {
        cout << "Vertex " << v << ": Color " << colors[v] << endl;
    }
    cout << "Number of colors used: " << numColors << endl;
    cout << "Number of conflicts: " << numConflicts << endl;
    cout << "Uncolored vertices: " << uncoloredVertices.size() << endl;
}
void ColoringState::unassignColor(int vertex) {
    if (colors[vertex] != -1) {
        colorClass[colors[vertex]].erase(vertex);
        colors[vertex] = -1;
        uncoloredVertices.insert(vertex);
        updateConflicts();
    }
}
