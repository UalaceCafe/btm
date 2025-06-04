#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdlib.h>

#include "config.h"
#include "transition.h"

typedef struct {
    size_t current_state;
    size_t head_position;
    buffer_s tape;
} state_machine_s;

typedef enum {
	SM_RUNNING,
	SM_HALTED_ACCEPT,
	SM_HALTED_REJECT_OUT_OF_BOUNDS_LEFT,
	SM_HALTED_REJECT_OUT_OF_BOUNDS_RIGHT,
	SM_HALTED_REJECT_NO_TRANSITION
} sm_state_s;

void sm_populate_tape(state_machine_s* sm, buffer_s input);
void sm_init(configuration_s config, buffer_s input, state_machine_s* sm);
sm_state_s sm_step(state_machine_s* sm, configuration_s config);

#endif // STATE_MACHINE_H
