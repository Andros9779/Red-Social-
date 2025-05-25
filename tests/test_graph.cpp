/**
 * @file test_graph.cpp
 * @brief Unit tests for Graph class: checks edge addition, symmetry, degree, and component counting.
 */
#include <cassert>
#include "../include/graph.h"

/**
 * @brief Executes unit tests to verify the Graph implementation.
 * 
 * Tests symmetry of edges, node degrees, and connected component count.
 * @return 0 on success.
 */
int main() {
    Graph g;

    // Componente 1: triángulo 1-2-3
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 1);

    // Componente 2: arista 10-11
    g.addEdge(10, 11);

    // -------- Symmetry tests ------------
    assert(g.areFriends(1, 2));
    assert(g.areFriends(2, 1));
    assert(!g.areFriends(1, 10));

    // -------- Degree tests --------------
    assert(g.degree(1) == 2);
    assert(g.degree(10) == 1);

    // -------- Component count tests -----
    // Esperamos 2 componentes conexas: {1,2,3} y {10,11}
    assert(g.bfsComponentCount() == 2);

    return 0; // éxito
}
