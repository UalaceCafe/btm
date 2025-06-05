#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "transition.h"

#define MAX_LINE_LENGTH 256

typedef struct {
    char* data;
    size_t size;
} buffer_s;

static inline bool is_whitespace_except_space(char sym) {
	return (sym == '\t') || (sym == '\n') || (sym == '\v') || (sym == '\f') || (sym == '\r');
}

static inline bool is_valid_symbol_input(char sym) {
    return isalnum(sym) || (sym == '*') || (sym == '#');
}

static inline bool is_valid_symbol_rw(char sym) {
    return is_valid_symbol_input(sym) || (sym == '_') || (sym == '$');
}

static inline bool is_non_empty(const char* line) {
    while (*line) {
        if (!isspace((unsigned char)*line)) {
            return true;
        }
        line++;
    }
    return false;
}

static inline bool state_already_exists(const size_t* states, size_t state_count, size_t state) {
	if (states == NULL || state_count == 0)
		return false;

	for (size_t i = 0; i < state_count; i++) {
		if (states[i] == state) {
			return true;
		}
	}
	return false;
}

static inline int sort_func(const void* a, const void* b) {
	return (*(size_t*)a - *(size_t*)b);
}

void read_input_file(const char* filename, buffer_s* buffer);
void read_config_file(const char* filename, configuration_s* config);
void define_final_states(configuration_s* config);


#endif // CONFIG_H
