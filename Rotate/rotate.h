#ifndef ROTATE_H
#define ROTATE_H

#include <gtk/gtk.h>
#include <cairo.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

void rotate(char *filepath, double angle);
void rotate_and_update_image(GtkRange *range);
void create_rotate_window(char *filepath);

#endif