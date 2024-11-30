#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../Detection/Detection.h"
#include "../Rotate/rotate.h"


void autoRotate(char* filepath) {

    if (filepath == NULL) {
        puts("Filepath is undefined");
        return;
    }

    SDL_Surface* surface = IMG_Load(filepath);
    if (surface == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        SDL_Quit();
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


    if ((int)angle % 90 != 0) {
        if (angle > 90) {
            rotate(surface, filepath, 90 - angle);
        }
        else {
            rotate(surface, filepath, -angle);
        }
    }

    for (int rho = 0; rho < size * 2; rho++) {
        free(accumulatorArray[rho]);
    }
    free(accumulatorArray);
}