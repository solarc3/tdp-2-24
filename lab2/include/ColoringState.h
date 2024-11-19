#pragma once
#include "Graph.h"
#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;

class ColoringState {
    public:
    using AdjList = cc_hash_table<int, null_type, hash<int>>;
    using OrderedSet = tree<int, null_type, less<int>, rb_tree_tag,
                            tree_order_statistics_node_update>;

<<<<<<< HEAD
    const Graph &graph; // Referencia al grafo original
    vector<int> colors; // Color asignado a cada vértice (-1 = sin color)
    int numColors;
    int numConflicts;
    vector<AdjList> colorClass;
    AdjList uncoloredVertices;
=======
    const Graph &graph; // Debe ser el primero ya que se inicializa primero
    vector<int> colors; // Segundo
    vector<OrderedSet> colorClass; // Tercero
    AdjList uncoloredVertices;     // Cuarto
    int numColors;                 // Quinto
    int numConflicts;              // Sexto
    // Constructor normal también debe seguir el mismo orden
    ColoringState(const Graph &g, int initialColors)
        : graph(g), colors(g.getVertexCount(), -1), colorClass(initialColors),
          uncoloredVertices(), numColors(0), numConflicts(0) {
        // Resto de la inicialización...
    }
>>>>>>> d9d247d (no funciona deje la zorra)

    // Operador de asignación de movimiento
    // En ColoringState.h, agrega:

    ColoringState &operator=(const ColoringState &other) {
        if (this != &other) {
            // No copiamos graph ya que es una referencia const
            // Copiamos todos los contenedores
            colors = other.colors;
            colorClass = other.colorClass;
            uncoloredVertices = other.uncoloredVertices;

            // Copiamos los valores primitivos
            numColors = other.numColors;
            numConflicts = other.numConflicts;
        }
        return *this;
    }
    void updateConflicts();
    bool isConflicting(int vertex) const;

    // Getters basicos
    int getNumColors() const { return numColors; }
    int getNumConflicts() const { return numConflicts; }
    int getColor(int vertex) const { return colors[vertex]; }
    const vector<int> &getColors() const { return colors; }

    // Operaciones principales
    void assignColor(int vertex, int color);
    bool isValidAssignment(int vertex, int color) const;
    vector<int> getAvailableColors(int vertex) const;
    bool isComplete() const { return uncoloredVertices.empty(); }
    bool isLegal() const { return numConflicts == 0; }

    // Metodos para analisis y debug
    void print() const;
    vector<pair<int, int>> getConflictingPairs() const;
    // Metodos para B&B y busqueda tabu
    int getDeltaConflicts(int vertex, int newColor) const;
    vector<int> getVerticesWithColor(int color) const;
    int getMaxUsedColor() const;
    int getDifferentColoredNeighbors(int vertex) const;
    int getUncoloredNeighbors(int vertex) const;
};
