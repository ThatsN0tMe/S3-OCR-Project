CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wvla -D_GNU_SOURCE
LDFLAGS = -lSDL2 -lm -lSDL2_image
SPEED := -Ofast -march=native -mtune=native -mavx2 

SRC = \
			convert.c \
			neural-net/Network.c \
			neural-net/network_functions.c \
			shared/math_helpers.c \
			shared/arr_helpers.c 

OBJ = convert.o Network.o \
			math_helpers.o arr_helpers.o network_functions.o
BIN = convert


all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $^ $(SPEED) -o $@ $(LDFLAGS)

$(OBJ):
	$(CC) $(CFLAGS) $(SPEED) -c $(SRC)

debug: CFLAGS += -fsanitize=address -g
debug: LDFLAGS += -fsanitize=address -g
debug: $(BIN)

.PHONY: clean
clean:
	$(RM) $(BIN) $(OBJ)
