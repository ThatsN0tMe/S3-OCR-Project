#include <stdlib.h>
#include <SDL2/SDL.h>


int** brightness(SDL_Surface* surface) {
    
    int r, g, b;
    Uint32* pixels = (Uint32)surface->pixels;

    int height = surface->h, width = surface->w;
    double** matrix = malloc(height * sizeof(double*));

    for (int y = 0; y < height; y++) {

        matrix[y] = malloc(width * sizeof(double));

        for (int x = 0; x < width; x++) {
            SDL_GetRGB(pixels[y * width + x], surface->format, &r, &g, &b);
            matrix[y][x] = 0.299*r + 0.587*g + 0.114*b;
            printf("%f|", matrix[y][x]);
        }

        puts("");
    }
}


/*void detectEdges(SDL_Surface surface) {

}*/