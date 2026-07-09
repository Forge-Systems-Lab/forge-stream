#include <stdio.h>
#include <time.h>
#include "parser.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filepath>\n", argv[0]);
        return 1;
    }

    LedgerSummary summary;
    
    clock_t start_time = clock();
    int result = parse_ledger(argv[1], &summary);
    clock_t end_time = clock();

    if (result != 0) {
        fprintf(stderr, "Error: Failed to process ledger stream (code: %d)\n", result);
        return 1;
    }

    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    double lines_per_second = summary.total_records / execution_time;

    printf("===========================================\n");
    printf("FORGE-STREAM HIGH-THROUGHPUT METRICS\n");
    printf("===========================================\n");
    printf("Total Records Processed : %zu\n", summary.total_records);
    printf("Valid Records Ingested  : %zu\n", summary.valid_records);
    printf("Exception Errors Logged : %zu\n", summary.error_records);
    printf("Aggregated Cash Flow    : %.2f\n", summary.total_amount);
    printf("Execution Horizon Time  : %.4f seconds\n", execution_time);
    printf("Processing Velocity     : %.2f lines/sec\n", lines_per_second);
    printf("===========================================\n");

    return 0;
}
