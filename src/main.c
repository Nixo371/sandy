#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 900
#define HEIGHT 600

#define FPS 120
#define FRAME_TIME_MS (1000 / FPS)

#define BLOB_RADIUS 10

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

	return (state);
}

void add_sand_particle(game_state* state, int x, int y) {
	if (in_bounds(x, y)) {
		state->board[x][y].type = SAND;
	}
}

void add_sand_blob(game_state* state, int x, int y, int radius) {
	for (int x2 = x - radius; x2 < x + radius; x2++) {
		for (int y2 = y - radius; y2 < y + radius; y2++) {
			int x2_norm = x2 - x;
			int y2_norm = y2 - y;
			if ((x2_norm * x2_norm) + (y2_norm * y2_norm) < (radius * radius)) {
				add_sand_particle(state, x2, y2);
			}
		}
	}
}

void free_game_state(game_state* state) {
	for (int i = 0; i < WIDTH; i++) {
		free(state->board[i]);
	}
	free(state->board);
	state->board = NULL;

	free(state);
}

void update_particle(game_state* state, int x, int y) {
	switch (state->board[x][y].type) {
		case AIR:
			break;
		case SAND:
			// Randomize which direction is checked first
			int dir = rand() % 2 == 0 ? 1 : -1;

			// Straight Down
			if (in_bounds(x, y + 1) && state->board[x][y + 1].type == AIR) {
				state->board[x][y].type = AIR;
				state->board[x][y + 1].type = SAND;
			}
			else if (in_bounds(x - dir, y + 1) && state->board[x - dir][y + 1].type == AIR) {
				state->board[x][y].type = AIR;
				state->board[x - dir][y + 1].type = SAND;
			}
			else if (in_bounds(x + dir, y + 1) && state->board[x + dir][y + 1].type == AIR) {
				state->board[x][y].type = AIR;
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

int main() {
	srand(time(NULL));

	game_state* state = init_game_state();
	/*
	for (int x = WIDTH * 0.4; x < WIDTH * 0.6; x++) {
		for (int y = HEIGHT * 0.4; y < HEIGHT * 0.6; y++) {
			if (rand() % 3 == 0) {
				add_sand_particle(state, x, y);
			}
		}
	}
	*/
	float mouse_x;
	float mouse_y;
	SDL_MouseButtonFlags mouse_buttons = SDL_GetGlobalMouseState(&mouse_x, &mouse_y);

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

		// #FFD000 "Gold Web" sand-ish color
		SDL_SetRenderDrawColor(main_renderer, 255, 208, 0, SDL_ALPHA_OPAQUE);
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				if (state->board[x][y].type == SAND) {
					SDL_RenderPoint(main_renderer, x, y);
				}
			}
		}

		SDL_RenderPresent(main_renderer);

		update_all_particles(state);

		SDL_GetMouseState(&mouse_x, &mouse_y);
		mouse_buttons = SDL_GetGlobalMouseState(NULL, NULL);
		if (mouse_buttons & SDL_BUTTON_LMASK) {
			add_sand_blob(state, (int)mouse_x, (int)mouse_y, BLOB_RADIUS);
		}

		SDL_Delay(FRAME_TIME_MS);
	}

	SDL_DestroyRenderer(main_renderer);
	SDL_DestroyWindow(main_window);

	SDL_Quit();
}
