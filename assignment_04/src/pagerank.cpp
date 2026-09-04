#include "pagerank.h"
#include <cmath>

using namespace std;
PageRankResult pageRank(const CSRGraph& csr, double damping, double tolerance, int maxIterations) {
    int N = csr.V;
    vector<double> rank(N, N > 0 ? 1.0 / N : 0.0);
    vector<long long> outdeg(N);
    for (int u = 0; u < N; ++u) outdeg[u] = csr.row_ptr[u + 1] - csr.row_ptr[u];

    PageRankResult result;

    for (int it = 1; it <= maxIterations; ++it) {
        double danglingSum = 0.0;
        for (int u = 0; u < N; ++u) if (outdeg[u] == 0) danglingSum += rank[u];

        double base = (1.0 - damping) / N + damping * danglingSum / N;
        vector<double> newRank(N, base);

        for (int u = 0; u < N; ++u) {
            if (outdeg[u] == 0) continue;
            double contribution = damping * rank[u] / (double)outdeg[u];
            for (long long e = csr.row_ptr[u]; e < csr.row_ptr[u + 1]; ++e) {
                newRank[csr.col_idx[e]] += contribution;
            }
        }

        double totalChange = 0.0;
        for (int v = 0; v < N; ++v) totalChange += abs(newRank[v] - rank[v]);

        rank = move(newRank);
        result.iterations = it;

        if (totalChange <= tolerance) { result.converged = true; break; }
    }

    result.ranks = move(rank);
    return result;
}