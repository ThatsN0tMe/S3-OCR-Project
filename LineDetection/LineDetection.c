#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>


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

    if (matrix == NULL) return NULL;

    for (int y = 1; y < height - 1; y++) {
        res[y-1] = malloc((width - 2) * sizeof(int));

        for (int x = 1; x < width - 1; x++) {
            res[y-1][x-1] = (int)sqrt(pow(-1 * matrix[y-1][x-1] - 2 * matrix[y][x-1] - 1 * matrix[y+1][x-1] + matrix[y-1][x+1] + 2 * matrix[y][x+1] + matrix[y+1][x+1], 2)
                             + pow(-1 * matrix[y-1][x-1] - 2 * matrix[y-1][x] - 1 * matrix[y-1][x+1] + matrix[y+1][x-1] + 2 * matrix[y+1][x] + matrix[y+1][x+1], 2)) / 4;
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


    int threshold = 180,
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

            if (matrix[y][x] <= threshold) continue;

            int rho;
            for (int theta = 0; theta < 180; theta++) {
                rho = x * cos(theta) + y * sin(theta);
                accumulatorArray[rho + size][theta]++;
            }
        }
    }

    char* title = "on a detecte les lignes";
    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);


    for (int rho = 0; rho < size * 2; rho++) {
        for (int theta = 0; theta < 180; theta++) {
            
            if (accumulatorArray[rho][theta] > 100) {
                drawLine(renderer, (double)rho, (double)theta, height, width);
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

    // Convert polar to Cartesian coordinates
    double x_center = rho * cos(theta);
    double y_center = rho * sin(theta);

    // Translate to screen coordinates
    int x0 = (int)(w / 2 + x_center);
    int y0 = (int)(h / 2 - y_center);

    // Compute endpoints for the line segment
    int x1 = (int)(x0 + w * cos(theta + M_PI / 2));
    int y1 = (int)(y0 - h * sin(theta + M_PI / 2));
    int x2 = (int)(x0 - w * cos(theta + M_PI / 2));
    int y2 = (int)(y0 + h * sin(theta + M_PI / 2));

    printf("%d, %d  |  %d, %d\n", x1, y1, x2, y2);
    // Draw the line
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
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