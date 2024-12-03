CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lz

# Fichiers sources
SRC = main.c read_emnist.c
OBJ = $(SRC:.c=.o)

# Cible par défaut
all: main

# Compilation
main: $(OBJ)
	$(CC) $(OBJ) -o main $(LDFLAGS)

# Nettoyage
clean:
	rm -f $(OBJ) main

