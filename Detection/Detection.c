#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Detection.h"
#include "../Functions.h"
#include "../Rotate/rotate.h"
#include "../Interface/Interface.h"
#include "../Solver/solver.h"


static int x1 = 0, yy1 = 0, x2 = 0, y2 = 0;
static char* path = NULL;
static SDL_Surface* surface = NULL;

int*** letterCoords = NULL;
int lines, columns;

void freeSurface() {
    if (surface != NULL) {
        SDL_FreeSurface(surface);
        surface = NULL;
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

    detectWords(surface, path);
    saveSurface();
}

void print_result() {
    if (letterCoords == NULL || x1 == x2 || yy1 == y2) {
        puts("Cannot print the result the previous steps were not performed.");
        return;
    }

    char* grid[] = {
        "MSWATERMELON",
        "YTBNEPEWRMAE",
        "RRLWPAPAYANA",
        "RANLEMONANEP",
        "EWLEAPRIABPR",
        "BBILBBWBRLAY",
        "KEMPMAWLRARB",
        "CREPRNRERRGR",
        "ARYAYAOANLAM",
        "LYYARNERKIWI",
        "BEBAAANAAPRT",
        "YRREBPSARNNW",
        "YRREBEULBLGI",
        "TYPATEAEPACE"
    };

    char* source_path = getSourcePath();

    if (source_path == NULL)
        return;

    find_word(&source_path, grid, letterCoords, 14, 12, "apple");
    find_word(&source_path, grid, letterCoords, 14, 12, "lemon");
    find_word(&source_path, grid, letterCoords, 14, 12, "banana");
    find_word(&source_path, grid, letterCoords, 14, 12, "lime");
    find_word(&source_path, grid, letterCoords, 14, 12, "orange");
    find_word(&source_path, grid, letterCoords, 14, 12, "watermelon");
    find_word(&source_path, grid, letterCoords, 14, 12, "grape");
    find_word(&source_path, grid, letterCoords, 14, 12, "kiwi");
    find_word(&source_path, grid, letterCoords, 14, 12, "strawberry");
    find_word(&source_path, grid, letterCoords, 14, 12, "blackberry");
    find_word(&source_path, grid, letterCoords, 14, 12, "papaya");
    find_word(&source_path, grid, letterCoords, 14, 12, "blueberry");
    find_word(&source_path, grid, letterCoords, 14, 12, "raspberry");

    create_detection_window(path);

    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            free(letterCoords[i][j]);
        }
        free(letterCoords[i]);
    }
    free(letterCoords);
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

    create_detection_window(filepath);
}