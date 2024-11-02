#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h> 
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

void grayscale(SDL_Surface *surface) {
    Uint32* pixels = (Uint32*)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[y * width + x];

            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            Uint8 gris = 0.299 * r + 0.587 * g + 0.114 * b;
            Uint32 pixelGris = SDL_MapRGB(surface->format, gris, gris, gris);

            pixels[y * width + x] = pixelGris;
        }
    }
}

void contrast(SDL_Surface *surface, float contrastFactor) {
    Uint8 r, g, b;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;

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
}

Uint8 luminance(SDL_Surface* image) {
    Uint32 pixel;
    Uint8 r, g, b, gray;
    int sum = 0;
    int count = 0;

    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            pixel = ((Uint32*)image->pixels)[y * image->w + x];
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            gray = 0.299 * r + 0.587 * g + 0.114 * b;
            sum += gray;
            count++;
        }
    }

    return sum / count;
}

void binarize_image(SDL_Surface* image, Uint8 threshold) {
    Uint32 pixel;
    Uint8 r, g, b, gray;

    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            pixel = ((Uint32*)image->pixels)[y * image->w + x];
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            gray = 0.299 * r + 0.587 * g + 0.114 * b;

            if (gray >= threshold)
                ((Uint32*)image->pixels)[y * image->w + x] = SDL_MapRGB(image->format, 255, 255, 255);
            else if (gray < threshold * 0.96)
                ((Uint32*)image->pixels)[y * image->w + x] = SDL_MapRGB(image->format, 0, 0, 0);
        }
    }
}

void gaussian(SDL_Surface *surface, int kernel_size) {
    int width = surface->w;
    int height = surface->h;
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
}

double standard_deviation(SDL_Surface* image) {
    Uint8 r, g, b;
    double sum = 0.0, sum_sq = 0.0;
    int count = 0;

    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            Uint32 pixel = ((Uint32*)image->pixels)[y * image->w + x];
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            Uint8 gray = 0.299 * r + 0.587 * g + 0.114 * b;
            sum += gray;
            sum_sq += gray * gray;
            count++;
        }
    }

    double mean = sum / count;
    double variance = (sum_sq / count) - (mean * mean);
    return sqrt(variance);
}

void ApplyPretreatment(char *filepath) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL initialization error: %s\n", SDL_GetError());
        return;
    }

    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
        printf("SDL_image initialization error: %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }

    SDL_Surface* originalSurface = IMG_Load(filepath);
    if (!originalSurface) {
        printf("Image loading error: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(originalSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(originalSurface);

    if (!surface) {
        printf("Image format conversion error: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    grayscale(surface);

    if (standard_deviation(surface) < 40) {
        gaussian(surface, 5);
        contrast(surface, 52);
        binarize_image(surface, luminance(surface));
        gaussian(surface, 5);
        contrast(surface, 255);
    }
    else {
        binarize_image(surface, luminance(surface));
        contrast(surface, 255);
    }
    


    if (SDL_SaveBMP(surface, filepath) != 0) {
        printf("Image saving error: %s\n", SDL_GetError());
    }
    else {
        printf("Image processed successfully and saved as '%s'.\n", filepath);
    }

    SDL_FreeSurface(surface);
    IMG_Quit();
    SDL_Quit();
}