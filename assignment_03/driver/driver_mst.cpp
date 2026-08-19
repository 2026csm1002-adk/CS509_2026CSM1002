// Usage:
//   driver_mst <input_file>
//

#include "../../assignment_01/src/csr.h"
#include "../src/mst.h"
#include <chrono>
#include <iostream>
#include <string>

using namespace std;

static void printMSTResult(const string & label, const MSTResult &result, double ms){
    cout << "Algorithm" << label << "\n";
    cout << "MST edges:\n";
    
    for(const auto &e: result.edges){
        cout << e.u << " " << e.v << " " << e.weight << endl;
    }

    cout << "Total MST weight: " << result.totalWeight << endl;
    cout << "Execution time: " << ms << " ms" << endl; 
}

int main(int argc, char**argv){
    if(argc < 2){
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }
    string path = argv[1];

    AdjacencyList adjList;

    try{
        adjList = readAdjacencyList(path, true);    // (path, weighted)
    }
    catch(const exception &e){
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    CSRGraph csr = adjacencyListToCSR(adjList);

    auto t1 = chrono::high_resolution_clock::now();
    MSTResult kruskalResult = kruskalMST(csr);
    auto t2 = chrono::high_resolution_clock::now();

    double kruskalMs = chrono::duration<double, milli>(t2 - t1).count();

    auto t3 = chrono::high_resolution_clock::now();
    MSTResult primResult = primMST(csr, 0);
    auto t4 = chrono::high_resolution_clock::now();

    double primMs = chrono::duration<double, milli>(t4 - t3).count();


    printMSTResult("Kruskal's MST", kruskalResult, kruskalMs);
    cout << endl;
    printMSTResult("Prim's MST", primResult, primMs);

    if(kruskalResult.totalWeight != primResult.totalWeight){
        cerr << "Warning: Kruskal and Prim total weights disagree" << kruskalResult.totalWeight << " vs " << primResult.totalWeight << endl;

    }
    return 0;
}