#ifndef SANDY_H
# define SANDY_H

// Forward Declarations //
typedef struct s_particle particle;
// -------------------- //

typedef struct s_game_state {
	// Row major
	// x, y = [y * columns + x]
	particle* board;
	int blob_radius;
} game_state;

int in_bounds(int x, int y);

#endif
