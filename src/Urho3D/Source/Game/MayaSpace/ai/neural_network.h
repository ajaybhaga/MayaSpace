//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on Deep Learning Cars design of Samuel Arzt (March 2017)
// https://arztsamuel.github.io/en/projects/unity/deepCars/deepCars.html
//

#pragma once

#include "neural_layer.h"

class NeuralNetwork {
public:

    NeuralNetwork(int *topology, int numLayers);
    ~NeuralNetwork();

    double *processInputs(double *inputs);
    void setRandomWeights(double minValue, double maxValue);

    // Returns a new NeuralNetwork instance with the same topology and activation functions,
    // but the weights set to their default value.
    NeuralNetwork getTopologyCopy();

    // Deep copy of this NeuralNetwork
    NeuralNetwork deepCopy();

    // Returns a string representing this network in layer order.
    std::string toString();

    // The individual neural layers of this network.
    NeuralLayer **layers;

    // An array of integers representing the node count of each layer of the network from input to output layer.
    int *topology;

    // The amount of overall weights of the connections of this network.
    int weightCount;

    int numLayers;

private:
};
