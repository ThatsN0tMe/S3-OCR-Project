#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Cutting.h"

void cut_and_save(char* input_file, char* output_file, int x1, int y1, int x2, int y2) {
    SDL_Surface* image = IMG_Load(input_file);
    if (!image) {
        printf("Image loading error: %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }

    if (x1 < 0 || y1 < 0 || x2 > image->w || y2 > image->h || x1 >= x2 || y1 >= y2) {
        printf("Invalid coordinates.\n");
        SDL_FreeSurface(image);
        SDL_Quit();
        return;
    }

    int width = x2 - x1;
    int height = y2 - y1;

    SDL_Surface *cut_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, image->format->BitsPerPixel, image->format->format);
    if (!cut_surface) {
        printf("Cut surface loading error: %s\n", SDL_GetError());
        SDL_FreeSurface(image);
        SDL_Quit();
        return;
    }

    SDL_Rect cut_area = {x1, y1, width, height};
    SDL_BlitSurface(image, &cut_area, cut_surface, NULL);

    if (IMG_SavePNG(cut_surface, output_file) != 0) {
        printf("Image saving error: %s\n", IMG_GetError());
        SDL_FreeSurface(cut_surface);
        SDL_FreeSurface(image);
        SDL_Quit();
        return;
    }

    printf("Image cut out and saved in '%s'.\n", output_file);

    SDL_FreeSurface(cut_surface);
    SDL_FreeSurface(image);
}