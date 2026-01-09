#ifndef PARTICLE_H
# define PARTICLE_H

// Forward Declarations //
typedef struct s_game_state game_state;
// -------------------- //

typedef enum e_particle_type {
	EMPTY,
	SAND
} particle_type;

typedef struct s_particle {
	int x;
	int y;
	particle_type type;
} particle;

void update_particle(game_state* state, int x, int y);
void update_all_particles(game_state* state);
void add_particle(game_state* state, int x, int y, particle_type type);
void add_particle_blob(game_state* state, int x, int y, particle_type type);

#endif
