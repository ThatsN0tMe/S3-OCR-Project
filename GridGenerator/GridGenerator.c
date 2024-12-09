#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define GRID_SIZE 10
#define MAX_WORD_LENGTH 50
#define MAX_WORDS 100

// Remplit la grille avec des lettres aléatoires
void fill_with_random_letters(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == '\0') {
                grid[i][j] = 'A' + rand() % 26;
            }
        }
    }
}

// Ajoute un mot à la grille à une position aléatoire
int place_word_in_grid(char grid[GRID_SIZE][GRID_SIZE], const char *word) {
    int word_length = strlen(word);
    if (word_length > GRID_SIZE) {
        return 0; // Le mot est trop long pour la grille
    }

    int row, col, attempts = 100;
    while (attempts--) {
        row = rand() % GRID_SIZE;
        col = rand() % (GRID_SIZE - word_length + 1);

        // Vérifie si l'espace est libre
        int can_place = 1;
        for (int i = 0; i < word_length; i++) {
            if (grid[row][col + i] != '\0') {
                can_place = 0;
                break;
            }
        }

        if (can_place) {
            // Place le mot dans la grille
            for (int i = 0; i < word_length; i++) {
                grid[row][col + i] = toupper(word[i]);
            }
            return 1;
        }
    }
    return 0; // Échec après plusieurs tentatives
}

// Génère une grille avec des mots cachés
void generate_grid(char grid[GRID_SIZE][GRID_SIZE], const char *words[], int word_count) {
    // Initialise la grille
    memset(grid, 0, sizeof(char) * GRID_SIZE * GRID_SIZE);
    srand(time(NULL));

    // Place chaque mot dans la grille
    for (int i = 0; i < word_count; i++) {
        if (!place_word_in_grid(grid, words[i])) {
            printf("Impossible de placer le mot : %s\n", words[i]);
        }
    }

    // Remplit le reste de la grille avec des lettres aléatoires
    fill_with_random_letters(grid);
}

// Affiche la grille
void print_grid(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

// Lit les mots depuis un fichier et les stocke dans un tableau
int read_words_from_file(const char *filename, char words[MAX_WORDS][MAX_WORD_LENGTH]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        return 0;
    }

    int count = 0;
    while (fgets(words[count], MAX_WORD_LENGTH, file) && count < MAX_WORDS) {
        // Retirer le saut de ligne
        words[count][strcspn(words[count], "\n")] = '\0';
        count++;
    }

    fclose(file);
    return count;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s <fichier de mots>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char words[MAX_WORDS][MAX_WORD_LENGTH];
    int word_count = read_words_from_file(argv[1], words);

    if (word_count == 0) {
        fprintf(stderr, "Aucun mot trouvé dans le fichier.\n");
        return EXIT_FAILURE;
    }

    // Convertit le tableau 2D de mots en tableau de pointeurs
    const char *word_pointers[MAX_WORDS];
    for (int i = 0; i < word_count; i++) {
        word_pointers[i] = words[i];
    }

    char grid[GRID_SIZE][GRID_SIZE];
    generate_grid(grid, word_pointers, word_count);

    printf("Grille générée :\n");
    print_grid(grid);

    return EXIT_SUCCESS;
}

