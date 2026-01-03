#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 900
#define HEIGHT 600

#define FPS 60
#define FRAME_TIME_MS (1000 / FPS)

typedef enum e_particle_type {
	AIR,
	SAND
} particle_type;

typedef struct s_particle {
	int x;
	int y;
	particle_type type;
} particle;

typedef struct s_game_state {
	particle** board;

	int sand_capacity;
	int sand_count;
	particle* sand;
} game_state;

int in_bounds(int x, int y) {
	if (x < 0 || x >= WIDTH) {
		return (0);
	}
	if (y < 0 || y >= HEIGHT) {
		return (0);
	}

	return (1);
}

game_state* init_game_state() {
	game_state* state = malloc(sizeof(game_state));

	state->board = (particle **) calloc(WIDTH, sizeof(particle *));
	for (int i = 0; i < WIDTH; i++) {
		state->board[i] = (particle *) calloc(HEIGHT, sizeof(particle));
		for (int j = 0; j < HEIGHT; j++) {
			state->board[i][j].x = i;
			state->board[i][j].y = j;
			state->board[i][j].type = AIR;
		}
	}

	state->sand_capacity = 128;
	state->sand_count = 0;
	state->sand = calloc(state->sand_capacity, sizeof(particle));

	return (state);
}

void add_sand_particle(game_state* state, int x, int y) {
	if (state->sand_count >= state->sand_capacity) {
		state->sand_capacity *= 2;
		state->sand = (particle *) realloc(state->sand, state->sand_capacity * sizeof(particle));
	}

	state->board[x][y].type = SAND;

	state->sand[state->sand_count].x = x;
	state->sand[state->sand_count].y = y;
	state->sand[state->sand_count].type = SAND;


	state->sand_count += 1;
}

void free_game_state(game_state* state) {
	for (int i = 0; i < WIDTH; i++) {
		free(state->board[i]);
	}
	free(state->board);
	state->board = NULL;

	state->sand_capacity = 0;
	state->sand_count = 0;
	free(state->sand);
	state->sand = NULL;

	free(state);
}

void update_particle(game_state* state, particle* p) {
	int x = p->x;
	int y = p->y;

	switch (state->board[x][y].type) {
		case AIR:
			break;
		case SAND:
			if (in_bounds(x, y + 1) && state->board[x][y + 1].type == AIR) {
				state->board[x][y].type = AIR;
				state->board[x][y + 1].type = SAND;

				p->x = x;
				p->y = y + 1;
			}
			else if (in_bounds(x - 1, y + 1) && state->board[x - 1][y + 1].type == AIR) {
				state->board[x][y].type = AIR;
				state->board[x - 1][y + 1].type = SAND;

				p->x = x - 1;
				p->y = y + 1;
			}
			else if (in_bounds(x + 1, y + 1) && state->board[x + 1][y + 1].type == AIR) {
				state->board[x][y].type = AIR;
				state->board[x + 1][y + 1].type = SAND;

				p->x = x + 1;
				p->y = y + 1;
			}

			break;
	}
}

void update_sand_particles(game_state* state) {
	for (int i = 0; i < state->sand_count; i++) {
		update_particle(state, &state->sand[i]);
	}
}

int main() {
	srand(time(NULL));

	game_state* state = init_game_state();
	for (int x = WIDTH * 0.4; x < WIDTH * 0.6; x++) {
		for (int y = HEIGHT * 0.4; y < HEIGHT * 0.6; y++) {
			if (rand() % 3 == 0) {
				add_sand_particle(state, x, y);
			}
		}
	}

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_Window* main_window = SDL_CreateWindow("Sandy", WIDTH, HEIGHT, 0);
	if (main_window == NULL) {
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	
	SDL_Renderer* main_renderer = SDL_CreateRenderer(main_window, NULL);
	if (main_renderer == NULL) {
		fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_Event event;
	int running = 1;
	while (running == 1) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				printf("Quitting...\n");
				running = 0;
			}
		}

		SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(main_renderer);

		SDL_SetRenderDrawColor(main_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		for (int i = 0; i < state->sand_count; i++) {
			SDL_RenderPoint(main_renderer, state->sand[i].x, state->sand[i].y);
		}

		SDL_RenderPresent(main_renderer);

		update_sand_particles(state);
		SDL_Delay(FRAME_TIME_MS);
	}

	SDL_DestroyRenderer(main_renderer);
	SDL_DestroyWindow(main_window);

	SDL_Quit();
}
