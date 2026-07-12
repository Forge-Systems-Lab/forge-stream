# forge-stream v0.2

A zero-dependency, non-allocating string ingestion engine core written in pure systems C. Optimized for robust stream chunk processing of unstandardized financial flat-files and multi-tenant ledger rows.

## 🏢 1. The Expensive Business Problem
Modern columnar analytical database layers (e.g., ClickHouse, DuckDB, Apache Arrow) utilize rigid, high-throughput text tokenizers to maximize parallel ingestion velocity. However, multi-tenant ERP configurations and accounting software routinely receive unstandardized transaction streams from legacy third-party suppliers. 

Variations in formatting—specifically unquoted fractional timestamps and localized currency delimiters (e.g., mixing `1,250.00` and `1.250,00`) within the same column matrices—cause naive parsers to trigger fatal column alignment shifts. This results in single-threaded scalar execution fallbacks, silent data corruption, or catastrophic transaction stream leaks inside automated bookkeeping ledgers.

## ⚙️ 2. Current Engineering Capabilities (v0.2 Core)
* **Single-Pass Scalar Processing:** Performs zero-copy field extraction across continuous byte streams using reentrant double-pointer token metrics.
* **Collection Boundary Insulation:** Implements an internal non-backtracking character lookahead state tracking loop. It isolates hidden commas encapsulated within array brackets (`[...]`) or quotes (`""`), preventing false field-slicing and data drift.
* **Zero Heap Allocations:** Operates entirely within pre-allocated local stack footprints and 64KB chunk buffers (`fread`), bypassing the memory overhead and performance degradation of standard library allocation routines (`malloc`/`realloc`).

## 📐 3. Technical Architecture Map
Raw Data Ingestion (64KB Chunk Streams via fread)
│
▼
[ BUFFER TRAVERSAL LOOP ]
│
├───► Read Byte ──► Check Character Token State
│                      │
│                      ├──► Bracket Hit ('[') ──► Toggle in_array = 1
│                      ├──► Bracket Hit (']') ──► Toggle in_array = 0
│                      └──► Comma Hit (',') ────► If !in_array -> Flush Field
▼
Stateless Callback Interface (RecordHandler Pipeline Execution)


## 🛠️ 4. Build, Verification, & Regression Harness
The system contains a differential regression testing suite (`run_tests.sh`) that cross-validates data ingestion passes against an immutable reference ledger baseline to verify parsing correctness under hostile formatting conditions.

```bash
# Clean binary artifacts and trigger optimized compilation
make clean
make CC=gcc CFLAGS="-Wall -Wextra -O3 -Isrc"

# Execute full differential verification suite
./run_tests.sh
🛑 5. Current Structural Limitations
Scalar Constraints: The current processing loop operates via optimized scalar pointer increments. It protects parallel engine streams from stalling but does not natively leverage hardware-level SIMD vector instructions yet.

Boundary Caps: Constrained by a hard-coded layout footprint limit (MAX_FIELDS 16) and fixed line buffer thresholds (line_buffer[1024]).

Static Offsets: Field target destination maps inside src/main.c are hardcoded to specific indices, requiring identical structural layout sequences.

🗺️ 6. System Horizon Roadmap
Phase 1 (Next): Implement a Dynamic Variable Pointer Indexing Schema to map changing file layouts to runtime data structures via pointer offset blocks.

Phase 2: Introduce compile-time macro overrides to remove hardcoded field allocation caps dynamically.

Phase 3: Engineer explicit hardware acceleration vector extensions (SIMD) to parallelize token scanning streams.

📜 7. License
MIT Open Source Primitives. Developed under Forge Systems Lab Infrastructure.
