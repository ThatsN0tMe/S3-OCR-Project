#ifndef ROTATE_H
#define ROTATE_H

#include <gtk/gtk.h>

void rotate(const char *filepath, double angle);
void apply_rotate();
void rotate_and_update_image(GtkRange *range);
void create_rotate_window(char* filepath);

#endif