// Driver for Assignment 4 - Vertex Coloring.
//
// Usage:
//   driver_vc <input_file>
//
// Reads an unweighted, undirected adjacency-list graph (Section 5.1
// format), converts to CSR, runs greedy Welsh-Powell coloring, times only
// the algorithm call, prints per Section 5.3.

#include "../../assignment_01/src/csr.h"
#include "../src/vertex_coloring.h"
#include <chrono>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }
    string path = argv[1];

    AdjacencyList adjList;
    try {
        adjList = readAdjacencyList(path, /*weighted=*/false);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    CSRGraph csr = adjacencyListToCSR(adjList); // CSR conversion is NOT timed

    if (hasSelfLoop(csr)) {
        std::cerr << "Error: input graph contains a self-loop, which is invalid for Vertex Coloring.\n";
        return 1;
    }

    auto t1 = chrono::high_resolution_clock::now();
    VertexColoringResult result = vertexColoring(csr);
    auto t2 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, std::milli>(t2 - t1).count();

    bool valid = isValidColoring(csr, result.color);

    cout << "Algorithm: Greedy Vertex Coloring\n";
    cout << "Vertex colors:\n";
    for (int v = 0; v < csr.V; ++v) {
        cout << v << " " << result.color[v] << "\n";
    }
    cout << "Colors used: " << result.colorsUsed << "\n";
    if (!valid) {
        cerr << "Warning: produced coloring is INVALID (an adjacent pair shares a color).\n";
    }
    cout << "Execution time: " << ms << " ms\n";

    return 0;
}