#include <stdio.h>
#include <stdlib.h>

#include "../../include/config.h"
#include "../../include/transition.h"

void read_input_file(const char* filename, buffer_s* buffer) {
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		printf("Error: cannot open file '%s'\n", filename);
		exit(EXIT_FAILURE);
	}

	char c;
	size_t count = 0;
	while ((c = fgetc(file)) != EOF) {
		if (!is_whitespace_except_space(c))
			count++;
	}
	buffer->size = count;
	rewind(file);

	buffer->data = malloc(buffer->size);
	if (buffer->data == NULL) {
		puts("Error: cannot allocate memory for buffer");
		exit(EXIT_FAILURE);
	}

	count = 0;
	while ((c = fgetc(file)) != EOF) {
		if (is_whitespace_except_space(c))
			continue;

		if (!is_valid_symbol_input(c)) {
			printf("Error: invalid symbol '%c' at input\n", c);
			exit(EXIT_FAILURE);
		}

		buffer->data[count++] = c;
	}

	if (count != buffer->size) {
		printf("Error: cannot read file '%s'\n", filename);
		exit(EXIT_FAILURE);
	}

	fclose(file);
}

void read_config_file(const char* filename, configuration_s* config) {
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		printf("Error: cannot open file '%s'\n", filename);
		exit(EXIT_FAILURE);
	}

	char line[MAX_LINE_LENGTH];
    size_t count = 0;
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        if (is_non_empty(line)) {
            count++;
        }
    }
	rewind(file);

	if (count == 0) {
		puts("Error: configuration file is empty or contains only whitespace");
		exit(EXIT_FAILURE);
	}

	config->transitions_count = count;

	config->transitions = malloc(count * sizeof(transition_s));
	if (config->transitions == NULL) {
		puts("Error: cannot allocate memory for transitions array");
		exit(EXIT_FAILURE);
	}

	for (size_t count = 0, line_num = 0; fgets(line, MAX_LINE_LENGTH, file); line_num++) {
        if (is_non_empty(line)) {
			if (sscanf(line, "%zu: %c %c %c %zu", &config->transitions[count].state_num, &config->transitions[count].current_symbol,
					   &config->transitions[count].next_symbol, &config->transitions[count].dir, &config->transitions[count].next_state_num) != 5) {
				printf("Error: invalid transition '%s' format in line: '%zu'\n", line, line_num + 1);
				exit(EXIT_FAILURE);
			}

			if (!is_valid_symbol_rw(config->transitions[count].current_symbol)) {
				printf("Error: invalid symbol '%c' in transition: '%s'\n", config->transitions[count].current_symbol, line);
				exit(EXIT_FAILURE);
			}

			if (!is_valid_symbol_rw(config->transitions[count].next_symbol)) {
				printf("Error: invalid symbol '%c' in transition: '%s'\n", config->transitions[count].next_symbol, line);
				exit(EXIT_FAILURE);
			}

			if (config->transitions[count].dir != 'L' && config->transitions[count].dir != 'R') {
				printf("Error: invalid head movement direction '%c' in transition: '%s'\n", config->transitions[count].dir, line);
				exit(EXIT_FAILURE);
			}

			count++;
		}
    }
	rewind(file);

	config->states = malloc(0 * sizeof(size_t));
	if (config->states == NULL) {
		puts("Error: cannot allocate memory for states array");
		exit(EXIT_FAILURE);
	}

	size_t states_count = 0;
	for (size_t i = 0; i < config->transitions_count; i++) {
		if (!state_already_exists(config->states, states_count, config->transitions[i].state_num)) {
			size_t* new_states = realloc(config->states, (states_count + 1) * sizeof(size_t));
			if (new_states == NULL) {
				puts("Error: cannot allocate memory for states array");
				exit(EXIT_FAILURE);
			}
			config->states = new_states;
			config->states[states_count++] = config->transitions[i].state_num;
		}
	}

	config->states_count = states_count;

	qsort(config->states, config->states_count, sizeof(size_t), sort_func);

	if (config->states[0] != 0) {
		puts("Error: initial state must be 0");
		exit(EXIT_FAILURE);
	}

    fclose(file);
}

void define_final_states(configuration_s* config) {
	config->final_states = malloc(0 * sizeof(size_t));
	if (config->final_states == NULL) {
		puts("Error: cannot allocate memory for final states array");
		exit(EXIT_FAILURE);
	}

	size_t final_states_count = 0;
	for (size_t i = 0; i < config->transitions_count; i++) {
		bool is_final = true;

		for (size_t j = 0; j < config->states_count; j++) {
			if (config->transitions[i].next_state_num == config->states[j]) {
				is_final = false;
				break;
			}
		}

		if (is_final && !state_already_exists(config->final_states, final_states_count, config->transitions[i].next_state_num)) {
			size_t* new_final_states = realloc(config->final_states, (final_states_count + 1) * sizeof(size_t));
			if (new_final_states == NULL) {
				puts("Error: cannot allocate memory for final states array");
				exit(EXIT_FAILURE);
			}
			config->final_states = new_final_states;
			config->final_states[final_states_count++] = config->transitions[i].next_state_num;
		}
	}

	config->final_states_count = final_states_count;

	qsort(config->final_states, config->final_states_count, sizeof(size_t), sort_func);
}
