#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Detection.h"
#include "../Functions.h"


static int sqW = 0,
           sqH = 0,
           sqNum = 20,
           thresholdCoef = 4;
static SDL_Surface* surface = NULL;


int getPixelNum(int posX, int posY) {

    Uint8 r, g, b;
    Uint32* pixels = (Uint32*)surface->pixels;

    int pixelsSum = 0,
        x = posX < 0 ? 0 : posX;

    for (; x < posX + sqW; x++) {
        if (x >= surface->w) break;

        for (int y = posY < 0 ? 0 : posY; y < posY + sqH; y++) {
            if (y >= surface->h) break;

            SDL_GetRGB(pixels[y * surface->w + x], surface->format, &r, &g, &b);
            if (r < 150 && g < 150 && b < 150) {
                pixelsSum++;
            }
        }
    }

    return pixelsSum;
}



void pixelLimits(int* x, int* y) {
    if (*x < 0) *x = 0;
    else if (*x >= surface->w) *x = surface->w - 1;
    if (*y < 0) *y = 0;
    else if (*y >= surface->h) *y = surface->h - 1;
}

void resizeSides(int* x1, int* x2, int* y1, int* y2) {

    if (!isWhiteLine(surface, *x1, *y1, *x1, *y2)) {
        do {
            if (isWhiteLine(surface, *x1, *y1, *x1, *y2)) break;
            (*x1)--;
        } while (*x1 > 0);
    }
    else {
        do {
            if (!isWhiteLine(surface, *x1, *y1, *x1, *y2)) break;
            (*x1)++;
        } while (*x1 < surface->w - 1);
    }

    if (!isWhiteLine(surface, *x2, *y1, *x2, *y2)) {
        do {
            if (isWhiteLine(surface, *x2, *y1, *x2, *y2)) break;
            (*x2)++;
        } while (*x2 < surface->w - 1);
    }
    else {
        do {
            if (!isWhiteLine(surface, *x2, *y1, *x2, *y2)) break;
            (*x2)--;
        } while (*x2 > 0);
    }

    if (!isWhiteLine(surface, *x1, *y1, *x2, *y1)) {
        do {
            if (isWhiteLine(surface, *x1, *y1, *x2, *y1)) break;
            (*y1)--;
        } while (*y1 > 0);
    }
    else {
        do {
            if (!isWhiteLine(surface, *x1, *y1, *x2, *y1)) break;
            (*y1)++;
        } while (*y1 < surface->w - 1);
    }
    
    if (!isWhiteLine(surface, *x1, *y2, *x2, *y2)) {
        do {
            if (isWhiteLine(surface, *x1, *y2, *x2, *y2)) break;
            (*y2)++;
        } while (*y2 < surface->w - 1);
    }
    else {
        do {
            if (!isWhiteLine(surface, *x1, *y2, *x2, *y2)) break;
            (*y2)--;
        } while (*y2 > 0);
    }
}



void searchCorners(char* filepath) {
    int x = sqNum / 2 * sqW,
        y = sqNum / 2 * sqH,
        pixelNum = getPixelNum(x, y);
    
    int *topRight = searchTopRight(x, y, pixelNum),
        *topLeft = searchTopLeft(x, y, pixelNum),
        *bottomRight = searchBottomRight(x, y, pixelNum),
        *bottomLeft = searchBottomLeft(x, y, pixelNum);

    int x1 = Max(topLeft[0], bottomLeft[0]),
        x2 = Min(topRight[0], bottomRight[0]),
        y1 = Max(topLeft[1], topRight[1]),
        y2 = Min(bottomLeft[1], bottomRight[1]);
    
    free(topRight);
    free(topLeft);
    free(bottomRight);
    free(bottomLeft);

    pixelLimits(&x1, &y1);
    pixelLimits(&x2, &y2);
    resizeSides(&x1, &x2, &y1, &y2);
    x1 -= 10;
    x2 += 10;
    y1 -= 10;
    y2 += 10;
    pixelLimits(&x1, &y1);
    pixelLimits(&x2, &y2);

    detectLetters(surface, filepath, x1, x2, y1, y2);

    SDL_Rect rect = {x1, y1, x2 - x1, y2 - y1};
    Uint32 color = SDL_MapRGB(surface->format, 255, 255, 255);
    SDL_FillRect(surface, &rect, color);

    detectWords(surface, filepath);
}


int* searchTopRight(int x, int y, int pixels) {

    if (pixels > 0) {

        int pixelNum;
        if ((pixelNum = getPixelNum(x + sqW, y - sqH)) > pixels/thresholdCoef) {
            return searchTopRight(x + sqW, y - sqH, pixelNum);
        }
        pixels = getPixelNum(x + sqW, y) > getPixelNum(x, y - sqH) ? -1 : -2;
    }

    if (pixels == -1 && getPixelNum(x + sqW, y) > 0) {
        return searchTopRight(x + sqW, y, -1);
    }
    if (pixels == -2 && getPixelNum(x, y - sqH) > 0) {
        return searchTopRight(x, y - sqH, -2);
    }

    int* coords = malloc(2 * sizeof(int));
    coords[0] = x + sqW;
    coords[1] = y;
    
    return coords;
}


int* searchTopLeft(int x, int y, int pixels) {

    if (pixels > 0) {

        int pixelNum;
        if ((pixelNum = getPixelNum(x - sqW, y - sqH)) > pixels/thresholdCoef) {
            return searchTopLeft(x - sqW, y - sqH, pixelNum);
        }
        pixels = getPixelNum(x - sqW, y) > getPixelNum(x, y - sqH) ? -1 : -2;
    }

    if (pixels == -1 && getPixelNum(x - sqW, y) > 0) {
        return searchTopLeft(x - sqW, y, -1);
    }
    if (pixels == -2 && getPixelNum(x, y - sqH) > 0) {
        return searchTopLeft(x, y - sqH, -2);
    }

    int* coords = malloc(2 * sizeof(int));
    coords[0] = x;
    coords[1] = y;
    
    return coords;
}


int* searchBottomRight(int x, int y, int pixels) {
    
    if (pixels > 0) {

        int pixelNum;
        if ((pixelNum = getPixelNum(x + sqW, y + sqH)) > pixels/thresholdCoef) {
            return searchBottomRight(x + sqW, y + sqH, pixelNum);
        }
        pixels = getPixelNum(x + sqW, y) > getPixelNum(x, y + sqH) ? -1 : -2;
    }

    if (pixels == -1 && getPixelNum(x + sqW, y) > 0) {
        return searchBottomRight(x + sqW, y, -1);
    }
    if (pixels == -2 && getPixelNum(x, y + sqH) > 0) {
        return searchBottomRight(x, y + sqH, -2);
    }

    int* coords = malloc(2 * sizeof(int));
    coords[0] = x + sqW;
    coords[1] = y + sqH;
    
    return coords;
}


int* searchBottomLeft(int x, int y, int pixels) {

    if (pixels > 0) {

        int pixelNum;
        if ((pixelNum = getPixelNum(x - sqW, y + sqH)) > pixels/thresholdCoef) {
            return searchBottomLeft(x - sqW, y + sqH, pixelNum);
        }
        pixels = getPixelNum(x - sqW, y) > getPixelNum(x, y + sqH) ? -1 : -2;
    }

    if (pixels == -1 && getPixelNum(x - sqW, y) > 0) {
        return searchBottomLeft(x - sqW, y, -1);
    }
    if (pixels == -2 && getPixelNum(x, y + sqH) > 0) {
        return searchBottomLeft(x, y + sqH, -2);
    }

    int* coords = malloc(2 * sizeof(int));
    coords[0] = x;
    coords[1] = y + sqH;

    return coords;
}



void detectGrid(char* filepath) {
    if (filepath == NULL) {
        puts("Filepath is undefined");
        return;
    }

    surface = IMG_Load(filepath);
    if (surface == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }

    sqW = (surface->w - 1) / sqNum;
    sqH = (surface->h - 1) / sqNum;

    searchCorners(filepath);

    if (SDL_SaveBMP(surface, filepath) != 0) {
        printf("Image saving error: %s\n", SDL_GetError());
    }
    else {
        printf("Image processed successfully and saved as '%s'.\n", filepath);
    }
}