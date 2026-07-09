# forge-stream

A high-performance, zero-dependency data ingestion stream engine written in pure C.

## Features
* **Chunk-Buffered I/O:** Processes raw data using deterministic 64KB block stream reads to reduce system overhead.
* **Reentrant Parser Core:** Thread-safe pointer traversal engine designed for concurrent multi-tenant data structures.
* **Zero Abstraction:** Bypasses external libraries and heavy runtimes to operate directly at the system memory layer.
* **Defensive Firewalling:** Isolates missing delimiters, invalid entries, and malformed fields without execution drift.

## Performance Baseline
Evaluated natively under standard Linux operational runtimes:
* **Dataset Volume:** 100,000 structured ledger records
* **Execution Delta:** 0.0274 seconds
* **Ingestion Throughput:** ~3.6M lines/second

## Usage & Compilation
Build the binary using the provided automation layout:
```bash
make
./forge-stream <path_to_file.csv>
