// Driver for Assignment 4 - PageRank.
//
// Usage:
//   driver_pagerank <input_file>
//
// Reads a directed, unweighted adjacency-list graph plus DAMPING /
// TOLERANCE / MAX_ITERATIONS parameters (Section 6.1 format), converts to
// CSR, runs PageRank, times only the algorithm call, prints per Section 6.3.

#include "../../assignment_01/src/csr.h"
#include "../src/pagerank.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
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
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    if (adjList.damping <= 0.0 || adjList.damping >= 1.0) {
        cerr << "Error: damping factor must satisfy 0 < d < 1.\n";
        return 1;
    }
    if (adjList.tolerance <= 0.0) {
        cerr << "Error: tolerance must be positive.\n";
        return 1;
    }
    if (adjList.maxIterations <= 0) {
        cerr << "Error: MAX_ITERATIONS must be positive.\n";
        return 1;
    }

    CSRGraph csr = adjacencyListToCSR(adjList); // CSR conversion is NOT timed

    auto t1 = chrono::high_resolution_clock::now();
    PageRankResult result = pageRank(csr, adjList.damping, adjList.tolerance, adjList.maxIterations);
    auto t2 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, std::milli>(t2 - t1).count();

    double sumRanks = 0.0;
    for (double r : result.ranks) sumRanks += r;

    cout << fixed << setprecision(6);
    cout << "Algorithm: PageRank\n";
    cout << "Damping: " << adjList.damping << "\n";
    cout << "Vertex ranks:\n";
    for (int v = 0; v < csr.V; ++v) {
        cout << v << " " << result.ranks[v] << "\n";
    }
    cout << "Sum of ranks: " << sumRanks << "\n";
    cout << "Iterations: " << result.iterations << "\n";
    cout << "Converged: " << (result.converged ? "true" : "false") << "\n";
    cout << std::defaultfloat;
    cout << "Execution time: " << ms << " ms\n";

    return 0;
}