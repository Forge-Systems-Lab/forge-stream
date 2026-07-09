#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 65536

// Zero-dependency, thread-safe, reentrant token extraction
static char *extract_field(char **cursor) {
    if (!cursor || !*cursor || **cursor == '\0') return NULL;
    char *start = *cursor;
    while (**cursor != '\0' && **cursor != ',') {
        (*cursor)++;
    }
    if (**cursor == ',') {
        **cursor = '\0';
        (*cursor)++;
    }
    return start;
}

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
                    
                    char *cursor = line_buffer;
                    char *timestamp = extract_field(&cursor);
                    char *tx_id = extract_field(&cursor);
                    char *account_id = extract_field(&cursor);
                    char *amount_str = extract_field(&cursor);
                    char *currency = extract_field(&cursor);

                    // Defensive Firewall: Ensure all pointers exist and amount is populated
                    if (timestamp && tx_id && account_id && amount_str && currency && *amount_str != '\0') {
                        char *endptr;
                        double amount = strtod(amount_str, &endptr);
                        
                        // Strict validation of the numeric transformation boundary
                        if (endptr != amount_str) {
                            summary->total_amount += amount;
                            summary->valid_records++;
                        } else {
                            summary->error_records++;
                        }
                    } else {
                        summary->error_records++;
                    }
                }
                line_pos = 0;
            } else {
                // Natively sanitize Carriage Returns (\r) to guarantee absolute CRLF compatibility
                if (buffer[i] != '\r') {
                    line_buffer[line_pos++] = buffer[i];
                }
            }
        }
    }

    fclose(file);
    return 0;
}
