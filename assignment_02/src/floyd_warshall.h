#ifndef FLOYD_WARSHALL_H
#define FLOYD_WARSHALL_H

#include <vector>
using namespace std;

constexpr long long FW_INF = 1LL << 50;

struct FloydWarshallResult{
    vector<vector<long long>>dist;
    bool negativeCycle = false;
};

FloydWarshallResult floydWarshall(vector<vector<long long>>dist);

#endif