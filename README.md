# CS509 Laboratory Repository 

## Repository Overview

This repository contains the implementations and test cases for all individual lab assignments completed as part of the CS509 course.

## Student Details

- Name: Aditya Kumar
- Entry No.: 2026CSM1002
- Mode for this repository: Individual


## Language and Environment

- Language: C++
- Compiler: g++.exe (Rev3, Built by MSYS2 project) 13.2.0
- OS: Windows 11
- Compile flags used: `-O2 -std=c++17`
- Timing method: `chrono::high_resolution_clock`, wall-clock duration in
  milliseconds

## Directory Structure

```
CS509_2026CSM1002/
|-- README.md
|-- common_wrapper/
|   `-- wrapper.cpp              <- repo-level menu (build/run either driver)
|-- assignment_01/
|   |-- src/
|   |   |-- gemm.h / gemm.cpp    <- GEMM simple + blocking implementations
|   |   `-- csr.h  / csr.cpp     <- adjacency-list reader + CSR conversion
|   |-- driver/
|   |   |-- driver_gemm.cpp      <- GEMM driver
|   |   `-- driver_csr.cpp       <- CSR conversion driver
|   |-- tests/
|   |   |-- gemm/                <- gemm_test_01.txt ...
|   |   `-- csr/                 <- csr_10.txt ... csr_100000.txt
|   `-- outputs/                 <- (optional) saved run logs
`-- tools/
    |-- gen_gemm_test.cpp        <- generates random GEMM test files
    `-- gen_graph.cpp            <- generates random graph test files
```

## Common Wrapper: Build and Usage
Build (from the repository root):
```powershell
cd common_wrapper
g++ -O2 -std=c++17 -o wrapper.exe wrapper.cpp
cd ..
```
Run (from the repository root, not from inside common_wrapper):
```powershell
.\common_wrapper\wrapper.exe
```

It lists the available algorithms (GEMM, CSR conversion), compiles the chosen
driver on demand, and lets you run either a single test file or every test
file for the given assignment.

## Timing Conventions
- The timer starts immediately before the algorithm call and stops
  immediately after it returns.
- File reading, input parsing, memory allocation for inputs, and result
  printing are excluded from the reported time.
- For the CSR driver, the "algorithm" being timed is the conversion step
  itself — that is the deliverable for this task.
- All times below are in milliseconds (ms).

---

## Assignment 01 — GEMM (Simple + Blocking) and CSR Graph Implementation

### Assignment Mode: Single

Implement GEMM using both a direct triple-nested-loop ("simple") approach and a cache-blocked ("blocking") approach, along with adjacency-list to CSR (Compressed Sparse Row) graph conversion.

### Algorithm / Approach
**GEMM Simple**: direct `i-k-j` loop order, `C[i][j] += A[i][k] * B[k][j]`.

**GEMM Blocking**: the M, K, N dimensions are divided into `blockSize` tiles
(default 32); the same `i-k-j` loop runs within each tile so the sub-blocks
of A, B, C accessed by the inner loops stay resident in cache longer.


**CSR Conversion**: `row_ptr` is computed as a prefix sum of per-vertex
degrees; a single pass then writes each vertex's neighbours (and weights, if
weighted) into flat `col_idx` / `values` arrays at the offsets given by
`row_ptr`. O(V + E) time.


### Input Format

#### GEMM

```text
M K N
<Matrix A (M × K)>
<Matrix B (K × N)>
```

#### Graph

```text
V E
u1 v1
u2 v2
...
```

Weighted graph:

```text
V E
u1 v1 w1
u2 v2 w2
...
```

### File Structure
- `assignment_01/src/gemm.h`, `gemm.cpp` — GEMM implementations
- `assignment_01/src/csr.h`, `csr.cpp` — adjacency-list reader + CSR conversion
- `assignment_01/driver/driver_gemm.cpp`, `driver_csr.cpp` — drivers
- `assignment_01/tests/gemm/`, `tests/csr/` — test files
- `tools/gen_gemm_test.cpp`, `tools/gen_graph.cpp` — test-file generators

### Compilation
```powershell
cd assignment_01
g++ -O2 -std=c++17 -o driver\driver_gemm.exe driver\driver_gemm.cpp src\gemm.cpp
g++ -O2 -std=c++17 -o driver\driver_csr.exe driver\driver_csr.cpp src\csr.cpp
```

### Execution
```powershell
.\driver\driver_gemm.exe tests\gemm\gemm_test_01.txt [block_size]
.\driver\driver_csr.exe tests\csr\csr_10.txt
.\driver\driver_csr.exe tests\csr\csr_10_weighted.txt --weighted
```

### 9.1 GEMM Results Table

| Test File | Input Size (M K N) | Simple Time (ms) | Blocking Time (ms) | Block Size | Status |
|------------------|-----------|-----------|-----------|----|-------------|
| gemm_test_01.txt | [2 3 2]   | 0.0006 | 0.0003 | 32 | ✅ Pass |
| gemm_test_02.txt | [64 64 64] | 0.2146 | 0.2093 | 32 | ✅ Pass |
| gemm_test_03.txt | [256 256 256] | 13.557 | 20.9001| 32 | ✅ Pass |

### 9.2 CSR Conversion Results Table


| Test File | Vertices (V) | Edges (E) | Weighted | Conversion Time (ms) | Status |
|-----------|--------------|-----------|----------|-----------------|--------|
| csr_10.txt | 10 | 20 | No | 0.0018 |✅ Pass |
| csr_100.txt | 100 | 300 | No | 0.0038 | ✅ Pass |
| csr_10000.txt | 10000 | 30000 | No | 0.4508 | ✅ Pass |
| csr_50000.txt | 50000 | 150000 | No | 2.5442 | ✅ Pass |
| csr_100000.txt | 100000 | 300000 | No | 4.9595 | ✅ Pass |
| csr_10_weighted.txt | 10 | 20 | Yes | 0.002 | ✅ Pass |

### Complexity
### Complexity

| Algorithm | Time Complexity | Space Complexity |
|-----------|-----------------|------------------|
| GEMM (Simple) | O(M × K × N) | O(M × N) |
| GEMM (Blocking) | O(M × K × N) | O(M × N) |
| CSR Conversion | O(V + E) | O(V + E) |
