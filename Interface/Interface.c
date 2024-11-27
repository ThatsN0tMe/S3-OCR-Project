#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Interface.h"
#include "../afflelou.h"
#include "../Pretreatment/pretreatment.h"

#define presentationText "Bienvenue dans l'application d'analyse de grilles de mots !"


void display_home() {
    //Remove all widget in window
    GList *children = gtk_container_get_children(GTK_CONTAINER(window));
    for (GList *child = children; child != NULL; child = child->next) {
        gtk_widget_destroy(GTK_WIDGET(child->data));
    }
    g_list_free(children);
    //Box
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); //Create a box
    gtk_container_add(GTK_CONTAINER(window), box); //Add box to window

    //Text
    GtkWidget *text = gtk_label_new(presentationText); //Create a text
    gtk_box_pack_start(GTK_BOX(box), text, TRUE, TRUE, 0); //Add text to box

    //Selected button
    GtkWidget *button_select = gtk_button_new_with_label("Select file"); //Create button
    g_signal_connect(button_select, "clicked", G_CALLBACK(select_file), NULL); //Action when button is clicked
    gtk_box_pack_start(GTK_BOX(box), button_select, TRUE, TRUE, 0); //Add button to box

    gtk_widget_show_all(window); //Show window
}


void display_image_options() {
    //Remove all widget in window
    GList *children = gtk_container_get_children(GTK_CONTAINER(window));
    for (GList *child = children; child != NULL; child = child->next) {
        gtk_widget_destroy(GTK_WIDGET(child->data));
    }
    g_list_free(children);

    //Box
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    // Button "Preprocess"
    GtkWidget *button_preprocess = gtk_button_new_with_label("Preprocess");
    g_signal_connect(button_preprocess, "clicked", G_CALLBACK(applyPreprocess), NULL);
    gtk_box_pack_start(GTK_BOX(box), button_preprocess, TRUE, TRUE, 0);

    // Button "Rotate Image"
    GtkWidget *button_rotate = gtk_button_new_with_label("Rotate Image");
    g_signal_connect(button_rotate, "clicked", G_CALLBACK(doRotation), NULL);
    gtk_box_pack_start(GTK_BOX(box), button_rotate, TRUE, TRUE, 0);

    //Button "Detection"
    GtkWidget *button_detect = gtk_button_new_with_label("Detection");
    g_signal_connect(button_detect, "clicked", G_CALLBACK(detectElements), NULL);
    gtk_box_pack_start(GTK_BOX(box), button_detect, TRUE, TRUE, 0);

    // Button "Back"
    GtkWidget *button_back = gtk_button_new_with_label("Back");
    g_signal_connect(button_back, "clicked", G_CALLBACK(display_home), window);
    gtk_box_pack_start(GTK_BOX(box), button_back, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}


void create_preprocess_window(char *filepath) {
    //Remove all widget in window
    GList *children = gtk_container_get_children(GTK_CONTAINER(window));
    for (GList *child = children; child != NULL; child = child->next) {
        gtk_widget_destroy(GTK_WIDGET(child->data));
    }
    g_list_free(children);
    
    GdkPixbuf* original_pixbuf = gdk_pixbuf_new_from_file(filepath, NULL);
    if (original_pixbuf == NULL) {
        g_print("Error: Could not load image from file %s\n", filepath);
        return;
    }

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *last_operation = gtk_button_new_with_label("Return to previous operation");
    g_signal_connect(last_operation, "clicked", G_CALLBACK(undo), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), last_operation, FALSE, FALSE, 1);

    GtkWidget *original_image = gtk_image_new_from_pixbuf(original_pixbuf);
    gtk_box_pack_start(GTK_BOX(vbox), original_image, TRUE, TRUE, 0);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 1);


    GtkWidget *button_grayscale = gtk_button_new_with_label("Grayscale");
    g_signal_connect(button_grayscale, "clicked", G_CALLBACK(grayscale), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), button_grayscale, TRUE, TRUE, 1);

    GtkWidget *button_contrast = gtk_button_new_with_label("Contrast");
    g_signal_connect(button_contrast, "clicked", G_CALLBACK(contrast), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), button_contrast, TRUE, TRUE, 1);

    GtkWidget *button_binarize = gtk_button_new_with_label("Binarize");
    g_signal_connect(button_binarize, "clicked", G_CALLBACK(binarize), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), button_binarize, TRUE, TRUE, 1);

    GtkWidget *button_brightness = gtk_button_new_with_label("Brightness");
    g_signal_connect(button_brightness, "clicked", G_CALLBACK(brightness), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), button_brightness, TRUE, TRUE, 1);

    GtkWidget *button_gaussian = gtk_button_new_with_label("Gaussian");
    g_signal_connect(button_gaussian, "clicked", G_CALLBACK(gaussian), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), button_gaussian, TRUE, TRUE, 1);

    GtkWidget *button_median = gtk_button_new_with_label("Median");
    g_signal_connect(button_median, "clicked", G_CALLBACK(median), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), button_median, TRUE, TRUE, 1);

    GtkWidget *button_back = gtk_button_new_with_label("Back");
    g_signal_connect(button_back, "clicked", G_CALLBACK(display_image_options), window);
    gtk_box_pack_start(GTK_BOX(hbox), button_back, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}


void select_file(GtkWidget *button, gpointer user_data) {
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    //Create a dialog box that opens the file explorer
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Choose a file",
                                         GTK_WINDOW(user_data),
                                         action,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    //Applies filters in the dialog box to force the user to open an image file
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Images");
    gtk_file_filter_add_mime_type(filter, "image/jpeg");
    gtk_file_filter_add_mime_type(filter, "image/png");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_filter_add_pattern(filter, "*.jpeg");
    gtk_file_filter_add_pattern(filter, "*.png");

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {

        char* sourcepath = gtk_file_chooser_get_filename(chooser);
        filepath = getDestPath(sourcepath);
        
        if (filepath == NULL || cloneFile(sourcepath, filepath) == -1)
            return;

        display_image_options();
        gtk_button_set_label(GTK_BUTTON(button), "File open !");

        image_change();
    }

    gtk_widget_destroy(dialog);
}



//Creer un nouveau path dans le directory /Modified pour pas changer le fichier originel
char* getDestPath(const char* sourcepath) {

    if (sourcepath == NULL) return NULL;

    char* res = NULL;
    const char* dir = "/Modified";
    int index = 0, startIndex = 0, endIndex = 0;

    while (sourcepath[index] != 0) {
        if (sourcepath[index] == '/') {
            endIndex = index;
        }
        index++;
    }
    startIndex = endIndex - 1;

    while (startIndex >= 0 && sourcepath[startIndex] != '/'){
        startIndex--;
    }


    res = calloc(startIndex + index - endIndex + 10, 1);

    if (startIndex > 0) {
        memcpy(res, sourcepath, startIndex);
    }
    memcpy(res + startIndex, dir, 9);
    memcpy(res + startIndex + 9, sourcepath + endIndex, index - endIndex);

    return res;
}


//Clone le fichier originel dans le nouveau path au cas ou un golmon rotate avant d'appliquer le pretraitement
int cloneFile(const char *src_path, const char *dest_path) {
    FILE *src = fopen(src_path, "rb");
    FILE *dest = fopen(dest_path, "wb");

    if (src == NULL || dest == NULL) {
        if (src) fclose(src);
        if (dest) fclose(dest);
        return -1;
    }

    size_t bytes;
    char buffer[4096];

    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }

    fclose(src);
    fclose(dest);
    return 0;
}