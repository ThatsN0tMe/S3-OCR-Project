#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <zlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define INPUT_SIZE 784
#define HIDDEN_SIZE 128
#define OUTPUT_SIZE 26
#define LEARNING_RATE 0.001
#define EPOCHS 10

#define TRAIN_IMAGES "gzip/emnist-letters-train-images-idx3-ubyte.gz"
#define TRAIN_LABELS "gzip/emnist-letters-train-labels-idx1-ubyte.gz"
#define TEST_IMAGES "gzip/emnist-letters-test-images-idx3-ubyte.gz"
#define TEST_LABELS "gzip/emnist-letters-test-labels-idx1-ubyte.gz"


// Activation and derivatives
float sigmoid(float x) { return 1.0f / (1.0f + expf(-x)); }
float sigmoid_derivative(float x) { return x * (1.0f - x); }

// Memory allocation for matrices
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

void load_gz_file(const char *filename, unsigned char *buffer, size_t size) {
    gzFile file = gzopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error opening %s\n", filename);
        exit(1);
    }
    gzseek(file, 16, SEEK_SET); // Sauter l'en-tête
    gzread(file, buffer, size);
    gzclose(file);
}

// Normalize images
void normalize_images(unsigned char *input, float *output, int image_count) {
    for (int i = 0; i < image_count * INPUT_SIZE; i++) {
        output[i] = input[i] / 255.0f;
    }
}

// Forward pass
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

// Backpropagation
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


int load_weights(const char *filename, float **weights1, float *bias1, float **weights2, float *bias2) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Error opening weights file.\n");
    return 0;
  }

  for (int i = 0; i < INPUT_SIZE; i++) {
    for (int j = 0; j < HIDDEN_SIZE; j++) {
      fscanf(file, "%f", &weights1[i][j]);
    }
  }
  for (int i = 0; i < HIDDEN_SIZE; i++) {
    fscanf(file, "%f", &bias1[i]);
    for (int j = 0; j < OUTPUT_SIZE; j++) {
      fscanf(file, "%f", &weights2[i][j]);
    }
  }
  for (int i = 0; i < OUTPUT_SIZE; i++) {
    fscanf(file, "%f", &bias2[i]);
  }

  fclose(file);
  printf("Weights succesfully loaded !\n");
  return 1;
}


int save_weights(const char *filename, float **weights1, float *bias1, float **weights2, float *bias2) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error opening file for saving weights.\n");
        return 0;
    }

    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            fprintf(file, "%f\n", weights1[i][j]);
        }
    }
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        fprintf(file, "%f\n", bias1[i]);
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            fprintf(file, "%f\n", weights2[i][j]);
        }
    }
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        fprintf(file, "%f\n", bias2[i]);
    }

    fclose(file);
    return 1;
}

float random_weight() {
    return ((float)rand() / RAND_MAX) * 2.0f - 1.0f; // Poids aléatoires dans [-1, 1]
}

void train(float *train_images, unsigned char *train_labels, int train_size,
           float **weights1, float *bias1, float **weights2, float *bias2,
           float *hidden, float *output, float *hidden_error,
           float *output_error) {
  for (int epoch = 0; epoch < EPOCHS; epoch++) {
    float total_error = 0.0f;
    for (int i = 0; i < train_size; i++) {
      float *input = train_images + i * INPUT_SIZE;
      int label = train_labels[i];


      // Forward and Backward passes
      forward(input, weights1, bias1, hidden, weights2, bias2, output);
      backward(input, hidden, output, label, weights1, bias1, weights2,
               bias2, hidden_error, output_error);

      // Mise à jour des poids et accumulation d'erreur
      for (int j = 0; j < OUTPUT_SIZE; j++) {
        float target = (j == label ? 1.0f : 0.0f);
        total_error += 0.5f * powf(target - output[j], 2);
      }

      // Print progress tous les 1000 échantillons
      if ((i + 1) % 1000 == 0) {
        printf("\rEpoch %d/%d, Sample %d/%d, Error: %.4f",
               epoch + 1, EPOCHS, i + 1, train_size, total_error / (i + 1));
        fflush(stdout); // Assure que la sortie est imprimée immédiatement
      }
    }
    printf("\nEpoch %d completed. Average Error: %.4f\n", epoch + 1, total_error / train_size);
  }
}

// Test network
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

char label_to_char(int label) {
  return 'A' + (label - 1); // Convertit 1-26 en 'A'-'Z'
}

void print_image(float *image, int label) {
    printf("Label: %c\n", label_to_char(label));
    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++) {
            // Si la valeur du pixel est plus grande que 128, affiche "#", sinon "."
            if (image[i * 28 + j] != 0) {
                printf("#");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
}


void print_raw_image(unsigned char *image) {
    for (int i = 0; i < INPUT_SIZE; i++) {
        if (i % 28 == 0 && i != 0) {
            printf("\n"); // Nouvelle ligne après chaque ligne de 28 pixels
        }
        printf("%c", image[i] > 200 ? '#' : '.'); // Utilise '#' pour les pixels sombres et '.' pour les pixels clairs
    }
    printf("\n");
}


void predict(float *image, float **weights1, float *bias1, float **weights2, float *bias2) {
    float *hidden = allocate_array(HIDDEN_SIZE);
    float *output = allocate_array(OUTPUT_SIZE);

    forward(image, weights1, bias1, hidden, weights2, bias2, output);

    int predicted_label = 0;
    for (int i = 1; i < OUTPUT_SIZE; i++) {
        if (output[i] > output[predicted_label]) {
            predicted_label = i;
        }
    }

    // Convert label to character (1=A, 2=B, ..., 26=Z)
    char predicted_char = label_to_char(predicted_label + 1);
    printf("Predicted Label: %d (%c)\n", predicted_label + 1, predicted_char);

    // Print the image (converted back to raw for visualization)
    unsigned char *raw_image = (unsigned char *)malloc(INPUT_SIZE);
    for (int i = 0; i < INPUT_SIZE; i++) {
        raw_image[i] = (unsigned char)(image[i] * 255);
    }
    print_image(image, -1);
    free(raw_image);

    free(hidden);
    free(output);
}

float * reverse_image(float * image) {
  for (int i = 0; i < 28; i++) {
    for (int j = 0; j < 28; j++) {
      image[i * 28 + j] = 255 - image[i * 28 + j];
    }
  }
}


void load_and_predict_sdl(const char *filename, float **weights1, float *bias1,
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
  Uint8 r, g, b;
  for (int y = 0; y < 28; y++) {
    for (int x = 0; x < 28; x++) {
      Uint32 pixel = ((Uint32 *)image->pixels)[y * image->w + x];
      SDL_GetRGB(pixel, image->format, &r, &g, &b);
      normalized_image[y * 28 + x] = (0.299 * r + 0.587 * g + 0.114 * b) / 1;
    }
  }

  reverse_image(normalized_image);

  predict(normalized_image, weights1, bias1, weights2, bias2);

  free(normalized_image);
  SDL_FreeSurface(image);
  SDL_Quit();
}

char emnist_label_to_char(int label) {
    // Mapping basé sur EMNIST
    const char mapping[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return mapping[label];
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s TRAIN | <image.png>\n", argv[0]);
    return 1;
  }

  // Allouer les ressources nécessaires
  float **weights1 = allocate_matrix(INPUT_SIZE, HIDDEN_SIZE);
  float *bias1 = allocate_array(HIDDEN_SIZE);
  float **weights2 = allocate_matrix(HIDDEN_SIZE, OUTPUT_SIZE);
  float *bias2 = allocate_array(OUTPUT_SIZE);


  if (strcmp(argv[1], "TRAIN") == 0) {
    // Mode entraînement
    printf("Starting training...\n");

    int train_size = 100000;
    int test_size = 10000;

    unsigned char *train_images_raw = (unsigned char *)malloc(train_size * INPUT_SIZE);
    unsigned char *train_labels = (unsigned char *)malloc(train_size);
    unsigned char *test_images_raw = (unsigned char *)malloc(test_size * INPUT_SIZE);
    unsigned char *test_labels = (unsigned char *)malloc(test_size);

    load_gz_file(TRAIN_IMAGES, train_images_raw, train_size * INPUT_SIZE);
    load_gz_file(TRAIN_LABELS, train_labels, train_size);
    load_gz_file(TEST_IMAGES, test_images_raw, test_size * INPUT_SIZE);
    load_gz_file(TEST_LABELS, test_labels, test_size);

    float *train_images = allocate_array(train_size * INPUT_SIZE);
    float *test_images = allocate_array(test_size * INPUT_SIZE);

    normalize_images(train_images_raw, train_images, train_size);
    normalize_images(test_images_raw, test_images, test_size);

    float *hidden = allocate_array(HIDDEN_SIZE);
    float *output = allocate_array(OUTPUT_SIZE);
    float *hidden_error = allocate_array(HIDDEN_SIZE);
    float *output_error = allocate_array(OUTPUT_SIZE);

    if (!load_weights("weights.txt", weights1, bias1, weights2, bias2)) {
      printf("No weights file can be found, initializing new ones...\n");

      for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
          weights1[i][j] = random_weight();
        }
      }
    }

    int min_label = train_labels[0], max_label = train_labels[0];
    for (int i = 1; i < train_size; i++) {
      if (train_labels[i] < min_label) min_label = train_labels[i];
      if (train_labels[i] > max_label) max_label = train_labels[i];
    }
    printf("Label range: %d to %d\n", min_label, max_label);

    for (int i = 0; i < 10; i++) {
      printf("%d\n", train_labels[i]);
      print_image(&train_images[i * INPUT_SIZE], train_labels[i]);
    }

    train(train_images, train_labels, train_size, weights1, bias1, weights2, bias2, hidden, output, hidden_error, output_error);

    if (save_weights("weights.txt", weights1, bias1, weights2, bias2) != 1) {
      fprintf(stderr, "Error saving weights!\n");
      exit(1);
    }

    test(test_images, test_labels, test_size, weights1, bias1, weights2, bias2, hidden, output);

    // Libérer les ressources
    free(train_images_raw);
    free(train_labels);
    free(test_images_raw);
    free(test_labels);
    free(train_images);
    free(test_images);
    free(hidden);
    free(output);
    free(hidden_error);
    free(output_error);

  } else {
    // Mode prédiction
    printf("Predicting for image: %s\n", argv[1]);

    // Charger les poids sauvegardés
    load_weights("weights.txt", weights1, bias1, weights2, bias2);

    // Charger et prédire pour une image donnée
    load_and_predict_sdl(argv[1], weights1, bias1, weights2, bias2);
  }

  // Libérer les poids et biais
  for (int i = 0; i < INPUT_SIZE; i++) free(weights1[i]);
  for (int i = 0; i < HIDDEN_SIZE; i++) free(weights2[i]);
  free(weights1);
  free(weights2);
  free(bias1);
  free(bias2);

  return 0;
}

