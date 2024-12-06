#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define IMAGE_SIZE 28
#define THRESHOLD 1

void convert_black_lines_to_white(SDL_Surface *image) {
    Uint32 black_color = SDL_MapRGB(image->format, 0, 0, 0);
    Uint32 white_color = SDL_MapRGB(image->format, 255, 255, 255);

    int width = image->w;
    int height = image->h;
    Uint32 *pixels = (Uint32 *)image->pixels;

    int threshold = (int)(THRESHOLD * width);

    for (int y = 0; y < height; y++) {
        int black_pixel_count = 0;

        for (int x = 0; x < width; x++) {
            Uint32 pixel_color = pixels[y * (image->pitch / 4) + x];
            if (pixel_color == black_color)
                black_pixel_count++;
        }

        if (black_pixel_count >= threshold)
            for (int x = 0; x < width; x++)
                pixels[y * (image->pitch / 4) + x] = white_color;
    }
}

void convert_black_columns_to_white(SDL_Surface *image) {
    Uint32 black_color = SDL_MapRGB(image->format, 0, 0, 0);
    Uint32 white_color = SDL_MapRGB(image->format, 255, 255, 255);

    int width = image->w;
    int height = image->h;
    Uint32 *pixels = (Uint32 *)image->pixels;

    int threshold = (int)(THRESHOLD * height);

    for (int x = 0; x < width; x++) {
        int black_pixel_count = 0;

        for (int y = 0; y < height; y++) {
            Uint32 pixel_color = pixels[y * (image->pitch / 4) + x];
            if (pixel_color == black_color)
                black_pixel_count++;
        }

        if (black_pixel_count >= threshold)
            for (int y = 0; y < height; y++)
                pixels[y * (image->pitch / 4) + x] = white_color;
    }
}

void cut_and_save(char* input_file, char* output_file, int x1, int y1, int x2, int y2) {
    SDL_Surface* image = IMG_Load(input_file);
    if (!image) {
        printf("Image loading error: %s\n", IMG_GetError());
        return;
    }

    if (x1 < 0 || y1 < 0 || x2 > image->w || y2 > image->h || x1 >= x2 || y1 >= y2) {
        printf("Invalid coordinates.\n");
        SDL_FreeSurface(image);
        return;
    }

    int width = x2 - x1;
    int height = y2 - y1;

    SDL_Surface *cut_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, image->format->BitsPerPixel, image->format->format);
    if (!cut_surface) {
        printf("Cut surface creation error: %s\n", SDL_GetError());
        SDL_FreeSurface(image);
        SDL_Quit();
        return;
    }

    SDL_Rect cut_area = {x1, y1, width, height};
    SDL_BlitSurface(image, &cut_area, cut_surface, NULL);

    convert_black_lines_to_white(cut_surface);
    convert_black_columns_to_white(cut_surface);

    SDL_Surface *resized_surface = SDL_CreateRGBSurfaceWithFormat(0, 28, 28, image->format->BitsPerPixel, image->format->format);
    if (!resized_surface) {
        printf("Resized surface creation error: %s\n", SDL_GetError());
        SDL_FreeSurface(cut_surface);
        SDL_FreeSurface(image);
        SDL_Quit();
        return;
    }

    SDL_Rect resize_rect = {0, 0, IMAGE_SIZE, IMAGE_SIZE};
    SDL_BlitScaled(cut_surface, NULL, resized_surface, &resize_rect);

    if (IMG_SavePNG(resized_surface, output_file) != 0) {
        printf("Image saving error: %s\n", IMG_GetError());
        SDL_FreeSurface(resized_surface);
        SDL_FreeSurface(cut_surface);
        SDL_FreeSurface(image);
        SDL_Quit();
        return;
    }

    printf("Image cut, resized to %dx%d, and saved in '%s'.\n",IMAGE_SIZE, IMAGE_SIZE, output_file);

    SDL_FreeSurface(resized_surface);
    SDL_FreeSurface(cut_surface);
    SDL_FreeSurface(image);
}