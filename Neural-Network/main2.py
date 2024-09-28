import numpy as np

class NeuralNetwork:
    def __init__(self, input_size, hidden_size, output_size, learning_rate):
        # Poids et biais pour la première couche (entrée -> cachée)
        self.weights_input_hidden = np.random.randn(input_size, hidden_size)
        self.bias_hidden = np.random.randn(hidden_size)

        # Poids et biais pour la deuxième couche (cachée -> sortie)
        self.weights_hidden_output = np.random.randn(hidden_size, output_size)
        self.bias_output = np.random.randn(output_size)

        self.learning_rate = learning_rate

    def _relu(self, x):
        return np.maximum(0, x)

    def _relu_deriv(self, x):
        return np.where(x > 0, 1, 0)

    def _softmax(self, x):
        exp_x = np.exp(x - np.max(x))
        return exp_x / exp_x.sum(axis=1, keepdims=True)

    def predict(self, input_vector):
        # Propagation avant (entrée -> couche cachée -> sortie)

        # Couche 1 : Entrée -> Cachée
        hidden_input = np.dot(input_vector, self.weights_input_hidden) + self.bias_hidden
        hidden_output = self._relu(hidden_input)

        # Couche 2 : Cachée -> Sortie
        final_input = np.dot(hidden_output, self.weights_hidden_output) + self.bias_output
        final_output = self._softmax(final_input)

        return final_output

    def _compute_gradients(self, input_vector, target):
        # Propagation avant (entrée -> couche cachée -> sortie)
        hidden_input = np.dot(input_vector, self.weights_input_hidden) + self.bias_hidden
        hidden_output = self._relu(hidden_input)

        final_input = np.dot(hidden_output, self.weights_hidden_output) + self.bias_output
        prediction = self._softmax(final_input)

        # Calcul des gradients pour la rétropropagation
        derror_dprediction = prediction - target

        # Couche cachée -> sortie (poids et biais)
        dlayer2_dweights = np.dot(hidden_output.T, derror_dprediction)
        dlayer2_dbias = np.sum(derror_dprediction, axis=0)

        # Couche entrée -> cachée (poids et biais)
        derror_dhidden_output = np.dot(derror_dprediction, self.weights_hidden_output.T)
        dhidden_dinput = self._relu_deriv(hidden_input)
        dlayer1_dweights = np.dot(input_vector.T, derror_dhidden_output * dhidden_dinput)
        dlayer1_dbias = np.sum(derror_dhidden_output * dhidden_dinput, axis=0)

        return dlayer1_dweights, dlayer1_dbias, dlayer2_dweights, dlayer2_dbias

    def _update_parameters(self, dlayer1_dweights, dlayer1_dbias, dlayer2_dweights, dlayer2_dbias):
        # Mise à jour des poids et des biais
        self.weights_input_hidden -= self.learning_rate * dlayer1_dweights
        self.bias_hidden -= self.learning_rate * dlayer1_dbias
        self.weights_hidden_output -= self.learning_rate * dlayer2_dweights
        self.bias_output -= self.learning_rate * dlayer2_dbias

    def train(self, input_vectors, targets, iterations):
        for iteration in range(iterations):
            random_data_index = np.random.randint(len(input_vectors))

            input_vector = input_vectors[random_data_index:random_data_index+1]
            target = targets[random_data_index:random_data_index+1]

            # Calcul des gradients
            dlayer1_dweights, dlayer1_dbias, dlayer2_dweights, dlayer2_dbias = self._compute_gradients(
                input_vector, target
            )

            # Mise à jour des paramètres
            self._update_parameters(dlayer1_dweights, dlayer1_dbias, dlayer2_dweights, dlayer2_dbias)


# Exemple d'utilisation
input_vectors = np.random.rand(8, 784)  # Par exemple, 8 images de 28x28 pixels
targets = np.eye(26)[np.random.randint(0, 26, size=8)]  # 8 cibles, correspondant aux 26 lettres

learning_rate = 0.01
neural_network = NeuralNetwork(input_size=784, hidden_size=128, output_size=26, learning_rate=learning_rate)

# Entraîner le réseau avec plusieurs couches
neural_network.train(input_vectors, targets, iterations=1000)

# Prédiction sur de nouveaux exemples
predictions = neural_network.predict(input_vectors)
print(predictions)

