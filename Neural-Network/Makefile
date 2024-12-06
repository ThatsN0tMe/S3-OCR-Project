CC = gcc
CFLAGS = -Wall -Wextra 
LDFLAGS = -lz -lSDL2 -lSDL2_image -lm -O3

# Fichiers sources
SRC = temp.c
OBJ = $(SRC:.c=.o)

# Cible par défaut
all: main

# Compilation
main: $(OBJ)
	$(CC) $(OBJ) -o main $(LDFLAGS)

# Nettoyage
clean:
	rm -f $(OBJ) main

