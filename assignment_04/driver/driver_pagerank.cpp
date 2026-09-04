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

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }
    std::string path = argv[1];

    AdjacencyList adjList;
    try {
        adjList = readAdjacencyList(path, /*weighted=*/false);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    if (adjList.damping <= 0.0 || adjList.damping >= 1.0) {
        std::cerr << "Error: damping factor must satisfy 0 < d < 1.\n";
        return 1;
    }
    if (adjList.tolerance <= 0.0) {
        std::cerr << "Error: tolerance must be positive.\n";
        return 1;
    }
    if (adjList.maxIterations <= 0) {
        std::cerr << "Error: MAX_ITERATIONS must be positive.\n";
        return 1;
    }

    CSRGraph csr = adjacencyListToCSR(adjList); // CSR conversion is NOT timed

    auto t1 = std::chrono::high_resolution_clock::now();
    PageRankResult result = pageRank(csr, adjList.damping, adjList.tolerance, adjList.maxIterations);
    auto t2 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    double sumRanks = 0.0;
    for (double r : result.ranks) sumRanks += r;

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Algorithm: PageRank\n";
    std::cout << "Damping: " << adjList.damping << "\n";
    std::cout << "Vertex ranks:\n";
    for (int v = 0; v < csr.V; ++v) {
        std::cout << v << " " << result.ranks[v] << "\n";
    }
    std::cout << "Sum of ranks: " << sumRanks << "\n";
    std::cout << "Iterations: " << result.iterations << "\n";
    std::cout << "Converged: " << (result.converged ? "true" : "false") << "\n";
    std::cout << std::defaultfloat;
    std::cout << "Execution time: " << ms << " ms\n";

    return 0;
}