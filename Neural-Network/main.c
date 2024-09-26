#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

// computing the dot product of input_vector and weights
double dot(double* input_vectors, double* wheights) {
  int len = sizeof(wheights) / 4;
  if (len != sizeof(input_vectors) / 4) 
    errx(EXIT_FAILURE, "There is a diferent amount of wheights and input vectors");
  double sum = 0;
  for (int i = 0; i < len; i++) {
    sum += input_vectors[i] * wheights[i];
  }
  return sum;

}

int main() {
  double input_vectors[] = {1.72, 1.23};
  double weights_1[] = {2.17, 0.32};
  printf("%f", dot(input_vectors, weights_1));
}
