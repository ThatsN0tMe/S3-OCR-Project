#include "neural-net/Network.h"
#include "neural-net/network_functions.h"
#include "shared/arr_helpers.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <err.h>

char* model = "models/easy-final.model";

char use_model(char* model_path, char* image_path){
  struct Network net = load_network(model_path);
  double* input = image_to_input(image_path);
  double **a_mat = calloc(net.layernb, sizeof(double*));
  double **z_mat = calloc(net.layernb, sizeof(double*));
  double* res = feedforward(net, input, z_mat, a_mat);
  double confidence;
  char prediction = output_to_prediction(res, &confidence);
  free_double_matrix(a_mat, net.layernb);
  free_double_matrix(z_mat, net.layernb);
  // we dont free res because it is just a_mat[L] which was already freed
  free(input);
  free_network(&net);
  return prediction;
}

char get_char(char* image_path) {
  return use_model(model, image_path);
}


int main(int argc, char** argv) {
  if (argc != 2) {
    err(EXIT_FAILURE, "Parameter must be : {Path_to_image}");
  }
  printf("Prediction of %s...\nResult : %c", argv[1], get_char(argv[1]));
}

