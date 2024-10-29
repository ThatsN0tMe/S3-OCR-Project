#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "pretreatment.h"


void grayscale(SDL_Surface *surface) {
    Uint32* pixels = (Uint32*)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[y * width + x];

            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            Uint8 gris = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
            Uint32 pixelGris = SDL_MapRGB(surface->format, gris, gris, gris);

            pixels[y * width + x] = pixelGris;
        }
    }
}

void contrast(SDL_Surface *surface, float contrastFactor) {
    Uint8 r, g, b;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    float factor = (259 * (contrastFactor + 255)) / (255 * (259 - contrastFactor));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Uint32 pixel = pixels[y * width + x];

            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            r = SDL_clamp(factor * (r - 128) + 128, 0, 255);
            g = SDL_clamp(factor * (g - 128) + 128, 0, 255);
            b = SDL_clamp(factor * (b - 128) + 128, 0, 255);

            pixels[y * width + x] = SDL_MapRGB(surface->format, r, g, b);
        }
    }
}

void ApplyPretreatment(char *filepath) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL initialization error: %s\n", SDL_GetError());
        return;
    }

    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
        printf("SDL_image initialization error: %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }

    SDL_Surface* originalSurface = IMG_Load(filepath);
    if (!originalSurface) {
        printf("Image loading error: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(originalSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(originalSurface);

    if (!surface) {
        printf("Image format conversion error: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    grayscale(surface);
    contrast(surface, -150);


    if (SDL_SaveBMP(surface, filepath) != 0) {
        printf("Image saving error: %s\n", SDL_GetError());
    }
    else {
        printf("Image processed successfully and saved as '%s'.\n", filepath);
    }

    SDL_FreeSurface(surface);
    IMG_Quit();
    SDL_Quit();
}