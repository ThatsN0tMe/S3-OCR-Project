#ifndef ROTATE_H
#define ROTATE_H

#include <gtk/gtk.h>

void apply_rotate();
void apply_auto_rotate();
void rotate(char *filepath, double angle);
void autoRotate(char* filepath, int save);

void update_image(double angle);
void slider_image_update(GtkRange *range);
void create_rotate_window(char* filepath);

#endif