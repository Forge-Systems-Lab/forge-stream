#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 65536
#define MAX_FIELDS 16

static char *extract_field(char **cursor) {
    if (!cursor || !*cursor || **cursor == '\0') return NULL;

    while (**cursor == ' ' || **cursor == '\t') {
        (*cursor)++;
    }

    char *start = *cursor;
    int in_quotes = 0;
    int in_array = 0; /* v0.2: Array boundary state tracker */
    char *write_ptr = start;

    if (**cursor == '"') {
        in_quotes = 1;
        (*cursor)++;
        start = *cursor;
        write_ptr = start;
    }

    while (**cursor != '\0') {
        if (in_quotes) {
            if (**cursor == '"') {
                if ((*cursor)[1] == '"') {
                    *write_ptr++ = '"';
                    *cursor += 2;
                    continue;
                } else {
                    in_quotes = 0;
                    (*cursor)++;
                    while (**cursor != '\0' && **cursor != ',') {
                        (*cursor)++;
                    }
                    if (**cursor == ',') (*cursor)++;
                    *write_ptr = '\0';
                    break;
                }
            }
            *write_ptr++ = **cursor;
            (*cursor)++;
        } else {
            /* State Machine: Track array boundaries to protect nested commas */
            if (**cursor == '[') in_array = 1;
            else if (**cursor == ']') in_array = 0;

            /* Only break on comma if we are NOT inside a nested collection */
            if (**cursor == ',' && !in_array) {
                *write_ptr = '\0';
                (*cursor)++;
                break;
            }
            *write_ptr++ = **cursor;
            (*cursor)++;
        }
    }

    if (!in_quotes) {
        *write_ptr = '\0';
    }

    char *end = write_ptr - 1;
    while (end >= start && (*end == ' ' || *end == '\t' || *end == '\0')) {
        *end = '\0';
        end--;
    }

    return start;
}

int stream_csv(const char *filepath, RecordHandler handler, void *user_data) {
    if (!filepath || !handler) return -1;

    FILE *file = fopen(filepath, "r");
    if (!file) return -2;

    char buffer[BUFFER_SIZE];
    char line_buffer[1024];
    size_t line_pos = 0;
    size_t current_line = 1;

    // Skip the schema headers automatically
    if (!fgets(line_buffer, sizeof(line_buffer), file)) {
        fclose(file);
        return -3;
    }

    char *fields[MAX_FIELDS];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n' || line_pos >= sizeof(line_buffer) - 1) {
                line_buffer[line_pos] = '\0';
                current_line++;

                if (line_pos > 0) {
                    char *cursor = line_buffer;
                    size_t field_idx = 0;
                    char *field;

                    while ((field = extract_field(&cursor)) != NULL && field_idx < MAX_FIELDS) {
                        fields[field_idx++] = field;
                    }

                    // Pipe the extracted string data straight into the business module handler callback
                    handler(current_line, fields, field_idx, user_data);
                }
                line_pos = 0;
            } else {
                if (buffer[i] != '\r') {
                    line_buffer[line_pos++] = buffer[i];
                }
            }
        }
    }

    fclose(file);
    return 0;
}
