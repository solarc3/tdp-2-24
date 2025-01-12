#include "../include/DangerHeuristic.h"

DangerHeuristic::DangerHeuristic(const Graph &g) : graph(g), targetColors(0) {}

double DangerHeuristic::calculateF(int diff_colored, int max_color) const {
    double y = diff_colored;
    if (max_color - y <= 0)
        return std::numeric_limits<double>::max();
    // formula del paper para el calculo
    return C / std::pow(max_color - y, k);
}

double DangerHeuristic::calculateVertexDanger(const ColoringState &state,
                                              int vertex) const {
    int max_color = state.getMaxUsedColor() + 1;
    if (max_color <= 0)
        max_color = 1;

    // calcular los con diferentes colores primero
    int diff_colored = 0;
    std::set<int> used_colors;
    for (int neighbor : graph.getNeighbors(vertex)) {
        int color = state.getColor(neighbor);
        if (color != -1) {
            used_colors.insert(color);
        }
    }
    diff_colored = used_colors.size();

    // calcular los vecinos que no tienen color
    int uncolored = 0;
    for (int neighbor : graph.getNeighbors(vertex)) {
        if (state.getColor(neighbor) == -1) {
            uncolored++;
        }
    }

    // calcular share/avail ratio
    auto availableColors = state.getAvailableColors(vertex, targetColors);
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
    // se computa
    double share_ratio = static_cast<double>(shared) / avail;

    // se agrega aca cada componente y se comupta segun paper
    double F = calculateF(diff_colored, max_color);
    return F + ku * uncolored + ka * share_ratio;
}

double DangerHeuristic::calculateColorDanger(const ColoringState &state,
                                             int vertex, int color) const {
    int max_color = state.getMaxUsedColor() + 1;
    if (max_color <= 0)
        max_color = 1;
    int max_diff_neighbors = 0;
    int nc = vertex;

    for (int neighbor : graph.getNeighbors(vertex)) {
        if (state.getColor(neighbor) == -1 &&
            state.isValidAssignment(neighbor, color)) {

            std::set<int> neighbor_colors;
            for (int n2 : graph.getNeighbors(neighbor)) {
                int c = state.getColor(n2);
                if (c != -1)
                    neighbor_colors.insert(c);
            }
            if (static_cast<int>(neighbor_colors.size()) > max_diff_neighbors) {
                max_diff_neighbors = static_cast<int>(neighbor_colors.size());
                nc = neighbor;
            }
        }
    }

    // calcula uncolored(nc)
    int uncolored_nc = 0;
    for (int neighbor : graph.getNeighbors(nc)) {
        if (state.getColor(neighbor) == -1) {
            uncolored_nc++;
        }
    }

    // calcular cantidad de vertices que esten usando ese color
    auto vertices_with_color = state.getVerticesWithColor(color);
    int num_c = vertices_with_color.size();

    // division por 0 -...-
    double denominator = std::pow(max_color - max_diff_neighbors, k2);
    if (denominator <= 0)
        return std::numeric_limits<double>::max();

    // comibnar segun formula del paper
    return (k1 / denominator) + k3 * uncolored_nc - k4 * num_c;
}

int DangerHeuristic::selectBestVertex(const ColoringState &state) const {
    double max_danger = -std::numeric_limits<double>::max();
    int selected_vertex = -1;

    std::set<int> top_saturation_vertices;
    int max_saturation = -1;

    // mayor saturacion primero, se combina DSATUR aca
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

    // entre los de mayor saturacion, se ve el con mas DANGER
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
    auto available_colors = state.getAvailableColors(vertex, targetColors);
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

ColoringState DangerHeuristic::generateInitialColoring(int targetColors) {
    this->targetColors = targetColors;

    ColoringState state(graph, targetColors);
    std::vector<int> vertices(graph.getVertexCount());

    for (int v = 0; v < graph.getVertexCount(); ++v) {
        vertices[v] = v;
    }

    std::sort(vertices.begin(), vertices.end(), [&](int a, int b) {
        return graph.getDegree(a) > graph.getDegree(b);
    });

    // asignacion simple, con danger genera error :(
    for (int vertex : vertices) {
        if (state.getColor(vertex) != -1)
            continue;

        // colores para ese nodo
        std::vector<int> availableColors =
            state.getAvailableColors(vertex, targetColors);

        // el menos posible para ese
        if (!availableColors.empty()) {
            int bestColor = availableColors[0];
            state.assignColor(vertex, bestColor, targetColors);
        }
        // si no se puede, se deja sin colorear, no es necesario mas
    }

    // invariante
    if (!state.isComplete() || !state.isLegal()) {
        std::cout << "Advertencia: La coloracion inicial no es legal.\n";
    }

    return state;
}

int DangerHeuristic::getDifferentColoredNeighbors(const ColoringState &state,
                                                  int vertex) const {
    std::set<int> colors;
    for (int neighbor : graph.getNeighbors(vertex)) {
        int color = state.getColor(neighbor);
        if (color != -1) {
            colors.insert(color);
        }
    }
    return colors.size();
}

int DangerHeuristic::getUncoloredNeighbors(const ColoringState &state,
                                           int vertex) const {
    int count = 0;
    for (int neighbor : graph.getNeighbors(vertex)) {
        if (state.getColor(neighbor) == -1) {
            count++;
        }
    }
    return count;
}

double DangerHeuristic::getColorShareRatio(const ColoringState &state,
                                           int vertex) const {
    auto availableColors = state.getAvailableColors(vertex, targetColors);
    if (availableColors.empty())
        return 0.0;
    int shared = 0;
    for (int color : availableColors) {
        for (int neighbor : graph.getNeighbors(vertex)) {
            if (state.getColor(neighbor) == -1 &&
                state.isValidAssignment(neighbor, color)) {
                shared++;
                break;
            }
        }
    }

    return static_cast<double>(shared) / availableColors.size();
}
