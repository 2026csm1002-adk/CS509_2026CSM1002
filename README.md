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
g++ -O2 -std=c++17 -o common_wrapper\wrapper.exe common_wrapper\wrapper.cpp assignment_01\src\gemm.cpp assignment_01\src\csr.cpp
cd ..
```
```powershell
g++ -O2 -std=c++17 -o common_wrapper\wrapper.exe common_wrapper\wrapper.cpp assignment_01\src\gemm.cpp assignment_01\src\csr.cpp assignment_02\src\bellman_ford.cpp assignment_02\src\floyd_warshall.cpp
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

| Algorithm | Time Complexity | Space Complexity |
|-----------|-----------------|------------------|
| GEMM (Simple) | O(M × K × N) | O(M × N) |
| GEMM (Blocking) | O(M × K × N) | O(M × N) |
| CSR Conversion | O(V + E) | O(V + E) |


---

## Assignment 02 — Bellman-Ford and Floyd-Warshall

### Assignment Mode: Single

Implement Bellman-Ford (single-source shortest path, tolerates negative edge
weights, detects negative-weight cycles) and Floyd-Warshall (all-pairs
shortest path via dynamic programming), reusing Assignment 01's CSR
conversion helper for Bellman-Ford rather than duplicating it.

### Algorithm / Approach

**Bellman-Ford**: relaxes every edge in the graph, repeated V-1 times. A
final extra pass checks whether any edge can still be relaxed — if so, a
negative-weight cycle reachable from the source exists and distances are
reported as undefined.

**Floyd-Warshall**: dynamic-programming approach. For every intermediate
vertex `k`, updates `dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])`
for every pair `(i, j)`. After completion, the diagonal is checked — a
negative value at `dist[i][i]` indicates a negative-weight cycle.

### Input Format

#### Bellman-Ford (directed, weights may be negative)
```text
V E
u0 degree neighbor1 weight1 neighbor2 weight2 ...
...
u(V-1) degree neighbor1 weight1 neighbor2 weight2 ...
SOURCE s
```

#### Floyd-Warshall (dense V×V matrix)
```text
V
row 0 values
row 1 values
...
row (V-1) values
```
`INF` marks a pair with no direct edge; entry `(i, i)` must be `0`.

### File Structure
- `assignment_02/src/bellman_ford.h`, `bellman_ford.cpp` — Bellman-Ford implementation
- `assignment_02/src/floyd_warshall.h`, `floyd_warshall.cpp` — Floyd-Warshall implementation
- `assignment_02/driver/driver_bf.cpp` — reuses `assignment_01/src/csr.h`/`csr.cpp` for CSR conversion (not duplicated)
- `assignment_02/driver/driver_fw.cpp` — reads the dense matrix directly, no CSR involved
- `assignment_02/tests/bf/`, `tests/fw/` — test files
- `tools/gen_bf_test.cpp`, `tools/gen_fw_test.cpp` — test-file generators (DAG-based construction guarantees no accidental negative-weight cycles)

### Compilation
```powershell
cd assignment_02
g++ -O2 -std=c++17 -o driver\driver_bf.exe driver\driver_bf.cpp src\bellman_ford.cpp ..\assignment_01\src\csr.cpp
g++ -O2 -std=c++17 -o driver\driver_fw.exe driver\driver_fw.cpp src\floyd_warshall.cpp
```

### Execution
```powershell
.\driver\driver_bf.exe tests\bf\bf_10.txt
.\driver\driver_fw.exe tests\fw\fw_10.txt
```

### 11.1 Bellman-Ford / Floyd-Warshall Results Table

| Algorithm | Test File | Vertices | Edges | Source | Negative Cycle | Time (ms) | Status |
|---|---|---|---|---|---|---|---|
| Bellman-Ford | bf_10.txt | 10 | 33 | 9 | No | 0.0026 | ✅ Pass |
| Bellman-Ford | bf_100.txt | 100 | 489 | 5 | No | 0.0905 | ✅ Pass |
| Bellman-Ford | bf_10000.txt | 10000 | 49981 | 2881 | No | [FILL IN] | ✅ Pass |
| Bellman-Ford | bf_50000.txt | 50000 | 199995 | 22858 | No | [FILL IN] | ✅ Pass |
| Bellman-Ford | bf_100000.txt | 100000 | 399997 | 77221 | No | [FILL IN] | ✅ Pass |
| Bellman-Ford | bf_negcycle_01.txt | 3 | 3 | 0 | Yes | [FILL IN] | ✅ Pass |
| Floyd-Warshall | fw_10.txt | 10 | — | N/A | No | 0.0084 | ✅ Pass |
| Floyd-Warshall | fw_100.txt | 100 | — | N/A | No | 0.7635 | ✅ Pass |
| Floyd-Warshall | fw_500.txt | 500 | — | N/A | No | [FILL IN] | ✅ Pass |
| Floyd-Warshall | fw_1000.txt | 1000 | — | N/A | No | [FILL IN] | ✅ Pass |
| Floyd-Warshall | fw_2000.txt | 2000 | — | N/A | No | [FILL IN] | ✅ Pass |
| Floyd-Warshall | fw_negcycle_01.txt | 3 | — | N/A | Yes | [FILL IN] | ✅ Pass |

### Cross-Check: Bellman-Ford vs Floyd-Warshall (V=10, V=100)

Per the assignment spec, the same underlying graph was used for both
algorithms at V=10 and V=100 so their outputs could be directly compared.

- **V=10** (source = 9): Bellman-Ford's distance vector
  `[4, -12, -4, -21, -30, -11, -29, -21, -31, 0]` exactly matches row 9 of
  the Floyd-Warshall distance matrix. ✅ Match confirmed (all 10 vertices).
- **V=100** (source = 5): Bellman-Ford's distance vector exactly matches
  row 5 of the Floyd-Warshall distance matrix. ✅ Match confirmed (all 100
  vertices).

### Complexity

| Algorithm | Time Complexity | Space Complexity |
|---|---|---|
| Bellman-Ford | O(V × E) | O(V) |
| Floyd-Warshall | O(V³) | O(V²) |

### Notes
- Bellman-Ford's two largest graphs (50,000 and 100,000 vertices) were kept
  sparse (E ≈ 2V–4V) per the assignment spec's guidance, since Bellman-Ford's
  O(V·E) complexity makes dense large graphs impractical.
- Floyd-Warshall is exempt from CSR conversion; it reads a dense adjacency
  matrix directly, per the assignment spec.
- All Bellman-Ford and Floyd-Warshall test graphs (other than the
  deliberate negative-cycle cases) were generated as random DAGs, which
  guarantees no negative-weight cycle can occur by construction.

#### Links: [Assignment(Personal)](https://github.com/2026csm1002-adk/CS509_2026CSM1002)
