#pragma once
#include "ColoringState.h"
#include "DangerHeuristic.h"
#include "Graph.h"
#include <queue>
#include <vector>

class Bounds {
    private:
    const Graph &graph;
    int lowerBound; // Cota inferior (tamaño del clique máximo encontrado)
    int upperBound; // Cota superior (colores usados por la heurística DANGER)
    vector<int> maxClique; // Vértices del clique máximo encontrado
    std::unique_ptr<ColoringState> bestColoring; // Mejor coloreo encontrado

    // Método privado para encontrar un clique grande heurísticamente
    vector<int> findMaximalClique() const;

    // Método para ordenar vértices por grado para búsqueda de clique
    vector<pair<int, int>> getVerticesByDegree() const;

    // Calcula cota superior usando la heurística DANGER
    int calculateUpperBound();

    public:
    Bounds(const Graph &g);

    // Getters para las cotas
    int getLowerBound() const { return lowerBound; }
    int getUpperBound() const { return upperBound; }

    // Obtener el clique máximo encontrado
    const vector<int> &getMaxClique() const { return maxClique; }

    // Obtener el mejor coloreo encontrado
    const ColoringState *getBestColoring() const { return bestColoring.get(); }

    // Actualizar cotas
    void updateLowerBound(int newBound);
    void updateUpperBound(int newBound, const ColoringState &coloring);

    // Verificar si un coloreo con k colores es factible
    bool isFeasible(int k) const { return k >= lowerBound; }
};
