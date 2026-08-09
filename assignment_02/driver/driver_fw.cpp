#include "../src/floyd_warshall.h"
#include <fstream>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

static vector<vector<long long>> readMatrix(const string &path, int &V){
    ifstream in(path);
    if(!in.is_open()) throw runtime_error("Could not open input file: " + path);

    if(!(in >> V))throw runtime_error("Malformed header(Expected V)");

    vector<vector<long long>>mat(V, vector<long long>(V));

    for(int i = 0; i < V; ++i){
        for(int j = 0; j < V; ++j){
            string tok;
            if(!(in >> tok)) throw runtime_error("Malformed Matrix Data(expected " + to_string(V) + " values per row).");

            if(tok == "INF") mat[i][j] = FW_INF;
            else mat[i][j] = stoll(tok);
        }
    }

    return mat;
}

int main(int argc, char **argv){
    if(argc < 2){
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }
    string path = argv[1];

    int V = 0;
    vector<vector<long long>>mat;

    try{
        mat = readMatrix(path, V);
    }
    catch(const exception &e){
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    auto t1 = chrono::high_resolution_clock::now();
    FloydWarshallResult result = floydWarshall(mat);
    auto t2 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(t2 - t1).count();

    cout << "Algorithm: Floyd-Warshall\n";

    if (result.negativeCycle) {
        cout << "Negative cycle: true" << endl;
    } else {
        cout << "Distance matrix:" << endl;
        for (int i = 0; i < V; ++i) {
            for (int j = 0; j < V; ++j) {
                if (result.dist[i][j] >= FW_INF) cout << "INF";
                else cout << result.dist[i][j];
                if (j + 1 < V) cout << " ";
            }
            cout << endl;
        }
        cout << "Negative cycle: none" << endl;
    }

    cout << "Execution time: " << ms << " ms" << endl;
    return 0;
}