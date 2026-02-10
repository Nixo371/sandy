#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_time.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_main.h>

#include <SDL3_ttf/SDL_ttf.h>

#include "constants.h"

#include "particle.h"
#include "sandy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>

particle_type* empty;
particle_type* sand;

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

void parse_all_particle_types(game_state* state) {
	struct dirent *de;

	DIR *dr = opendir("/home/nucieda/Projects/Sandy/particles");

	if (dr == NULL)  // opendir returns NULL if couldn't open directory
	{
		printf("Could not open current directory");
		return;
	}

	int particle_type_capacity = 1;
	state->particle_type_count = 0;
	state->particle_types = calloc(particle_type_capacity, sizeof(particle_type));

	// Refer https://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
	// for readdir()
	while ((de = readdir(dr)) != NULL) {
		if (de->d_type != DT_REG) {
			continue;
		}
		// TODO fix the fuck out of this
		char* file_path = calloc(strlen("/home/nucieda/Projects/Sandy/particles/") + 256, sizeof(char));
		sprintf(file_path, "/home/nucieda/Projects/Sandy/particles/%s", de->d_name);
		printf("Particle file found: %s\n", file_path);

		state->particle_types[state->particle_type_count] = *parse_particle_type(file_path);
		printf("Parsed\n");
		state->particle_type_count += 1;
		if (state->particle_type_count >= particle_type_capacity) {
			particle_type_capacity *= 2;
			state->particle_types = realloc(state->particle_types, particle_type_capacity * sizeof(particle_type));
		}
	}

	closedir(dr);
}

game_state* init_game_state() {
	game_state* state = malloc(sizeof(game_state));

	parse_all_particle_types(state);

	empty = get_particle_type(state, '.');
	state->board = (particle **) calloc(WIDTH, sizeof(particle *));
	for (int i = 0; i < WIDTH; i++) {
		state->board[i] = (particle *) calloc(HEIGHT, sizeof(particle));
		for (int j = 0; j < HEIGHT; j++) {
			state->board[i][j].x = i;
			state->board[i][j].y = j;
			state->board[i][j].type = empty;
		}
	}

	state->blob_radius = DEFAULT_BLOB_RADIUS;

	return (state);
}

void free_game_state(game_state* state) {
	free(state->board);
	state->board = NULL;

	free(state);
}

int main() {
	srand(time(NULL));

	game_state* state = init_game_state();

	float mouse_x;
	float mouse_y;
	SDL_MouseButtonFlags mouse_buttons = SDL_GetGlobalMouseState(&mouse_x, &mouse_y);

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	if (!TTF_Init()) {
		fprintf(stderr, "Failed to initialize SDL_ttf: %s\n", SDL_GetError());
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

	// TTF_Font* font = TTF_OpenFont("assets/fonts/Inter_18pt-ExtraBold.ttf", 16);
	TTF_Font* font = TTF_OpenFont("assets/fonts/JetBrainsMonoNerdFont-ExtraBold.ttf", 16);
	SDL_Surface* text = TTF_RenderText_Blended(font, "0 FPS", 0, WHITE);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(main_renderer, text);
	char fps_text[32];
	SDL_FRect fps_text_rect = { 20, 20, 0, 0 };

	SDL_HideCursor();

	sand = get_particle_type(state, 's');
	int sand_points_capacity = 1024;
	int sand_points_count = 0;
	SDL_FPoint* sand_points = calloc(sand_points_capacity, sizeof(SDL_FPoint));

	SDL_Time start_time;
	SDL_Time end_time;
	Uint64 frame_time;

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
		sand_points_count = 0;
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				if (state->board[x][y].type == sand) {
					if (sand_points_count >= sand_points_capacity) {
						sand_points_capacity *= 2;
						sand_points = (SDL_FPoint *) realloc(sand_points, sand_points_capacity * sizeof(SDL_FPoint));
					}
					sand_points[sand_points_count].x = x;
					sand_points[sand_points_count].y = y;
					sand_points_count += 1;
				}
			}
		}
		SDL_RenderPoints(main_renderer, sand_points, sand_points_count);

		// Render Cursor //
		SDL_SetRenderDrawColor(main_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		draw_circle(main_renderer, mouse_x, mouse_y, state->blob_radius, CURSOR_THICKNESS);

		// FPS Display //
		sprintf(fps_text, "%d FPS", 1000000000 / (int)frame_time);
		text = TTF_RenderText_Blended(font, fps_text, 0, WHITE);
		texture = SDL_CreateTextureFromSurface(main_renderer, text);
		SDL_GetTextureSize(texture, &fps_text_rect.w, &fps_text_rect.h);
		SDL_RenderTexture(main_renderer, texture, NULL, &fps_text_rect);

		SDL_RenderPresent(main_renderer);

		update_all_particles(state);

		SDL_GetMouseState(&mouse_x, &mouse_y);
		mouse_buttons = SDL_GetGlobalMouseState(NULL, NULL);
		if (mouse_buttons & SDL_BUTTON_LMASK) {
			add_particle_blob(state, (int)mouse_x, (int)mouse_y, sand);
		}
		if (mouse_buttons & SDL_BUTTON_RMASK) {
			add_particle_blob(state, (int)mouse_x, (int)mouse_y, empty);
		}

		// Calculate how much time to wait after the frame
		SDL_GetCurrentTime(&end_time);
		frame_time = end_time - start_time;
		if (frame_time < FRAME_TIME_NS) {
			// Need to wait some extra
			SDL_DelayNS(FRAME_TIME_NS - frame_time);
			frame_time = FRAME_TIME_NS;
			// printf("FPS: %03.2f", (float)FPS);
		}
		else {
			// printf("FPS: %03.2f", 1000000000.0 / frame_time);
		}
		// printf(" Sand Particles: %d\n", sand_points_count);
	}

	SDL_DestroyRenderer(main_renderer);
	SDL_DestroyWindow(main_window);

	TTF_CloseFont(font);

	TTF_Quit();
	SDL_Quit();
}
