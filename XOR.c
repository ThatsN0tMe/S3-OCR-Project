#include <math.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>

#define INPUT_NEURONS 2
#define HIDDEN_NEURONS 2
#define LAYERS 2
#define LEARNING_RATE 0.0001
#define EPOCHS 10000
int NEURONS_NUMBER[LAYERS] = {2, 2};


typedef struct neuron_t
{
  double actv;
  double *out_weights;
  double bias;
  double z;

  double d_actv;
  double *d_weights;
  double d_bias;
  double d_z;

} neuron;


typedef struct layer_t
{
  int neuron_number;
  struct neuron_t *neurons; 
} layer;




layer create_layer(int neuron_number) {
  neuron * neurons = calloc(neuron_number, sizeof(neuron));
  layer lay;
  lay.neuron_number = neuron_number;
  lay.neurons = neurons;
  return lay;
}

neuron create_neuron(int out_connections) {
  neuron neu;
  neu.actv = 0.0f;
  neu.bias = ((double)rand() / RAND_MAX) * 2 - 1;  // Random bias between -1 and 1
  neu.z = 0.0f;
  neu.d_actv = 0.0f;
  neu.d_bias = 0.0f;
  neu.d_z = 0.0f;

  if (out_connections > 0) {
    neu.out_weights = calloc(out_connections, sizeof(double));
    neu.d_weights = calloc(out_connections, sizeof(double));

    for (int i = 0; i < out_connections; i++) {
      neu.out_weights[i] = ((float)rand() / RAND_MAX) * 2 - 1;  // Random weight between -1 and 1
      neu.d_weights[i] = 0.0f;
    }
  }

  else {
    neu.out_weights = NULL;
    neu.d_weights = NULL;
  }

  return neu;
}



int create_network() {

  layer * layer = malloc(sizeof(layer) * LAYERS);

  for (int i = 0; i < LAYERS; i++) {

    layer[i] = create_layer(NEURONS_NUMBER[i]);

    printf("Created Layer: %d\n", i+1);
    printf("Number of Neurons in Layer %d: %d\n", i+1,layer[i].neuron_number);

    for (int j = 0; j < NEURONS_NUMBER[i]; j++) {
      if (i < LAYERS - 1) {
        neuron neuron = create_neuron(NEURONS_NUMBER[i+1]);
        layer[i].neurons[j] = neuron;

        printf("Neuron %d in Layer %d created\n",j+1,i+1);

      }
      else {
        neuron neuron = create_neuron(0);
        layer[i].neurons[j] = neuron;
        printf("Neuron %d in Layer %d created\n",j+1,i+1);
      }
    }
  }
}










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
    random = (double)rand() / (double)RAND_MAX * 2.0f - 1.0f;
    weights[i] = random;
  }
}

void initialize_biases(double biases[], int size) {
  double random;
  for (int i = 0; i < size; i++) {
    random = (double)rand() / (double)RAND_MAX * 2.0f - 1.0f;
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


int main() {
  create_network();
  return 0;
}

