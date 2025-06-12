#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/config.h"
#include "../include/state_machine.h"

#define FILENAME_MAX_SIZE 256

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define PRINT_X_TIMES(x, c) 			 \
	do {                                 \
		for (size_t i = 0; i < (x); i++) \
			putchar(c);                  \
	} while (0);                         \


const char* help_message = "Usage: btm [-h | --help] [<config_file> <input_file>]\n\n"
						   "Options:\n"
						   " -h, --help\tDisplay this help message";

static void print_configuration(const configuration_s config);
static void print_input(const buffer_s input);
static void print_tape(const state_machine_s sm);
static inline void free_buffer(buffer_s* input);
static inline void free_configuration(configuration_s* config);

int main(int argc, char* argv[]) {
	if (argc < 2) {
		puts("Error: missing argument\n");
		puts(help_message);
		exit(EXIT_FAILURE);
	}

	char config_file[FILENAME_MAX_SIZE] = { 0 }, input_file[FILENAME_MAX_SIZE] = { 0 };

	// Not ideal, but might be useful in case more positional flags are added in the future
	if (argc == 2) {
		if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
			puts(help_message);
			exit(EXIT_SUCCESS);
		} else {
			printf("Error: unknown command-line option '%s'\n\n", argv[1]);
			puts(help_message);
			exit(EXIT_FAILURE);
		}
	} else if (argc == 3) {
		strncpy(config_file, argv[1], FILENAME_MAX_SIZE - 1);
		strncpy(input_file, argv[2], FILENAME_MAX_SIZE - 1);
	} else {
		puts("Error: invalid arguments\n");
		puts(help_message);
		exit(EXIT_FAILURE);
	}

	buffer_s input = { 0 };
	read_input_file(input_file, &input);

	configuration_s config = { 0 };
	read_config_file(config_file, &config);
	define_final_states(&config);

	print_configuration(config);
	print_input(input);

	state_machine_s sm;
	sm_init(config, input, &sm);
	puts("");
	print_tape(sm);

#ifdef STEPS
	size_t hyphen_len = (sm.tape.size + 3) / 2;
	puts("");
	PRINT_X_TIMES(hyphen_len, '=');
	printf(" Steps ");
	PRINT_X_TIMES(hyphen_len, '=');
	puts("");
#endif // !STEPS

	size_t computing_steps = 0;
	sm_state_s sm_state = SM_RUNNING;

	while (sm_state == SM_RUNNING) {
#ifdef STEPS
		printf("%03zu. (", computing_steps);
		for (size_t i = 0; i < sm.head_position; i++) {
			putchar(sm.tape.data[i]);
		}
		printf(",%zu,", sm.current_state);
		for (size_t i = sm.head_position; i < sm.tape.size; i++) {
			putchar(sm.tape.data[i]);
		}
		puts(")");
#endif // !STEPS

		sm_state = sm_step(&sm, config);

		switch (sm_state) {
		case SM_HALTED_ACCEPT:
			puts("\n" ANSI_COLOR_GREEN "Machine halted in ACCEPTING state." ANSI_COLOR_RESET);
			break;
		case SM_HALTED_REJECT_OUT_OF_BOUNDS_LEFT:
			puts("\n" ANSI_COLOR_RED "Machine halted in REJECTING state: head moved out of bounds to the left." ANSI_COLOR_RESET);
			break;
		case SM_HALTED_REJECT_OUT_OF_BOUNDS_RIGHT:
			puts("\n" ANSI_COLOR_RED "Machine halted in REJECTING state: head moved out of bounds to the right." ANSI_COLOR_RESET);
			break;
		case SM_HALTED_REJECT_NO_TRANSITION:
			puts("\n" ANSI_COLOR_RED "Machine halted in REJECTING state: no transition found for current state and symbol." ANSI_COLOR_RESET);
			break;
		default:
			break;
		}

		computing_steps++;
	}

#ifdef STEPS
	puts("");
	print_tape(sm);
#endif // !STEPS

	free_buffer(&input);
	free_configuration(&config);
	free_buffer(&sm.tape);

	return 0;
}

static void print_configuration(const configuration_s config) {
	puts("Machine configuration: {");
	printf("  Non-final states: [");
	for (size_t i = 0; i < config.states_count; i++) {
		printf("%zu" , config.states[i]);
		if (i < config.states_count - 1) {
			printf(", ");
		}
	}
	puts("]");
	printf("  Non-final states count: %zu\n", config.states_count);
	printf("  Final states: [");
	for (size_t i = 0; i < config.final_states_count; i++) {
		printf("%zu" , config.final_states[i]);
		if (i < config.final_states_count - 1) {
			printf(", ");
		}
	}
	puts("]");
	printf("  Final states count: %zu\n", config.final_states_count);
	printf("  Transitions: [\n");
	for (size_t i = 0; i < config.transitions_count; i++) {
		printf(
			"    (%zu -> %zu): (%c, %c, %s)\n",
			config.transitions[i].state_num,
			config.transitions[i].next_state_num,
			config.transitions[i].current_symbol,
			config.transitions[i].next_symbol,
			(config.transitions[i].dir == 'R') ? "->" : "<-"
		);
	}
	puts("  ]");
	printf("  Transitions count: %zu\n", config.transitions_count);
	puts("}");
	puts("");
}

static void print_input(const buffer_s input) {
	printf("Input (%zu symbols):\n", input.size);
	if (input.size == 0) {
		puts(" (empty)");
		return;
	}

	PRINT_X_TIMES((input.size * 4) + 1, '-');
	puts("");
	printf("|");
	for (size_t i = 0; i < input.size; i++) {
		printf(" %c |", input.data[i]);
	}
	puts("");
	PRINT_X_TIMES((input.size * 4) + 1, '-');
	puts("");
}

static void print_tape(const state_machine_s sm) {
	printf("Tape (%zu symbols):\n", sm.tape.size);
	PRINT_X_TIMES((sm.tape.size * 4) + 1, '-');
	puts("");
	printf("|");
	for (size_t i = 0; i < sm.tape.size; i++) {
		printf(" %c |", sm.tape.data[i]);
	}
	puts("");
	PRINT_X_TIMES((sm.tape.size * 4) + 1, '-');
	puts("");
}

static inline void free_buffer(buffer_s* buffer) {
	free(buffer->data);
}

static inline void free_configuration(configuration_s* config) {
	free(config->states);
	free(config->final_states);
	free(config->transitions);
}
