#include "test.h"
#include "../include/Bounds.h"
#include "../include/BranchAndBound.h"
#include "../include/ColoringState.h"
#include "../include/DangerHeuristic.h"
#include "../include/Graph.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>

using namespace std;

void testGraph() {
    cout << "Iniciando test de la clase Graph..." << endl;

    Graph g(5);
    assert(g.getVertexCount() == 5);
    assert(g.getDegree(0) == 0);
    assert(g.getDegree(1) == 0);
    assert(g.getDegree(2) == 0);
    assert(g.getDegree(3) == 0);
    assert(g.getDegree(4) == 0);

    g.addEdge(0, 1);
    g.addEdge(1, 0);

    g.addEdge(0, 2);
    g.addEdge(2, 0);

    g.addEdge(1, 2);
    g.addEdge(2, 1);

    g.addEdge(3, 4);
    g.addEdge(4, 3);

    assert(g.hasEdge(0, 1) == true);
    assert(g.hasEdge(1, 0) == true);
    assert(g.hasEdge(0, 2) == true);
    assert(g.hasEdge(2, 0) == true);
    assert(g.hasEdge(1, 2) == true);
    assert(g.hasEdge(2, 1) == true);
    assert(g.hasEdge(3, 4) == true);
    assert(g.hasEdge(4, 3) == true);
    assert(g.hasEdge(2, 3) == false);

    assert(g.getDegree(0) == 2); // Conectado a 1 y 2
    assert(g.getDegree(1) == 2); // Conectado a 0 y 2
    assert(g.getDegree(2) == 2); // Conectado a 0 y 1
    assert(g.getDegree(3) == 1); // Conectado a 4
    assert(g.getDegree(4) == 1); // Conectado a 3

    auto neighbors0 = g.getNeighbors(0);
    assert(neighbors0.size() == 2);
    assert(find(neighbors0.begin(), neighbors0.end(), 1) != neighbors0.end());
    assert(find(neighbors0.begin(), neighbors0.end(), 2) != neighbors0.end());

    auto neighbors1 = g.getNeighbors(1);
    assert(neighbors1.size() == 2);
    assert(find(neighbors1.begin(), neighbors1.end(), 0) != neighbors1.end());
    assert(find(neighbors1.begin(), neighbors1.end(), 2) != neighbors1.end());

    auto neighbors2 = g.getNeighbors(2);
    assert(neighbors2.size() == 2);
    assert(find(neighbors2.begin(), neighbors2.end(), 0) != neighbors2.end());
    assert(find(neighbors2.begin(), neighbors2.end(), 1) != neighbors2.end());

    auto neighbors3 = g.getNeighbors(3);
    assert(neighbors3.size() == 1);
    assert(find(neighbors3.begin(), neighbors3.end(), 4) != neighbors3.end());

    auto neighbors4 = g.getNeighbors(4);
    assert(neighbors4.size() == 1);
    assert(find(neighbors4.begin(), neighbors4.end(), 3) != neighbors4.end());

    // sacar una arista
    g.removeEdge(0, 1);
    assert(g.hasEdge(0, 1) == false);
    assert(g.getDegree(0) == 1); // solo queda con 2 conectado
    auto neighborsAfterRemoval0 = g.getNeighbors(0);
    assert(neighborsAfterRemoval0.size() == 1);
    assert(find(neighborsAfterRemoval0.begin(), neighborsAfterRemoval0.end(),
                2) != neighborsAfterRemoval0.end());

    // sacar inversa ahora
    g.removeEdge(1, 0);
    assert(g.hasEdge(1, 0) == false);
    assert(g.getDegree(1) == 1); // solo con 2 ahora
    auto neighborsAfterRemoval1 = g.getNeighbors(1);
    assert(neighborsAfterRemoval1.size() == 1);
    assert(find(neighborsAfterRemoval1.begin(), neighborsAfterRemoval1.end(),
                2) != neighborsAfterRemoval1.end());

    // creacion de archivo con base 1
    string testFileName = "test_graph.txt";
    ofstream testFile(testFileName);
    testFile << "1 2\n";
    testFile << "1 3\n";
    testFile << "2 3\n";
    testFile << "4 5\n";
    testFile.close();

    Graph gFromFile;
    bool loaded = gFromFile.createFromFile(testFileName);
    assert(loaded == true);
    assert(gFromFile.getVertexCount() == 5);

    // verificar aristas
    assert(gFromFile.hasEdge(0, 1) == true);
    assert(gFromFile.hasEdge(1, 0) == true);
    assert(gFromFile.hasEdge(0, 2) == true);
    assert(gFromFile.hasEdge(2, 0) == true);
    assert(gFromFile.hasEdge(1, 2) == true);
    assert(gFromFile.hasEdge(2, 1) == true);
    assert(gFromFile.hasEdge(3, 4) == true);
    assert(gFromFile.hasEdge(4, 3) == true);
    assert(gFromFile.hasEdge(2, 3) == false);

    // verificar grados
    assert(gFromFile.getDegree(0) == 2);
    assert(gFromFile.getDegree(1) == 2);
    assert(gFromFile.getDegree(2) == 2);
    assert(gFromFile.getDegree(3) == 1);
    assert(gFromFile.getDegree(4) == 1);

    remove(testFileName.c_str());

    cout << "Test de la clase Graph pasado exitosamente." << endl;
}

void testBounds() {
    cout << "Iniciando test de la clase Bounds..." << endl;
    Graph g(4);
    g.addEdge(0, 1);
    g.addEdge(1, 0);

    g.addEdge(0, 2);
    g.addEdge(2, 0);

    g.addEdge(1, 2);
    g.addEdge(2, 1);

    g.addEdge(2, 3);
    g.addEdge(3, 2);

    Bounds bounds(g);
    assert(bounds.getUpperBound() >= bounds.getLowerBound());

    cout << "Lower Bound: " << bounds.getLowerBound() << endl;
    cout << "Upper Bound: " << bounds.getUpperBound() << endl;
    assert(bounds.getUpperBound() == 3);

    bounds.updateLowerBound(3);
    assert(bounds.getLowerBound() == 3);

    bounds.updateUpperBound(3);
    assert(bounds.getUpperBound() == 3);

    cout << "Test de la clase Bounds pasado exitosamente." << endl;
}

void testColoringState() {
    cout << "Iniciando test de la clase ColoringState..." << endl;

    Graph g(3);

    g.addEdge(0, 1);
    g.addEdge(1, 0);

    g.addEdge(0, 2);
    g.addEdge(2, 0);

    g.addEdge(1, 2);
    g.addEdge(2, 1);

    ColoringState state(g, 2);
    assert(state.isComplete() == false);
    assert(state.isLegal() == true);
    assert(state.getNumColors() == 0);

    state.assignColor(0, 0, 2);
    assert(state.getColor(0) == 0);
    assert(state.isComplete() == false);
    assert(state.isLegal() == true);
    assert(state.getNumColors() == 1);

    state.assignColor(1, 1, 2);
    assert(state.getColor(1) == 1);
    assert(state.isLegal() == true);
    assert(state.getNumColors() == 2);

    state.assignColor(2, 0, 2);
    assert(state.getColor(2) == 0);
    assert(state.isLegal() == false);
    assert(state.getNumConflicts() == 1);

    state.unassignColor(2);
    assert(state.getColor(2) == -1);
    assert(state.isLegal() == true);
    assert(state.getNumConflicts() == 0);
    assert(state.getNumColors() == 2);

    cout << "Test de la clase ColoringState pasado exitosamente." << endl;
}
void testDangerHeuristic() {
    cout << "Iniciando test de la clase DangerHeuristic..." << endl;

    Graph g(4);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(2, 3);

    ColoringState state(g, 3);
    state.assignColor(0, 0, 3);
    state.assignColor(1, 1, 3);
    state.assignColor(2, 2, 3);
    state.assignColor(3, 0, 3);

    DangerHeuristic dh(g);
    double danger = dh.calculateVertexDanger(state, 3);
    cout << "Danger para el vertice 3: " << danger << endl;
    cout << "Test de la clase DangerHeuristic pasado exitosamente." << endl;
}

void testBranchAndBound() {
    cout << "Iniciando test de la clase BranchAndBound..." << endl;

    Graph g(4);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(2, 3);

    Bounds bounds(g);
    DangerHeuristic dh(g);
    BranchAndBound bnb(g, bounds, dh);

    ColoringState solution(g, bounds.getUpperBound());

    bnb.solve(solution);

    assert(solution.isComplete() == true);
    assert(solution.isLegal() == true);

    cout << "solucion encontrada:" << endl;
    solution.print();

    cout << "Test de la clase BranchAndBound pasado exitosamente." << endl;
}

void runAllTests() {
    cout << "Ejecutando todos los tests..." << endl;

    testGraph();
    testBounds();
    testColoringState();
    testDangerHeuristic();
    testBranchAndBound();

    cout << "Todos los tests fueron ejecutados exitosamente." << endl;
}
