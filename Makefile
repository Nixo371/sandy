SRCDIR = src
INCDIRS = include
OBJDIR = obj
BUILDDIR = build

CC = gcc
FLAGS = -Wall -Wextra -Werror $(foreach D, $(INCDIRS), -I$(D))
SDL_FLAGS = -lSDL3 -lSDL3_ttf

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

NAME = sandy
BIN = $(BUILDDIR)/$(NAME)

all: $(BIN)

$(BIN): $(OBJS) | $(BUILDDIR)
	$(CC) $(OBJS) -O3 -o $(BUILDDIR)/$(NAME) $(SDL_FLAGS)

$(OBJDIR):
	mkdir -p $@

$(BUILDDIR):
	mkdir -p $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(FLAGS) -O3 -c -o $@ $<

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(BIN)

.PHONY: all clean fclean
