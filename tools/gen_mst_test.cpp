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
    vector<vector<pair<int, long long>>> adj(V); // adj[u] = list of (v, weight)
    set<pair<int, int>> present;

    uniform_int_distribution<int> weightDist(1, 50); // positive weights 

    // Random spanning tree: connect each new vertex to a random earlier one.
    vector<int> order(V);
    for (int i = 0; i < V; ++i) order[i] = i;
    shuffle(order.begin(), order.end(), rng);

    for (int i = 1; i < V; ++i) {
        uniform_int_distribution<int> prevDist(0, i - 1);
        int a = order[prevDist(rng)];
        int b = order[i];
        long long w = weightDist(rng);
        adj[a].push_back({b, w});
        adj[b].push_back({a, w});
        present.insert({min(a, b), max(a, b)});
    }

    long long targetEdges = (long long)V * avgDegree / 2;
    long long currentEdges = V - 1;
    long long attempts = 0;
    long long maxAttempts = targetEdges * 20 + 1000;
    uniform_int_distribution<int> vertexDist(0, V - 1);

    while (currentEdges < targetEdges && attempts < maxAttempts) {
        int a = vertexDist(rng), b = vertexDist(rng);
        ++attempts;
        if (a == b) continue;
        auto key = make_pair(min(a, b), max(a, b));
        if (present.count(key)) continue;
        present.insert(key);
        long long w = weightDist(rng);
        adj[a].push_back({b, w});
        adj[b].push_back({a, w});
        ++currentEdges;
    }

    long long E = currentEdges;

    ofstream out(outPath);
    if (!out.is_open()) {
        cerr << "Error: could not open output file: " << outPath << "\n";
        return 1;
    }

    out << V << " " << E << "\n";
    for (int u = 0; u < V; ++u) {
        out << u << " " << adj[u].size();
        for (auto& e : adj[u]) out << " " << e.first << " " << e.second;
        out << "\n";
    }

    cout << "Wrote " << outPath << ": V=" << V << ", E=" << E
         << " (connected via random spanning tree + extra edges)\n";
    return 0;
}