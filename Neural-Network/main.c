#include <stdio.h>
#include <err.h>
#include <math.h>

// These are tests arrays
double input_vectors[] = {2, 1.5};
double weights_1[] = {1.45, -0.66};
double bias = 1;


struct Neuron {
  double weights;
  double bias;
  double learning_rate;
}

// computing the dot product of input_vector and weights
double dot(double* input_vectors, double* weights) {
  int len = sizeof(weights) / 4;
  if (len != sizeof(input_vectors) / 4) 
    errx(1, "There is a diferent amount of wheights and input vectors");
  double sum = 0;
  for (int i = 0; i < len; i++) {
    sum += input_vectors[i] * weights[i];
  }
  return sum;

}

double sigmoid(double x) {
  return 1 / (1 + exp(-x));
}

double sigmoid_deriv(double x) {
  return sigmoid(x) * (1 - sigmoid(x));
}

double make_prediction(double* input_vectors, double* weights, double bias) {
  double layer_1 = dot(input_vectors, weights) + bias;
  double layer_2 = sigmod(layer_1);
  return layer_2;
}

int main() {
  double target = 0;
  double prediction = make_prediction(input_vectors, weights_1, 0);
  double mse = (prediction - target) * (prediction - target);

  double derror_dpredictions = 2 * (prediction - target);
  double layer_1 = dot(input_vectors, weights_1) + bias;
  double dprediction_dlayer1 = sigmoid_deriv(layer1);
  double dlayer1_dbias = 1;
  double dlayer1_dweights = input_vectors;

  // New calculus of the weights
  double derror_dbias = derror_dpredictions * dprediction_dlayer1 * dlayer1_dbias;

  // New calculus of the bias
  double derror_weights = derror_dpredictions * dprediction_dlayer1 * dlayer1_dweights;

  printf("Prediction: %f; Error: %f", prediction, error);
}
