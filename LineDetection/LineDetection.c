#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include "LineDetection.h"


double** getBrightness(SDL_Surface* surface) {

    Uint8 r, g, b;
    Uint32* pixels = (Uint32*)surface->pixels;

    int height = surface->h, width = surface->w;
    double** matrix = malloc(height * sizeof(double*));

    for (int y = 0; y < height; y++) {
        matrix[y] = malloc(width * sizeof(double));

        for (int x = 0; x < width; x++) {
            SDL_GetRGB(pixels[y * width + x], surface->format, &r, &g, &b);
            matrix[y][x] = 0.299*r + 0.587*g + 0.114*b;
        }
    }

    return matrix;
}


int** detectEdges(SDL_Surface* surface) {

    int height = surface->h, width = surface->w;
    int** res = malloc((height - 2) * sizeof(int*));
    double** matrix = getBrightness(surface);

    Uint32* pixels = (Uint32*)surface->pixels;

    if (matrix == NULL) return NULL;

    for (int y = 1; y < height - 1; y++) {
        res[y-1] = malloc((width - 2) * sizeof(int));

        for (int x = 1; x < width - 1; x++) {
            int temp = (int)sqrt(pow(-1 * matrix[y-1][x-1] - 2 * matrix[y][x-1] - 1 * matrix[y+1][x-1] + matrix[y-1][x+1] + 2 * matrix[y][x+1] + matrix[y+1][x+1], 2)
                             + pow(-1 * matrix[y-1][x-1] - 2 * matrix[y-1][x] - 1 * matrix[y-1][x+1] + matrix[y+1][x-1] + 2 * matrix[y+1][x] + matrix[y+1][x+1], 2)) / 4;
            
            pixels[y * width + x] = SDL_MapRGB(surface->format, temp, temp, temp);
            res[y-1][x-1] = temp;
        }
    }

    for (int i = 0; i < height; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return res;
}


void detectLines(char* filepath) {

    SDL_Surface *surface = IMG_Load(filepath);
    if (!surface) {
        printf("Error loading image: %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }

    int** matrix = detectEdges(surface);

    if (matrix == NULL) {
        puts("Aieaieaieaie on a un petit probleme");
        return;
    }

    if (SDL_SaveBMP(surface, filepath) != 0) {
        printf("Image saving error: %s\n", SDL_GetError());
    }
    else {
        printf("Image processed successfully and saved as '%s'.\n", filepath);
    }


    int threshold,
        width = surface->w - 2,
        height = surface->h - 2,
        size = (int)sqrt(pow((double)height, 2) + pow((double)width, 2)) + 1;

    int** accumulatorArray = malloc(size * 2 * sizeof(int*));
    for (int i = 0; i < size * 2; i++) {
        accumulatorArray[i] = malloc(180 * sizeof(int));
        for (int j = 0; j < 180; j++) {
            accumulatorArray[i][j] = 0;
        }
    }


    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            if (matrix[y][x] <= 50) continue;

            int rho;
            for (int theta = 0; theta < 180; theta++) {

                rho = x * cos(theta * M_PI / 180) + y * sin(theta * M_PI / 180);
                if (++accumulatorArray[rho + size][theta] > threshold) {
                    threshold = accumulatorArray[rho + size][theta];
                }
            }
        }
    }

    threshold -= threshold / 4;

    char* title = "Lines";
    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    for (int rho = 0; rho < size * 2; rho++) {
        for (int theta = 0; theta < 180; theta++) {

            if (accumulatorArray[rho][theta] > threshold) {
                drawLine(renderer, (double)rho - size, (double)(theta * M_PI / 180), height, width);
            }
        }
        free(accumulatorArray[rho]);
    }
    free(accumulatorArray);

	SDL_RenderPresent(renderer);


    int keep_alive = 1;
	while (keep_alive)
	{
		keep_alive = process_events();
	}
	close_program(window, renderer);
}


void drawLine(SDL_Renderer* renderer, double rho, double theta, int h, int w) {

    // Calculate the endpoints of the line
    double cos_theta = cos(theta);
    double sin_theta = sin(theta);

    // Calculate the intersection points with the borders of the image
    double x1, y1, x2, y2;

    // Handle vertical lines
    if (fabs(cos_theta) < 1e-6) {
        // Vertical line: use rho to set x-coordinates
        x1 = x2 = rho; // rho is the x position
        y1 = 0; // Top edge
        y2 = h; // Bottom edge
    }
    // Handle horizontal lines
    else if (fabs(sin_theta) < 1e-6) {
        // Horizontal line: use rho to set y-coordinates
        y1 = y2 = rho; // rho is the y position
        x1 = 0; // Left edge
        x2 = w; // Right edge
    } else {
        // Calculate intersections with the image borders
        // Left border (x = 0)
        y1 = rho / sin_theta;
        if (y1 < 0) { // If it doesn't intersect, use the top border
            y1 = 0;
            x1 = rho / cos_theta;
        } else if (y1 > h) { // If it doesn't intersect, use the bottom border
            y1 = h;
            x1 = (rho - h * sin_theta) / cos_theta;
        } else { // Valid intersection with the left border
            x1 = 0;
        }

        // Right border (x = width)
        y2 = (rho - w * cos_theta) / sin_theta;
        if (y2 < 0) { // If it doesn't intersect, use the top border
            y2 = 0;
            x2 = rho / cos_theta;
        } else if (y2 > h) { // If it doesn't intersect, use the bottom border
            y2 = h;
            x2 = (rho - h * sin_theta) / cos_theta;
        } else { // Valid intersection with the right border
            x2 = w;
        }
    }

    // Set the draw color (optional)
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for the line

    // Draw the line using SDL
    SDL_RenderDrawLine(renderer, (int)x1, (int)y1, (int)x2, (int)y2);
}

void close_program(SDL_Window *window, SDL_Renderer *renderer) {

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();
}

int process_events() {

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) return 0;
	}
	return 1;
}