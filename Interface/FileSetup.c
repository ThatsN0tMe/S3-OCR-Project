#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Interface.h"
#include "../Pretreatment/pretreatment.h"


//Creer un nouveau path dans le directory /Modified pour pas changer le fichier originel
char* getDestPath(const char* sourcepath) {

    if (sourcepath == NULL) return NULL;

    char* res = NULL;
    const char* dir = "/Modified";
    int index = 0, startIndex = 0, endIndex = 0;

    while (sourcepath[index] != 0) {
        if (sourcepath[index] == '/') {
            endIndex = index;
        }
        index++;
    }
    startIndex = endIndex - 1;

    const char* fileNameWithExt = sourcepath + endIndex + 1;
    const char* dot = strrchr(fileNameWithExt, '.');
    int fileNameLength = (dot != NULL) ? (size_t)(dot - fileNameWithExt) : strlen(fileNameWithExt);

    while (startIndex >= 0 && sourcepath[startIndex] != '/'){
        startIndex--;
    }

    res = calloc(startIndex + index - endIndex + fileNameLength + strlen(dir) + strlen(fileNameWithExt) + 2, 1);

    if (startIndex > 0) {
        memcpy(res, sourcepath, startIndex);
    }
    memcpy(res + startIndex, dir, strlen(dir));
    memcpy(res + startIndex + strlen(dir), "/", 1);
    memcpy(res + startIndex + strlen(dir) + 1, fileNameWithExt, fileNameLength);
    memcpy(res + startIndex + strlen(dir) + 1 + fileNameLength, "/", 1);
    memcpy(res + startIndex + strlen(dir) + 1 + fileNameLength + 1, fileNameWithExt, strlen(fileNameWithExt));
    
    return res;
}


int cloneFile(const char *src_path, const char *dest_path) {
    FILE *src = fopen(src_path, "rb");
    FILE *dest = fopen(dest_path, "wb");

    if (src == NULL || dest == NULL) {
        if (src) fclose(src);
        if (dest) fclose(dest);
        return -1;
    }

    size_t bytes;
    char buffer[4096];

    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }

    fclose(src);
    fclose(dest);
    
    return 0;
}