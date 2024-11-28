#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Detection.h"
#include "../Functions.h"



int pixelTolerance = 10;


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


int getNumLines(SDL_Surface* surface, int x1, int x2, int y1, int y2) {

    int numLines = 0,
        nextLine = 1;

    for (int y = y1; y < y2; y++) {
        if (isWhiteLine(surface, x1 + pixelTolerance, y, x2 - pixelTolerance, y)) {
            if (nextLine) {
                nextLine = 0;
                numLines++;
            }
        }
        else if (!nextLine) {
            nextLine = 1;
        }
    }

    return numLines;
}


int getNumColumns(SDL_Surface* surface, int x1, int x2, int y1, int y2) {

    int numColumns = 0,
        nextColumn = 1;

    for (int x = x1; x < x2; x++) {
        if (isWhiteLine(surface, x, y1 + pixelTolerance, x, y2 - pixelTolerance)) {
            if (nextColumn) {
                nextColumn = 0;
                numColumns++;
            }
        }
        else if (!nextColumn) {
            nextColumn = 1;
        }
    }

    return numColumns;
}



void detectLetters(SDL_Surface* surface, int x1, int x2, int y1, int y2) {

    //Nombre de lignes et de colonnes de lettres
    int lines = getNumLines(surface, x1, x2, y1, y2) - 1,
        columns = getNumColumns(surface, x1, x2, y1, y2) - 1;

    //Liste 3 dimensionnelle dont la 3eme represente les corners topleft et bottomright de la lettre : x1, y1 | x2, y2
    int*** letterCoords = calloc(sizeof(int*), lines);
    for (int i = 0; i < lines; i++) {
        letterCoords[i] = calloc(sizeof(int*), columns);
        for (int j = 0; j < columns; j++) {
            letterCoords[i][j] = calloc(sizeof(int), 4);
        }
    }

    // -----------------------------------------------------------------------


    int nextLine = 1,
        lineCounter = 0;

    for (int y = y1; y < y2; y++) {

        if (!isWhiteLine(surface, x1 + pixelTolerance, y, x2 - pixelTolerance, y)) {
            if (!nextLine) {
                nextLine = 1;
            }
        }
        else if (nextLine) {

            if (lineCounter < lines) {
                for (int i = 0; i < columns; i++) {
                    letterCoords[lineCounter][i][1] = y;
                }
            }
            if (lineCounter > 0) {
                for (int i = 0; i < columns; i++) {
                    letterCoords[lineCounter - 1][i][3] = y;
                }
            }
            
            nextLine = 0;
            lineCounter++;
        }
    }
    
    // -----------------------------------------------------------------------


    int nextColumn = 1,
        columnCounter = 0;

    for (int x = x1; x < x2; x++) {

        if (!isWhiteLine(surface, x, y1 + pixelTolerance, x, y2 - pixelTolerance)) {
            if (!nextColumn) {
                nextColumn = 1;
            }
        }
        else if (nextColumn) {

            if (columnCounter < columns) {
                for (int i = 0; i < lines; i++) {
                    letterCoords[i][columnCounter][0] = x;
                }
            }
            if (columnCounter > 0) {
                for (int i = 0; i < lines; i++) {
                    letterCoords[i][columnCounter - 1][2] = x;
                }
            }

            nextColumn = 0;
            columnCounter++;
        }
    }

    // -----------------------------------------------------------------------


    for (int i = 0; i < lines; i++) {
        for (int j = 0; j < columns; j++) {
            int* coords = letterCoords[i][j];
            drawLineOnSurface(surface, coords[0], coords[1], coords[2], coords[1]);
            drawLineOnSurface(surface, coords[2], coords[1], coords[2], coords[3]);
            drawLineOnSurface(surface, coords[0], coords[1], coords[0], coords[3]);
            drawLineOnSurface(surface, coords[0], coords[3], coords[2], coords[3]);
        }
    }
}