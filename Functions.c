#include <SDL2/SDL.h>


int Min(int a, int b) {
    if (a > b) return b;
    return a;
}
int Max(int a, int b) {
    if (a > b) return a;
    return b;
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp; 
}

void save(SDL_Surface* surface, char* filepath) {
    if (SDL_SaveBMP(surface, filepath) != 0) {
        printf("Image saving error: %s\n", SDL_GetError());
    }
    else {
        printf("Image processed successfully and saved as '%s'.\n", filepath);
    }
}