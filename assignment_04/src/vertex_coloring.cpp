#include "vertex_coloring.h"
#include <algorithm>
#include <numeric>
using namespace std;
VertexColoringResult greedyVertexColoring(const CSRGraph& csr) {
    int V = csr.V;
    VertexColoringResult result;
    result.color.assign(V, -1);

    vector<int> order(V);
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](int a, int b) {
        long long degA = csr.row_ptr[a + 1] - csr.row_ptr[a];
        long long degB = csr.row_ptr[b + 1] - csr.row_ptr[b];
        return degA > degB;
    });

    int maxColorSeen = -1;
    vector<bool> usedByNeighbour;

    for (int u : order) {
        usedByNeighbour.assign(maxColorSeen + 2, false);
        for (long long e = csr.row_ptr[u]; e < csr.row_ptr[u + 1]; ++e) {
            int nbr = csr.col_idx[e];
            int c = result.color[nbr];
            if (c >= 0 && c < (int)usedByNeighbour.size()) usedByNeighbour[c] = true;
        }

        int chosen = 0;
        while (chosen < (int)usedByNeighbour.size() && usedByNeighbour[chosen]) ++chosen;

        result.color[u] = chosen;
        if (chosen > maxColorSeen) maxColorSeen = chosen;
    }

    result.colorsUsed = maxColorSeen + 1;
    return result;
}

bool isValidColoring(const CSRGraph& csr, const std::vector<int>& color) {
    for (int u = 0; u < csr.V; ++u)
        for (long long e = csr.row_ptr[u]; e < csr.row_ptr[u + 1]; ++e)
            if (color[u] == color[csr.col_idx[e]]) return false;
    return true;
}

bool hasSelfLoop(const CSRGraph& csr) {
    for (int u = 0; u < csr.V; ++u)
        for (long long e = csr.row_ptr[u]; e < csr.row_ptr[u + 1]; ++e)
            if (csr.col_idx[e] == u) return true;
    return false;
}