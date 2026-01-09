#include "sandy.h"

#include "constants.h"

int in_bounds(int x, int y) {
	if (x < 0 || x >= WIDTH) {
		return (0);
	}
	if (y < 0 || y >= HEIGHT) {
		return (0);
	}

	return (1);
}

