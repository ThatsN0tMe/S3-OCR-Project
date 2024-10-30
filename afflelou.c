#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "afflelou.h"
#include "Rotate/rotate.h"
#include "Interface/Interface.h"
#include "Pretreatment/pretreatment.h"
#include "LineDetection/LineDetection.h"


char *filepath = NULL;
SDL_Surface* surface = NULL;
GtkWidget *window = NULL;


void applyPreprocess() {
    ApplyPretreatment(filepath, surface);
}

void doRotation() {
    create_rotate_window(filepath, surface);
}

void detectLines() {
    brightness(surface);
}



void initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL initialization error: %s\n", SDL_GetError());
        return;
    }

    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
        printf("SDL_image initialization error: %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }
    
    surface = IMG_Load(filepath);
    if (!surface) {
        printf("Image loading error: %s\n", IMG_GetError());
        quitSDL();
        return;
    }

    /*
    SDL_Surface* surface = SDL_ConvertSurfaceFormat(originalSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(originalSurface);

    if (!surface) {
        printf("Image format conversion error: %s\n", SDL_GetError());
        quitSDL();
    }*/
}


void quitSDL() {
    SDL_FreeSurface(surface);
    IMG_Quit();
    SDL_Quit();
}



int main(int argc, char *argv[]) {

    //Initialization
    gtk_init(&argc, &argv);

    //Window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL); //Create a window
    gtk_window_set_title(GTK_WINDOW(window), "Afflelou"); //Window name
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 750); //Window length
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL); //Closing

    display_home();

    gtk_main();

    quitSDL();

    return 0;
}