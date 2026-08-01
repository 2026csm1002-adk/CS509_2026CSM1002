#include "../src/csr.h"
#include <chrono>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file> [--weighted]\n";
        return 1;
    }

    string path = argv[1];
    bool weighted = false;
    for (int i = 2; i < argc; ++i) {
        if (string(argv[i]) == "--weighted") weighted = true;
    }

    AdjacencyList adjList;
    try {
        adjList = readAdjacencyList(path, weighted);
    } 
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    // File reading is not timed
    
    // Time taken to convert Adjacency list to CSR

    auto t1 = chrono::high_resolution_clock::now();
    CSRGraph csr = adjacencyListToCSR(adjList);
    auto t2 = chrono::high_resolution_clock::now();
    double convMs = chrono::duration<double, milli>(t2 - t1).count();

    cout << "Algorithm: Adjacency-List to CSR Conversion" << endl;
    cout << "Vertices (V): " << csr.V << endl;
    cout << "Stored directed arcs (E_csr): " << csr.E << endl;
    cout << "Weighted: " << (csr.weighted ? "yes" : "no") << endl;

    int previewCount = csr.V < 10 ? csr.V : 10;
    cout << "row_ptr (first " << previewCount + 1 << "): ";
    for (int i = 0; i <= previewCount; ++i) cout << csr.row_ptr[i] << ' ';
    cout << endl;

    long long previewEdges = csr.E < 20 ? csr.E : 20;
    cout << "col_idx (first " << previewEdges << "): ";
    for (long long i = 0; i < previewEdges; ++i) cout << csr.col_idx[i] << ' ';
    cout << endl;

    if (csr.weighted) {
        cout << "values (first " << previewEdges << "): ";
        for (long long i = 0; i < previewEdges; ++i) cout << csr.values[i] << ' ';
        cout << endl;
    }

    cout << "Conversion time: " << convMs << " ms" << endl;

    return 0;
}
