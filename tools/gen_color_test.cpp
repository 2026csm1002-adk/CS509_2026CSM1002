// Generates a random undirected, unweighted graph for Vertex Coloring
// testing, in the Assignment 4 Section 5.1 format. No self-loops.
//
// Usage:
//   gen_color_test <V> <avg_degree> <output_file> [seed]

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>
using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <V> <avg_degree> <output_file> [seed]\n";
        return 1;
    }

    int V = stoi(argv[1]);
    int avgDegree = stoi(argv[2]);
    string outPath = argv[3];
    unsigned seed = (argc > 4) ? (unsigned)stoul(argv[4]) : 42u;

    mt19937 rng(seed);
    vector<vector<int>> adj(V);
    set<pair<int, int>> present;

    long long targetEdges = (long long)V * avgDegree / 2;
    long long added = 0, attempts = 0, maxAttempts = targetEdges * 20 + 1000;
    uniform_int_distribution<int> vertexDist(0, V - 1);

    while (added < targetEdges && attempts < maxAttempts) {
        int a = vertexDist(rng), b = vertexDist(rng);
        ++attempts;
        if (a == b) continue; // no self-loops
        auto key = make_pair(min(a, b), max(a, b));
        if (present.count(key)) continue; // no parallel edges
        present.insert(key);
        adj[a].push_back(b);
        adj[b].push_back(a);
        ++added;
    }

    long long E = added;

    ofstream out(outPath);
    if (!out.is_open()) {
        cerr << "Error: could not open output file: " << outPath << "\n";
        return 1;
    }

    out << V << " " << E << "\n";
    for (int u = 0; u < V; ++u) {
        out << u << " " << adj[u].size();
        for (int v : adj[u]) out << " " << v;
        out << "\n";
    }

    cout << "Wrote " << outPath << ": V=" << V << ", E=" << E << "\n";
    return 0;
}