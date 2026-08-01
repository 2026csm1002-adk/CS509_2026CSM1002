#ifndef GEMM_H
#define GEMM_H

// To prevent from Double Inclusion

#include <vector>
using namespace std;

// Flat array representation of matrix
struct Matrix{
    int rows;
    int cols;
    vector<double>data;  //ele(i, j) <- data[i*cols + j]

    Matrix(int r = 0, int c = 0): rows(r), cols(c), data((size_t)r * (size_t)c, 0.0){}

    // Functions for accessing the data
    double &at(int i, int j){
        return data[(size_t)i * cols + j];
    }

    const double &at(int i, int j) const{
        return data[(size_t)i * cols + j];
    }
};

Matrix gemmSimple(const Matrix &A, const Matrix &B); // Simple Matrix Multiplication
Matrix gemmBlocking(const Matrix &A, const Matrix &B, int blockSize); // For Matrix Multiplication using tiling

#endif