#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// Target is the word of the grid and s the string we want to compare
// It returns 0 if the start of s is diferent from Target
// It returns 1 if the start of s is the same than Target
// It returns 2 if they are the same

char* remove_lowercase(char* s) {
  char* res = malloc(strlen(s) + 1);
  int i = 0;
  while (*s != 0) {
    if (*s >= 'a' && *s <= 'z')  {
      res[i] = *s - 32;
    }
    else {
      res[i] = *s;
    }
    s++;
    i++;
  } 
  res[i] = 0;
  return res;
}

// Check if a target matches string the start of another string
// For example if target "aab" matches the start of "aabcd", it is in this case

int starts_with(char* target, char* s) {
  int len_s = strlen(s);
  int len_target = strlen(target);
  if (len_s > len_target) return 0;
  else {
    while (*s != 0) {
      if (*s != *target) return 0;
      target++;
      s++;
    }
  }
  return (*target == 0) ? 2 : 1;
}

int main(int argc, char* argv[]) {


  if (argc != 3) errx(1, "EXIT_FAILURE, paramters of solver are incorect. Must be : ./solver \"grid\" \"researched-word\"");

  // File oppenning 

  FILE * fgrid;
  fgrid = fopen(argv[1], "r");
  char s[101];

  char grid[100][100]; // Our file translated to a grid
  
  int i = 0;
  while (fgets(s, 100, fgrid)) {
      strcpy(grid[i], s);
      i++;

  }
  
  int lines = i;
  int columns = strlen(grid[0]) - 1;
  fclose(fgrid);
  
  // Solver
  
  int x1;
  int x2;
  int y1;
  int y2;


  char* target = remove_lowercase(argv[2]); // We remove the lowercases of argv
  
  for(int i = 0; i < lines; i++) {
    for (int j = 0; j < columns; j++) {

      x1 = j;
      y1 = i;
      char first_elem[1] = {grid[i][j]};

      if(starts_with(target, first_elem)) {

        for (int k = 0; k < 8; k++) { // Foreach 8 directions on the grid
          char str[100] = {0};
          str[0] = grid[i][j];
          int dir_x = 0;
          int dir_y = 0;

          switch(k) {
            case 0: dir_x = 1; break;
            case 1: dir_x = 1; dir_y = 1; break;
            case 2: dir_y = 1; break;
            case 3: dir_x = -1; dir_y = 1; break;
            case 4: dir_x = -1; break;
            case 5: dir_x = -1; dir_y = -1; break;
            case 6: dir_y = -1; break;
            case 7: dir_x = 1; dir_y = -1; break;
          }

          
          int l = 1;
          x2 = x1;
          y2 = y1;
          int same_start = starts_with(target, str);
          while (same_start == 2 || 
            (same_start && x2 >= 0 && y2 >= 0 && x2 < columns && y2 < lines)) {
            
            if (same_start == 2) { // We find the word
              printf("(%i,%i)(%i,%i)",x1,y1, x2, y2);
              return 0;
            }

            x2 = l * dir_x + x1;
            y2 = l * dir_y + y1;
            str[l] = grid[y2][x2];
            l++;

            same_start = starts_with(target, str);
          }
        }
      }
    }
  }
  printf("Not found");
  return 0;
}
