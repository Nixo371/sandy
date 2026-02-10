#ifndef PARTICLE_H
# define PARTICLE_H

#include <stdint.h>

// Forward Declarations //
typedef struct s_game_state game_state;
// -------------------- //

typedef enum e_dir {
        NW = 0,
        N = 1,
        NE = 2,
        W = 3,
        C = 4,
        E = 5,
        SW = 6,
        S = 7,
        SE = 8
} dir;

typedef struct s_rule {
        char input[9];
        char output[9];
} rule;

typedef struct s_particle_type {
        char* name;
        char id;

        uint8_t red;
        uint8_t green;
        uint8_t blue;

        rule* rules;
        int rule_count;
} particle_type;

typedef struct s_particle {
        int x;
        int y;

        particle_type* type;
} particle;

particle* get_particle(game_state* state, int x, int y);

void update_particle(game_state* state, int x, int y);
void update_all_particles(game_state* state);
void add_particle(game_state* state, int x, int y, particle_type* type);
void add_particle_blob(game_state* state, int x, int y, particle_type* type);

particle_type* parse_particle_type(char* particle_file_path);

#endif
