SRCDIRS = src
TEST_SRCDIRS = test
INCDIRS = include
BUILDDIR = build

CC = gcc
FLAGS = -Wall -Wextra -Werror $(foreach D, $(INCDIRS), -I$(D))
SDL_FLAGS = -lSDL3 -lSDL3_ttf

SRCS = $(foreach D, $(SRCDIRS), $(wildcard $(D)/*.c))
OBJS = $(patsubst %.c, %.o, $(SRCS))

NAME = sandy

all: $(BUILDDIR)/$(NAME)

$(BUILDDIR)/$(NAME): $(BUILDDIR)
$(BUILDDIR)/$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(BUILDDIR)/$(NAME) $(SDL_FLAGS)

$(BUILDDIR):
	mkdir -p $@

%.o: %.c
	$(CC) $(FLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

.PHONY: all clean fclean
