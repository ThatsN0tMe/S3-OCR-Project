#ifndef AFFLELOU_H_
#define AFFLELOU_H_

#include <gtk/gtk.h>

void display_home();
void select_file(GtkWidget *button, gpointer user_data);
void display_image_options();
char* getDestPath(const char* sourcepath);
int clone_file(const char *src_path, const char *dest_path);

#endif