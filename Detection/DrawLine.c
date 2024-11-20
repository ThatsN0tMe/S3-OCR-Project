#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Detection.h"


void drawLine(SDL_Surface* surface, double rho, double theta, int h, int w) {

    double x1, y1, x2, y2;

    if (theta == 90) {
        x1 = 0;
        x2 = w;
        y1 = y2 = rho;
    }
    else if (theta == 0) {
        x1 = x2 = rho;
        y1 = 0;
        y2 = h;
    }
    else {

        double rad_theta = theta * (double)M_PI / 180.0,
               cos_theta = cos(rad_theta),
               sin_theta = sin(rad_theta);

        y1 = rho / sin_theta;
        if (y1 < 0) {
            y1 = 0;
            x1 = rho / cos_theta;
        }
        else if (y1 > h) {
            y1 = h;
            x1 = (rho - h * sin_theta) / cos_theta;
        }
        else {
            x1 = 0;
        }

        y2 = (rho - w * cos_theta) / sin_theta;
        if (y2 < 0) {
            y2 = 0;
            x2 = rho / cos_theta;
        }
        else if (y2 > h) {
            y2 = h;
            x2 = (rho - h * sin_theta) / cos_theta;
        }
        else {
            x2 = w;
        }
    }

    drawLineOnSurface(surface, x1, y1, x2, y2);
}

void drawLineOnSurface(SDL_Surface *surface, int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (x1 != x2 || y1 != y2) {
        put_pixel(surface, x1, y1);
        
        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void put_pixel(SDL_Surface *surface, int x, int y) {
    if (x >= 0 && x < surface->w && y >= 0 && y < surface->h) {
        Uint32* pixels = (Uint32*)surface->pixels;
        pixels[y * surface->w + x] = SDL_MapRGB(surface->format, 255, 0, 0);
    }
}