//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//

#include "neural_network.h"

NeuralNetwork::NeuralNetwork(int *topology, int numLayers) {

    this->topology = topology;
    this->numLayers = numLayers;

    // Calculate overall weight count
    weightCount = 0;
    for (int i = 0; i < numLayers; i++) {
        weightCount += (int) ((topology[i] + 1) * topology[i + 1]); // + 1 for bias node
    }

    // Initialize layers
    layers = new NeuralLayer*[numLayers];
    for (int i = 0; i < numLayers; i++) {
        layers[i] = new NeuralLayer(topology[i], topology[i + 1]);
    }
}

NeuralNetwork::~NeuralNetwork() {

}

double *NeuralNetwork::processInputs(double *inputs) {

    // Process inputs by propagating values through all layer.
    double *outputs = inputs;

    for (int i = 0; i < numLayers; i++) {
        outputs = layers[i]->processInputs(inputs);
    }

    return outputs;
}

void NeuralNetwork::setRandomWeights(double minValue, double maxValue) {

    if (layers) {
        for (int i = 0; i < numLayers; i++) {
            layers[i]->setRandomWeights(minValue, maxValue);
        }
    }
}

// Returns a new NeuralNetwork instance with the same topology and
// activation functions, but the weights set to their default value.
NeuralNetwork NeuralNetwork::getTopologyCopy() {

    NeuralNetwork *copy = new NeuralNetwork(this->topology, this->numLayers);
    for (int i = 0; i < numLayers; i++) {
        copy->layers[i]->neuronActivationFunction = this->layers[i]->neuronActivationFunction;
    }
    return *copy;
}

NeuralNetwork NeuralNetwork::deepCopy() {

    NeuralNetwork *newNet = new NeuralNetwork(this->topology, this->numLayers);
    for (int i = 0; i < numLayers; i++) {
        // Copy neural layer
        NeuralLayer neuralLayer = this->layers[i]->deepCopy();
        newNet->layers[i] = &neuralLayer;
    }
    return *newNet;
}

std::string NeuralNetwork::toString() {

    std::string output = "";

    for (int i = 0; i < this->numLayers; i++) {
            output += "Layer ";
            output += i;
            output += ":\n";
            output += this->layers[i]->toString();
    }

    return output;
}
