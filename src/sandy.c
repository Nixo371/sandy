#include "sandy.h"
#include "particle.h"

#include "constants.h"
#include <unistd.h>

int in_bounds(int x, int y) {
	if (x < 0 || x >= WIDTH) {
		return (0);
	}
	if (y < 0 || y >= HEIGHT) {
		return (0);
	}

	return (1);
}

particle_type* get_particle_type(game_state* state, char id) {
        for (int i = 0; i < state->particle_type_count; i++) {
                if (state->particle_types[i].id == id) {
                        return (&state->particle_types[i]);
                }
        }

        return (NULL);
}
