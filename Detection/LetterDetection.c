#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <libgen.h>
#include "Detection.h"
#include "../Functions.h"
#include "../Solver/solver.h"


static int pixelToleranceX = 15,
           pixelToleranceY = 15;


int isWhiteLine(SDL_Surface* surface, int x1, int y1, int x2, int y2) {

    if (x1 != x2 && y1 != y2) return 0;
    
    Uint8 r, g, b;
    Uint32* pixels = (Uint32*)surface->pixels;

    if (x1 != x2) {

        if (x1 > x2)
            swap(&x1, &x2);
        if (x1 < 0 || x2 >= surface->w || y1 < 0 || y1 > surface->h)
            return 0;

        for (int x = x1; x < x2; x++) {
            SDL_GetRGB(pixels[y1 * surface->w + x], surface->format, &r, &g, &b);
            if (r < 200 || g < 200 || b < 200)
                return 0;
        }
        return 1;
    }

    if (y1 != y2) {

        if (y1 > y2)
            swap(&y1, &y2);
        if (y1 < 0 || y2 >= surface->h || x1 < 0 || x1 > surface->w)
            return 0;

        for (int y = y1; y < y2; y++) {
            SDL_GetRGB(pixels[y * surface->w + x1], surface->format, &r, &g, &b);
            if (r < 200 || g < 200 || b < 200)
                return 0;
        }
        return 1;
    }
    return 0;
}


int getNumLines(SDL_Surface* surface, int x1, int x2, int* y1, int y2) {

    int numLines = 0,
        nextLine = 1,
        currentSize = 0,
        averageSize = 0;

    for (int y = *y1; y < y2; y++) {
        if (isWhiteLine(surface, x1 + pixelToleranceX, y, x2 - pixelToleranceX, y)) {
            nextLine = 1;
        }
        else if (nextLine) {
            numLines++;
            nextLine = 0;
            averageSize += currentSize;
            currentSize = 0;
        }
        currentSize++;
    }

    if (numLines < 5)
        return numLines;
    if (numLines > 0)
        averageSize /= numLines;

    nextLine = 0;
    currentSize = 0;

    for (int y = *y1; y < y2; y++) {

        if (isWhiteLine(surface, x1 + pixelToleranceX, y, x2 - pixelToleranceX, y)) {
            if (!nextLine) {
                currentSize++;
                continue;
            }
            if ((double)currentSize > (double)averageSize * 1.45) {
                *y1 = y;
                return numLines - 1;
            }
            break;
        }
        
        nextLine = 1;
        currentSize++;
    }

    return numLines;
}


int getNumColumns(SDL_Surface* surface, int x1, int x2, int y1, int y2) {

    int numColumns = 0,
        nextColumn = 1;

    for (int x = x1; x < x2; x++) {
        if (isWhiteLine(surface, x, y1 + pixelToleranceY, x, y2 - pixelToleranceY)) {
            nextColumn = 1;
        }
        else if (nextColumn) {
            nextColumn = 0;
            numColumns++;
        }
    }

    return numColumns;
}



void removeLines(SDL_Surface* surface, int x1, int x2, int y1, int y2) {
    
    int size = 0,
        threshold = 0;
    int** accumulatorArray = detectLines(surface, &size, &threshold);

    threshold -= threshold / 4;

    for (int theta = 0; theta < 180; theta++) {
        for (int rho = 0; rho < size * 2; rho++) {

            if (accumulatorArray[rho][theta] > threshold) {

                int* coords = polarToCartesian((double)(rho - size), (double)theta, x2 - x1, y2 - y1);

                if (coords[0] == coords[2]) {
                    coords[1] += y1;
                    coords[3] += y1;
                    drawWhiteRect(surface, coords[0] - 5, coords[1], 10, coords[3] - coords[1]);
                }
                else if (coords[1] == coords[3]) {
                    coords[0] += x1;
                    coords[2] += x1;
                    drawWhiteRect(surface, coords[0], coords[1] - 5, coords[2] - coords[0], 10);
                }
            }
        }
    }
    
    for (int rho = 0; rho < size * 2; rho++) {
        free(accumulatorArray[rho]);
    }
    free(accumulatorArray);
}



int*** detectLetters(SDL_Surface* surface, char* filepath, int x1, int x2, int y1, int y2, int* lines, int* columns) {

    //Nombre de lignes et de colonnes de lettres
    *lines = getNumLines(surface, x1, x2, &y1, y2);
    *columns = getNumColumns(surface, x1, x2, y1, y2);

    if (*lines + *columns < 10) {
        removeLines(surface, x1, x2, y1, y2);
        *lines = getNumLines(surface, x1, x2, &y1, y2);
        *columns = getNumColumns(surface, x1, x2, y1, y2);
    }

    //Liste 3 dimensionnelle dont la 3eme represente les corners topleft et bottomright de la lettre : x1, y1 | x2, y2
    int*** letterCoords = calloc(sizeof(int*), *lines);
    for (int i = 0; i < *lines; i++) {
        letterCoords[i] = calloc(sizeof(int*), *columns);
        for (int j = 0; j < *columns; j++) {
            letterCoords[i][j] = calloc(sizeof(int), 4);
        }
    }


    // -----------------------------------------------------------------------


    int nextTopLine = 1,
        lineCounter = 0;

    for (int y = y1 + 1; y < y2; y++) {

        if (!isWhiteLine(surface, x1 + pixelToleranceX, y, x2 - pixelToleranceX, y)) {
            if (nextTopLine) {

                for (int i = 0; i < *columns; i++) {
                    letterCoords[lineCounter][i][1] = y - 1;
                }
                nextTopLine = 0;
            }
        }
        else if (!nextTopLine) {

            for (int i = 0; i < *columns; i++) {
                letterCoords[lineCounter][i][3] = y;
            }
            nextTopLine = 1;
            lineCounter++;
        }
    }
    
    // -----------------------------------------------------------------------


    int nextLeftColumn = 1,
        columnCounter = 0;

    for (int x = x1 + 1; x < x2; x++) {

        if (!isWhiteLine(surface, x, y1 + pixelToleranceY, x, y2 - pixelToleranceY)) {
            if (nextLeftColumn) {

                for (int i = 0; i < *lines; i++) {
                    letterCoords[i][columnCounter][0] = x - 1;
                }
                nextLeftColumn = 0;
            }
        }
        else if (!nextLeftColumn) {

            for (int i = 0; i < *lines; i++) {
                letterCoords[i][columnCounter][2] = x;
            }
            nextLeftColumn = 1;
            columnCounter++;
        }
    }

    // -----------------------------------------------------------------------


    for (int i = 0; i < *lines; i++) {
        for (int j = 0; j < *columns; j++) {

            int* coords = letterCoords[i][j];

            drawLineOnSurface(surface, coords[0], coords[1], coords[2], coords[1]);
            drawLineOnSurface(surface, coords[2], coords[1], coords[2], coords[3]);
            drawLineOnSurface(surface, coords[0], coords[1], coords[0], coords[3]);
            drawLineOnSurface(surface, coords[0], coords[3], coords[2], coords[3]);

            // Couper et sauvegarder chaque lettre
            char* dir = strdup(filepath);
            char* path = dirname(dir);

            char output_file[256];
            snprintf(output_file, sizeof(output_file), "%s/letter/letter_%d_%d.png", path, i, j);

            cut_and_save(filepath, output_file, coords[0], coords[1], coords[2], coords[3]);

            free(dir);
        }
    }

    return letterCoords;
}