#include "gemm.h"
#include <algorithm>
using namespace std;

Matrix gemmSimple(const Matrix &A, const Matrix &B) {
    int M = A.rows, K = A.cols, N = B.cols;
    Matrix C(M, N);     // Declaring the resultant Matrix

    for (int i = 0; i < M; ++i) {
        for (int k = 0; k < K; ++k) {
            double a_ik = A.at(i, k);
            for (int j = 0; j < N; ++j) {
                C.at(i, j) += a_ik * B.at(k, j);
            }
        }
    }
    return C;
}

Matrix gemmBlocking(const Matrix &A, const Matrix &B, int blockSize) {
    int M = A.rows, K = A.cols, N = B.cols;
    Matrix C(M, N);

    if (blockSize <= 0) blockSize = 32;

    for (int ii = 0; ii < M; ii += blockSize) {
        int iMax = std::min(ii + blockSize, M);
        for (int kk = 0; kk < K; kk += blockSize) {
            int kMax = std::min(kk + blockSize, K);
            for (int jj = 0; jj < N; jj += blockSize) {
                int jMax = std::min(jj + blockSize, N);

                for (int i = ii; i < iMax; ++i) {
                    for (int k = kk; k < kMax; ++k) {
                        double a_ik = A.at(i, k);
                        for (int j = jj; j < jMax; ++j) {
                            C.at(i, j) += a_ik * B.at(k, j);
                        }
                    }
                }
            }
        }
    }
    return C;
}
