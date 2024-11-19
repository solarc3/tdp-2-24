#include "../include/ColoringState.h"

void ColoringState::assignColor(int vertex, int color) {
    // Validar vertex
    if (vertex < 0 || vertex >= graph.getVertexCount()) {
        throw std::invalid_argument("Invalid vertex index");
    }

    // Validar color
    if (color < -1) { // -1 es válido para "sin color"
        throw std::invalid_argument("Invalid color index");
    }

    // Expandir colorClass si es necesario
    if (color >= (int)colorClass.size() && color != -1) {
        colorClass.resize(color + 1);
    }

    int oldColor = colors[vertex];

    // Quitar color anterior
    if (oldColor != -1) {
        colorClass[oldColor].erase(vertex);
    } else {
        uncoloredVertices.erase(vertex);
    }

    // Asignar nuevo color
    colors[vertex] = color;
    if (color != -1) {
        colorClass[color].insert(vertex);
        if (color >= numColors) {
            numColors = color + 1;
        }
    } else {
        uncoloredVertices.insert(vertex);
    }

    updateConflicts();
}

bool ColoringState::isValidAssignment(int vertex, int color) const {
    if (vertex < 0 || vertex >= graph.getVertexCount() || color < 0) {
        return false;
    }

    // Revisar si algun vecino tiene el mismo color
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

int ColoringState::getDeltaConflicts(
    int vertex, int newColor) const { // TODO: NO SE EJECUTA
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

vector<pair<int, int>>
ColoringState::getConflictingPairs() const { // TODO: NO SE EJECUTA
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

void ColoringState::print() const { // TODO: NO SE EJECUTA
    cout << "Current coloring state:" << endl;
    for (int v = 0; v < graph.getVertexCount(); v++) {
        cout << "Vertex " << v << ": Color " << colors[v] << endl;
    }
    cout << "Number of colors used: " << numColors << endl;
    cout << "Number of conflicts: " << numConflicts << endl;
    cout << "Uncolored vertices: " << uncoloredVertices.size() << endl;
}

int ColoringState::getDifferentColoredNeighbors(int vertex) const {
    vector<bool> usedColors(numColors, false);
    int count = 0;
    for (int neighbor : graph.getNeighbors(vertex)) {
        int color = colors[neighbor];
        if (color != -1 && !usedColors[color]) {
            usedColors[color] = true;
            count++;
        }
    }
    return count;
}
int ColoringState::getUncoloredNeighbors(int vertex) const {
    int count = 0;
    for (int neighbor : graph.getNeighbors(vertex)) {
        if (colors[neighbor] == -1) {
            count++;
        }
    }
    return count;
}
