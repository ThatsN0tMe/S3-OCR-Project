#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../Detection/Detection.h"
#include "../Rotate/rotate.h"


void autoRotate(char* filepath, int save) {

    SDL_Surface* surface = IMG_Load(filepath);
    if (surface == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return;
    }

    int size = 0,
        threshold = 0;
    int** accumulatorArray = detectLines(surface, &size, &threshold);

    threshold /= 2;
    int maxTheta = 0, angle = 0;

    for (int theta = 0; theta < 180; theta++) {
        int currentTheta = 0;

        for (int rho = 0; rho < size * 2; rho++) {
            if (accumulatorArray[rho][theta] > threshold) {
                currentTheta++;
            }
        }

        if (currentTheta > maxTheta) {
            maxTheta = currentTheta;
            angle = theta;
        }
    }


    for (int rho = 0; rho < size * 2; rho++) {
        free(accumulatorArray[rho]);
    }
    free(accumulatorArray);
    SDL_FreeSurface(surface);


    if ((int)angle % 90 != 0) {
        if (angle > 90) {
            if (save)
                rotate(filepath, 90 - angle);
            else
                update_image(90 - angle);
        }
        else {
            if (save)
                rotate(filepath, -angle);
            else
                update_image(-angle);
        }
    }
}