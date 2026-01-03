SRCDIRS = src
TEST_SRCDIRS = test
INCDIRS = include
BUILDDIR = build

CC = gcc
FLAGS = -Wall -Wextra -Werror $(foreach D, $(INCDIRS), -I$(D))
SDL_FLAGS = -lSDL3

SRCS = $(foreach D, $(SRCDIRS), $(wildcard $(D)/*.c))
OBJS = $(patsubst %.c, %.o, $(SRCS))

NAME = sandy

all: $(NAME)

$(NAME): $(OBJS) | $(BUILDDIR)
	$(CC) $^ -o $(NAME) $(SDL_FLAGS) 

$(BUILDDIR):
	mkdir -p $@

%.o: %.c
	$(CC) $(FLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

.PHONY: all clean fclean
