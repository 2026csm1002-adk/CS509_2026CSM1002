
// Usage:
//   gen_bf_test <V> <avg_out_degree> <output_file> [seed]

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <vector>
using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <V> <avg_out_degree> <output_file> [seed]\n";
        return 1;
    }

    int V = stoi(argv[1]);
    int avgDegree = stoi(argv[2]);
    string outPath = argv[3];
    unsigned seed = (argc > 4) ? (unsigned)stoul(argv[4]) : 42u;

    mt19937 rng(seed);

    // Random topological order: order[i] = the vertex at rank i.
    vector<int> order(V);
    iota(order.begin(), order.end(), 0);
    shuffle(order.begin(), order.end(), rng);

    vector<vector<pair<int, long long>>> adj(V); // adj[u] = list of (v, weight)

    uniform_int_distribution<int> weightDist(-10, 20);
    uniform_int_distribution<int> posDist(0, V - 1);

    long long targetEdges = (long long)V * avgDegree;
    set<pair<int, int>> usedPositions; // (rank_i, rank_j) pairs already used
    long long added = 0;
    long long attempts = 0;
    long long maxAttempts = targetEdges * 20 + 1000;

    while (added < targetEdges && attempts < maxAttempts && V > 1) {
        int i = posDist(rng), j = posDist(rng);
        ++attempts;
        if (i >= j) continue; // only forward edges (i < j in topological order)
        if (usedPositions.count({i, j})) continue;
        usedPositions.insert({i, j});
        int u = order[i], v = order[j];
        adj[u].push_back({v, weightDist(rng)});
        ++added;
    }

    // Guarantee a connected chain along the topological order so a
    // reasonable fraction of vertices are reachable from the source.
    for (int i = 0; i + 1 < V; ++i) {
        if (usedPositions.count({i, i + 1})) continue;
        int u = order[i], v = order[i + 1];
        adj[u].push_back({v, weightDist(rng)});
        usedPositions.insert({i, i + 1});
        ++added;
    }

    long long E = added;
    int source = order[0]; // topologically first vertex: can reach the whole chain

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
    out << "SOURCE " << source << "\n";

    cout << "Wrote " << outPath << ": V=" << V << ", E=" << E << ", source=" << source
         << " (DAG construction, guaranteed cycle-free)\n";
    return 0;
}