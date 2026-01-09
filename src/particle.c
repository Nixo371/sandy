#include "particle.h"
#include "sandy.h"

#include "constants.h"

#include <stdlib.h>

void update_particle(game_state* state, int x, int y) {
	switch (state->board[x][y].type) {
		case EMPTY:
			break;
		case SAND:
			// Randomize which direction is checked first
			int dir = rand() % 2 == 0 ? 1 : -1;

			if (in_bounds(x, y + 1) && state->board[x][y + 1].type == EMPTY) {
				state->board[x][y].type = EMPTY;
				state->board[x][y + 1].type = SAND;
			}
			else if (in_bounds(x - dir, y + 1) && state->board[x - dir][y + 1].type == EMPTY) {
				state->board[x][y].type = EMPTY;
				state->board[x - dir][y + 1].type = SAND;
			}
			else if (in_bounds(x + dir, y + 1) && state->board[x + dir][y + 1].type == EMPTY) {
				state->board[x][y].type = EMPTY;
				state->board[x + dir][y + 1].type = SAND;
			}

			break;
	}
}

void update_all_particles(game_state* state) {
	// Update from down up
	for (int y = HEIGHT - 1; y >= 0; y--) {
		for (int x = 0; x < WIDTH; x += 2) {
			update_particle(state, x, y);
		}
		for (int x = 1; x < WIDTH; x += 2) {
			update_particle(state, x, y);
		}
	}
}

void add_particle(game_state* state, int x, int y, particle_type type) {
	if (in_bounds(x, y)) {
		state->board[x][y].type = type;
	}
}

void add_particle_blob(game_state* state, int x, int y, particle_type type) {
	int radius = state->blob_radius;

	for (int x2 = x - radius; x2 < x + radius; x2++) {
		for (int y2 = y - radius; y2 < y + radius; y2++) {
			int x2_norm = x2 - x;
			int y2_norm = y2 - y;
			if ((x2_norm * x2_norm) + (y2_norm * y2_norm) < (radius * radius)) {
				add_particle(state, x2, y2, type);
			}
		}
	}
}

