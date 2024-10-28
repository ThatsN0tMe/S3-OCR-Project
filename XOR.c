#include <math.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>

#define INPUT_NEURONS 2
#define HIDDEN_NEURONS 2
#define LEARNING_RATE 0.1
#define EPOCHS 10000

double** array_to_matrix(double* m1, size_t width_m1) {
  double** res = malloc(width_m1 * sizeof(double*));
  for (int i = 0; i < width_m1; i++) {
    res[i] = calloc(1, sizeof(double));
    res[i][0] = m1[i];
  }
  return res;
}

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

double** matrix_sum(double** m1, size_t width_m1, size_t height_m1,
                    double** m2, size_t width_m2, size_t height_m2) {
  if (width_m1 != width_m2 || height_m1 != height_m2)
    errx(EXIT_FAILURE, "In matrix_sum, the matrix size are incorrect");

  double** res = malloc(height_m1 * sizeof(double));

  for (size_t i = 0 ; i < height_m1; i++) {
    res[i] = calloc(width_m1, sizeof(double));
    for (size_t j = 0; j < width_m1; j++) {
      res[i][j] = m1[i][j] + m2[i][j];
    }
  }

  return res;
}


double sigmoid (double x) {
  return (1 / (1 + exp(-x)));
}


double sigmoid_derviate (double x) {
  return sigmoid(x) * (1 - sigmoid(x));
}

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

void printMatrix(double** matrix, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      printf("%lf ", matrix[i][j]);
    }
    printf("\n");
  }
}

double forward_propagation(double* input, double** weights_hidden, double* biases_hidden, double* weights_output, double bias_output, double* hidden_output) {

  size_t weights_hidden_height = INPUT_NEURONS;
  size_t weights_hidden_width = HIDDEN_NEURONS;

  size_t weights_output_width = HIDDEN_NEURONS;

  size_t input_width = INPUT_NEURONS;

  size_t biases_hidden_width = HIDDEN_NEURONS;

  double** input_in_matrix = array_to_matrix(input, input_width);

  size_t hidden_output_width = HIDDEN_NEURONS;

  double** hidden_output_matrix = calloc(1, sizeof(double*));
  hidden_output_matrix[0] = calloc(hidden_output_width, sizeof(double));


  hidden_output_matrix = matrix_product(weights_hidden, weights_hidden_width, weights_hidden_height,
                                        input_in_matrix, 1, input_width);

  hidden_output_matrix = matrix_sum(hidden_output_matrix, 1, hidden_output_width,
                                    array_to_matrix(biases_hidden, biases_hidden_width), 1, biases_hidden_width);

  for (size_t i = 0; i < hidden_output_width; i++) {
    hidden_output[i] = sigmoid(hidden_output_matrix[i][0]);
  }


  double** final_output_matrix = matrix_product(array_to_matrix(weights_output, weights_output_width), weights_output_width, 1,
                                                array_to_matrix(hidden_output, hidden_output_width), 1, hidden_output_width);

  double final_output = final_output_matrix[0][0] + bias_output;
  final_output = sigmoid(final_output);

  return final_output;
}

void backward_propagation(double* input, double* hidden_output, double output,
                          double expected_output, double* weights_hidden[], double* biases_hidden,
                          double* weights_output, double* bias_output) {
  
    double output_error = output - expected_output;
    double output_delta = output_error * output * (1 - output);

    for (size_t i = 0; i < HIDDEN_NEURONS; i++) {
        weights_output[i] -= LEARNING_RATE * output_delta * hidden_output[i];
    }
    *bias_output -= LEARNING_RATE * output_delta;

    double hidden_delta[HIDDEN_NEURONS];
    for (size_t i = 0; i < HIDDEN_NEURONS; i++) {
        double hidden_error = output_delta * weights_output[i];
        hidden_delta[i] = hidden_error * hidden_output[i] * (1 - hidden_output[i]); 
    }

    for (size_t i = 0; i < INPUT_NEURONS; i++) {
        for (size_t j = 0; j < HIDDEN_NEURONS; j++) {
            weights_hidden[i][j] -= LEARNING_RATE * hidden_delta[j] * input[i];
        }
    }

    for (size_t i = 0; i < HIDDEN_NEURONS; i++) {
        biases_hidden[i] -= LEARNING_RATE * hidden_delta[i];
    }
}


// cross entropy loss
double calculate_error(double* targets, double* predictions, size_t size) { 
  double epsilon = 1e-15;
  double loss = 0.0;

  for (size_t i = 0; i < size; i++) {
    double pred = predictions[i];
    if (pred < epsilon) {
      pred = epsilon;
    }
    if (pred > 1 - epsilon) {
      pred = 1 - epsilon;
    }

    loss += targets[i] * log(pred) + (1 - targets[i]) * log(1 - pred);
  }

  loss = -loss / size;

  return loss;
}



int main(int argc, char** argv) {
  double input[INPUT_NEURONS] = {atoi(argv[1]), atoi(argv[2])};
  double weights_hidden_0[HIDDEN_NEURONS] = {10.0, -10.0};
  double weights_hidden_1[HIDDEN_NEURONS] = {-10.0, 10.0};
  double* weights_hidden[INPUT_NEURONS] = {weights_hidden_0, weights_hidden_1};
  double biases_hidden[HIDDEN_NEURONS] = {-5, 15};
  double weights_output[HIDDEN_NEURONS] = {10.0, 10.0};
  double bias_output = -15;
  double hidden_output[HIDDEN_NEURONS];

  double output = forward_propagation(input, weights_hidden, biases_hidden, weights_output, bias_output, hidden_output);

  printf("Résultat de la forward propagation : %f\n", output);

  double * predictions;
  predictions = calloc(1, sizeof(double));
  predictions[0] = output;
  
  double * targets = calloc(1, sizeof(double));
  targets[0] = atoi(argv[3]);

  size_t size = sizeof(predictions) / sizeof(predictions[0]);

  double ce_loss = calculate_error(predictions, targets, size);
  printf("Cross-Entropy Loss: %f\n", ce_loss);

  return 0;
}
