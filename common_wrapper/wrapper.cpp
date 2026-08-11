// Common Wrapper — repository-level menu for running the submitted
// assignments.
// Menu flow: Assignment -> Algorithm -> (pick a test file by number | Run ALL)
//
// Build (from the repository root):
//   g++ -O2 -std=c++17 -o common_wrapper/wrapper.exe common_wrapper/wrapper.cpp assignment_01/src/gemm.cpp assignment_01/src/csr.cpp assignment_02/src/bellman_ford.cpp assignment_02/src/floyd_warshall.cpp
//
// Run (from the repository root):
//   .\common_wrapper\wrapper.exe

#include "../assignment_01/src/gemm.h"
#include "../assignment_01/src/csr.h"
#include "../assignment_02/src/bellman_ford.h"
#include "../assignment_02/src/floyd_warshall.h"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;
namespace fs = filesystem;

// ---------------------------------------------------------------------
// GEMM: reads a matrix file, calls gemmSimple/gemmBlocking, times each,
// prints results. Mirrors driver_gemm.cpp.
// ---------------------------------------------------------------------
static Matrix readMatrixGemm(ifstream& in, int rows, int cols) {
    Matrix m(rows, cols);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (!(in >> m.at(i, j))) {
                throw runtime_error("Malformed matrix data in input file.");
            }
        }
    }
    return m;
}

static void printMatrix(const Matrix& m) {
    for (int i = 0; i < m.rows; ++i) {
        for (int j = 0; j < m.cols; ++j) {
            cout << m.at(i, j);
            if (j + 1 < m.cols) cout << ' ';
        }
        cout << "\n";
    }
}

static void runGemmOnFile(const string& path, int blockSize) {
    ifstream in(path);
    if (!in.is_open()) {
        cout << "Error: could not open input file: " << path << "\n";
        return;
    }

    int M, K, N;
    if (!(in >> M >> K >> N)) {
        cout << "Error: malformed header, expected 'M K N'.\n";
        return;
    }

    Matrix A, B;
    try {
        A = readMatrixGemm(in, M, K);
        B = readMatrixGemm(in, K, N);
    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
        return;
    }
    // --- setup / parsing above this line is NOT timed ---

    auto t1 = chrono::high_resolution_clock::now();
    Matrix Csimple = gemmSimple(A, B);
    auto t2 = chrono::high_resolution_clock::now();
    double simpleMs = chrono::duration<double, milli>(t2 - t1).count();

    auto t3 = chrono::high_resolution_clock::now();
    Matrix Cblock = gemmBlocking(A, B, blockSize);
    auto t4 = chrono::high_resolution_clock::now();
    double blockMs = chrono::duration<double, milli>(t4 - t3).count();

    cout << "Algorithm: GEMM Simple\n";
    cout << "Result matrix:\n";
    printMatrix(Csimple);
    cout << "Execution time: " << simpleMs << " ms\n\n";

    cout << "Algorithm: GEMM Blocking (block size = " << blockSize << ")\n";
    cout << "Result matrix:\n";
    printMatrix(Cblock);
    cout << "Execution time: " << blockMs << " ms\n";
}

// ---------------------------------------------------------------------
// CSR: reads a graph file, calls adjacencyListToCSR, times it, prints
// row_ptr / col_idx / (values). Mirrors driver_csr.cpp.
// ---------------------------------------------------------------------
static void runCsrOnFile(const string& path, bool weighted) {
    AdjacencyList adjList;
    try {
        adjList = readAdjacencyList(path, weighted);
    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
        return;
    }
    // --- file reading / parsing above this line is NOT timed ---

    auto t1 = chrono::high_resolution_clock::now();
    CSRGraph csr = adjacencyListToCSR(adjList);
    auto t2 = chrono::high_resolution_clock::now();
    double convMs = chrono::duration<double, milli>(t2 - t1).count();

    cout << "Algorithm: Adjacency-List to CSR Conversion\n";
    cout << "Vertices (V): " << csr.V << "\n";
    cout << "Stored directed arcs (E_csr): " << csr.E << "\n";
    cout << "Weighted: " << (csr.weighted ? "yes" : "no") << "\n";

    int previewCount = csr.V < 10 ? csr.V : 10;
    cout << "row_ptr (first " << previewCount + 1 << "): ";
    for (int i = 0; i <= previewCount; ++i) cout << csr.row_ptr[i] << ' ';
    cout << endl;

    long long previewEdges = csr.E < 20 ? csr.E : 20;
    cout << "col_idx (first " << previewEdges << "): ";
    for (long long i = 0; i < previewEdges; ++i) cout << csr.col_idx[i] << ' ';
    cout << endl;

    if (csr.weighted) {
        cout << "values (first " << previewEdges << "): ";
        for (long long i = 0; i < previewEdges; ++i) cout << csr.values[i] << ' ';
        cout << endl;
    }

    cout << "Conversion time: " << convMs << " ms\n";
}

// ---------------------------------------------------------------------
// Bellman-Ford: reads a directed weighted adjacency-list file (weights may
// be negative), converts to CSR via Assignment 1's shared helper
// ---------------------------------------------------------------------
static void runBfOnFile(const string& path) {
    AdjacencyList adjList;
    try {
        adjList = readAdjacencyList(path, /*weighted=*/true);
    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
        return;
    }

    CSRGraph csr = adjacencyListToCSR(adjList); // CSR conversion is NOT timed
    int source = adjList.source;

    auto t1 = chrono::high_resolution_clock::now();
    BellmanFordResult result = bellmanFord(csr, source);
    auto t2 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(t2 - t1).count();

    cout << "Algorithm: Bellman-Ford\n";
    cout << "Source: " << source << "\n";

    if (result.negativeCycle) {
        cout << "Negative cycle: true\n";
    } else {
        cout << "Vertex Distance\n";
        for (int v = 0; v < csr.V; ++v) {
            cout << v << " ";
            if (result.dist[v] >= BF_INF) cout << "INF";
            else cout << result.dist[v];
            cout << "\n";
        }
        cout << "Negative cycle: none\n";
    }

    cout << "Execution time: " << ms << " ms\n";
}

// ---------------------------------------------------------------------
// Floyd-Warshall: reads a dense V x V matrix file(NO CSR Involved).
// ---------------------------------------------------------------------
static vector<vector<long long>> readMatrixFw(const string& path, int& V) {
    ifstream in(path);
    if (!in.is_open()) {
        throw runtime_error("Could not open input file: " + path);
    }
    if (!(in >> V)) {
        throw runtime_error("Malformed header, expected 'V' on the first line.");
    }

    vector<vector<long long>> mat(V, vector<long long>(V));
    for (int i = 0; i < V; ++i) {
        for (int j = 0; j < V; ++j) {
            string tok;
            if (!(in >> tok)) {
                throw runtime_error("Malformed matrix data (expected " + to_string(V) +
                                     " values per row).");
            }
            if (tok == "INF") mat[i][j] = FW_INF;
            else mat[i][j] = stoll(tok);
        }
    }
    return mat;
}

static void runFwOnFile(const string& path) {
    int V = 0;
    vector<vector<long long>> mat;
    try {
        mat = readMatrixFw(path, V);
    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
        return;
    }
    // --- matrix loading above this line is NOT timed ---

    auto t1 = chrono::high_resolution_clock::now();
    FloydWarshallResult result = floydWarshall(mat);
    auto t2 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(t2 - t1).count();

    cout << "Algorithm: Floyd-Warshall\n";

    if (result.negativeCycle) {
        cout << "Negative cycle: true\n";
    } else {
        cout << "Distance matrix:\n";
        for (int i = 0; i < V; ++i) {
            for (int j = 0; j < V; ++j) {
                if (result.dist[i][j] >= FW_INF) cout << "INF";
                else cout << result.dist[i][j];
                if (j + 1 < V) cout << " ";
            }
            cout << "\n";
        }
        cout << "Negative cycle: none\n";
    }

    cout << "Execution time: " << ms << " ms\n";
}

// ---------------------------------------------------------------------
// Menu structure: Assignment -> Algorithm -> (single test file | all).
// New assignments/algorithms just get appended to buildCatalog() below.
// ---------------------------------------------------------------------
struct Algorithm {
    string label;
    string testsDir;                       // relative to the repo root (cwd when running the wrapper)
    function<void(const string&)> runOne;  // runs the algorithm on one test file path
};

struct AssignmentMenu {
    string label;
    vector<Algorithm> algorithms;
};

static vector<AssignmentMenu> buildCatalog() {
    auto gemmRunner = [](const string& path) { runGemmOnFile(path, 32); };
    auto csrRunner = [](const string& path) {
        // For weighted file e.g. csr_10_weighted.txt
        bool weighted = path.find("_weighted.txt") != string::npos;
        runCsrOnFile(path, weighted);
    };
    auto bfRunner = [](const string& path) { runBfOnFile(path); };
    auto fwRunner = [](const string& path) { runFwOnFile(path); };

    return {
        {"Assignment 1", {
            {"GEMM (Simple + Blocking)", "assignment_01/tests/gemm", gemmRunner},
            {"CSR Graph Conversion",     "assignment_01/tests/csr",  csrRunner},
        }},
        {"Assignment 2", {
            {"Bellman-Ford",   "assignment_02/tests/bf", bfRunner},
            {"Floyd-Warshall", "assignment_02/tests/fw", fwRunner},
        }},
    };
}

// Lists .txt files in algo.testsDir, lets the user pick one (or "all"),
// and runs algo.runOne on the selected file(s).
static void listAndRun(const Algorithm& algo) {
    if (!fs::exists(algo.testsDir)) {
        cerr << "Error: tests directory not found: " << algo.testsDir << endl;
        return;
    }

    vector<fs::path> files;
    for (const auto& entry : fs::directory_iterator(algo.testsDir)) {
        if (entry.path().extension() == ".txt") files.push_back(entry.path());
    }
    sort(files.begin(), files.end());

    if (files.empty()) {
        cerr << "No .txt test files found in " << algo.testsDir << endl;
        return;
    }

    cout << "\nTest files in " << algo.testsDir << ":\n";
    for (int i = 0; i < files.size(); ++i) {
        cout << "  " << (i + 1) << ") " << files[i].filename().string() << "\n";
    }
    int allOption = files.size() + 1;
    cout << "  " << allOption << ") Run ALL test files\n";
    cout << "Select an option (number): ";

    int sel;
    if (!(cin >> sel) || sel < 1 || sel > allOption) {
        cerr << "Error: invalid selection.\n";
        return;
    }

    if (sel == allOption) {
        for (const auto& f : files) {
            cout << "\n--- Running on " << f.string() << " ---\n";
            algo.runOne(f.string());
        }
    } else {
        const auto& f = files[sel - 1];
        cout << "\n--- Running on " << f.string() << " ---\n";
        algo.runOne(f.string());
    }
}

int main() {
    auto catalog = buildCatalog();

    cout << "=== CS509 Common Wrapper ===\n";
    cout << "Available assignments:\n";
    for (int i = 0; i < catalog.size(); ++i) {
        cout << "  " << (i + 1) << ") " << catalog[i].label << "\n";
    }
    cout << "Select an option (number): ";

    int aSel;
    if (!(cin >> aSel) || aSel < 1 || aSel > catalog.size()) {
        cerr << "Error: invalid selection.\n";
        return 1;
    }
    const AssignmentMenu& assignment = catalog[aSel - 1];

    cout << "\nAvailable algorithms in " << assignment.label << ":\n";
    for (int i = 0; i < assignment.algorithms.size(); ++i) {
        cout << "  " << (i + 1) << ") " << assignment.algorithms[i].label << "\n";
    }
    cout << "Select an option (number): ";

    int algSel;
    if (!(cin >> algSel) || algSel < 1 || algSel > assignment.algorithms.size()) {
        cerr << "Error: invalid selection.\n";
        return 1;
    }
    const Algorithm& algo = assignment.algorithms[algSel - 1];

    listAndRun(algo);
    return 0;
}