#include <stdio.h>

#include "../../include/state_machine.h"

void sm_init(configuration_s config, buffer_s input, state_machine_s* sm) {
	sm->head_position = 0;
	sm->current_state = 0;
	sm->tape = (buffer_s){ 0 };

	sm_populate_tape(sm, input);
}

void sm_populate_tape(state_machine_s* sm, buffer_s input) {
#ifndef TAPE_SIZE
#define TAPE_SIZE ((input.size * 2) + 2)
#endif
	if (TAPE_SIZE < input.size + 2) {
		puts("Error: tape size is too small to fit input and the start-of-tape and end-of-tape symbols");
		exit(EXIT_FAILURE);
	}

	sm->tape.size = TAPE_SIZE;

	sm->tape.data = malloc(TAPE_SIZE);
	if (sm->tape.data == NULL) {
		puts("Error: cannot allocate memory for tape");
		exit(EXIT_FAILURE);
	}

	sm->tape.data[0] = '<';

	for (size_t i = 1; i < TAPE_SIZE; i++) {
		if ((i - 1) < input.size) {
			sm->tape.data[i] = input.data[i - 1];
		} else {
			sm->tape.data[i] = '_';
		}
	}
	sm->tape.data[TAPE_SIZE - 1] = '>';
}

sm_state_s sm_step(state_machine_s* sm, configuration_s config) {
	for (size_t i = 0; i < config.final_states_count; i++) {
		if (sm->current_state == config.final_states[i]) {
			return SM_HALTED_ACCEPT;
		}
	}

	for (size_t i = 0; i < config.transitions_count; i++) {
		if (config.transitions[i].state_num == sm->current_state &&
		    config.transitions[i].current_symbol == sm->tape.data[sm->head_position]) {
			sm->tape.data[sm->head_position] = config.transitions[i].next_symbol;

			if (config.transitions[i].dir == 'R') {
				if (sm->head_position < sm->tape.size - 1) {
					sm->head_position++;
				} else {
					return SM_HALTED_REJECT_OUT_OF_BOUNDS_RIGHT;
				}
			} else if (config.transitions[i].dir == 'L') {
				if (sm->head_position > 0) {
					sm->head_position--;
				} else {
					return SM_HALTED_REJECT_OUT_OF_BOUNDS_LEFT;
				}
			}

			sm->current_state = config.transitions[i].next_state_num;
			return SM_RUNNING;
		}
	}

	return SM_HALTED_REJECT_NO_TRANSITION;
}
