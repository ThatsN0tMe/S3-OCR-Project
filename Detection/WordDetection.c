#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Detection.h"


int getNumWords(SDL_Surface* surface) {

    int numWords = 0,
        nextLine = 1;

    for (int y = 0; y < surface->h; y += 10) {
        if (isWhiteLine(surface, 0, y, surface->w - 1, y)) {
            nextLine = 1;
        }
        else if (nextLine) {
            nextLine = 0;
            numWords++;
        }
    }

    return numWords;
}


int getNumLetters(SDL_Surface* surface, int y1, int y2) {

    int nextLetter = 1,
        numLetters = 0;

    for (int x = 0; x < surface->w; x++) {
        if (isWhiteLine(surface, x, y1, x, y2)) {
            nextLetter = 1;
        }
        else if (nextLetter) {
            nextLetter = 0;
            numLetters++;
        }
    }

    return numLetters;
}


void detectWords(SDL_Surface* surface, char* filepath) {

    int width = surface->w,
        height = surface->h,
        numWords = getNumWords(surface);
    
    //List 3d coords des lettres des mots : coords[mots][lettre][point], avec point 4 valeures : corner topleft et bottomright -> x1, y1 | x2, y2
    int*** words = malloc(sizeof(int*) * numWords);

    int nextTopLine = 1,
        wordCounter = 0,
        previousLineY = 0;


    for (int y = 1; y < height; y++) {

        if (!isWhiteLine(surface, 0, y, width - 1, y)) {
            if (nextTopLine) {
                previousLineY = y - 1;
                nextTopLine = 0;
            }
        }

        else if (!nextTopLine) {
            
            int nextLetter = 1, letterCounter = 0,
                numLetters = getNumLetters(surface, previousLineY, y);

            words[wordCounter] = calloc(numLetters + 1, sizeof(int*));

            for (int x = 1; x < width; x++) {

                if (isWhiteLine(surface, x, previousLineY, x, y)) {
                    if (!nextLetter) {
						words[wordCounter][letterCounter][2] = x;
                        nextLetter = 1;
                    	letterCounter++;

						if (letterCounter >= numLetters) break;
                    }
                }
                else if (nextLetter) {
                    if (letterCounter < numLetters) {
                        words[wordCounter][letterCounter] = malloc(4 * sizeof(int));
                        words[wordCounter][letterCounter][0] = x - 1;
						words[wordCounter][letterCounter][1] = previousLineY;
                        words[wordCounter][letterCounter][3] = y;
                    }
                    nextLetter = 0;
                }
            }

            nextTopLine = 1;
            wordCounter++;

			if (wordCounter >= numWords) break;
        }
    }


	for (int i = 0; i < numWords; i++) {

		if (words[i] == NULL) break;

		int j = 0;
		while (words[i][j] != NULL) {

			int* coords = words[i][j];
			drawLineOnSurface(surface, coords[0], coords[1], coords[2], coords[1]);
            drawLineOnSurface(surface, coords[2], coords[1], coords[2], coords[3]);
            drawLineOnSurface(surface, coords[0], coords[1], coords[0], coords[3]);
            drawLineOnSurface(surface, coords[0], coords[3], coords[2], coords[3]);
			j++;
		}
	}


	for (int i = 0; i < numWords; i++) {
		int j = 0;
		do {
			free(words[i][j]);
			j++;
		} while (words[i][j] != NULL);
		free(words[i]);
	}
	free(words);
}