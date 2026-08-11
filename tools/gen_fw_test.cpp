
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <vector>
using namespace std;

static const long long NO_EDGE = LLONG_MIN; // internal sentinel for "no direct edge yet"

static void writeMatrix(const string& outPath, int V, const vector<vector<long long>>& mat) {
    ofstream out(outPath);
    if (!out.is_open()) {
        cerr << "Error: could not open output file: " << outPath << "\n";
        exit(1);
    }
    out << V << "\n";
    for (int i = 0; i < V; ++i) {
        for (int j = 0; j < V; ++j) {
            if (mat[i][j] == NO_EDGE) out << "INF";
            else out << mat[i][j];
            if (j + 1 < V) out << " ";
        }
        out << "\n";
    }
}

static int genStandalone(int V, int avgDegree, const string& outPath, unsigned seed) {
    mt19937 rng(seed);

    vector<int> order(V);
    iota(order.begin(), order.end(), 0);
    shuffle(order.begin(), order.end(), rng);

    vector<vector<long long>> mat(V, vector<long long>(V, NO_EDGE));
    for (int i = 0; i < V; ++i) mat[i][i] = 0;

    uniform_int_distribution<int> weightDist(-10, 20);
    uniform_int_distribution<int> posDist(0, V - 1);

    long long targetEdges = (long long)V * avgDegree;
    set<pair<int, int>> usedPositions;
    long long added = 0, attempts = 0;
    long long maxAttempts = targetEdges * 20 + 1000;

    while (added < targetEdges && attempts < maxAttempts && V > 1) {
        int i = posDist(rng), j = posDist(rng);
        ++attempts;
        if (i >= j) continue;
        if (usedPositions.count({i, j})) continue;
        usedPositions.insert({i, j});
        int u = order[i], v = order[j];
        mat[u][v] = weightDist(rng);
        ++added;
    }

    for (int i = 0; i + 1 < V; ++i) {
        if (usedPositions.count({i, i + 1})) continue;
        int u = order[i], v = order[i + 1];
        mat[u][v] = weightDist(rng);
        usedPositions.insert({i, i + 1});
        ++added;
    }

    writeMatrix(outPath, V, mat);
    cout << "Wrote " << outPath << ": V=" << V << " (dense matrix, DAG construction, guaranteed cycle-free)\n";
    return 0;
}

static int genFromBF(const string& bfPath, const string& outPath) {
    ifstream in(bfPath);
    if (!in.is_open()) {
        cerr << "Error: could not open input file: " << bfPath << "\n";
        return 1;
    }
    int V;
    long long E;
    if (!(in >> V >> E)) {
        cerr << "Error: malformed Bellman-Ford header in " << bfPath << "\n";
        return 1;
    }

    vector<vector<long long>> mat(V, vector<long long>(V, NO_EDGE));
    for (int i = 0; i < V; ++i) mat[i][i] = 0;

    for (int line = 0; line < V; ++line) {
        int u, degree;
        if (!(in >> u >> degree)) {
            cerr << "Error: malformed adjacency line in " << bfPath << "\n";
            return 1;
        }
        for (int d = 0; d < degree; ++d) {
            int v;
            long long w;
            if (!(in >> v >> w)) {
                cerr << "Error: malformed neighbor/weight in " << bfPath << "\n";
                return 1;
            }
            if (u == v) continue; // diagonal is always 0 by spec, ignore any self-loop
            if (mat[u][v] == NO_EDGE || w < mat[u][v]) mat[u][v] = w; // take min on duplicates
        }
    }

    writeMatrix(outPath, V, mat);
    cout << "Wrote " << outPath << ": V=" << V << " (converted from " << bfPath << " for cross-check)\n";
    return 0;
}

int main(int argc, char** argv) {
    if (argc >= 2 && string(argv[1]) == "--from-bf") {
        if (argc < 4) {
            cerr << "Usage: " << argv[0] << " --from-bf <bf_input_file> <output_file>\n";
            return 1;
        }
        return genFromBF(argv[2], argv[3]);
    }

    if (argc < 4) {
        cerr << "Usage:\n"
             << "  " << argv[0] << " <V> <avg_out_degree> <output_file> [seed]\n"
             << "  " << argv[0] << " --from-bf <bf_input_file> <output_file>\n";
        return 1;
    }

    int V = stoi(argv[1]);
    int avgDegree = stoi(argv[2]);
    string outPath = argv[3];
    unsigned seed = (argc > 4) ? (unsigned)stoul(argv[4]) : 42u;

    return genStandalone(V, avgDegree, outPath, seed);
}