#include <math.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>


double** matrix_product(double** m1, size_t width_m1, size_t height_m1,
                        double** m2, size_t width_m2, size_t height_m2) {

  if (width_m1 != height_m2)
    errx(EXIT_FAILURE, "In matrix_product, the matrix size are incorrect");

  double** res = calloc(height_m1, sizeof(double*));
  for (size_t i = 0; i < height_m1; i++) {
    res[i] = calloc(width_m2, sizeof(double));
  }

  for (size_t i = 0; i < height_m1; i++) {
    for (size_t j = 0; j < width_m2; j++) {
      for (size_t k = 0; k < width_m1; k++) {
        res[i][j] += m1[i][k] * m2[k][j];
      }
    }
  }
  return res;
}

//double sigmod (double x) {
//  return (1 / (1 + exp(-x)));
//}

double sigmoid_derivative(double x) {
  return (x * (1 - x));
}

void initialize_weights(double weights[], int size) {
  double random;
  for (int i = 0; i < size; i++) {
    random = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
    weights[i] = random;
  }
}

void initialize_biases(double biases[], int size) {
double random;
  for (int i = 0; i < size; i++) {
    random = (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
    biases[i] = random;
  }
}

double forward_propagation(double input[], double weights_hidden[][2], double biases_hidden[], double weights_output[], double bias_output, double hidden_output[]) {
}

int main () {
  double** a = calloc(2, sizeof(double*));
  for (int i = 0 ; i < 2; i ++) {
    a[i] = calloc(2, sizeof(double));
  }
  a[0][0]= 2;
  a[0][1] = 1;
  a[1][0] = 1;
  a[1][1] = 4;
  
  double** b = calloc(2, sizeof(double*));
  for (int i = 0 ; i < 2; i ++) {
    b[i] = calloc(3, sizeof(double));
  }
  b[0][0] = 1;
  b[0][1] = 2;
  b[0][2] = 0;
  b[1][0] = 0;
  b[1][1] = 1;
  b[1][2] = 2;

  double** matrix = matrix_product(a, 2, 2, b, 3, 2);

  for (int row = 0; row < 2; row++) {
    for (int column = 0; column < 3; column++) {
      printf("%f     ", matrix[row][column]);
    }
    printf("\n");
  }

  for (int i = 0; i < 2; i++) {
    free(a[i]);
    free(matrix[i]);
  }
  for (int i = 0; i < 2; i++) {
    free(b[i]);
  }
  free(a);
  free(b);
  free(matrix);

  getchar();
}
