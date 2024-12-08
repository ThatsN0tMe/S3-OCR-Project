#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <gtk/gtk.h>

extern GtkWidget *window;

void display_home();
void display_image_options();
void close_preprocess_window();
void close_detection_window();
void create_preprocess_window(char *filepath);
void create_detection_window(char *filepath);
void select_file(GtkWidget *button, gpointer user_data);

char* getDestPath(const char* sourcepath);
char* getSourcePath();
int cloneFile(const char *src_path, const char *dest_path);

#endif