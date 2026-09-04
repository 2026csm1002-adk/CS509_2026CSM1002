// Common Wrapper — repository-level menu for running the submitted
// assignments. Does NOT shell out via system(); it links directly against
// each assignment's source and calls the algorithm functions in-process.
//
// Menu flow: Assignment -> Algorithm -> (pick a test file by number | Run ALL)
//
// Build (from the repository root):
//   g++ -O2 -std=c++17 -o common_wrapper/wrapper.exe common_wrapper/wrapper.cpp assignment_01/src/gemm.cpp assignment_01/src/csr.cpp assignment_02/src/bellman_ford.cpp assignment_02/src/floyd_warshall.cpp assignment_03/src/mst.cpp assignment_04/src/vertex_coloring.cpp assignment_04/src/pagerank.cpp
//
// Run (from the repository root):
//   .\common_wrapper\wrapper.exe

#include "../assignment_01/src/gemm.h"
#include "../assignment_01/src/csr.h"
#include "../assignment_02/src/bellman_ford.h"
#include "../assignment_02/src/floyd_warshall.h"
#include "../assignment_03/src/mst.h"
#include "../assignment_04/src/vertex_coloring.h"
#include "../assignment_04/src/pagerank.h"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;
namespace fs = filesystem;

// ---------------------------------------------------------------------
// GEMM
// ---------------------------------------------------------------------
static Matrix readMatrixGemm(ifstream& in, int rows, int cols) {
    Matrix m(rows, cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            if (!(in >> m.at(i, j))) throw runtime_error("Malformed matrix data in input file.");
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
    if (!in.is_open()) { cout << "Error: could not open input file: " << path << "\n"; return; }
    int M, K, N;
    if (!(in >> M >> K >> N)) { cout << "Error: malformed header, expected 'M K N'.\n"; return; }
    Matrix A, B;
    try {
        A = readMatrixGemm(in, M, K);
        B = readMatrixGemm(in, K, N);
    } catch (const exception& e) { cout << "Error: " << e.what() << "\n"; return; }

    auto t1 = chrono::high_resolution_clock::now();
    Matrix Csimple = gemmSimple(A, B);
    auto t2 = chrono::high_resolution_clock::now();
    double simpleMs = chrono::duration<double, milli>(t2 - t1).count();

    auto t3 = chrono::high_resolution_clock::now();
    Matrix Cblock = gemmBlocking(A, B, blockSize);
    auto t4 = chrono::high_resolution_clock::now();
    double blockMs = chrono::duration<double, milli>(t4 - t3).count();

    cout << "Algorithm: GEMM Simple\nResult matrix:\n";
    printMatrix(Csimple);
    cout << "Execution time: " << simpleMs << " ms\n\n";
    cout << "Algorithm: GEMM Blocking (block size = " << blockSize << ")\nResult matrix:\n";
    printMatrix(Cblock);
    cout << "Execution time: " << blockMs << " ms\n";
}

// ---------------------------------------------------------------------
// CSR
// ---------------------------------------------------------------------
static void runCsrOnFile(const string& path, bool weighted) {
    AdjacencyList adjList;
    try { adjList = readAdjacencyList(path, weighted); }
    catch (const exception& e) { cout << "Error: " << e.what() << "\n"; return; }

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
    cout << "\n";
    long long previewEdges = csr.E < 20 ? csr.E : 20;
    cout << "col_idx (first " << previewEdges << "): ";
    for (long long i = 0; i < previewEdges; ++i) cout << csr.col_idx[i] << ' ';
    cout << "\n";
    if (csr.weighted) {
        cout << "values (first " << previewEdges << "): ";
        for (long long i = 0; i < previewEdges; ++i) cout << csr.values[i] << ' ';
        cout << "\n";
    }
    cout << "Conversion time: " << convMs << " ms\n";
}

// ---------------------------------------------------------------------
// Bellman-Ford
// ---------------------------------------------------------------------
static void runBfOnFile(const string& path) {
    AdjacencyList adjList;
    try { adjList = readAdjacencyList(path, true); }
    catch (const exception& e) { cout << "Error: " << e.what() << "\n"; return; }

    CSRGraph csr = adjacencyListToCSR(adjList);
    int source = adjList.source;

    auto t1 = chrono::high_resolution_clock::now();
    BellmanFordResult result = bellmanFord(csr, source);
    auto t2 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(t2 - t1).count();

    cout << "Algorithm: Bellman-Ford\nSource: " << source << "\n";
    if (result.negativeCycle) {
        cout << "Negative cycle: true\n";
    } else {
        cout << "Vertex Distance\n";
        for (int v = 0; v < csr.V; ++v) {
            cout << v << " ";
            if (result.dist[v] >= BF_INF) cout << "INF"; else cout << result.dist[v];
            cout << "\n";
        }
        cout << "Negative cycle: none\n";
    }
    cout << "Execution time: " << ms << " ms\n";
}

// ---------------------------------------------------------------------
// Floyd-Warshall
// ---------------------------------------------------------------------
static vector<vector<long long>> readMatrixFw(const string& path, int& V) {
    ifstream in(path);
    if (!in.is_open()) throw runtime_error("Could not open input file: " + path);
    if (!(in >> V)) throw runtime_error("Malformed header, expected 'V' on the first line.");
    vector<vector<long long>> mat(V, vector<long long>(V));
    for (int i = 0; i < V; ++i)
        for (int j = 0; j < V; ++j) {
            string tok;
            if (!(in >> tok)) throw runtime_error("Malformed matrix data.");
            if (tok == "INF") mat[i][j] = FW_INF; else mat[i][j] = stoll(tok);
        }
    return mat;
}

static void runFwOnFile(const string& path) {
    int V = 0;
    vector<vector<long long>> mat;
    try { mat = readMatrixFw(path, V); }
    catch (const exception& e) { cout << "Error: " << e.what() << "\n"; return; }

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
                if (result.dist[i][j] >= FW_INF) cout << "INF"; else cout << result.dist[i][j];
                if (j + 1 < V) cout << " ";
            }
            cout << "\n";
        }
        cout << "Negative cycle: none\n";
    }
    cout << "Execution time: " << ms << " ms\n";
}

// ---------------------------------------------------------------------
// MST
// ---------------------------------------------------------------------
static void printMSTResult(const string& label, const MSTResult& result, double ms) {
    cout << "Algorithm: " << label << "\nMST edges:\n";
    for (const auto& e : result.edges) cout << e.u << " " << e.v << " " << e.weight << "\n";
    cout << "Total MST weight: " << result.totalWeight << "\n";
    cout << "Execution time: " << ms << " ms\n";
}

static void runMstOnFile(const string& path) {
    AdjacencyList adjList;
    try { adjList = readAdjacencyList(path, true); }
    catch (const exception& e) { cout << "Error: " << e.what() << "\n"; return; }

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
    cout << "\n";
    printMSTResult("Prim's MST", primResult, primMs);

    if (kruskalResult.totalWeight != primResult.totalWeight) {
        cerr << "Warning: Kruskal and Prim total weights disagree ("
             << kruskalResult.totalWeight << " vs " << primResult.totalWeight << ")\n";
    }
}

// ---------------------------------------------------------------------
// Vertex Coloring
// ---------------------------------------------------------------------
static void runVcOnFile(const string& path) {
    AdjacencyList adjList;
    try { adjList = readAdjacencyList(path, false); }
    catch (const exception& e) { cout << "Error: " << e.what() << "\n"; return; }

    CSRGraph csr = adjacencyListToCSR(adjList);

    if (hasSelfLoop(csr)) {
        cout << "Error: input graph contains a self-loop, which is invalid for Vertex Coloring.\n";
        return;
    }

    auto t1 = chrono::high_resolution_clock::now();
    VertexColoringResult result = vertexColoring(csr);
    auto t2 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(t2 - t1).count();

    bool valid = isValidColoring(csr, result.color);

    cout << "Algorithm: Greedy Vertex Coloring\nVertex colors:\n";
    for (int v = 0; v < csr.V; ++v) cout << v << " " << result.color[v] << "\n";
    cout << "Colors used: " << result.colorsUsed << "\n";
    if (!valid) cerr << "Warning: produced coloring is INVALID.\n";
    cout << "Execution time: " << ms << " ms\n";
}

// ---------------------------------------------------------------------
// PageRank
// ---------------------------------------------------------------------
static void runPagerankOnFile(const string& path) {
    AdjacencyList adjList;
    try { adjList = readAdjacencyList(path, false); }
    catch (const exception& e) { cout << "Error: " << e.what() << "\n"; return; }

    if (adjList.damping <= 0.0 || adjList.damping >= 1.0) { cout << "Error: damping factor must satisfy 0 < d < 1.\n"; return; }
    if (adjList.tolerance <= 0.0) { cout << "Error: tolerance must be positive.\n"; return; }
    if (adjList.maxIterations <= 0) { cout << "Error: MAX_ITERATIONS must be positive.\n"; return; }

    CSRGraph csr = adjacencyListToCSR(adjList);

    auto t1 = chrono::high_resolution_clock::now();
    PageRankResult result = pageRank(csr, adjList.damping, adjList.tolerance, adjList.maxIterations);
    auto t2 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(t2 - t1).count();

    double sumRanks = 0.0;
    for (double r : result.ranks) sumRanks += r;

    cout << fixed << setprecision(6);
    cout << "Algorithm: PageRank\nDamping: " << adjList.damping << "\nVertex ranks:\n";
    for (int v = 0; v < csr.V; ++v) cout << v << " " << result.ranks[v] << "\n";
    cout << "Sum of ranks: " << sumRanks << "\n";
    cout << "Iterations: " << result.iterations << "\n";
    cout << "Converged: " << (result.converged ? "true" : "false") << "\n";
    cout << defaultfloat;
    cout << "Execution time: " << ms << " ms\n";
}

// ---------------------------------------------------------------------
// Menu structure
// ---------------------------------------------------------------------
struct Algorithm {
    string label;
    string testsDir;
    function<void(const string&)> runOne;
};

struct AssignmentMenu {
    string label;
    vector<Algorithm> algorithms;
};

static vector<AssignmentMenu> buildCatalog() {
    auto gemmRunner = [](const string& path) { runGemmOnFile(path, 32); };
    auto csrRunner = [](const string& path) {
        bool weighted = path.find("_weighted.txt") != string::npos;
        runCsrOnFile(path, weighted);
    };
    auto bfRunner = [](const string& path) { runBfOnFile(path); };
    auto fwRunner = [](const string& path) { runFwOnFile(path); };
    auto mstRunner = [](const string& path) { runMstOnFile(path); };
    auto vcRunner = [](const string& path) { runVcOnFile(path); };
    auto prRunner = [](const string& path) { runPagerankOnFile(path); };

    return {
        {"Assignment 1", {
            {"GEMM (Simple + Blocking)", "assignment_01/tests/gemm", gemmRunner},
            {"CSR Graph Conversion",     "assignment_01/tests/csr",  csrRunner},
        }},
        {"Assignment 2", {
            {"Bellman-Ford",   "assignment_02/tests/bf", bfRunner},
            {"Floyd-Warshall", "assignment_02/tests/fw", fwRunner},
        }},
        {"Assignment 3", {
            {"MST (Kruskal + Prim)", "assignment_03/tests/mst", mstRunner},
        }},
        {"Assignment 4", {
            {"Vertex Coloring", "assignment_04/tests/color",    vcRunner},
            {"PageRank",        "assignment_04/tests/pagerank", prRunner},
        }},
    };
}

static void listAndRun(const Algorithm& algo) {
    if (!fs::exists(algo.testsDir)) {
        cerr << "Error: tests directory not found: " << algo.testsDir << "\n";
        return;
    }

    vector<fs::path> files;
    for (const auto& entry : fs::directory_iterator(algo.testsDir))
        if (entry.path().extension() == ".txt") files.push_back(entry.path());
    sort(files.begin(), files.end());

    if (files.empty()) { cerr << "No .txt test files found in " << algo.testsDir << "\n"; return; }

    cout << "\nTest files in " << algo.testsDir << ":\n";
    for (size_t i = 0; i < files.size(); ++i) cout << "  " << (i + 1) << ") " << files[i].filename().string() << "\n";
    size_t allOption = files.size() + 1;
    cout << "  " << allOption << ") Run ALL test files\n";
    cout << "Select an option (number): ";

    size_t sel;
    if (!(cin >> sel) || sel < 1 || sel > allOption) { cerr << "Error: invalid selection.\n"; return; }

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

    cout << "=== CS509 Common Wrapper ===\nAvailable assignments:\n";
    for (size_t i = 0; i < catalog.size(); ++i) cout << "  " << (i + 1) << ") " << catalog[i].label << "\n";
    cout << "Select an option (number): ";

    size_t aSel;
    if (!(cin >> aSel) || aSel < 1 || aSel > catalog.size()) { cerr << "Error: invalid selection.\n"; return 1; }
    const AssignmentMenu& assignment = catalog[aSel - 1];

    cout << "\nAvailable algorithms in " << assignment.label << ":\n";
    for (size_t i = 0; i < assignment.algorithms.size(); ++i) cout << "  " << (i + 1) << ") " << assignment.algorithms[i].label << "\n";
    cout << "Select an option (number): ";

    size_t algSel;
    if (!(cin >> algSel) || algSel < 1 || algSel > assignment.algorithms.size()) { cerr << "Error: invalid selection.\n"; return 1; }
    const Algorithm& algo = assignment.algorithms[algSel - 1];

    listAndRun(algo);
    return 0;
}