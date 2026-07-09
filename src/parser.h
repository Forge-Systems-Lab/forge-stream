#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

typedef struct {
    size_t total_records;
    size_t valid_records;
    size_t error_records;
    double total_amount;
} LedgerSummary;

int parse_ledger(const char *filepath, LedgerSummary *summary);

#endif // PARSER_H
