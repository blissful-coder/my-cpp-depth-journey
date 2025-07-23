#include <iostream>
#include <vector>
#include <numeric> // For std::inner_product
#include <random>  // For random weight initialization

/**
 * @class Perceptron
 * @brief Represents a single neuron, the simplest form of a neural network.
 *
 * This class encapsulates the logic for a perceptron, which can be trained
 * to solve binary linear classification problems. It has weights, a bias,
_TITLE_ and methods to predict an output and to train itself based on feedback.
 */
class Perceptron {
private:
    // Member Variables
    std::vector<double> weights; // Stores the weight for each input feature.
    double bias;                 // The bias term, acts like an adjustable threshold.
    double learning_rate;        // Controls how much the weights and bias are adjusted during training.

    /**
     * @brief The activation function.
     * @param x The weighted sum of inputs plus bias.
     * @return 1 if x is greater than or equal to 0, otherwise 0.
     *
     * This is a simple "Heaviside step function". It's the decision-making
     * part of the neuron. If the combined signal (x) is strong enough, it "fires" (returns 1).
     */
    int step_function(double x) {
        return (x >= 0) ? 1 : 0;
    }

public:
    /**
     * @brief Constructor for the Perceptron class.
     * @param num_inputs The number of input features the perceptron will have.
     * @param lr The learning rate for training. Defaults to 0.01.
     *
     * Initializes the perceptron with a specific number of inputs.
     * Weights are initialized to small random values to break symmetry,
     * and the bias is also initialized randomly.
     */
    Perceptron(int num_inputs, double lr = 0.01) : learning_rate(lr) {
        // Setup for random number generation
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        // Initialize weights with small random numbers
        for (int i = 0; i < num_inputs; ++i) {
            weights.push_back(dis(gen));
        }
        // Initialize bias with a small random number
        bias = dis(gen);
    }

    /**
     * @brief Predicts the output for a given set of inputs.
     * @param inputs A vector of input values.
     * @return The predicted output (0 or 1).
     *
     * This method calculates the weighted sum of the inputs, adds the bias,
     * and then passes the result through the activation function to get the final prediction.
     */
    int predict(const std::vector<double>& inputs) {
        // Ensure the number of inputs matches the number of weights
        if (inputs.size() != weights.size()) {
            std::cerr << "Error: Number of inputs must match number of weights." << std::endl;
            return -1; // Indicate an error
        }

        // Calculate the weighted sum using std::inner_product for efficiency and clarity.
        // It computes: (inputs[0]*weights[0] + inputs[1]*weights[1] + ...)
        double weighted_sum = std::inner_product(inputs.begin(), inputs.end(), weights.begin(), 0.0);
        
        // Pass the weighted sum plus bias to the activation function
        return step_function(weighted_sum + bias);
    }

    /**
     * @brief Trains the perceptron on a single data sample.
     * @param inputs The vector of input values for the training sample.
     * @param target The correct output (0 or 1) for the given inputs.
     *
     * This is the core of the learning algorithm. It makes a prediction,
     * calculates the error, and then adjusts the weights and bias to reduce
     * that error in the future.
     */
    void train(const std::vector<double>& inputs, int target) {
        int prediction = predict(inputs);
        
        // Calculate the error: (correct_answer - our_prediction)
        double error = target - prediction;

        // The Perceptron Learning Rule:
        // Update each weight based on the error.
        // If the error is 0, no changes are made.
        // If the error is non-zero, the weights are "nudged" in the direction
        // that would have reduced the error.
        for (size_t i = 0; i < weights.size(); ++i) {
            weights[i] += learning_rate * error * inputs[i];
        }

        // Update the bias as well.
        bias += learning_rate * error;
    }

    // A helper function to print the current weights and bias
    void print_weights() {
        std::cout << "Weights: [ ";
        for (double w : weights) {
            std::cout << w << " ";
        }
        std::cout << "], Bias: " << bias << std::endl;
    }
};


int main() {
    // --- 1. Setup ---
    // Create a Perceptron with 2 inputs (since an AND gate takes two inputs)
    Perceptron p(2);

    // --- 2. Training Data (for a logical AND gate) ---
    // Input: [0, 0], Target: 0
    // Input: [0, 1], Target: 0
    // Input: [1, 0], Target: 0
    // Input: [1, 1], Target: 1
    std::vector<std::vector<double>> training_inputs = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    std::vector<int> targets = {0, 0, 0, 1};

    std::cout << "--- Before Training ---" << std::endl;
    p.print_weights();

    // --- 3. Training Loop ---
    // We will loop through the entire dataset multiple times (epochs)
    // to give the perceptron enough chances to learn.
    int epochs = 100;
    for (int i = 0; i < epochs; ++i) {
        // In each epoch, train on every sample in our dataset
        for (size_t j = 0; j < training_inputs.size(); ++j) {
            p.train(training_inputs[j], targets[j]);
        }
    }

    std::cout << "\n--- After " << epochs << " Epochs of Training ---" << std::endl;
    p.print_weights();

    // --- 4. Testing ---
    // Now we test our trained perceptron to see if it learned the AND gate.
    std::cout << "\n--- Testing the Trained Perceptron ---" << std::endl;
    std::cout << "Input: [0, 0] -> Prediction: " << p.predict({0, 0}) << " (Expected: 0)" << std::endl;
    std::cout << "Input: [0, 1] -> Prediction: " << p.predict({0, 1}) << " (Expected: 0)" << std::endl;
    std::cout << "Input: [1, 0] -> Prediction: " << p.predict({1, 0}) << " (Expected: 0)" << std::endl;
    std::cout << "Input: [1, 1] -> Prediction: " << p.predict({1, 1}) << " (Expected: 1)" << std::endl;
    return 0;
}
