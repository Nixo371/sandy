#ifndef CONSTANTS_H
# define CONSTANTS_H

# define WIDTH 900
# define HEIGHT 600

# define FPS 120
# define FRAME_TIME_MS (1000 / FPS)
# define FRAME_TIME_NS (1000000000 / FPS)

# define DEFAULT_BLOB_RADIUS 10
# define MIN_BLOB_RADIUS 1
# define MAX_BLOB_RADIUS 100

# define CURSOR_THICKNESS 2

// Colors //
# define WHITE  (SDL_Color) { 255, 255, 255, SDL_ALPHA_OPAQUE }
# define BLACK  (SDL_Color) {   0,   0,   0, SDL_ALPHA_OPAQUE }
# define RED    (SDL_Color) { 255,   0,   0, SDL_ALPHA_OPAQUE }
# define ORANGE (SDL_Color) { 255, 127,   0, SDL_ALPHA_OPAQUE }
# define YELLOW (SDL_Color) { 255, 255,   0, SDL_ALPHA_OPAQUE }
# define GREEN  (SDL_Color) {   0, 255,   0, SDL_ALPHA_OPAQUE }
# define BLUE   (SDL_Color) {   0,   0, 255, SDL_ALPHA_OPAQUE }
# define PURPLE (SDL_Color) { 255,   0, 255, SDL_ALPHA_OPAQUE }

#endif
