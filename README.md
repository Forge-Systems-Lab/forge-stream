# forge-stream v0.2

A non-allocating, high-velocity stream processing substrate built to ingest heterogeneous enterprise ledger data layouts without memory overhead or vector lane stalls.

## 🏢 The Expensive Business Problem
Modern columnar analytical engines (e.g., ClickHouse, DuckDB, Polars) rely on optimized text tokenizers that trade flexibility for speed. When multi-tenant accounting platforms or corporate ERP synchronizers ingest flat files from unstandardized external suppliers, localized formatting variations—such as unquoted fractional timestamps or mixed currency delimiters (e.g., `1,250.00` vs. `1.250,00`)—cause parsing shifts. This results in column misalignment, single-threaded scalar fallbacks, or catastrophic transaction stream leaks.

## ⚙️ Core Capability
`forge-stream` utilizes a single-pass, reentrant state machine tokenizer in zero-dependency systems C. It intercepts and isolates collection markers (`[` `]`) and string wrappers dynamically on a single pass over a 64KB chunk buffer configuration. This allows nested data containing internal delimiters to parse cleanly into native 64-bit primitives without heap re-allocations or pointer backtracking.

## 🛠️ Verification & Harness
The engine contains a strict differential regression testing harness (`run_tests.sh`) that cross-validates data ingestion passes against verified baseline reference files to ensure formatting anomalies never trigger calculation drift.

### Building and Running
```bash
make clean
make CC=gcc CFLAGS="-Wall -Wextra -O3 -Isrc"
./run_tests.sh
