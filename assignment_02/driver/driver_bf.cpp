// Reads a directed, weighted adjacency-list graph, converts into CSR with the help of assignment 1
#include "../../assignment_01/src/csr.h"
#include "../src/bellman_ford.h"
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
        adjList = readAdjacencyList(path, true); // Second parameter is weighted part
    }
    catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    
    CSRGraph csr = adjacencyListToCSR(adjList);
    int source = adjList.source;
    
    auto t1 = chrono::high_resolution_clock::now();
    BellmanFordResult result = bellmanFord(csr, source); 
    auto t2 = chrono::high_resolution_clock::now();

    double ms = chrono::duration<double, milli>(t2 - t1).count();

    cout << "Algorithm: Bellman-Ford" << endl;
    cout << "Source: " << source << endl;

    if (result.negativeCycle) {
        cout << "Negative cycle: true" << endl;
    }
    else {
        cout << "Vertex Distance" << endl;
        for (int v = 0; v < csr.V; ++v) {
            cout << v << " ";
            if (result.dist[v] >= BF_INF) cout << "INF";
            else cout << result.dist[v];
            cout << endl;
        }
        cout << "Negative cycle: none" << endl;
    }

    cout << "Execution time: " << ms << " ms" << endl;
    return 0;
}