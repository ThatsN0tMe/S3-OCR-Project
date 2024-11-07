#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <gtk/gtk.h>

extern GtkWidget *window;
void display_home();
void display_image_options();
void select_file(GtkWidget *button, gpointer user_data);
char* getDestPath(const char* sourcepath);
int cloneFile(const char *src_path, const char *dest_path);
void create_preprocess_window(char *filepath);

#endif