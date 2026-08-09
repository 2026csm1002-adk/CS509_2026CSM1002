#include "csr.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
using namespace std;

AdjacencyList readAdjacencyList(const string& path, bool weighted) {

    // Error Handling while reading the file
    ifstream in(path);
    if (!in.is_open()) {
        throw runtime_error("Could not open input file: " + path);
    }

    AdjacencyList result;
    result.weighted = weighted;

    long long V, E;
    if (!(in >> V >> E)) {
        throw runtime_error("Malformed header (expected: V E) in file: " + path);
    }
    result.V = (int)V;
    result.E = E;
    result.adj.assign(result.V, {});

    for (int line = 0; line < result.V; ++line) {
        int u, degree;
        if (!(in >> u >> degree)) {
            throw runtime_error("Malformed adjacency line in file: " + path);
        }
        if (u < 0 || u >= result.V) {
            throw runtime_error("Vertex id out of range in file: " + path);
        }
        result.adj[u].reserve(degree);
        for (int d = 0; d < degree; ++d) {
            int nbr;
            if (!(in >> nbr)) {
                throw runtime_error("Malformed neighbor list in file: " + path);
            }
            double w = 1.0;
            if (weighted) {
                if (!(in >> w)) {
                    throw runtime_error("Expected edge weight in file: " + path);
                }
                // if (w <= 0.0) {
                //     throw runtime_error("Edge weights must be positive in file: " + path);
                // } Removed due to assignment 2
            }
            result.adj[u].push_back({nbr, w});
        }
    }

    string tok;
    if (in >> tok) {
        if (tok == "SOURCE") {
            in >> result.source;
        }
    }

    return result;
}


CSRGraph adjacencyListToCSR(const AdjacencyList& adjList) {
    CSRGraph csr;
    csr.V = adjList.V;
    csr.weighted = adjList.weighted;

    csr.row_ptr.assign(csr.V + 1, 0);
    for (int u = 0; u < csr.V; ++u) {
        csr.row_ptr[u + 1] = csr.row_ptr[u] + (long long)adjList.adj[u].size();
    }
    csr.E = csr.row_ptr[csr.V];

    csr.col_idx.assign(csr.E, 0);
    if (csr.weighted) csr.values.assign(csr.E, 0.0);

    for (int u = 0; u < csr.V; ++u) {
        long long base = csr.row_ptr[u];
        const auto& list = adjList.adj[u];
        for (size_t k = 0; k < list.size(); ++k) {
            csr.col_idx[base + k] = list[k].neighbor;
            if (csr.weighted) csr.values[base + k] = list[k].weight;
        }
    }

    return csr;
}