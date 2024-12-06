#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <pthread.h>
#include <err.h>

#define INPUT_SIZE 784
#define HIDDEN_SIZE 128
#define OUTPUT_SIZE 56
#define LEARNING_RATE 0.01
#define EPOCHS 10000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char * dataset = "/dataset/letters.csv";

struct thread_parameter {
  float * bach_images;
  unsigned char *train_labels;
  int bach_size;
  float **weights1;
  float *bias1;
  float **weights2;
  float *bias2;
  float *hidden;
  float *output;
  float *hidden_error;
  float *output_error;
}args;


// Activation et dérivées
float sigmoid(float x) { return 1.0f / (1.0f + expf(-x)); }
float sigmoid_derivative(float x) { return x * (1.0f - x); }

// Allocation mémoire pour les matrices
float *allocate_array(int size) {
  return (float *)calloc(size, sizeof(float));
}

float **allocate_matrix(int rows, int cols) {
  float **matrix = (float **)malloc(rows * sizeof(float *));
  for (int i = 0; i < rows; i++) {
    matrix[i] = allocate_array(cols);
  }
  return matrix;
}

void initialize_weights(float **weights, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      weights[i][j] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f; // Poids entre -1 et 1
    }
  }
}

void initialize_biases(float *biases, int size) {
  for (int i = 0; i < size; i++) {
    biases[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f; // Biases entre -1 et 1
  }
}


// Chargement des données CSV
void load_csv_file(const char *filename, float *images, unsigned char *labels, int num_samples) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Error opening %s\n", filename);
    exit(1);
  }

  for (int i = 0; i < num_samples; i++) {
    fscanf(file, "%hhu,", &labels[i]);
    for (int j = 0; j < INPUT_SIZE; j++) {
      int pixel;
      fscanf(file, "%d,", &pixel);
      images[i * INPUT_SIZE + j] = pixel / 255.0f;
    }
  }

  fclose(file);
}

void shuffle_data(float *images, unsigned char *labels, int num_samples) {
  srand(time(NULL)); // Initialiser le générateur de nombres aléatoires
  for (int i = num_samples - 1; i > 0; i--) {
    int j = rand() % (i + 1);

    // Échanger les images
    for (int k = 0; k < INPUT_SIZE; k++) {
      float temp = images[i * INPUT_SIZE + k];
      images[i * INPUT_SIZE + k] = images[j * INPUT_SIZE + k];
      images[j * INPUT_SIZE + k] = temp;
    }

    // Échanger les labels
    unsigned char temp_label = labels[i];
    labels[i] = labels[j];
    labels[j] = temp_label;
  }
}

void forward(float *input, float **weights1, float *bias1, float *hidden,
             float **weights2, float *bias2, float *output) {
  for (int i = 0; i < HIDDEN_SIZE; i++) {
    hidden[i] = bias1[i];
    for (int j = 0; j < INPUT_SIZE; j++) {
      hidden[i] += input[j] * weights1[j][i];
    }
    hidden[i] = sigmoid(hidden[i]);
  }


  for (int i = 0; i < OUTPUT_SIZE; i++) {
    output[i] = bias2[i];
    for (int j = 0; j < HIDDEN_SIZE; j++) {
      output[i] += hidden[j] * weights2[j][i];
    }
    output[i] = sigmoid(output[i]);
  }
}

void backward(float *input, float *hidden, float *output, int label,
              float **weights1, float *bias1, float **weights2, float *bias2,
              float *hidden_error, float *output_error) {
  for (int i = 0; i < OUTPUT_SIZE; i++) {
    output_error[i] = (i == label ? 1.0f : 0.0f) - output[i];
  }

  for (int i = 0; i < HIDDEN_SIZE; i++) {
    hidden_error[i] = 0.0f;
    for (int j = 0; j < OUTPUT_SIZE; j++) {
      hidden_error[i] += output_error[j] * weights2[i][j];
    }
    hidden_error[i] *= sigmoid_derivative(hidden[i]);
  }

  for (int i = 0; i < HIDDEN_SIZE; i++) {
    for (int j = 0; j < OUTPUT_SIZE; j++) {
      weights2[i][j] += LEARNING_RATE * output_error[j] * hidden[i];
    }
  }

  for (int i = 0; i < OUTPUT_SIZE; i++) {
    bias2[i] += LEARNING_RATE * output_error[i];
  }

  for (int i = 0; i < INPUT_SIZE; i++) {
    for (int j = 0; j < HIDDEN_SIZE; j++) {
      weights1[i][j] += LEARNING_RATE * hidden_error[j] * input[i];
    }
  }

  for (int i = 0; i < HIDDEN_SIZE; i++) {
    bias1[i] += LEARNING_RATE * hidden_error[i];
  }
}

int load_weights(const char *filename, float **weights1, float *bias1,
                 float **weights2, float *bias2) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    fprintf(stderr, "Error opening file for reading: %s\n", filename);
    return 0;
  }

  for (int i = 0; i < INPUT_SIZE; i++) {
    fread(weights1[i], sizeof(float), HIDDEN_SIZE, file);
  }
  fread(bias1, sizeof(float), HIDDEN_SIZE, file);

  for (int i = 0; i < HIDDEN_SIZE; i++) {
    fread(weights2[i], sizeof(float), OUTPUT_SIZE, file);
  }
  fread(bias2, sizeof(float), OUTPUT_SIZE, file);

  fclose(file);
  printf("Weights and biases loaded from %s\n", filename);
  return 1;
}


void save_weights(const char *filename, float **weights1, float *bias1,
                  float **weights2, float *bias2) {
  FILE *file = fopen(filename, "wb");
  if (!file) {
    fprintf(stderr, "Error opening file for writing: %s\n", filename);
    return;
  }

  for (int i = 0; i < INPUT_SIZE; i++) {
    fwrite(weights1[i], sizeof(float), HIDDEN_SIZE, file);
  }
  fwrite(bias1, sizeof(float), HIDDEN_SIZE, file);

  for (int i = 0; i < HIDDEN_SIZE; i++) {
    fwrite(weights2[i], sizeof(float), OUTPUT_SIZE, file);
  }
  fwrite(bias2, sizeof(float), OUTPUT_SIZE, file);

  fclose(file);
}

void print_image(float *images, int index) {
  if (index < 0) {
    fprintf(stderr, "Invalid image index: %d\n", index);
    return;
  }

  printf("Image at index %d:\n", index);

  // Chaque image est de 28x28 pixels
  for (int i = 0; i < 28; i++) {
    for (int j = 0; j < 28; j++) {
      float pixel = images[index * INPUT_SIZE + i * 28 + j];

      // S'assurer que la valeur est dans la plage [0, 1]
      if (pixel < 0.0f) pixel = 0.0f;
      if (pixel > 1.0f) pixel = 1.0f;

      // Choix de caractère basé sur l'intensité du pixel
      if (pixel > 0.75f) {
        printf("@"); // Pixel sombre
      } else if (pixel > 0.5f) {
        printf("#");
      } else if (pixel > 0.25f) {
        printf(".");
      } else {
        printf(" "); // Pixel clair
      }
    }
    printf("\n");
  }
}

void * train(void * thr_p) {
  struct thread_parameter *new_args = thr_p;
  float * bach_images = new_args->bach_images;
  unsigned char *train_labels = new_args->train_labels;
  int bach_size = new_args->bach_size;
  float **weights1 = new_args-> weights1;
  float *bias1 = new_args->bias1;
  float **weights2 = new_args->weights2;
  float *bias2 = new_args->bias2;
  float *hidden = new_args->hidden;
  float *output = new_args->output;
  float *hidden_error = new_args->hidden_error;
  float *output_error = new_args->output_error;

  int max_bach_size = bach_size;
  bach_size = 100;

  float* input;

  for (int epoch = 0; epoch < EPOCHS; epoch++) {
    float total_error = 0.0f;

    for (int i = 0; i < bach_size; i++) {
      input = bach_images + i * INPUT_SIZE;
      int label = train_labels[i];

      forward(input, weights1, bias1, hidden, weights2, bias2, output);
      backward(input, hidden, output, label, weights1, bias1, weights2, bias2, hidden_error, output_error);

      for (int j = 0; j < OUTPUT_SIZE; j++) {
        float target = (j == label ? 1.0f : 0.0f);
        total_error += 0.5f * powf(target - output[j], 2);
      }
    }
    if (epoch % 500 == 0) {
      shuffle_data(bach_images, train_labels, max_bach_size);
      save_weights("weights.txt", weights1, bias1, weights2, bias2);
    }
    printf("Epoch %d completed by thread : %#lx. Average Error: %.4f\n", epoch + 1, pthread_self(), total_error / bach_size);
  }
  return EXIT_SUCCESS;
}

void test(float *test_images, unsigned char *test_labels, int test_size,
          float **weights1, float *bias1, float **weights2, float *bias2,
          float *hidden, float *output) {
  int correct = 0;
  for (int i = 0; i < test_size; i++) {
    float *input = test_images + i * INPUT_SIZE;
    int label = test_labels[i];
    forward(input, weights1, bias1, hidden, weights2, bias2, output);
    int predicted = 0;
    for (int j = 1; j < OUTPUT_SIZE; j++) {
      if (output[j] > output[predicted]) {
        predicted = j;
      }
    }
    if (predicted == label) {
      correct++;
    }
  }
  printf("Accuracy: %.2f%%\n", (correct / (float)test_size) * 100.0f);
}

void reverse_image(float *image) {
  for (int i = 0; i < 28; i++) {
    for (int j = 0; j < 28; j++) {
      image[i * 28 + j] = 1.0f - image[i * 28 + j];
    }
  }
}

void normalize_image(SDL_Surface *image, float *output) {
  Uint8 r, g, b;
  for (int y = 0; y < 28; y++) {
    for (int x = 0; x < 28; x++) {
      Uint32 pixel = ((Uint32 *)image->pixels)[y * image->w + x];
      SDL_GetRGB(pixel, image->format, &r, &g, &b);
      output[y * 28 + x] = (0.299 * r + 0.587 * g + 0.114 * b) / 255.0f;
    }
  }
}


void load_and_predict(const char *filename, float **weights1, float *bias1,
                      float **weights2, float *bias2) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
    return;
  }

  SDL_Surface *image = IMG_Load(filename);
  if (!image) {
    fprintf(stderr, "Error loading image %s: %s\n", filename, IMG_GetError());
    SDL_Quit();
    return;
  }

  if (image->w != 28 || image->h != 28) {
    fprintf(stderr, "Image must be 28x28 pixels.\n");
    SDL_FreeSurface(image);
    SDL_Quit();
    return;
  }

  float *normalized_image = allocate_array(INPUT_SIZE);
  normalize_image(image, normalized_image);
  reverse_image(normalized_image);

  float *hidden = allocate_array(HIDDEN_SIZE);
  float *output = allocate_array(OUTPUT_SIZE);

  print_image(normalized_image, 0);

  forward(normalized_image, weights1, bias1, hidden, weights2, bias2, output);

  for (int i = 0; i < 26; i++) {
    printf("%c : %f\n", 'A' + i, output[i]);
  }
  int predicted_label = 0;
  for (int i = 1; i < OUTPUT_SIZE; i++) {
    if (output[i] > output[predicted_label]) {
      predicted_label = i;
    }
  }

  char predicted_char = 'A' + predicted_label;
  printf("Predicted Label: %d (%c)\n", predicted_label, predicted_char);

  free(normalized_image);
  free(hidden);
  free(output);
  SDL_FreeSurface(image);
  SDL_Quit();
}


int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <MODE> [additional arguments]\n", argv[0]);
    return 1;
  }

  // Allocation des poids et biais
  float **weights1 = allocate_matrix(INPUT_SIZE, HIDDEN_SIZE);
  float *bias1 = allocate_array(HIDDEN_SIZE);
  float **weights2 = allocate_matrix(HIDDEN_SIZE, OUTPUT_SIZE);
  float *bias2 = allocate_array(OUTPUT_SIZE);

  size_t thread_number = 1;

  if (strcmp(argv[1], "TRAIN") == 0) {
    // Vérifie le nombre d'arguments
    if (argc != 3) {
      fprintf(stderr, "Usage: %s TRAIN <training_data_file>\n", argv[0]);
      return 1;
    }

    int train_size = 390000;
    if (train_size <= 0) {
      fprintf(stderr, "Invalid train size: %s\n", argv[3]);
      return 1;
    }

    float *train_images = allocate_array(train_size * INPUT_SIZE);
    unsigned char *train_labels = (unsigned char *)malloc(train_size * sizeof(unsigned char));
    dataset = argv[2];

    printf("Loading training data from: %s\n", dataset);
    load_csv_file(dataset, train_images, train_labels, train_size);

    float *hidden = allocate_array(HIDDEN_SIZE);
    float *output = allocate_array(OUTPUT_SIZE);
    float *hidden_error = allocate_array(HIDDEN_SIZE);
    float *output_error = allocate_array(OUTPUT_SIZE);
    pthread_t thrs[thread_number];

    shuffle_data(train_images, train_labels, train_size);

    for (size_t i = 0; i < thread_number; i++) {
      int bach_size = (train_size / thread_number);
      float *bach_images = train_images + i * bach_size;

      struct thread_parameter param;

      param.bach_images = bach_images;
      param.train_labels = train_labels;
      param.bach_size = bach_size;
      param.weights1 = weights1;
      param.bias1 = bias1;
      param.weights2 = weights2;
      param.bias2 = bias2;
      param.hidden = hidden;
      param.output = output;
      param.hidden_error = hidden_error;
      param.output_error = output_error;


      //train(&param);
      if (pthread_create(&thrs[i], NULL, train, &param) != 0) {
        err(EXIT_FAILURE, "Thread cration failed");
      }
    }

    for (unsigned long i = 0; i < thread_number; i++) {
      pthread_join(thrs[i], NULL);
    }

    save_weights("weights.txt", weights1, bias1, weights2, bias2);
    printf("Training completed and weights saved to weights.txt.\n");

    free(train_images);
    free(train_labels);
    free(hidden);
    free(output);
    free(hidden_error);
    free(output_error);
  } 
  else if (strcmp(argv[1], "PREDICT") == 0) {
    if (argc != 3) {
      fprintf(stderr, "Usage: %s PREDICT <image.png>\n", argv[0]);
      return 1;
    }

    printf("Predicting for image: %s\n", argv[2]);

    if (!load_weights("weights.txt", weights1, bias1, weights2, bias2)) {
      fprintf(stderr, "Weights file not found. Train the model first.\n");
      return 1;
    }

    load_and_predict(argv[2], weights1, bias1, weights2, bias2);
  } 
  else {
    fprintf(stderr, "Unknown mode: %s\n", argv[1]);
    fprintf(stderr, "Available modes: TRAIN, PREDICT\n");
    return 1;
  }

  // Libération de la mémoire
  for (int i = 0; i < INPUT_SIZE; i++) {
    free(weights1[i]);
  }
  free(weights1);

  free(bias1);

  for (int i = 0; i < HIDDEN_SIZE; i++) {
    free(weights2[i]);
  }
  free(weights2);

  free(bias2);

  return 0;
}
