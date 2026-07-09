# forge-stream

A high-performance, zero-dependency CSV streaming and ledger reconciliation engine written in pure C. Designed as a stateless, reentrant library component, it decouples raw chunk-buffered file I/O from custom business logic via a flexible callback native interface.

## Core Engineering Features
* **Stateless Chunk-Buffered Streaming:** Processes datasets using strict 64KB block stream transfers via `fread` to bypass heavy platform memory allocations.
* **Reentrant Token Extraction:** Implements custom double-pointer tokenization to preserve column alignment on empty fields (`,,`) and parse quoted string fields natively.
* **Inline Whitespace Trimming:** Executes zero-allocation, single-pass boundary stripping for both leading and trailing whitespaces/tabs to protect string comparisons.
* **Cross-Platform Line Ending Support:** Automatically normalizes both Unix LF (`\n`) and Windows CRLF (`\r\n`) architectures.
* **Defensive Reconciliation Controls:** Built-in protection against duplicate tracking keys and epsilon-bounded floating-point evaluations (`fabs`) to neutralize IEEE-754 rounding noise.

## System Architecture
```text
  [Raw Data Stream] ──► [64KB Buffer Blocks] ──► [Pointer-Traversal Parser]
                                                          │
                                            (Extracted Fields Pass-Through)
                                                          ▼
                                              [RecordHandler Callback]
                                                          │
                                     ┌────────────────────┴────────────────────┐
                                     ▼                                         ▼
                        [Financial Reconciliation]                  [Alternative Processing]
                        • In-Memory Match Map                       • Custom Metrics Engines
                        • Key Leakage Analytics                     • Analytics Dashboards
Compilation & Build Automation
Compile the optimized binary substrate using the provided automated compilation layout:

Bash
make clean && make
Running the Verification Example
The engine includes a data stream verification program inside src/main.c that maps an internal broker log and a bank settlement pipeline to isolate matching discrepancies:

Bash
./forge-stream sample_data/broker_ledger.csv sample_data/bank_ledger.csv
Expected Output Metrics
Plaintext
[WARNING] Line 5: Duplicate broker transaction ID isolated ('TXID-9901'). Skipping entry.
===================================================
        LEDGER RECONCILIATION ANOMALY AUDIT        
===================================================
[MISSING]  TXID: TXID-9902 | Account: ACC-20002
  -> Status: Completely missing from Bank Settlement.
  -> Delta: -$2450.75

[MISMATCH] TXID: TXID-9903 | Account: ACC-20003
  -> Broker: $890.00 | Bank: $880.00
  -> Delta: -$10.00

---------------------------------------------------
RECONCILIATION REPORT SUMMARY
---------------------------------------------------
Broker Records Processed : 4
Bank Records Processed   : 2
Duplicate Broker IDs     : 1
Successfully Matched     : 1
Missing from Bank        : 1
Value Mismatches Found   : 1
Total Absolute Delta     : $2460.75
===================================================
Future Expansion Scope
Migrate the in-memory lookup cache to a high-density Robin Hood Hash Map to scale tracking array operations past millions of lines within flat memory constraints.

Introduce an absolute schema definition parser configuration structure to dynamically support non-standard column layouts without rewriting the callback logic layer.
