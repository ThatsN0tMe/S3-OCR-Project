#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Detection.h"


int sqW = 0,
    sqH = 0,
    sqNum = 20,
    thresholdCoef = 4;



int Min(int a, int b) {
    if (a > b) return b;
    return a;
}
int Max(int a, int b) {
    if (a > b) return a;
    return b;
}


void drawSquare(SDL_Surface* surface, int posX, int posY) {
    
    SDL_Rect rect = {posX, posY, sqW, sqH};
    Uint32 color = SDL_MapRGB(surface->format, 50, 50, 50);
    SDL_FillRect(surface, &rect, color);
}

int getPixelNum(SDL_Surface* surface, int posX, int posY) {

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



void searchCorners(SDL_Surface* surface) {
    int x = sqNum / 2 * sqW,
        y = sqNum / 2 * sqH,
        pixelNum = getPixelNum(surface, x, y);
    
    int *topRight = searchTopRight(surface, x, y, pixelNum),
        *topLeft = searchTopLeft(surface, x, y, pixelNum),
        *bottomRight = searchBottomRight(surface, x, y, pixelNum),
        *bottomLeft = searchBottomLeft(surface, x, y, pixelNum);

    //prendre minimun de chaque corner

    int x1 = Max(topLeft[0], bottomLeft[0]),
        x2 = Min(topRight[0], bottomRight[0]),
        y1 = Max(topLeft[1], topRight[1]),
        y2 = Min(bottomLeft[1], bottomRight[1]);
    
    free(topRight);
    free(topLeft);
    free(bottomRight);
    free(bottomLeft);

    drawLineOnSurface(surface, x1, y1, x2, y1);
    drawLineOnSurface(surface, x2, y1, x2, y2);
    drawLineOnSurface(surface, x2, y2, x1, y2);
    drawLineOnSurface(surface, x1, y2, x1, y1);
}


int* searchTopRight(SDL_Surface* surface, int x, int y, int pixels) {
    
    drawSquare(surface, x, y);

    if (pixels > 0) {

        int pixelNum;
        if ((pixelNum = getPixelNum(surface, x + sqW, y - sqH)) > pixels/thresholdCoef) {
            return searchTopRight(surface, x + sqW, y - sqH, pixelNum);
        }
        pixels = getPixelNum(surface, x + sqW, y) > getPixelNum(surface, x, y - sqH) ? -1 : -2;
    }

    if (pixels == -1 && getPixelNum(surface, x + sqW, y) > 0) {
        return searchTopRight(surface, x + sqW, y, -1);
    }
    if (pixels == -2 && getPixelNum(surface, x, y - sqH) > 0) {
        return searchTopRight(surface, x, y - sqH, -2);
    }

    int* coords = malloc(2 * sizeof(int));
    coords[0] = x + sqW;
    coords[1] = y;
    
    return coords;
}


int* searchTopLeft(SDL_Surface* surface, int x, int y, int pixels) {
    
    drawSquare(surface, x, y);

    if (pixels > 0) {

        int pixelNum;
        if ((pixelNum = getPixelNum(surface, x - sqW, y - sqH)) > pixels/thresholdCoef) {
            return searchTopLeft(surface, x - sqW, y - sqH, pixelNum);
        }
        pixels = getPixelNum(surface, x - sqW, y) > getPixelNum(surface, x, y - sqH) ? -1 : -2;
    }

    if (pixels == -1 && getPixelNum(surface, x - sqW, y) > 0) {
        return searchTopLeft(surface, x - sqW, y, -1);
    }
    if (pixels == -2 && getPixelNum(surface, x, y - sqH) > 0) {
        return searchTopLeft(surface, x, y - sqH, -2);
    }

    int* coords = malloc(2 * sizeof(int));
    coords[0] = x;
    coords[1] = y;
    
    return coords;
}


int* searchBottomRight(SDL_Surface* surface, int x, int y, int pixels) {
    
    drawSquare(surface, x, y);

    if (pixels > 0) {

        int pixelNum;
        if ((pixelNum = getPixelNum(surface, x + sqW, y + sqH)) > pixels/thresholdCoef) {
            return searchBottomRight(surface, x + sqW, y + sqH, pixelNum);
        }
        pixels = getPixelNum(surface, x + sqW, y) > getPixelNum(surface, x, y + sqH) ? -1 : -2;
    }

    if (pixels == -1 && getPixelNum(surface, x + sqW, y) > 0) {
        return searchBottomRight(surface, x + sqW, y, -1);
    }
    if (pixels == -2 && getPixelNum(surface, x, y + sqH) > 0) {
        return searchBottomRight(surface, x, y + sqH, -2);
    }

    int* coords = malloc(2 * sizeof(int));
    coords[0] = x + sqW;
    coords[1] = y + sqH;
    
    return coords;
}


int* searchBottomLeft(SDL_Surface* surface, int x, int y, int pixels) {
    
    drawSquare(surface, x, y);

    if (pixels > 0) {

        int pixelNum;
        if ((pixelNum = getPixelNum(surface, x - sqW, y + sqH)) > pixels/thresholdCoef) {
            return searchBottomLeft(surface, x - sqW, y + sqH, pixelNum);
        }
        pixels = getPixelNum(surface, x - sqW, y) > getPixelNum(surface, x, y + sqH) ? -1 : -2;
    }

    if (pixels == -1 && getPixelNum(surface, x - sqW, y) > 0) {
        return searchBottomLeft(surface, x - sqW, y, -1);
    }
    if (pixels == -2 && getPixelNum(surface, x, y + sqH) > 0) {
        return searchBottomLeft(surface, x, y + sqH, -2);
    }

    int* coords = malloc(2 * sizeof(int));
    coords[0] = x;
    coords[1] = y + sqH;
    
    return coords;
}



void detectGrids(char* filepath) {
    if (filepath == NULL) {
        puts("Filepath is undefined");
        return;
    }

    SDL_Surface *surface = IMG_Load(filepath);
    if (!surface) {
        printf("Error loading image: %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }

    int** matrix = detectEdges(surface);
    if (matrix == NULL) {
        puts("Error with get edges detection");
        SDL_Quit();
        return;
    }

    int width = surface->w,
        height = surface->h;
    sqW = (width - 1) / sqNum;
    sqH = (height - 1) / sqNum;
    
    printf("sqW : %d  |  width : %d  |  newW : %d\n", sqW, width, sqW * sqNum);
    printf("sqH : %d  |  heigth : %d  |  newH : %d\n", sqH, height, sqH * sqNum);

    searchCorners(surface);

    if (SDL_SaveBMP(surface, filepath) != 0) {
        printf("Image saving error: %s\n", SDL_GetError());
    }
    else {
        printf("Image processed successfully and saved as '%s'.\n", filepath);
    }
}