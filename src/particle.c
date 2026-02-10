#include "particle.h"
#include "sandy.h"

#include "constants.h"

#include <iso646.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

void update_particle(game_state* state, int x, int y) {
        for (int i = 0; i < state->board[x][y].type->rule_count; i++) {
                int rule_valid = 1;
                rule r = state->board[x][y].type->rules[i];
                for (int input = 0; input < 9; input++) {
                        if (r.input[input] == '*') {
                                continue;
                        }
                        int x_off = 0;
                        int y_off = 0;
                        if (input == NW || input == W || input == SW) {
                                x_off = -1;
                        }
                        if (input == NE || input == E || input == SE) {
                                x_off = 1;
                        }
                        if (input == NW || input == N || input == NE) {
                                y_off = -1;
                        }
                        if (input == SW || input == S || input == SE) {
                                y_off = 1;
                        }

                        if (!in_bounds(x + x_off, y + y_off) || state->board[x + x_off][y + y_off].type->id != r.input[input]) {
                                rule_valid = 0;
                                break;
                        }
                }

                if (rule_valid == 0) {
                        continue;
                }

                for (int output = 0; output < 9; output++) {
                        if (r.output[output] == '*') {
                                continue;
                        }
                        int x_off = 0;
                        int y_off = 0;
                        if (output == NW || output == W || output == SW) {
                                x_off = -1;
                        }
                        if (output == NE || output == E || output == SE) {
                                x_off = 1;
                        }
                        if (output == NW || output == N || output == NE) {
                                y_off = -1;
                        }
                        if (output == SW || output == S || output == SE) {
                                y_off = 1;
                        }

                        if (in_bounds(x + x_off, y + y_off)) {
                                state->board[x + x_off][y + y_off].type = get_particle_type(state, r.output[output]);
                        }
                }

		// only 1 rule can happen
		if (rule_valid == 1) {
			break;
		}
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

void add_particle(game_state* state, int x, int y, particle_type* type) {
	if (in_bounds(x, y)) {
		state->board[x][y].type = type;
	}
}

void add_particle_blob(game_state* state, int x, int y, particle_type* type) {
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

uint8_t hex_char_to_uint(char c) {
        if (c >= 'a' && c <= 'z') {
                c += 'A' - 'a';
        }

        // All uppercase
        if (c >= 'A' && c <= 'F') {
                return (10 + (c - 'A'));
        }
        return (c - '0');
}

void parse_color(char* str, uint8_t* red, uint8_t* green, uint8_t* blue) {
        if (str[0] == '#') { // Ignore initial #
                str += 1;
        }

        *red = 0;
        *green = 0;
        *blue = 0;

        *red += hex_char_to_uint(str[0]) << 4;
        *red += hex_char_to_uint(str[1]);
        *green += hex_char_to_uint(str[2]) << 4;
        *green += hex_char_to_uint(str[3]);
        *blue += hex_char_to_uint(str[4]) << 4;
        *blue += hex_char_to_uint(str[5]);
}

char* get_next_line(FILE* file) {
	if (feof(file)) {
		return (NULL);
	}

        int line_size = 16;
        char* line = (char *) calloc(line_size, sizeof(char));
        int line_i = 0;

        char c;
        while (fread(&c, sizeof(char), 1, file) != 0) {
                if (c == '\n') {
                        break;
                }
                line[line_i] = c;

                line_i += 1;
                if (line_i >= line_size) {
                        line_size *= 2;
                        line = realloc(line, line_size * sizeof(char));
                }
        }
	if (line_i == 0 && feof(file)) {
		free(line);
		return (NULL);
	}

        line = realloc(line, (line_i + 1) * sizeof(char));
        line[line_i] = '\0';

        return (line);
}

// ***    ***
// *s* -> *.*
void print_particle_type(particle_type* p) {
        printf("Name: %s\n", p->name);
        printf("ID: %c\n", p->id);
        printf("Color: #%x%x%x\n", p->red, p->green, p->blue);
        printf("Rules:");
        for (int i = 0; i < p->rule_count; i++) {
                printf("\n");
                printf("%c%c%c    %c%c%c\n", p->rules[i].input[0], p->rules[i].input[1], p->rules[i].input[2], p->rules[i].output[0], p->rules[i].output[1], p->rules[i].output[2]);
                printf("%c%c%c -> %c%c%c\n", p->rules[i].input[3], p->rules[i].input[4], p->rules[i].input[5], p->rules[i].output[3], p->rules[i].output[4], p->rules[i].output[5]);
                printf("%c%c%c    %c%c%c\n", p->rules[i].input[6], p->rules[i].input[7], p->rules[i].input[8], p->rules[i].output[6], p->rules[i].output[7], p->rules[i].output[8]);
        }
}

/*
Sand\n
s
#FFD000

***
*s*
*.*

***
*.*
*s*
*/
particle_type* parse_particle_type(char* particle_file_path) {
        particle_type* p = (particle_type *)malloc(sizeof(particle_type));

        FILE* file = fopen(particle_file_path, "r");
        p->name = get_next_line(file);

        char* id = get_next_line(file);
        p->id = id[0];
        free(id);

        char* color = get_next_line(file);
        parse_color(color, &p->red, &p->green, &p->blue);
        free(color);

	char* buffer = get_next_line(file); // should be an empty line
	free(buffer);

        int rule_size = 1;
        p->rule_count = 0;
        p->rules = (rule *)calloc(rule_size, sizeof(rule));

        // ***
        // *s*
        // *.*
        while (1) {
                char* top = get_next_line(file); // ***
		if (top == NULL) {
			// no rules
			print_particle_type(p); // TODO remove
			return (p);
		}
                p->rules[p->rule_count].input[NW] = top[0];
                p->rules[p->rule_count].input[N] = top[1];
                p->rules[p->rule_count].input[NE] = top[2];
                free(top);

                char* middle = get_next_line(file); // *s*
                p->rules[p->rule_count].input[W] = middle[0];
                p->rules[p->rule_count].input[C] = middle[1];
                p->rules[p->rule_count].input[E] = middle[2];
                free(middle);

                char* bottom = get_next_line(file); // *.*
                p->rules[p->rule_count].input[SW] = bottom[0];
                p->rules[p->rule_count].input[S] = bottom[1];
                p->rules[p->rule_count].input[SE] = bottom[2];
                free(bottom);

                // Allow for single line break in between input/output
                char* top_o = get_next_line(file);
                if (top_o[0] == '\0') {
                        free(top_o);
                        top_o = get_next_line(file);
                }
                p->rules[p->rule_count].output[NW] = top_o[0];
                p->rules[p->rule_count].output[N] = top_o[1];
                p->rules[p->rule_count].output[NE] = top_o[2];
                free(top_o);

                char* middle_o = get_next_line(file); // *s*
                p->rules[p->rule_count].output[W] = middle_o[0];
                p->rules[p->rule_count].output[C] = middle_o[1];
                p->rules[p->rule_count].output[E] = middle_o[2];
                free(middle_o);

                char* bottom_o = get_next_line(file); // *.*
                p->rules[p->rule_count].output[SW] = bottom_o[0];
                p->rules[p->rule_count].output[S] = bottom_o[1];
                p->rules[p->rule_count].output[SE] = bottom_o[2];
                free(bottom_o);

                p->rule_count += 1;

                char* buffer = get_next_line(file);
                if (buffer == NULL) {
                        // eof
                        break;
                }


                if (p->rule_count >= rule_size) {
                        rule_size *= 2;
                        p->rules = realloc(p->rules, rule_size * sizeof(rule));
                }
        }

        print_particle_type(p); // TODO remove

        return (p);
}






































