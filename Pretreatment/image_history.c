#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "pretreatment.h"
#include "image_history.h"

SurfaceList* stack = NULL;

void push(SDL_Surface* surface) {
    SurfaceList* new_surface = malloc(sizeof(SurfaceList));
    new_surface->surface = SDL_ConvertSurface(surface, surface->format, 0);
    new_surface->next = stack;
    stack = new_surface;
}

SDL_Surface* pop() {
    if (stack == NULL) return NULL;

    SurfaceList* top = stack;
    SDL_Surface* surface = top->surface;
    stack = top->next;
    free(top);

    return surface;
}

int stack_is_empty() {
    return stack != NULL;
}

void free_stack() {
    struct SurfaceList* current = stack;
    while (current != NULL) {
        struct SurfaceList* next = current->next;
        if (current->surface != NULL) {
            SDL_FreeSurface(current->surface);
        }
        free(current);
        current = next;
    }
    stack = NULL;
}