#ifndef SANDY_H
# define SANDY_H

// Forward Declarations //
typedef struct s_particle particle;
typedef struct s_particle_type particle_type;
// -------------------- //

typedef struct s_game_state {
	particle** board;
        int particle_type_count;
        particle_type* particle_types;

	int blob_radius;
} game_state;

int in_bounds(int x, int y);
particle_type* get_particle_type(game_state* state, char id);

#endif
