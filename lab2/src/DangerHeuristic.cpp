#include "../include/DangerHeuristic.h"

DangerHeuristic::DangerHeuristic(const Graph &g) : graph(g) {}

double DangerHeuristic::calculateF(int diff_colored, int max_color) const {
    double y = diff_colored;
    if (max_color - y <= 0)
        return std::numeric_limits<double>::max();
    return C / std::pow(max_color - y, k);
}

double DangerHeuristic::calculateVertexDanger(const ColoringState &state,
                                              int vertex) const {
    int max_color = state.getMaxUsedColor() + 1;
    if (max_color <= 0)
        max_color = 1;

    // Calcula different_colored
    int diff_colored = 0;
    std::set<int> used_colors;
    for (int neighbor : graph.getNeighbors(vertex)) {
        int color = state.getColor(neighbor);
        if (color != -1) {
            used_colors.insert(color);
        }
    }
    diff_colored = used_colors.size();

    // Calcula uncolored neighbors
    int uncolored = 0;
    for (int neighbor : graph.getNeighbors(vertex)) {
        if (state.getColor(neighbor) == -1) {
            uncolored++;
        }
    }

    // Calcula share/avail ratio
    auto availableColors = state.getAvailableColors(vertex);
    int avail = availableColors.size();
    if (avail == 0)
        return std::numeric_limits<double>::max();

    int shared = 0;
    for (int color : availableColors) {
        bool is_shared = false;
        for (int neighbor : graph.getNeighbors(vertex)) {
            if (state.getColor(neighbor) == -1 &&
                state.isValidAssignment(neighbor, color)) {
                is_shared = true;
                break;
            }
        }
        if (is_shared)
            shared++;
    }

    double share_ratio = static_cast<double>(shared) / avail;

    // Calcula danger según la fórmula del paper
    double F = calculateF(diff_colored, max_color);
    return F + ku * uncolored + ka * share_ratio;
}

double DangerHeuristic::calculateColorDanger(const ColoringState &state,
                                             int vertex, int color) const {
    int max_color = state.getMaxUsedColor() + 1;
    if (max_color <= 0)
        max_color = 1;

    // Encuentra el vecino no coloreado con el máximo different_colored
    int max_diff_neighbors = 0;
    int nc = vertex;

    for (int neighbor : graph.getNeighbors(vertex)) {
        if (state.getColor(neighbor) == -1 &&
            state.isValidAssignment(neighbor, color)) {

            int diff_neighbors = 0;
            std::set<int> neighbor_colors;
            for (int n2 : graph.getNeighbors(neighbor)) {
                int c = state.getColor(n2);
                if (c != -1)
                    neighbor_colors.insert(c);
            }

            if (neighbor_colors.size() > max_diff_neighbors) {
                max_diff_neighbors = neighbor_colors.size();
                nc = neighbor;
            }
        }
    }

    // Calcula uncolored(nc)
    int uncolored_nc = 0;
    for (int neighbor : graph.getNeighbors(nc)) {
        if (state.getColor(neighbor) == -1) {
            uncolored_nc++;
        }
    }

    // Calcula num(c) - número de vértices usando el color
    auto vertices_with_color = state.getVerticesWithColor(color);
    int num_c = vertices_with_color.size();

    // Previene división por cero
    double denominator = std::pow(max_color - max_diff_neighbors, k2);
    if (denominator <= 0)
        return std::numeric_limits<double>::max();

    // Calcula danger según la fórmula del paper
    return (k1 / denominator) + k3 * uncolored_nc - k4 * num_c;
}

int DangerHeuristic::selectBestVertex(const ColoringState &state) const {
    double max_danger = -std::numeric_limits<double>::max();
    int selected_vertex = -1;

    std::set<int> top_saturation_vertices;
    int max_saturation = -1;

    // Primero encuentra los vértices con mayor saturación
    for (int v = 0; v < graph.getVertexCount(); ++v) {
        if (state.getColor(v) != -1)
            continue;

        std::set<int> used_colors;
        for (int neighbor : graph.getNeighbors(v)) {
            int color = state.getColor(neighbor);
            if (color != -1)
                used_colors.insert(color);
        }

        int saturation = used_colors.size();
        if (saturation > max_saturation) {
            max_saturation = saturation;
            top_saturation_vertices.clear();
            top_saturation_vertices.insert(v);
        } else if (saturation == max_saturation) {
            top_saturation_vertices.insert(v);
        }
    }

    // Entre los vértices con mayor saturación, elige el de mayor danger
    for (int v : top_saturation_vertices) {
        double danger = calculateVertexDanger(state, v);
        if (danger > max_danger) {
            max_danger = danger;
            selected_vertex = v;
        }
    }

    return selected_vertex;
}

int DangerHeuristic::selectBestColor(const ColoringState &state,
                                     int vertex) const {
    auto available_colors = state.getAvailableColors(vertex);
    if (available_colors.empty())
        return -1;

    double min_danger = std::numeric_limits<double>::max();
    int selected_color = available_colors[0];

    for (int color : available_colors) {
        double danger = calculateColorDanger(state, vertex, color);
        if (danger < min_danger) {
            min_danger = danger;
            selected_color = color;
        }
    }

    return selected_color;
}
