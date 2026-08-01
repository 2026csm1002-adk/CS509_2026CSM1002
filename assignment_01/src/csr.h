#ifndef CSR_H
#define CSR_H

#include <vector>
#include <string>
using namespace std;

// Compressed Sparse Row Representation of Graph
struct CSRGraph {
    int V = 0;
    long long E = 0; // number of directed edges stored (= sum of degrees written in file)
    vector<long long> row_ptr;
    vector<int> col_idx;
    vector<double> values; // empty if unweighted
    bool weighted = false;
};

// Adjacent list entry for adjacency list
struct AdjEntry {
    int neighbor;
    double weight;
};

struct AdjacencyList{
    int V = 0;
    long long E = 0; // number of directed edges stored
    int source = 0;
    bool weighted = false;
    vector<vector<AdjEntry>> adj; // Each Adjacency entry will store the list of AdjEntry
};

// Reads the adjacency list graph in file format
AdjacencyList readAdjacencyList(const string& path, bool weighted);

// Converts the adjacency list graph into file format
CSRGraph adjacencyListToCSR(const AdjacencyList& adjList);

#endif::