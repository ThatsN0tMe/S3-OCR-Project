#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include "rotate.h"
#include "../Functions.h"


static GdkPixbuf *original_pixbuf = NULL;
static GtkWidget *original_image = NULL;
static GtkWidget *image_window = NULL;
static double current_angle = 0;
static char* path = NULL;


void apply_rotate() {
    rotate(path, current_angle);
}

void apply_auto_rotate() {
    autoRotate(path, 0);
}


void rotate(char* filepath, double angle) {

    SDL_Surface* originalSurface = IMG_Load(filepath);
    if (originalSurface == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return;
    }

    current_angle = 0;
    SDL_Window *window = SDL_CreateWindow("Rotation", 10000, SDL_WINDOWPOS_CENTERED, originalSurface->w, originalSurface->h, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, originalSurface);

    SDL_Rect destRect;
    destRect.w = originalSurface->w;
    destRect.h = originalSurface->h;
    
    SDL_FreeSurface(originalSurface);
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, destRect.w, destRect.h, 32, SDL_PIXELFORMAT_ARGB8888);

    SDL_RenderCopyEx(renderer, texture, NULL, NULL, angle, NULL, SDL_FLIP_NONE);
    SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);

    save(surface, filepath);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    if (image_window != NULL) {
        gtk_window_close(GTK_WINDOW(image_window));
        image_window = NULL;
    }
}


void slider_image_update(GtkRange *range) {
    update_image(gtk_range_get_value(range));
}
void on_window_destroy() {
    image_window = NULL;
}


void update_image(double angle) {

    current_angle = angle;
    int width = gdk_pixbuf_get_width(original_pixbuf);
    int height = gdk_pixbuf_get_height(original_pixbuf);

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_paint(cr);

    cairo_translate(cr, width / 2.0, height / 2.0);
    cairo_rotate(cr, current_angle * G_PI / 180.0);
    cairo_translate(cr, -width / 2.0, -height / 2.0);

    gdk_cairo_set_source_pixbuf(cr, original_pixbuf, 0, 0);
    cairo_paint(cr);

    GdkPixbuf *rotated_pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, width, height);

    gtk_image_set_from_pixbuf(GTK_IMAGE(original_image), rotated_pixbuf);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    g_object_unref(rotated_pixbuf);
}


void create_rotate_window(char *filepath) {

    if (filepath == NULL) {
        puts("Filepath is undefined");
        return;
    }
    path = filepath;

    original_pixbuf = gdk_pixbuf_new_from_file(filepath, NULL);
    if (original_pixbuf == NULL) {
        g_print("Error: Could not load image from file %s\n", filepath);
        return;
    }

    image_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(image_window), "Image Rotate");
    gtk_window_set_default_size(GTK_WINDOW(image_window), 1000, 750);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    gtk_container_add(GTK_CONTAINER(image_window), vbox);

    original_image = gtk_image_new_from_pixbuf(original_pixbuf);
    gtk_box_pack_start(GTK_BOX(vbox), original_image, TRUE, TRUE, 0);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 1);

    GtkWidget *slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, -180, 180, 1);
    gtk_scale_set_value_pos(GTK_SCALE(slider), GTK_POS_TOP);
    gtk_range_set_value(GTK_RANGE(slider), 0);
    gtk_box_pack_start(GTK_BOX(hbox), slider, TRUE, TRUE, 0);

    GtkWidget *button_auto = gtk_button_new_with_label("Auto Rotate");
    g_signal_connect(button_auto, "clicked", G_CALLBACK(apply_auto_rotate), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), button_auto, FALSE, FALSE, 1);

    GtkWidget *button_save = gtk_button_new_with_label("Save");
    g_signal_connect(button_save, "clicked", G_CALLBACK(apply_rotate), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), button_save, FALSE, FALSE, 1);
    
    g_signal_connect(slider, "value-changed", G_CALLBACK(slider_image_update), NULL);

    g_signal_connect(image_window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    gtk_widget_show_all(image_window);
}