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
    vector<string> args(argv + 1, argv + argc);

    bool weighted = false;
    auto wIt = find(args.begin(), args.end(), "--weighted");
    if (wIt != args.end()) {
        weighted = true;
        args.erase(wIt);
    }

    if (args.size() < 3) {
        cerr << "Usage: " << argv[0]
                  << " <V> <avg_degree> <output_file> [--weighted] [seed]\n";
        return 1;
    }

    int V = stoi(args[0]);
    int avgDegree = stoi(args[1]);
    string outPath = args[2];
    unsigned seed = (args.size() > 3) ? (unsigned)stoul(args[3]) : 42u;

    mt19937 rng(seed);
    vector<set<int>> adj(V);

    // Spanning path so every vertex is reachable from 0.
    vector<int> order(V);
    for (int i = 0; i < V; ++i) order[i] = i;
    shuffle(order.begin(), order.end(), rng);
    // Force 0 to be first so SOURCE 0 can reach everything.
    order.erase(remove(order.begin(), order.end(), 0), order.end());
    order.insert(order.begin(), 0);

    for (int i = 0; i + 1 < V; ++i) {
        int a = order[i], b = order[i + 1];
        adj[a].insert(b);
        adj[b].insert(a);
    }

    long long targetEdges = (long long)V * avgDegree / 2;
    long long currentEdges = V - 1;
    long long attempts = 0;
    long long maxAttempts = targetEdges * 20 + 1000;

    uniform_int_distribution<int> vertexDist(0, V - 1);

    while (currentEdges < targetEdges && attempts < maxAttempts) {
        int a = vertexDist(rng);
        int b = vertexDist(rng);
        ++attempts;
        if (a == b || adj[a].count(b)) continue;
        adj[a].insert(b);
        adj[b].insert(a);
        ++currentEdges;
    }

    long long E = currentEdges;

    ofstream out(outPath);
    if (!out.is_open()) {
        cerr << "Error: could not open output file: " << outPath << endl;
        return 1;
    }

    uniform_int_distribution<int> weightDist(1, 20);

    out << V << " " << E << "\n";
    for (int u = 0; u < V; ++u) {
        vector<int> nbrs(adj[u].begin(), adj[u].end()); // set -> already sorted
        out << u << " " << nbrs.size();
        for (int n : nbrs) {
            out << " " << n;
            if (weighted) out << " " << weightDist(rng);
        }
        out << endl;
    }
    out << "SOURCE 0" << endl;

    cout << "Wrote " << outPath << ": V=" << V << ", E=" << E << endl;
    return 0;
}