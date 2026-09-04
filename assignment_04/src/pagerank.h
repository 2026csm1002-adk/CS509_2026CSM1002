#ifndef PAGERANK_H
#define PAGERANK_H

#include <vector>
#include "../../assignment_01/src/csr.h"
using namespace std;

struct PageRankResult {
    vector<double> ranks;
    int iterations = 0;
    bool converged = false;
};

PageRankResult pageRank(const CSRGraph& csr, double damping, double tolerance, int maxIterations);

#endif