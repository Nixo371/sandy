#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_time.h>
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
#define FRAME_TIME_NS (1000000000 / FPS)

#define DEFAULT_BLOB_RADIUS 10
#define MIN_BLOB_RADIUS 1
#define MAX_BLOB_RADIUS 100

#define CURSOR_THICKNESS 2

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
	int blob_radius;
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

void draw_circle(SDL_Renderer* renderer, float x, float y, float radius, float thickness) {
	for (float x2 = x - radius - (thickness/2 + 1); x2 <= x + radius + (thickness/2 + 1); x2 += 1) {
		for (float y2 = y - radius - (thickness/2 + 1); y2 <= y + radius + (thickness/2 + 1); y2 += 1) {
			float x_norm = x2 - x;
			float y_norm = y2 - y;
			float dist_sq = (x_norm * x_norm) + (y_norm * y_norm);
			float radius_sq = radius * radius;
			if (dist_sq < radius_sq + (radius * thickness) && dist_sq > radius_sq - (radius * thickness)) {
				SDL_RenderPoint(renderer, x2, y2);
			}
		}
	}
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

	state->blob_radius = DEFAULT_BLOB_RADIUS;

	return (state);
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
	int sand_count;

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

	SDL_HideCursor();

	SDL_Time start_time;
	SDL_Time end_time;

	SDL_Event event;
	int running = 1;
	while (running == 1) {
		SDL_GetCurrentTime(&start_time);
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				printf("Quitting...\n");
				running = 0;
			}
			if (event.type == SDL_EVENT_MOUSE_WHEEL) {
				state->blob_radius += event.wheel.y;
				if (state->blob_radius < MIN_BLOB_RADIUS) {
					state->blob_radius = MIN_BLOB_RADIUS;
				}
				if (state->blob_radius > MAX_BLOB_RADIUS) {
					state->blob_radius = MAX_BLOB_RADIUS;
				}
			}
		}

		SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(main_renderer);

		// Render particles
		// #FFD000 "Gold Web" sand-ish color
		SDL_SetRenderDrawColor(main_renderer, 255, 208, 0, SDL_ALPHA_OPAQUE);
		sand_count = 0;
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				if (state->board[x][y].type == SAND) {
					SDL_RenderPoint(main_renderer, x, y);
					sand_count += 1;
				}
			}
		}

		// Render cursor //
		SDL_SetRenderDrawColor(main_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		draw_circle(main_renderer, mouse_x, mouse_y, state->blob_radius, CURSOR_THICKNESS);

		SDL_RenderPresent(main_renderer);

		update_all_particles(state);

		SDL_GetMouseState(&mouse_x, &mouse_y);
		mouse_buttons = SDL_GetGlobalMouseState(NULL, NULL);
		if (mouse_buttons & SDL_BUTTON_LMASK) {
			add_particle_blob(state, (int)mouse_x, (int)mouse_y, SAND);
		}
		if (mouse_buttons & SDL_BUTTON_RMASK) {
			add_particle_blob(state, (int)mouse_x, (int)mouse_y, AIR);
		}

		// Calculate how much time to wait after the frame
		SDL_GetCurrentTime(&end_time);
		Uint64 frame_time = end_time - start_time;
		if (frame_time < FRAME_TIME_NS) {
			// Need to wait some extra
			SDL_DelayNS(FRAME_TIME_NS - frame_time);
			// printf("FPS: %03.2f", (float)FPS);
		}
		else {
			// printf("FPS: %03.2f", 1000000000.0 / frame_time);
		}
		// printf(" Sand Particles: %d\n", sand_count);
	}

	SDL_DestroyRenderer(main_renderer);
	SDL_DestroyWindow(main_window);

	SDL_Quit();
}
