//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on Deep Learning Cars design of Samuel Arzt (March 2017)
// https://arztsamuel.github.io/en/projects/unity/deepCars/deepCars.html
//

#include "neural_layer.h"
#include <Urho3D/Math/MathDefs.h>

NeuralLayer::NeuralLayer() {
}

NeuralLayer::NeuralLayer(int nodeCount, int outputCount) {
    initLayers(nodeCount, outputCount);
}

void NeuralLayer::initLayers(int nodeCount, int outputCount) {
    this->neuronCount = nodeCount;
    this->outputCount = outputCount;

    // Allocate dynamic 2D array for weights
    this->weights = new double*[nodeCount + 1]; // + 1 for bias node
    for (int i = 0; i < nodeCount + 1; i++) {
        this->weights[i] = new double[outputCount];
    }
}


NeuralLayer::~NeuralLayer() {

    for (int i = 0; i < this->neuronCount + 1; i++) {
        delete [] this->weights[i];
    }
    delete [] this->weights;
}


/*void setWeights(double[] weights);
double[] processInputs(double[] inputs);
NeuralLayer deepCopy();
void setRandomWeights(double minValue, double maxValue);
string toString();
*/


// The values to set the weights of the connections from this layer to the next layer.

// The values are ordered in neuron order, i.e. in a layer with two neurons with a next layer of three neurons
// the values [0-2] are the weights from neuron 0 of this layer to neurons 0-2 of the next layer respectively and
// the values [3-5] are the weights from neuron 1 of this layer to neurons 0-2 of the next layer respectively.
void NeuralLayer::setWeights(double *weights) {


    // TODO: assert weights that do not match layer weight count

    // Copy weights from given value array
   int k = 0;
   for (int i = 0; i < this->neuronCount; i++) {
       for (int j = 0; j < this->outputCount; j++) {
           this->weights[i][j] = weights[k++];
       }
   }

}

double *NeuralLayer::processInputs(double *inputs) {

    // TODO: Optionally, assert inputs where given xValues do not match layer input count - bad input will crash.

    // Calculate sum for each neuron from weighted inputs and bias
    double *sums = new double[this->outputCount];
    // Add bias (always on) neuron to inputs
    double *biasedInputs = new double[this->neuronCount + 1];
    for (int i = 0; i < this->neuronCount; i++) {
        biasedInputs[i] = 0.0;
    }
    biasedInputs[this->neuronCount] = 1.0;

    for (int j = 0; j < this->outputCount; j++) {
        for (int i = 0; i < this->neuronCount; i++) {
            sums[j] += biasedInputs[i] * this->weights[i][j];
        }
    }

    // Apply activation function to sum, if set
    if (this->neuronActivationFunction) {
        for (int i = 0; i < this->outputCount; i++) {
            sums[i] = this->neuronActivationFunction(sums[i]);
        }
    }

    return sums;
}

// Copies this NeuralLayer including its weights.
NeuralLayer NeuralLayer::deepCopy() {

    // Copy weights
    double **copiedWeights;

    // Allocate dynamic 2D array for weights
    copiedWeights = new double*[this->neuronCount + 1]; // + 1 for bias node
    for (int i = 0; i < this->neuronCount + 1; i++) {
        copiedWeights[i] = new double[outputCount];
    }

    for (int i = 0; i < this->neuronCount; i++) {
        for (int j = 0; j < this->outputCount; j++) {
            copiedWeights[i][j] = weights[i][j];
        }
    }

    // Create copy
    NeuralLayer *newLayer = new NeuralLayer(this->neuronCount, this->outputCount);
    newLayer->weights = copiedWeights;
    newLayer->neuronActivationFunction = this->neuronActivationFunction;

    return *newLayer;
}

void NeuralLayer::setRandomWeights(double minValue, double maxValue) {

    double range = std::abs(minValue - maxValue);

    for (int i = 0; i < this->neuronCount; i++) {
        for (int j = 0; j < this->outputCount; j++) {
            weights[i][j] = minValue + (Urho3D::Random(0.0f,1.0f) * range);
        }
    }
}

std::string NeuralLayer::toString() {

    std::string output = "";

    for (int i = 0; i < this->neuronCount; i++) {
        for (int j = 0; j < this->outputCount; j++) {
            output += "[";
            output += i;
            output += ", ";
            output += j;
            output += "]: ";
            output += weights[i][j];
        }
        output += "\n";
    }

    return output;
}
