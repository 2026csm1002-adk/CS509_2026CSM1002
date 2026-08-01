#include "../src/gemm.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
using namespace std;

static Matrix readMatrix(ifstream& in, int rows, int cols) {
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
        cout << endl;
    }
}


int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file> [block_size]\n";
        return 1;
    }

    string path = argv[1];
    int blockSize = (argc >= 3) ? stoi(argv[2]) : 32;

    ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "Error: could not open input file: " << path << "\n";
        return 1;
    }

    int M, K, N;
    if (!(in >> M >> K >> N)) {
        cerr << "Error: malformed header, expected 'M K N'.\n";
        return 1;
    }

    Matrix A, B;
    try {
        A = readMatrix(in, M, K);
        B = readMatrix(in, K, N);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    // --- setup / parsing above this line is NOT timed ---

    auto t1start = chrono::high_resolution_clock::now();
    Matrix Csimple = gemmSimple(A, B);
    auto t1end = chrono::high_resolution_clock::now();
    double simpleMs = chrono::duration<double, std::milli>(t1end - t1start).count();

    auto t2start = chrono::high_resolution_clock::now();
    Matrix Cblock = gemmBlocking(A, B, blockSize);
    auto t2end = chrono::high_resolution_clock::now();
    double blockMs = chrono::duration<double, milli>(t2end - t2start).count();

    cout << "Algorithm: GEMM Simple\n";
    cout << "Result matrix:\n";
    printMatrix(Csimple);
    cout << "Execution time: " << simpleMs << " ms\n\n";

    cout << "Algorithm: GEMM Blocking (block size = " << blockSize << ")\n";
    cout << "Result matrix:\n";
    printMatrix(Cblock);
    cout << "Execution time: " << blockMs << " ms\n";

    return 0;
}
