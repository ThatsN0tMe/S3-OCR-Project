#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <ctype.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../Interface/Interface.h"

#define MARGIN 0

char** read_file(char* filepath, int* lines, int* columns) { //Reads from a file and copies it to a variable
    FILE* file = fopen(filepath, "r"); //Read file
    if (file == NULL)
        err(EXIT_FAILURE, "Error : file is empty or not found\n");
    int current_columns = 0; //Variables for count lines and columns
    char c;
    *lines = 0;
    *columns = -1;
    while ((c = fgetc(file)) != EOF) { //Read each character and see if it is not equal to the end of file character
        if (c == '\n') {
            (*lines)++;
            if (*columns != -1 && *columns != current_columns)
                err(EXIT_FAILURE, "Error : The grid is not valid\n");
            *columns = current_columns;
            current_columns = 0;
        } 
        else
            current_columns++;
    }
    if (current_columns > 0) {
        if (*columns != -1 && *columns != current_columns)
            err(EXIT_FAILURE, "Error : The grid is not valid\n");
        (*lines)++;
        *columns = current_columns;
    }
    rewind(file);
    char** grid = (char**)malloc((*lines) * sizeof(char*)); //Allocate memory to store the grid
    for (int i = 0; i < (*lines); i++) {
        grid[i] = (char*)malloc((*columns) * sizeof(char));
        fscanf(file, "%s", grid[i]);
    }
    fclose(file);
    return grid;
}

int check_direction(char** grid, int lines, int columns, int x, int y, int dx, int dy, char* word) { //Check if we are looking for the word in the right direction
    int length = strlen(word);
    for (int i = 0; i < length; i++) {
        int nx = x + i * dx;
        int ny = y + i * dy;
        if (nx < 0 || ny < 0 || nx >= columns || ny >= lines || tolower(grid[ny][nx]) != tolower(word[i]))
            return 0;
    }    
    return 1;
}

void draw_line(SDL_Surface* image, int x0, int y0, int x1, int y1, Uint32 color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        if (x0 >= 0 && x0 < image->w && y0 >= 0 && y0 < image->h) {
            ((Uint32*)image->pixels)[y0 * image->w + x0] = color;
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void draw_rect(SDL_Surface* image, int x0, int y0, int x1, int y1, Uint32 color) {
    draw_line(image, x0, y0, x1, y0, color);
    draw_line(image, x1, y0, x1, y1, color);
    draw_line(image, x1, y1, x0, y1, color);
    draw_line(image, x0, y1, x0, y0, color);
}

int find_word(char** filepath, char** grid, int*** coordinates, int lines, int columns, char* word) {
    int directions[8][2] = {
        {1, 0},   // Right
        {0, 1},   // Down
        {1, 1},   // Diagonal bottom right
        {0, -1},  // Up
        {-1, 0},  // Left
        {-1, -1}, // Diagonal top left
        {-1, 1},  // Diagonal bottom left
        {1, -1}   // Diagonal top right
    };

    SDL_Surface* image = IMG_Load(*filepath);
    if (!image) {
        printf("Error loading image: %s\n", IMG_GetError());
        return 0;
    }

    Uint32 red_color = SDL_MapRGB(image->format, 255, 0, 0);

    for (int y = 0; y < lines; y++) {
        for (int x = 0; x < columns; x++) {
            if (tolower(grid[y][x]) == tolower(word[0])) {
                for (int d = 0; d < 8; d++) {
                    int dx = directions[d][0];
                    int dy = directions[d][1];

                    if (check_direction(grid, lines, columns, x, y, dx, dy, word)) {
                        int x1 = x + (strlen(word) - 1) * dx;
                        int y1 = y + (strlen(word) - 1) * dy;

                        if (d == 2 || d == 5 || d == 4 || d== 7)
                            draw_line(image,
                            coordinates[y][x][0] + MARGIN,
                            coordinates[y][x][1] + MARGIN,
                            coordinates[y1][x1][2],
                            coordinates[y1][x1][3],
                            red_color);
                        else
                            draw_rect(image,
                            coordinates[y][x][0] + MARGIN,
                            coordinates[y][x][1] + MARGIN,
                            coordinates[y1][x1][2],
                            coordinates[y1][x1][3],
                            red_color);

                        char* save_path = getDestPath(*filepath);

                        IMG_SavePNG(image, save_path);
                        *filepath = save_path;
                        SDL_FreeSurface(image);
                        return 1;
                    }
                }
            }
        }
    }

    printf("Not found\n");
    SDL_FreeSurface(image);
    return 0;
}

/*
void print_grid(char** grid, int lines, int columns) {
    printf("The grid size is %d lines * %d columns :\n\n", lines, columns);
    for (int y = -4; y < lines; y++) {
        if (y >= 0)
            printf(" %d |", y);
        for (int x = 0; x < columns; x++) {
            if (y == -4 || y == -2) {
                printf("    ");
                y++;
            }
            if (y == -3)
                printf(" %d", x);
            else if (y == -1)
                printf(" —");
            else
                printf(" %c", grid[y][x]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char* argv[]) { //Main function that searches for the word in the grid and returns its coordinates
    if (argc != 3)
        err(EXIT_FAILURE, "Error : Too much argument\n");
    char* filepath = argv[1];
    char* word = argv[2];
    int lines, columns;
    char** grid = read_file(filepath, &lines, &columns);
    if (grid == NULL)
        return 0;


    size_t length = strlen(word);
    for (size_t i = 0; i < length; i++)
        word[i] = tolower(word[i]);
    if (strcmp(word, "print") == 0) {
        print_grid(grid, lines, columns);
        return 1;
    }


    int x0, y0, x1, y1;
    if (find_word(grid, lines, columns, word, &x0, &y0, &x1, &y1))
        printf("(%d,%d)(%d,%d)\n", x0, y0, x1, y1);
    else
        printf("Not found\n");
    for (int i = 0; i < lines; i++) { //Free the memory
        free(grid[i]);
    }
    free(grid);

    return 1;
}
*/