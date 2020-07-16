//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//

#include "../shared_libs.h"

// Initializes a new agent from given genotype, constructing a new feed-forward neural network from
// the parameters of the genotype.
Agent::Agent(Genotype *genotype, NeuralLayer::ActivationFunction defaultActivation, int *topology) {

    id = this->generateId();
    // Random position spread magnitude
    float mag = 3.0f;
    this->position = Vector3(Random(0.0f,1.0f)*mag, Random(0.0f,1.0f)*mag, Random(0.0f,1.0f)*mag);
    this->rotation = Quaternion();
    this->colour = Vector3(Random(0.0f,1.0f), Random(0.0f,1.0f), Random(0.0f,1.0f));

    alive = false;
    this->genotype = genotype;
    ffn = new NeuralNetwork(topology, NUM_NEURAL_LAYERS);

    for (int i = 0; i < NUM_NEURAL_LAYERS; i++) {
        ffn->layers[i]->neuronActivationFunction = defaultActivation;
    }

    // Check if topology is valid
    if (ffn->weightCount != genotype->getParameterCount()) {
        std::cout << "Error: the given genotype's parameter count must match the neural network topology's weight count." << std::endl;
    } else {
        //std::cout << "Success: the given genotype's parameter count matches the neural network topology's weight count." << std::endl;
    }

    // Construct FFN from genotype
    for (int k = 0; k < NUM_NEURAL_LAYERS; k++) {

        for (int i = 0; i < ffn->layers[k]->neuronCount; i++) {
            for (int j = 0; j < ffn->layers[k]->outputCount; j++) {

                // Retrieve parameters for genotype
                std::vector<float> parameters = genotype->getParameterCopy();

                // Set weights to parameter values
                for (int p = 0; p < genotype->getParameterCount(); p++) {
                    ffn->layers[k]->weights[i][j] = parameters[p];
                }
            }
        }
    }
}

Agent::~Agent() {

}

// Reset this agent to be alive again.
void Agent::reset() {
    this->genotype->evaluation = 0;
    genotype->fitness = 0;
    alive = true;
}

void Agent::kill() {
    agentDied();
    alive = false;
}

int Agent::compareTo(Agent &other) {

    bool match = true;

    // Construct FFN from genotype
    for (int k = 0; k < NUM_NEURAL_LAYERS; k++) {

        for (int i = 0; i < ffn->layers[k]->neuronCount; i++) {
            for (int j = 0; j < ffn->layers[k]->outputCount; j++) {
                // Retrieve parameters for genotype
                std::vector<float> parametersA = this->genotype->getParameterCopy();
                std::vector<float> parametersB = other.genotype->getParameterCopy();

                // Compare genotypes
                for (int p = 0; p < this->genotype->getParameterCount(); p++) {

                    if (parametersA[p] != parametersB[p]) {
                        match = false;
                        std::cout << "Agent.compareTo -> Match failed on parameter #: " << p << std::endl;
                    }
                }
            }
        }
    }

    if (match) {
        return 0;
    } else {
        return -1;
    }
}

bool Agent::isAlive() {
    return alive;
}

const Vector3 &Agent::getPosition() const {
    return position;
}

void Agent::setPosition(const Vector3 &position) {
    Agent::position = position;
}

const Quaternion &Agent::getRotation() const {
    return rotation;
}

void Agent::setRotation(const Quaternion &rotation) {
    Agent::rotation = rotation;
}

const Vector3 &Agent::getColour() const {
    return colour;
}

void Agent::setColour(const Vector3 &colour) {
    Agent::colour = colour;
}

const Vector3 &Agent::getWinPos() const {
    return winPos;
}

void Agent::setWinPos(const Vector3 &winPos) {
    Agent::winPos = winPos;
}

const char* Agent::getName() {
    this->name = std::string("Agent ") + std::to_string(id);
    return name.data();
}

unsigned Agent::generateId() {
    static unsigned int idGenerator = 0;
    return ++idGenerator;
}

bool Agent::isHit() const {
    return hit;
}

void Agent::setHit(bool hit) {
    Agent::hit = hit;
}

long Agent::getLastHit() const {
    return lastHit;
}

void Agent::setLastHit(long lastHit) {
    Agent::lastHit = lastHit;
}
