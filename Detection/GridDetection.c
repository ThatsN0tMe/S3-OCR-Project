#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Detection.h"


int sqW = 0,
    sqH = 0,
    sqNum = 20;


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

    drawSquare(surface, x, y);
    
    int *topRight = searchTopRight(surface, x, y, pixelNum),
        *topLeft = searchTopLeft(surface, x, y, pixelNum),
        *bottomRight = searchBottomRight(surface, x, y, pixelNum),
        *bottomLeft = searchBottomLeft(surface, x, y, pixelNum);

    drawLineOnSurface(surface, topLeft[0], topLeft[1], topRight[0], topRight[1]);
    drawLineOnSurface(surface, topRight[0], topRight[1], bottomRight[0], bottomRight[1]);
    drawLineOnSurface(surface, bottomRight[0], bottomRight[1], bottomLeft[0], bottomLeft[1]);
    drawLineOnSurface(surface, bottomLeft[0], bottomLeft[1], topLeft[0], topLeft[1]);
}


int* searchTopRight(SDL_Surface* surface, int x, int y, int pixels) {
    
    int pixelNum = 0,
        *coords = malloc(2 * sizeof(int));

    coords[0] = x;
    coords[1] = y;
    pixels /= 4;

    if ((pixelNum = getPixelNum(surface, x + sqW, y - sqH)) > pixels) {
        coords[0] += sqW;
        coords[1] -= sqH;
    }
    else if ((pixelNum = getPixelNum(surface, x, y - sqH)) > pixels) {
        coords[1] -= sqH;
    }
    else if ((pixelNum = getPixelNum(surface, x + sqW, y)) > pixels) {
        coords[0] += sqW;
    }
    else {
        coords[0] += sqW;
        return coords;
    }

    return searchTopRight(surface, coords[0], coords[1], pixelNum);
}


int* searchTopLeft(SDL_Surface* surface, int x, int y, int pixels) {
    
    int pixelNum = 0,
        *coords = malloc(2 * sizeof(int));

    coords[0] = x;
    coords[1] = y;
    pixels /= 4;

    if ((pixelNum = getPixelNum(surface, x - sqW, y - sqH)) > pixels) {
        coords[0] -= sqW;
        coords[1] -= sqH;
    }
    else if ((pixelNum = getPixelNum(surface, x, y - sqH)) > pixels) {
        coords[1] -= sqH;
    }
    else if ((pixelNum = getPixelNum(surface, x - sqW, y)) > pixels) {
        coords[0] -= sqW;
    }
    else {
        return coords;
    }

    return searchTopLeft(surface, coords[0], coords[1], pixelNum);
}


int* searchBottomRight(SDL_Surface* surface, int x, int y, int pixels) {
    
    int pixelNum = 0,
        *coords = malloc(2 * sizeof(int));

    coords[0] = x;
    coords[1] = y;
    pixels /= 4;

    if ((pixelNum = getPixelNum(surface, x + sqW, y + sqH)) > pixels) {
        coords[0] += sqW;
        coords[1] += sqH;
    }
    else if ((pixelNum = getPixelNum(surface, x, y + sqH)) > pixels) {
        coords[1] += sqH;
    }
    else if ((pixelNum = getPixelNum(surface, x + sqW, y)) > pixels) {
        coords[0] += sqW;
    }
    else {
        coords[0] += sqW;
        coords[1] += sqH;
        return coords;
    }

    return searchBottomRight(surface, coords[0], coords[1], pixelNum);
}


int* searchBottomLeft(SDL_Surface* surface, int x, int y, int pixels) {
    
    int pixelNum = 0,
        *coords = malloc(2 * sizeof(int));

    coords[0] = x;
    coords[1] = y;
    pixels /= 4;

    if ((pixelNum = getPixelNum(surface, x - sqW, y + sqH)) > pixels) {
        coords[0] -= sqW;
        coords[1] += sqH;
    }
    else if ((pixelNum = getPixelNum(surface, x, y + sqH)) > pixels) {
        coords[1] += sqH;
    }
    else if ((pixelNum = getPixelNum(surface, x - sqW, y)) > pixels) {
        coords[0] -= sqW;
    }
    else {
        coords[1] += sqH;
        return coords;
    }

    return searchBottomLeft(surface, coords[0], coords[1], pixelNum);
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