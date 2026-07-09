#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 65536 // 64KB Execution Block

int parse_ledger(const char *filepath, LedgerSummary *summary) {
    if (!filepath || !summary) return -1;

    FILE *file = fopen(filepath, "r");
    if (!file) return -2;

    summary->total_records = 0;
    summary->valid_records = 0;
    summary->error_records = 0;
    summary->total_amount = 0.0;

    char buffer[BUFFER_SIZE];
    char line_buffer[1024];
    size_t line_pos = 0;

    // Skip Header line
    if (!fgets(line_buffer, sizeof(line_buffer), file)) {
        fclose(file);
        return -3;
    }

    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n' || line_pos >= sizeof(line_buffer) - 1) {
                line_buffer[line_pos] = '\0';
                
                if (line_pos > 0) {
                    summary->total_records++;
                    
                    // Tokenize CSV parameters fields natively
                    char *timestamp = strtok(line_buffer, ",");
                    char *tx_id = strtok(NULL, ",");
                    char *account_id = strtok(NULL, ",");
                    char *amount_str = strtok(NULL, ",");
                    char *currency = strtok(NULL, ",");

                    if (timestamp && tx_id && account_id && amount_str && currency) {
                        double amount = strtod(amount_str, NULL);
                        summary->total_amount += amount;
                        summary->valid_records++;
                    } else {
                        summary->error_records++;
                    }
                }
                line_pos = 0;
            } else {
                line_buffer[line_pos++] = buffer[i];
            }
        }
    }

    fclose(file);
    return 0;
}
