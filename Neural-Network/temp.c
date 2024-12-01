#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <zlib.h>

#define INPUT_SIZE 784
#define HIDDEN_SIZE 128
#define OUTPUT_SIZE 10
#define LEARNING_RATE 0.01
#define EPOCHS 10

#define TRAIN_IMAGES "gzip/emnist-digits-train-images-idx3-ubyte.gz"
#define TRAIN_LABELS "gzip/emnist-digits-train-labels-idx1-ubyte.gz"
#define TEST_IMAGES "gzip/emnist-digits-test-images-idx3-ubyte.gz"
#define TEST_LABELS "gzip/emnist-digits-test-labels-idx1-ubyte.gz"

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

// Load gzipped files
void load_gz_file(const char *filename, unsigned char *buffer, size_t size) {
    gzFile file = gzopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error opening %s\n", filename);
        exit(1);
    }
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

            // Accumulate error for progress tracking
            for (int j = 0; j < OUTPUT_SIZE; j++) {
                float target = (j == label ? 1.0f : 0.0f);
                total_error += 0.5f * powf(target - output[j], 2);
            }

            // Print progress every 1000 samples
            if ((i + 1) % 1000 == 0) {
                printf("\rEpoch %d/%d, Sample %d/%d, Error: %.4f", 
                       epoch + 1, EPOCHS, i + 1, train_size, total_error / (i + 1));
                fflush(stdout); // Ensure the output is printed immediately
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

// Main function
int main() {
    int train_size = 60000;
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

    float **weights1 = allocate_matrix(INPUT_SIZE, HIDDEN_SIZE);
    float *bias1 = allocate_array(HIDDEN_SIZE);
    float **weights2 = allocate_matrix(HIDDEN_SIZE, OUTPUT_SIZE);
    float *bias2 = allocate_array(OUTPUT_SIZE);
    float *hidden = allocate_array(HIDDEN_SIZE);
    float *output = allocate_array(OUTPUT_SIZE);
    float *hidden_error = allocate_array(HIDDEN_SIZE);
    float *output_error = allocate_array(OUTPUT_SIZE);

    train(train_images, train_labels, train_size, weights1, bias1, weights2, bias2, hidden, output, hidden_error, output_error);
    test(test_images, test_labels, test_size, weights1, bias1, weights2, bias2, hidden, output);

    free(train_images_raw);
    free(train_labels);
    free(test_images_raw);
    free(test_labels);
    free(train_images);
    free(test_images);
    for (int i = 0; i < INPUT_SIZE; i++) free(weights1[i]);
    for (int i = 0; i < HIDDEN_SIZE; i++) free(weights2[i]);
    free(weights1);
    free(weights2);
    free(bias1);
    free(bias2);
    free(hidden);
    free(output);
    free(hidden_error);
    free(output_error);

    return 0;
}

