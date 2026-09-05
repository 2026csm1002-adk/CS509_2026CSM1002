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

## Directory Structure

```
CS509_2026CSM1002/
|-- README.md
|-- common_wrapper/
|   `-- wrapper.cpp              <- repo-level menu (build/run any driver)
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
|                
|-- assignment_02/
|   |-- src/
|   |   |-- bellman_ford.h / bellman_ford.cpp     <- Bellman-Ford (reuses assignment_01's CSR)
|   |   `-- floyd_warshall.h / floyd_warshall.cpp <- Floyd-Warshall (dense matrix, no CSR)
|   |-- driver/
|   |   |-- driver_bf.cpp        <- Bellman-Ford driver
|   |   `-- driver_fw.cpp        <- Floyd-Warshall driver
|   |-- tests/
|   |   |-- bf/                  <- bf_10.txt ... bf_100000.txt
|   |   `-- fw/                  <- fw_10.txt ... fw_2000.txt
|                   
|-- assignment_03/
|   |-- src/
|   |   `-- mst.h / mst.cpp      <- Kruskal's and Prim's MST (reuses assignment_01's CSR)
|   |-- driver/
|   |   `-- driver_mst.cpp       <- MST driver (runs both algorithms on the same graph)
|   |-- tests/
|      `-- mst/                 <- mst_10.txt ... mst_100000.txt
|                   
`-- tools/
    |-- gen_gemm_test.cpp        <- generates random GEMM test files
    |-- gen_graph.cpp            <- generates random graph test files
    |-- gen_bf_test.cpp          <- generates random Bellman-Ford test files
    |-- gen_fw_test.cpp          <- generates random Floyd-Warshall test files
    `-- gen_mst_test.cpp         <- generates random connected MST test files
```

## Common Wrapper: Build and Usage
Build (from the repository root):
```powershell
cd common_wrapper
```powershell
g++ -O2 -std=c++17 -o common_wrapper\wrapper.exe common_wrapper\wrapper.cpp assignment_01\src\gemm.cpp assignment_01\src\csr.cpp assignment_02\src\bellman_ford.cpp assignment_02\src\floyd_warshall.cpp assignment_03\src\mst.cpp
```
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
| Bellman-Ford | bf_10000.txt | 10000 | 49981 | 2881 | No | 2227.14 | ✅ Pass |
| Bellman-Ford | bf_50000.txt | 50000 | 199995 | 22858 | No | 50880.2 | ✅ Pass |
| Bellman-Ford | bf_100000.txt | 100000 | 399997 | 77221 | No | 237097 | ✅ Pass |
| Bellman-Ford | bf_negcycle_01.txt | 3 | 3 | 0 | Yes | 0.0006 | ✅ Pass |
| Floyd-Warshall | fw_10.txt | 10 | — | N/A | No | 0.0084 | ✅ Pass |
| Floyd-Warshall | fw_100.txt | 100 | — | N/A | No | 0.7635 | ✅ Pass |
| Floyd-Warshall | fw_500.txt | 500 | — | N/A | No | 28.9822 | ✅ Pass |
| Floyd-Warshall | fw_1000.txt | 1000 | — | N/A | No | 221.686 | ✅ Pass |
| Floyd-Warshall | fw_2000.txt | 2000 | — | N/A | No | 1720.68 | ✅ Pass |
| Floyd-Warshall | fw_negcycle_01.txt | 3 | — | N/A | Yes | 0.0054 | ✅ Pass |

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


## Assignment 03 — Minimum Spanning Tree (Kruskal and Prim)

**Kruskal's Algorithm**: all edges are sorted by non-decreasing weight, then
processed in that order. An edge is added to the MST only if its two
endpoints are currently in different components, checked and merged via a
Disjoint Set Union (Union-Find) structure with path compression and
union-by-rank. Stops after V-1 edges have been selected.

**Prim's Algorithm**: starts from vertex 0 and grows a single tree, using a
min-priority queue keyed on edge weight. At each step, the cheapest edge
connecting the current tree to a vertex outside it is selected and that
vertex is added to the tree. Stops when all vertices are included.


### Input Format

#### MST (undirected, weighted; each edge appears at both endpoints)
```text
V E
u0 degree neighbor1 weight1 neighbor2 weight2 ...
...
u(V-1) degree neighbor1 weight1 neighbor2 weight2 ...
```
`E` counts each undirected edge once. Weights may be positive, zero, or
negative integers. The graph must be connected.

### File Structure
- `assignment_03/src/mst.h`, `mst.cpp` — Kruskal's and Prim's MST implementations
- `assignment_03/driver/driver_mst.cpp` — reuses `assignment_01/src/csr.h`/`csr.cpp` for CSR conversion (not duplicated); runs both algorithms on the same CSR graph
- `assignment_03/tests/mst/` — test files
- `tools/gen_mst_test.cpp` — test-file generator (builds a random spanning tree first to guarantee connectivity, then adds extra random edges)


### Compilation
```powershell
cd assignment_03
g++ -O2 -std=c++17 -o driver\driver_mst.exe driver\driver_mst.cpp src\mst.cpp ..\assignment_01\src\csr.cpp
```

### Execution
```powershell
.\driver\driver_mst.exe tests\mst\mst_10.txt
```

### 9.1 MST Results Table

| Test File | V | E | Kruskal Weight | Prim Weight | Kruskal Time (ms) | Prim Time (ms) | Equal? | Status |
|---|---|---|---|---|---|---|---|---|
| mst_10.txt | 10 | 15 | 150 | 150 | 0.0034| 0.0051 | Yes | ✅ Pass |
| mst_100.txt | 100 | 200 | 1735 | 1735 | 0.0291 | 0.0668 | Yes | ✅ Pass |
| mst_10000.txt | 10000 | 15000 | 189314 | 189314 | 3.0376 | 1.8295 | Yes | ✅ Pass |
| mst_50000.txt | 50000 | 75000 | 935674 | 935674 | 24.7651 | 9.409 | Yes | ✅ Pass |
| mst_100000.txt | 100000 | 150000 | 1873762 | 1873762 | 58.4767 | 17.1694 | Yes | ✅ Pass |

### Complexity

| Algorithm | Time Complexity | Space Complexity |
|---|---|---|
| Kruskal's MST | O(E log E) | O(V + E) |
| Prim's MST | O(E log V) | O(V + E) |


## Assignment 04 - PageRank and Vertex Coloring

### Algorithm / Approach

**Vertex Coloring (Welsh-Powell)**: vertices are ordered by non-increasing
degree (degree read directly from each vertex's CSR row length). Vertices
are then processed in that order; each is assigned the smallest color index
not already used by any of its already-colored neighbours.

**PageRank**: each vertex starts at rank `1/N`. Every iteration, each
vertex's current rank is pushed along its outgoing CSR edges to its
neighbours (`contribution = damping * rank[u] / outdegree(u)`), added to a
base value `(1-damping)/N` shared by all vertices. Dangling vertices
(outdegree 0) have their rank redistributed evenly across all N vertices
each iteration, instead of causing a divide-by-zero. Iteration stops when
the total absolute change across all vertices falls at or below the
tolerance, or `MAX_ITERATIONS` is reached.


### Folder Structure

```
|-- assignment_04/
|   |-- src/
|   |   |-- vertex_coloring.h / vertex_coloring.cpp  <- Greedy Welsh-Powell coloring (reuses assignment_01's CSR)
|   |   `-- pagerank.h / pagerank.cpp                <- PageRank (reuses assignment_01's CSR)
|   |-- driver/
|   |   |-- driver_vc.cpp        <- Vertex Coloring driver
|   |   `-- driver_pagerank.cpp  <- PageRank driver
|   |-- tests/
|   |   |-- color/               <- color_10.txt ... color_100000.txt
|   |   `-- pagerank/            <- pagerank_10.txt ... pagerank_50000.txt
|   `-- outputs/                 <- (optional) saved run logs
`-- tools/
    |-- gen_gemm_test.cpp
    |-- gen_graph.cpp
    |-- gen_bf_test.cpp
    |-- gen_fw_test.cpp
    |-- gen_mst_test.cpp
    |-- gen_color_test.cpp       <- generates random undirected graphs for Vertex Coloring
    `-- gen_pagerank_test.cpp    <- generates random directed graphs for PageRank

```
## Common Wrapper: Build and Usage
Build (from the repository root):
```powershell
g++ -O2 -std=c++17 -o common_wrapper\wrapper.exe common_wrapper\wrapper.cpp assignment_01\src\gemm.cpp assignment_01\src\csr.cpp assignment_02\src\bellman_ford.cpp assignment_02\src\floyd_warshall.cpp assignment_03\src\mst.cpp assignment_04\src\vertex_coloring.cpp assignment_04\src\pagerank.cpp
```
### Input Format

#### Vertex Coloring (undirected, unweighted; each edge at both endpoints)
```text
V E
u0 degree neighbor1 neighbor2 ...
...
u(V-1) degree neighbor1 neighbor2 ...
```
No self-loops permitted.

#### PageRank (directed, unweighted; outgoing edges only)
```text
V E
u0 outdegree neighbor1 neighbor2 ...
...
u(V-1) outdegree neighbor1 neighbor2 ...
DAMPING d
TOLERANCE epsilon
MAX_ITERATIONS n
```

### File Structure
- `assignment_04/src/vertex_coloring.h`, `vertex_coloring.cpp` — greedy coloring implementation
- `assignment_04/src/pagerank.h`, `pagerank.cpp` — PageRank implementation
- `assignment_04/driver/driver_vc.cpp` — reuses `assignment_01/src/csr.h`/`csr.cpp` (not duplicated); rejects self-loops before running
- `assignment_04/driver/driver_pagerank.cpp` — reuses the same shared CSR module; the adjacency-list reader was extended to also parse `DAMPING`/`TOLERANCE`/`MAX_ITERATIONS` trailing lines (in addition to the existing `SOURCE` line used by earlier assignments)
- `assignment_04/tests/color/`, `tests/pagerank/` — test files
- `tools/gen_color_test.cpp`, `tools/gen_pagerank_test.cpp` — test-file generators

### Compilation
```powershell
cd assignment_04
g++ -O2 -std=c++17 -o driver\driver_vc.exe driver\driver_vc.cpp src\vertex_coloring.cpp ..\assignment_01\src\csr.cpp
g++ -O2 -std=c++17 -o driver\driver_pagerank.exe driver\driver_pagerank.cpp src\pagerank.cpp ..\assignment_01\src\csr.cpp
```

### Execution
```powershell
.\driver\driver_vc.exe tests\color\color_10.txt
.\driver\driver_pagerank.exe tests\pagerank\pagerank_10.txt
```

### 10.1 Vertex Coloring Results Table

| File | V | E | Colors Used | Valid? | Time | Status |
|---|---|---|---|---|---|---|
| color_10.txt | 10 | 15 | 3 | Yes | 0.0033 ms | ✅ Pass |
| color_100.txt | 100 | 200 | 4 | Yes | 0.0131 ms | ✅ Pass |
| color_10000.txt | 10,000 | 15000 | 4 | Yes | 0.7813 ms | ✅ Pass |
| color_50000.txt | 50,000 | 75000 | 5 | Yes | 4.0749 ms | ✅ Pass |
| color_100000.txt | 100,000 | 150000 | 5 | Yes | 9.7767 ms | ✅ Pass |

"Valid?" was checked programmatically (`isValidColoring`) for every run — no adjacent pair shares a color at any size.


### 10.2 PageRank Results Table

| File | V | E | Damping | Top Vertex | Sum of Ranks | Iter. / Time | Status |
|---|---|---|---|---|---|---|---|
| pagerank_10.txt | 10 | 30 | 0.85 | 4 | 1.000000 | 25 / 0.0054 ms | ✅ Pass |
| pagerank_100.txt | 100 | 400 | 0.85 | 33 | 1.000000 | 18 / 0.0247 ms | ✅ Pass |
| pagerank_1000.txt | 1,000 | 4000 | 0.85 | 720 | 1.000000 | 19 / 0.2149 ms | ✅ Pass |
| pagerank_10000.txt | 10,000 | 30000 | 0.85 | 1237 | 1.000000 | 22 / 3.9772 ms | ✅ Pass |
| pagerank_50000.txt | 50,000 | 150000 | 0.85 | 44059 | 1.000000 | 22 / 24.6967 ms | ✅ Pass |

"Sum of Ranks" matched exactly `1.000000` at every size, confirming correctness of the dangling-vertex redistribution and the overall probability-mass invariant.

### Complexity

| Algorithm | Time Complexity | Space Complexity |
|---|---|---|
| Vertex Coloring (Welsh-Powell greedy) | O(V log V + E) | O(V) |
| PageRank | O((V + E) × iterations) | O(V) |

### Notes on the Assignment Specification
- **The PageRank worked example in Section 6.3 of the assignment spec appears to contain an error.** Given the exact input graph in Section 6.2 (edges `0→1, 1→2, 2→0, 2→1, 3→2`), vertex 3 has zero in-degree — nothing in the graph points to it. Under the PageRank formula stated in the same spec (Section 2.2), a vertex with no incoming edges can only receive rank from the teleportation term, forcing `PR(3) = (1-d)/N = 0.0375` exactly, in any correct implementation. This was confirmed independently in both C++ and Python. The spec's claimed value of `0.208624` for vertex 3 is not reachable under the stated formula and input — this is being reported as a documentation discrepancy, not a bug in this implementation.
- Vertex Coloring test graphs were generated via random edge sampling (no self-loops, no parallel edges); connectivity is not guaranteed but is not required by the spec.
- PageRank test graphs were generated as random directed graphs; the dangling-vertex handling was separately verified on a hand-constructed 4-vertex test case with one dangling vertex, confirming `Sum of ranks` still equals exactly 1.0.

#### Links: [Assignment(Personal)](https://github.com/2026csm1002-adk/CS509_2026CSM1002)
