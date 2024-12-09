#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <libgen.h>

#include "Detection.h"
#include "../Functions.h"
#include "../Rotate/rotate.h"
#include "../Interface/Interface.h"
#include "../Solver/solver.h"
#include "../Neural-Network/convert.h"


static int x1 = 0, yy1 = 0, x2 = 0, y2 = 0;
static char* path = NULL;
static SDL_Surface* surface = NULL;
static SDL_Surface* originalSurface = NULL;

int*** letterCoords = NULL;
int*** words = NULL;

int lines, columns, numWords;


void freeSurface() {
    if (surface != NULL) {
        SDL_FreeSurface(surface);
        SDL_FreeSurface(originalSurface);
        surface = NULL;
        originalSurface = NULL;
    }
    x1 = 0;
    yy1 = 0;
    x2 = 0;
    y2 = 0;
}

void saveSurface() {
    if (path == NULL || surface == NULL) {
        puts("Cannot save image.");
        return;
    }

    save(surface, path);
    create_detection_window(path);
}


void gridDetection() {
    autoRotate(path, 1);
    surface = IMG_Load(path);

    int* coords = detectGrid(surface);
    x1 = coords[0];
    yy1 = coords[1];
    x2 = coords[2];
    y2 = coords[3];
    free(coords);

    if (x1 != x2 && yy1 != y2) {
        printf("Grid was successfully detected at coordinates : %d, %d  |  %d, %d\n", x1, yy1, x2, y2);
    }

    /*drawLineOnSurface(surface, x1, yy1, x2, yy1);
    drawLineOnSurface(surface, x2, yy1, x2, y2);
    drawLineOnSurface(surface, x1, yy1, x1, y2);
    drawLineOnSurface(surface, x1, y2, x2, y2);*/
    
    saveSurface();
}

void letterDetection() {
    if (x1 == x2 || yy1 == y2) {
        puts("Cannot detect letters, grid was not correctly detected before.");
        return;
    }
    letterCoords = detectLetters(surface, path, x1, x2, yy1, y2, &lines, &columns);
    saveSurface();
}

void wordDetection() {
    if (x1 == x2 || yy1 == y2) {
        puts("Cannot detect words, grid was not correctly detected before.");
        return;
    }

    SDL_Rect rect = {x1, yy1, x2 - x1, y2 - yy1};
    Uint32 color = SDL_MapRGB(surface->format, 255, 255, 255);
    SDL_FillRect(surface, &rect, color);

    words = detectWords(surface, path, &numWords);
    saveSurface();
}

void print_grid(char** grid, int lines, int columns) {
    printf("The grid size is %d lines * %d columns :\n\n", lines, columns);
    for (int y = -4; y < lines; y++) {
        if (y >= 0)
            printf(" %d |", y);
        for (int x = 0; x < columns; x++) {
            if (y == -4 || y == -2) {
                printf("    ");
                y++;
            }
            if (y == -3)
                printf(" %d", x);
            else if (y == -1)
                printf(" —");
            else
                printf(" %c", grid[y][x]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_result() {
    if (letterCoords == NULL || words == NULL || x1 == x2 || yy1 == y2) {
        puts("Cannot print the result the previous steps were not performed.");
        return;
    }

    char** grid = malloc(lines * sizeof(char*));
    for (int i = 0; i < lines; i++) {
        grid[i] = malloc(columns);
        for (int j = 0; j < columns; j++) {
            char* dir = strdup(path);
            char* image_path = dirname(dir);

            char output_file[256];
            snprintf(output_file, sizeof(output_file), "%s/letter/letter_%d_%d.png", image_path, i, j);

            grid[i][j] = get_char(output_file);
            free(dir);
        }
    }

    print_grid(grid, lines, columns);
    
    save(originalSurface, path);
    autoRotate(path, 1);

    for (int i = 0; i < numWords; i++) {
		char word[32];
        if (words[i] == NULL) break;

		int j = 0;
		while (words[i][j] != NULL) {
            // Couper et sauvegarder chaque mot
            char* dir = strdup(path);
            char* word_path = dirname(dir);

            char output_file[256];
            snprintf(output_file, sizeof(output_file), "%s/word/word_%d_letter_%d.png", word_path, i, j);
            
            word[j] = get_char(output_file);

            free(dir);
			j++;
		}
        word[j] = '\0';
        printf("Word: %s\n", word);
        find_word(path, grid, letterCoords, lines, columns, word);
	}

    create_detection_window(path);

    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            free(letterCoords[i][j]);
        }
        free(letterCoords[i]);
        free(grid[i]);
    }
    free(letterCoords);
    free(grid);

    for (int i = 0; i < numWords; i++) {
		int j = 0;
		do {
			free(words[i][j]);
			j++;
		} while (words[i][j] != NULL);
		free(words[i]);
	}
	free(words);
}


void detection(char* filepath) {

    if (filepath == NULL) {
        puts("Filepath is undefined");
        return;
    }
    path = filepath;

    surface = IMG_Load(filepath);
    if (surface == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return;
    }
    originalSurface = IMG_Load(filepath);

    create_detection_window(filepath);
}