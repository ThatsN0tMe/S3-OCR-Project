#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include "Detection.h"


double** getBrightness(SDL_Surface* surface) {

    Uint8 r, g, b;
    Uint32* pixels = (Uint32*)surface->pixels;

    int height = surface->h, width = surface->w;
    double** res = malloc(height * sizeof(double*));

    for (int y = 0; y < height; y++) {
        res[y] = malloc(width * sizeof(double));

        for (int x = 0; x < width; x++) {
            SDL_GetRGB(pixels[y * width + x], surface->format, &r, &g, &b);
            res[y][x] = 0.299*r + 0.587*g + 0.114*b;
        }
    }

    return res;
}


int** detectEdges(SDL_Surface* surface) {

    int height = surface->h, width = surface->w;
    int** res = malloc((height - 2) * sizeof(int*));
    double** matrix = getBrightness(surface);

    if (matrix == NULL) return NULL;

    for (int y = 1; y < height - 1; y++) {
        res[y-1] = malloc((width - 2) * sizeof(int));

        for (int x = 1; x < width - 1; x++) {
            res[y-1][x-1] = (int)sqrt(pow(-1 * matrix[y-1][x-1] - 2 * matrix[y][x-1] - 1 * matrix[y+1][x-1] + matrix[y-1][x+1] + 2 * matrix[y][x+1] + matrix[y+1][x+1], 2)
                             + pow(-1 * matrix[y-1][x-1] - 2 * matrix[y-1][x] - 1 * matrix[y-1][x+1] + matrix[y+1][x-1] + 2 * matrix[y+1][x] + matrix[y+1][x+1], 2)) / 4;
        }
    }

    for (int i = 0; i < height; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return res;
}


int** detectLines(SDL_Surface* surface, int* size, int* threshold) {

    int** matrix = detectEdges(surface);
    if (matrix == NULL) {
        puts("Error with edges detection");
        SDL_Quit();
        return NULL;
    }


    int width = surface->w - 2,
        height = surface->h - 2;
    *size = (int)sqrt(pow((double)height, 2) + pow((double)width, 2)) + 1;

    int** accumulatorArray = malloc((*size) * 2 * sizeof(int*));
    for (int i = 0; i < (*size) * 2; i++) {
        accumulatorArray[i] = malloc(180 * sizeof(int));
        for (int j = 0; j < 180; j++) {
            accumulatorArray[i][j] = 0;
        }
    }


    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            if (matrix[y][x] <= 50) continue;

            int rho;
            for (int theta = 0; theta < 180; theta++) {

                rho = x * cos(theta * M_PI / 180) + y * sin(theta * M_PI / 180);
                if (++accumulatorArray[rho + *size][theta] > *threshold) {
                    *threshold = accumulatorArray[rho + *size][theta];
                }
            }
        }
    }

    return accumulatorArray;
}