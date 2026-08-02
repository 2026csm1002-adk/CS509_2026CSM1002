#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
using namespace std;

int main(int argc, char** argv) {
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " <M> <K> <N> <output_file> [seed]" << endl;
        return 1;
    }

    int M = stoi(argv[1]);
    int K = stoi(argv[2]);
    int N = stoi(argv[3]);
    string outPath = argv[4];
    unsigned seed = (argc > 5) ? (unsigned)stoul(argv[5]) : 42u;

    ofstream out(outPath);
    if (!out.is_open()) {
        cerr << "Error: could not open output file: " << outPath << endl;
        return 1;
    }

    mt19937 rng(seed);
    uniform_int_distribution<int> dist(-10, 10);

    out << M << " " << K << " " << N << endl;

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < K; ++j) {
            out << dist(rng);
            if (j + 1 < K) out << ' ';
        }
        out << "\n";
    }
    for (int i = 0; i < K; ++i) {
        for (int j = 0; j < N; ++j) {
            out << dist(rng);
            if (j + 1 < N) out << ' ';
        }
        out << "\n";
    }

    cout << "Wrote " << outPath << ": M=" << M << " K=" << K << " N=" << N << endl;
    return 0;
}
