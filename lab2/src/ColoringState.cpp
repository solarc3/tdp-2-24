// ColoringState.cpp
#include "../include/ColoringState.h"

ColoringState::ColoringState(const Graph &g, int initialColors)
    : graph(g), colors(g.getVertexCount(), -1), numColors(0), numConflicts(0),
      colorClass(initialColors) {
    for (int v = 0; v < graph.getVertexCount(); ++v) {
        uncoloredVertices.insert(v);
    }
}

void ColoringState::assignColor(int vertex, int color) {
    int oldColor = colors[vertex];

    if (oldColor != -1) {
        colorClass[oldColor].erase(vertex);
        // Decrease conflicts with neighbors of the old color
        for (int neighbor : graph.getNeighbors(vertex)) {
            if (colors[neighbor] == oldColor) {
                numConflicts--;
            }
        }
    } else {
        uncoloredVertices.erase(vertex);
    }

    // Ensure the colorClass vector is large enough
    if (color >= static_cast<int>(colorClass.size())) {
        colorClass.resize(color + 1);
    }

    colors[vertex] = color;
    colorClass[color].insert(vertex);

    // Update numColors
    if (color >= numColors) {
        numColors = color + 1;
    }

    // Increase conflicts with neighbors of the new color
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

std::vector<int> ColoringState::getAvailableColors(int vertex) const {
    std::vector<bool> usedColors(numColors + 1, false);
    for (int neighbor : graph.getNeighbors(vertex)) {
        int neighborColor = colors[neighbor];
        if (neighborColor != -1) {
            usedColors[neighborColor] = true;
        }
    }

    std::vector<int> available;
    for (int c = 0; c <= numColors; ++c) {
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

    for (int neighbor : graph.getNeighbors(vertex)) {
        int neighborColor = colors[neighbor];
        if (neighborColor == currentColor && currentColor != -1) {
            delta--; // Removing a conflict
        }
        if (neighborColor == newColor && newColor != -1) {
            delta++; // Adding a conflict
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

        // Decrease conflicts with neighbors of the old color
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
