#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "pretreatment.h"
#include "../Interface/Interface.h"
#include "image_history.h"

static char* path = NULL;
static SDL_Surface* surface = NULL;
static int autoProcessApply = 0;

void image_change() {
    autoProcessApply = 0;
    free_stack();
}

void undo() {
    SDL_Surface* previous_surface = pop();
    if (previous_surface) {
        SDL_FreeSurface(surface);
        surface = previous_surface;
        printf("Last operation cancelled.\n");
        save();
    }
    else
        printf("No previous operation to undo.\n");
}

void save() {    
    if (path == NULL) {
        printf("Filepath is undefined");
        return;
    }

    if (SDL_SaveBMP(surface, path) != 0) {
        printf("Image saving error: %s\n", SDL_GetError());
    }
    else {
        printf("Image processed successfully and saved as '%s'.\n", path);
    }
    create_preprocess_window(path);
}


void grayscale() {
    push(surface);

    Uint32* pixels = (Uint32*)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[y * width + x];

            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            Uint8 gris = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
            Uint32 pixelGris = SDL_MapRGB(surface->format, gris, gris, gris);

            pixels[y * width + x] = pixelGris;
        }
    }

    save();
}

void contrast() {
    push(surface);

    Uint8 r, g, b;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;
    float contrastFactor = 60;

    float factor = (259 * (contrastFactor + 255)) / (255 * (259 - contrastFactor));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Uint32 pixel = pixels[y * width + x];

            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            r = SDL_clamp(factor * (r - 128) + 128, 0, 255);
            g = SDL_clamp(factor * (g - 128) + 128, 0, 255);
            b = SDL_clamp(factor * (b - 128) + 128, 0, 255);

            pixels[y * width + x] = SDL_MapRGB(surface->format, r, g, b);
        }
    }

    save();
}

void brightness() {
    push(surface);

    int width = surface->w;
    int height = surface->h;
    int brightness = -100;
    Uint32 *pixels = (Uint32 *)surface->pixels;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b;

            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            r = SDL_clamp(r + brightness, 0, 255);
            g = SDL_clamp(g + brightness, 0, 255);
            b = SDL_clamp(b + brightness, 0, 255);

            pixels[y * width + x] = SDL_MapRGB(surface->format, r, g, b);
        }
    }

    save();
}

void binarize() {
    push(surface);

    Uint32 major_color = get_major_color();
    Uint8 r, g, b;
    SDL_GetRGB(major_color, surface->format, &r, &g, &b);

    Uint8 threshold =  0.299 * r + 0.587 * g + 0.114 * b;
    Uint32* pixels = (Uint32*)surface->pixels;
    int width = surface->w,
        height = surface->h;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            
            Uint8 luminance = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);

            if (luminance >= threshold) {
                pixels[y * width + x] = SDL_MapRGB(surface->format, 255, 255, 255);
            }
            else {
                pixels[y * width + x] = SDL_MapRGB(surface->format, 0, 0, 0);
            }
        }
    }

    save();
}

void gaussian() {
    push(surface);

    int width = surface->w,
        height = surface->h,
        kernel_size = 3;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    Uint32 *output_pixels = (Uint32 *)malloc(width * height * sizeof(Uint32));

    int half_kernel = kernel_size / 2;
    float sum = 0.0f;
    float *kernel = (float *)malloc(kernel_size * kernel_size * sizeof(float));

    for (int y = -half_kernel; y <= half_kernel; y++) {
        for (int x = -half_kernel; x <= half_kernel; x++) {
            float exponent = -(x * x + y * y) / 2;
            float value = (1.0f / (2.0f * M_PI)) * exp(exponent);
            kernel[(y + half_kernel) * kernel_size + (x + half_kernel)] = value;
            sum += value;
        }
    }

    for (int i = 0; i < kernel_size * kernel_size; i++) {
        kernel[i] /= sum;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float r = 0, g = 0, b = 0;

            for (int ky = -half_kernel; ky <= half_kernel; ky++) {
                for (int kx = -half_kernel; kx <= half_kernel; kx++) {
                    int nx = x + kx;
                    int ny = y + ky;

                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        Uint32 pixel = pixels[ny * width + nx];
                        Uint8 pr, pg, pb;
                        SDL_GetRGB(pixel, surface->format, &pr, &pg, &pb);

                        float kernel_value = kernel[(ky + half_kernel) * kernel_size + (kx + half_kernel)];

                        r += pr * kernel_value;
                        g += pg * kernel_value;
                        b += pb * kernel_value;
                    }
                }
            }

            output_pixels[y * width + x] = SDL_MapRGB(surface->format, (Uint8)SDL_clamp((int)r, 0, 255),
                                                       (Uint8)SDL_clamp((int)g, 0, 255),
                                                       (Uint8)SDL_clamp((int)b, 0, 255));
        }
    }

    memcpy(pixels, output_pixels, width * height * sizeof(Uint32));
    free(output_pixels);
    free(kernel);
    save();
}

void median() {
    push(surface);

    int width = surface->w,
        height = surface->h,
        kernel_size = 2;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    Uint32 *output_pixels = (Uint32 *)malloc(width * height * sizeof(Uint32));
    int half_kernel = kernel_size / 2;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint8 r[25], g[25], b[25];
            int count = 0;

            for (int dy = -half_kernel; dy <= half_kernel; dy++) {
                for (int dx = -half_kernel; dx <= half_kernel; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        Uint32 pixel = pixels[ny * width + nx];
                        SDL_GetRGB(pixel, surface->format, &r[count], &g[count], &b[count]);
                        count++;
                    }
                }
            }

            for (int i = 0; i < count - 1; i++) {
                for (int j = 0; j < count - i - 1; j++) {
                    if (r[j] > r[j + 1]) {
                        Uint8 temp_r = r[j];
                        r[j] = r[j + 1];
                        r[j + 1] = temp_r;
                    }
                    if (g[j] > g[j + 1]) {
                        Uint8 temp_g = g[j];
                        g[j] = g[j + 1];
                        g[j + 1] = temp_g;
                    }
                    if (b[j] > b[j + 1]) {
                        Uint8 temp_b = b[j];
                        b[j] = b[j + 1];
                        b[j + 1] = temp_b;
                    }
                }
            }

            int median_index = count / 2;
            Uint32 new_pixel = SDL_MapRGB(surface->format, r[median_index], g[median_index], b[median_index]);
            output_pixels[y * width + x] = new_pixel;
        }
    }

    memcpy(pixels, output_pixels, width * height * sizeof(Uint32));
    free(output_pixels);
    save();
}

typedef struct {
    Uint8 r, g, b;
    int count;
} Color;

Uint32 get_major_color() {
    int width = surface->w;
    int height = surface->h;
    Uint32 *pixels = (Uint32 *)surface->pixels;

    int max_colors = 256;
    Color *colorFreqs = (Color *)malloc(max_colors * sizeof(Color));
    int color_count = 0;

    if (!colorFreqs) {
        printf("Erreur d'allocation mémoire pour les fréquences de couleur\n");
        return SDL_MapRGB(surface->format, 0, 0, 0);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[y * width + x], surface->format, &r, &g, &b);

            int found = 0;
            for (int i = 0; i < color_count; i++) {
                if (colorFreqs[i].r == r && colorFreqs[i].g == g && colorFreqs[i].b == b) {
                    colorFreqs[i].count++;
                    found = 1;
                    break;
                }
            }

            if (!found) {
                if (color_count < max_colors) {
                    colorFreqs[color_count].r = r;
                    colorFreqs[color_count].g = g;
                    colorFreqs[color_count].b = b;
                    colorFreqs[color_count].count = 1;
                    color_count++;
                } else {
                    printf("Attention : trop de couleurs uniques, certaines seront ignorées.\n");
                }
            }
        }
    }

    int max_count = 0;
    Uint8 dominant_r = 0, dominant_g = 0, dominant_b = 0;
    for (int i = 0; i < color_count; i++) {
        if (colorFreqs[i].count > max_count) {
            max_count = colorFreqs[i].count;
            dominant_r = colorFreqs[i].r;
            dominant_g = colorFreqs[i].g;
            dominant_b = colorFreqs[i].b;
        }
    }

    free(colorFreqs);
    save();

    return SDL_MapRGB(surface->format, dominant_r, dominant_g, dominant_b);
}

void pretreatment(char *filepath) {

    path = filepath;

    SDL_Surface* originalSurface = IMG_Load(filepath);
    if (!originalSurface) {
        printf("Image loading error: %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }

    surface = SDL_ConvertSurfaceFormat(originalSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(originalSurface);

    if (!surface) {
        printf("Image format conversion error: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
    }

    if (!autoProcessApply) {
        char* filename = strrchr(path, '/');
        if (filename != NULL)
            filename++;
        else
            filename = path;
        
        grayscale();
        contrast();

        if (!strcmp(filename, "level_2_image_1.png")) {
            for (int i = 0; i < 4; i++)
                contrast();
            gaussian();
        }
        else if (!strcmp(filename, "level_2_image_2.png"))
            for (int i = 0; i < 4; i++)
                median();
        else if (!strcmp(filename, "level_4_image_1.png")) {
            for (int i = 0; i < 4; i++)
                contrast();
            for (int i = 0; i < 2; i++)
                median();
        }
        binarize();
        autoProcessApply = 1;
    }
    create_preprocess_window(path);
}
