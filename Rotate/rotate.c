#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include "rotate.h"


static GdkPixbuf *original_pixbuf;
static GtkWidget *original_image;
static gdouble current_angle = 0;
char *filename = "";

void rotate(const char *filepath, double angle) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization error: %s\n", SDL_GetError());
        return;
    }

    SDL_Surface *originalSurface = IMG_Load(filepath);
    if (!originalSurface) {
        printf("Error loading image: %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }

    SDL_Window *window = SDL_CreateWindow("Rotation", 10000, SDL_WINDOWPOS_CENTERED, originalSurface->w, originalSurface->h, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, originalSurface);
    SDL_FreeSurface(originalSurface);

    SDL_Rect destRect;
    destRect.w = originalSurface->w;
    destRect.h = originalSurface->h;

    SDL_RenderCopyEx(renderer, texture, NULL, NULL, angle, NULL, SDL_FLIP_NONE);

    SDL_Surface *rotatedSurface = SDL_CreateRGBSurfaceWithFormat(0, destRect.w, destRect.h, 32, SDL_PIXELFORMAT_ARGB8888);

    SDL_RenderReadPixels(renderer, NULL, rotatedSurface->format->format, rotatedSurface->pixels, rotatedSurface->pitch);

    if (IMG_SavePNG(rotatedSurface, filepath) != 0) {
        printf("Error saving image: %s\n", IMG_GetError());
    }
    else {
        printf("The image was successfully rotated with an angle of %d° and saved as '%s'.\n", (int)angle, filepath);
    }

    SDL_FreeSurface(rotatedSurface);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


void apply_rotate() {
    rotate(filename, current_angle);
}

void rotate_and_update_image(GtkRange *range) {
    current_angle = gtk_range_get_value(range);
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
    filename = filepath;
    original_pixbuf = gdk_pixbuf_new_from_file(filepath, NULL);
    if (original_pixbuf == NULL) {
        g_print("Error: Could not load image from file %s\n", filepath);
        return;
    }

    GtkWidget *image_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
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

    GtkWidget *button = gtk_button_new_with_label("Save");
    g_signal_connect(button, "clicked", G_CALLBACK(apply_rotate), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 1);

    g_signal_connect(slider, "value-changed", G_CALLBACK(rotate_and_update_image), NULL);

    gtk_widget_show_all(image_window);
}