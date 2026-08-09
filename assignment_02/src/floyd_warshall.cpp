#include "floyd_warshall.h"

FloydWarshallResult floydWarshall(vector<vector<long long>>dist){
    int V = dist.size();
    
    for(int k = 0; k < V; ++k){
        for(int i = 0; i < V; ++i){
            if(dist[i][k] >= FW_INF) continue;      // No path exists between i and k
            for(int j = 0; j < V; ++j){
                if(dist[k][j] >= FW_INF) continue;  // No path exists between k and j
                long long dist_k =  dist[i][k] + dist[k][j];    // Path through k
                if (dist_k < dist[i][j]) dist[i][j] = dist_k; 
            }
        }
    }

    FloydWarshallResult result;

    result.dist = dist;

    for(int i = 0; i < V; ++i){
        if(dist[i][i] < 0) result.negativeCycle = true;
        break;
    }
    return result;
}