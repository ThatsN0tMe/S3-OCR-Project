#include <gtk/gtk.h>

#include "Pretreatment/pretreatment.h"
#include "Rotate/rotate.h"

void display_home();
void select_file(GtkWidget *button, gpointer user_data);
void display_image_options();

GtkWidget *window = NULL;
char *global_filepath = NULL;

//Macro
#define presentationText "Bienvenue dans l'application d'analyse de grilles de mots !"

void pretreatment() {
    ApplyPretreatment(global_filepath);
}

void rotatee() {
    create_rotate_window(global_filepath);
}

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
    GtkWidget *button = gtk_button_new_with_label("Select file"); //Create button
    g_signal_connect(button, "clicked", G_CALLBACK(select_file), NULL); //Action when button is clicked
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0); //Add button to box

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

    // Button "Convert to Grayscale"
    GtkWidget *button_grayscale = gtk_button_new_with_label("Pretreatment");
    g_signal_connect(button_grayscale, "clicked", G_CALLBACK(pretreatment), NULL);
    gtk_box_pack_start(GTK_BOX(box), button_grayscale, TRUE, TRUE, 0);

    // Button "Rotate Image"
    GtkWidget *button_rotate = gtk_button_new_with_label("Rotate Image");
    g_signal_connect(button_rotate, "clicked", G_CALLBACK(rotatee), NULL);
    gtk_box_pack_start(GTK_BOX(box), button_rotate, TRUE, TRUE, 0);

    // Button "Back"
    GtkWidget *button_back = gtk_button_new_with_label("Back");
    g_signal_connect(button_back, "clicked", G_CALLBACK(display_home), window);
    gtk_box_pack_start(GTK_BOX(box), button_back, FALSE, FALSE, 0);

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
        char *filepath = gtk_file_chooser_get_filename(chooser);

        global_filepath = g_strdup(filepath);
        display_image_options();

        gtk_button_set_label(GTK_BUTTON(button), "File open !");
    }

    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv); //GTK Initialization

    //Window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL); //Create a window
    gtk_window_set_title(GTK_WINDOW(window), "Aflelou"); //Window name
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 750); //Window length
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL); //Closing

    display_home(window, NULL);

    gtk_main();

    return 0;
}