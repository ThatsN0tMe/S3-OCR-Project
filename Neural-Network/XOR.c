#include <math.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define LAYER_NUMBER 3
#define LEARNING_RATE 0.001
#define EPOCHS 100000

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



layer* LAYERS;
int NEURONS_NUMBER[LAYER_NUMBER] = {2, 2, 1};
int** DESIRED_OUTPUTS;



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
    neu.bias = ((double)rand() / RAND_MAX) * 2 - 1;
    neu.z = 0.0f;
    neu.d_actv = 0.0f;
    neu.d_bias = 0.0f;
    neu.d_z = 0.0f;

    if (out_connections > 0) {
        neu.out_weights = calloc(out_connections, sizeof(double));
        neu.d_weights = calloc(out_connections, sizeof(double));

        for (int i = 0; i < out_connections; i++) {
            neu.out_weights[i] = ((double)rand() 
        / RAND_MAX) * 2 - 1;
            neu.d_weights[i] = 0.0f;
        }
    } else {
        neu.out_weights = NULL;
        neu.d_weights = NULL;
    }

    return neu;
}


int create_network() {

  LAYERS = malloc(sizeof(layer) * LAYER_NUMBER);

  for (int i = 0; i < LAYER_NUMBER; i++) {

    LAYERS[i] = create_layer(NEURONS_NUMBER[i]);

    for (int j = 0; j < NEURONS_NUMBER[i]; j++) {
      if (i < LAYER_NUMBER - 1) {
        neuron neuron = create_neuron(NEURONS_NUMBER[i+1]);
        LAYERS[i].neurons[j] = neuron;
      }
      else {
        neuron neuron = create_neuron(0);
        LAYERS[i].neurons[j] = neuron;
      }
    }
  }
  return EXIT_SUCCESS;
}


void forward_prop()
{

  for(int i = 1; i < LAYER_NUMBER; i++)
  {
    for(int j = 0; j < NEURONS_NUMBER[i]; j++)
    {
      LAYERS[i].neurons[j].z = LAYERS[i].neurons[j].bias;

      for(int k = 0; k < NEURONS_NUMBER[i-1]; k++)
      {
        LAYERS[i].neurons[j].z  = LAYERS[i].neurons[j].z 
          + ((LAYERS[i-1].neurons[k].out_weights[j])
          * (LAYERS[i-1].neurons[k].actv));
      }

      // Relu Activation Function for Hidden LAYERSers
      if (i < LAYER_NUMBER-1)
      {
        if ((LAYERS[i].neurons[j].z) < 0)
        {
          LAYERS[i].neurons[j].actv = 0;
        }

        else
      {
          LAYERS[i].neurons[j].actv = LAYERS[i].neurons[j].z;
        }
      }

      // Sigmoid Activation function for Output LAYERSer
      else
    {
        LAYERS[i].neurons[j].actv = 1/(1+exp(-LAYERS[i].neurons[j].z));
      }
    }
  }
}


void back_prop(int p)
{
  int i, j, k;

  if (p < 0 || p >= 4 || DESIRED_OUTPUTS == NULL) {
    fprintf(stderr, "Erreur: p (%d) est hors limites\n", p);
    return;
  }

  for (j = 0; j < NEURONS_NUMBER[LAYER_NUMBER - 1]; j++)
  {
    if (LAYERS[LAYER_NUMBER - 1].neurons == NULL 
      || DESIRED_OUTPUTS[p] == NULL) {
      fprintf(stderr, 
              "Erreur.\n");
      return;
    }

    double actv = LAYERS[LAYER_NUMBER - 1].neurons[j].actv;
    LAYERS[LAYER_NUMBER - 1].neurons[j].d_z = 
      (actv - DESIRED_OUTPUTS[p][j]) * actv * (1 - actv);

    for (k = 0; k < NEURONS_NUMBER[LAYER_NUMBER - 2]; k++)
    {
      LAYERS[LAYER_NUMBER - 2].neurons[k].d_weights[j] =
        LAYERS[LAYER_NUMBER - 1].neurons[j].d_z 
        * LAYERS[LAYER_NUMBER - 2].neurons[k].actv;

      LAYERS[LAYER_NUMBER - 2].neurons[k].d_actv =
        LAYERS[LAYER_NUMBER - 2].neurons[k].out_weights[j] 
        * LAYERS[LAYER_NUMBER - 1].neurons[j].d_z;
    }
    LAYERS[LAYER_NUMBER - 1].neurons[j].d_bias = 
      LAYERS[LAYER_NUMBER - 1].neurons[j].d_z;
  }

  for (i = LAYER_NUMBER - 2; i > 0; i--)
  {
    for (j = 0; j < NEURONS_NUMBER[i]; j++)
    {
      if (LAYERS[i].neurons[j].z >= 0) {
        LAYERS[i].neurons[j].d_z = LAYERS[i].neurons[j].d_actv;
      } else {
        LAYERS[i].neurons[j].d_z = 0;
      }

      for (k = 0; k < NEURONS_NUMBER[i - 1]; k++)
      {
        LAYERS[i - 1].neurons[k].d_weights[j] =
          LAYERS[i].neurons[j].d_z * LAYERS[i - 1].neurons[k].actv;

        if (i > 1) {
          LAYERS[i - 1].neurons[k].d_actv =
            LAYERS[i - 1].neurons[k].out_weights[j] 
            * LAYERS[i].neurons[j].d_z;
        }
      }
      LAYERS[i].neurons[j].d_bias = LAYERS[i].neurons[j].d_z;
    }
  }
}

void save_weights(const char *filename) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    perror("Failed to open file for writing");
    return;
  }

  for (int i = 0; i < LAYER_NUMBER; i++) {
    for (int j = 0; j < NEURONS_NUMBER[i]; j++) {
      fprintf(file, "%d,%d,%f", i, j, LAYERS[i].neurons[j].bias);
      for (int k = 0; k < NEURONS_NUMBER[i + 1]; k++) {
        fprintf(file, ",%f", LAYERS[i].neurons[j].out_weights[k]);
      }
      fprintf(file, "\n");
    }
  }

  fclose(file);
}


void load_weights(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file for reading");
        return;
    }

    int layer_index, neuron_index;
    double bias;

    while (fscanf(file, "%d,%d,%lf", &layer_index, 
                  &neuron_index, &bias) == 3) {
        LAYERS[layer_index].neurons[neuron_index].bias = bias;

        for (int k = 0; k < NEURONS_NUMBER[layer_index + 1]; k++) {
            double weight;
            if (fscanf(file, ",%lf", &weight) != 1) {
                fprintf(stderr, 
                        "Erreur de lecture des poids\n");
                fclose(file);
                return;
            }
            LAYERS[layer_index].neurons[neuron_index].out_weights[k] = weight;
        }
    }

    fclose(file);
    printf("Poids et biais chargés depuis %s\n", filename);
}


int main(int argc, char** argv) {

  int train = 0;
  if (argc > 1) {
    if (argc == 2) {
      if (strcmp(argv[1], "TRAIN") == 0)
        train = 1;
      else {
        err(EXIT_FAILURE, "Parameter must be : './XOR TRAIN' or './XOR'");
      }
    }
    else {
        err(EXIT_FAILURE, "Parameter must be : './XOR TRAIN' or './XOR'");
    }
  }
  srand(time(0)); 
  create_network();


  int input_data[4][2] = {
    {0, 0},
    {0, 1},
    {1, 0},
    {1, 1}
  };
  int output_data[4][1] = {
    {1},
    {0},
    {0},
    {1}
  };

  DESIRED_OUTPUTS = malloc(sizeof(int*) * 4);
  for (int i = 0; i < 4; i++) {
    DESIRED_OUTPUTS[i] = malloc(sizeof(int) 
                                * NEURONS_NUMBER[LAYER_NUMBER - 1]);
    for (int j = 0; j < NEURONS_NUMBER[LAYER_NUMBER - 1]; j++) {
      DESIRED_OUTPUTS[i][j] = output_data[i][j];
    }
  }

  if (train) {
    for (int epoch = 0; epoch < EPOCHS; epoch++) {
      double total_error = 0;

      for (int p = 0; p < 4; p++) {
        LAYERS[0].neurons[0].actv = input_data[p][0];
        LAYERS[0].neurons[1].actv = input_data[p][1];

        forward_prop();

        double output = LAYERS[LAYER_NUMBER - 1].neurons[0].actv;
        double error = output - DESIRED_OUTPUTS[p][0];
        total_error += error * error;

        if (epoch % 50000 == 0 && total_error > 0.5) {
          create_network();
          epoch = 0;
        }

        printf("Error probability : %f \n\n", total_error);

        back_prop(p);

        for (int i = 1; i < LAYER_NUMBER; i++) {
          for (int j = 0; j < NEURONS_NUMBER[i]; j++) {
            LAYERS[i].neurons[j].bias -= LEARNING_RATE 
              * LAYERS[i].neurons[j].d_bias;
            for (int k = 0; k < NEURONS_NUMBER[i - 1]; k++) {
              LAYERS[i - 1].neurons[k].out_weights[j] -= 
                LEARNING_RATE * LAYERS[i - 1].neurons[k].d_weights[j];
            }
          }
        }
      }
      save_weights("weights.txt");
      if (epoch % (EPOCHS / 10) == 0) {
        printf("Epoch %d, Mean Squared Error: %f\n", epoch, total_error / 4);
      }
    }
  }
  else {
    load_weights("weights.txt");
  }
  
  if (train) {
    return EXIT_SUCCESS;
  }
  else 
    printf("\nTest du réseau sans entraînement:\n\n");
  for (int p = 0; p < 4; p++) {
    LAYERS[0].neurons[0].actv = input_data[p][0];
    LAYERS[0].neurons[1].actv = input_data[p][1];

    forward_prop();

    double output = LAYERS[LAYER_NUMBER - 1].neurons[0].actv;
    printf(
      "Entrée: [%d, %d] => Sortie prédite: %.2f, Sortie attendue: %d\n\n",
           input_data[p][0], input_data[p][1], output, output_data[p][0]);
  }

  for (int i = 0; i < 4; i++) {
    free(DESIRED_OUTPUTS[i]);
  }
  free(DESIRED_OUTPUTS);
  for (int i = 0; i < LAYER_NUMBER; i++) {
    free(LAYERS[i].neurons);
  }
  free(LAYERS);

  return 0;
}

