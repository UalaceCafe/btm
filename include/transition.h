#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <stdlib.h>

// qi: x y m qj
typedef struct {
    size_t state_num;
    char current_symbol;
    char next_symbol;
    char dir;
    size_t next_state_num;
} transition_s;

typedef struct {
	size_t* states;
	size_t states_count;
	size_t* final_states;
	size_t final_states_count;
	transition_s* transitions;
	size_t transitions_count;
} configuration_s;

#endif // CONFIGURE_H
