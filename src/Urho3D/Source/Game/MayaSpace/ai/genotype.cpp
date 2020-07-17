//
// Genotype represents one member of population
//
//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//

#include "genotype.h"
#include <Urho3D/Math/MathDefs.h>

Genotype::Genotype(int paramCount) {
    evaluation = 0.0;
    fitness = 0.0;

    parameters = std::vector<float>();
    for (int i = 0; i < paramCount; i++) {
        float f;
        parameters.push_back(f);
    }
    setRandomParameters(-1.0, 1.0);
}

Genotype::Genotype(std::vector<float> parameters) {

    this->parameters = std::vector<float>();
    for (int i = 0; i < parameters.size(); i++) {
        this->parameters.emplace_back(parameters[i]);
    }

    evaluation = 0.0;
    fitness = 0.0;
}

Genotype::Genotype(int paramCount, float *offParameters) {

    parameters = std::vector<float>();
    for (int i = 0; i < paramCount; i++) {
        parameters.push_back(offParameters[i]);
    }

    evaluation = 0.0;
    fitness = 0.0;
}

Genotype::~Genotype() {

}

void Genotype::setRandomParameters(float minValue, float maxValue) {
    assert(minValue < maxValue);

    // Generate random parameter vector
    float range = maxValue - minValue;
    for (int i = 0; i < this->parameters.size(); i++) {
        parameters[i] = Urho3D::Random(0.0f,1.0f);
    }
}

std::vector<float>& Genotype::getParameterCopy() {
    return parameters;
}

void Genotype::saveToFile(const char* filePath) {

        std::string dirPath = TRAINING_DATA_DIR;
        std::string fullPath = dirPath + filePath;

        GenotypeFile *file = new GenotypeFile();

        file->record.agentName = "agent01";
        file->record.evaluation = 2.3;
        file->record.fitness = 1.5;
        file->record.parameterCount = 5;
        file->record.parameters = new float[5];
        file->record.parameters[0] = file->record.parameters[1] = file->record.parameters[2] = file->record.parameters[3] = file->record.parameters[4] = 0.1;

        std::ofstream outfile(fullPath, std::ios::binary);
        outfile.write(reinterpret_cast<char*>(&file->record), sizeof(file->record));

        std::cout << "Writing file: " <<  fullPath << std::endl;
        std::cout << "[agentName: "
                  << file->record.agentName
                  << ", evaluation: "
                  <<  file->record.evaluation
                  << ", fitness: "
                  << file->record.fitness
                  << ", parameterCount: "
                  << file->record.parameterCount
                  << ", parameters: "
                  << file->record.parameters
                  << "]" << std::endl;

        std::cout << "Genotype record has been successfully saved." << std::endl;
}

Genotype *Genotype::loadFromFile(const char* filePath) {
    std::string dirPath = TRAINING_DATA_DIR;
    std::string fullPath = dirPath + filePath;
    std::ifstream dimensionsInFile;

    GenotypeFile *file = new GenotypeFile();

    // Read struct data from file
    std::ifstream infile(fullPath, std::ios::binary);
    infile.read(reinterpret_cast<char*>(&file->record), sizeof(file->record));

    std::cout << "Loading file: " << fullPath << std::endl;
    std::cout << "[agentName: "
        << file->record.agentName
            << ", evaluation: "
            <<  file->record.evaluation
            << ", fitness: "
            << file->record.fitness
            << ", parameterCount: "
            << file->record.parameterCount
            << ", parameters: "
            << file->record.parameters
            << "]" << std::endl;

    std::cout << "Genotype has been successfully loaded." << std::endl;
    dimensionsInFile.close();

    Genotype* genotype = new Genotype(file->record.parameterCount);
    genotype->evaluation = file->record.evaluation;
    genotype->fitness = file->record.fitness;
    // TODO: read file into Vector
//    genotype->parameters = file->record.parameters;
    return genotype;
}

float Genotype::getParameter(int index) {
    return parameters[index];
}

Genotype* Genotype::generateRandom(int parameterCount, float minValue, float maxValue) {

    if (parameterCount == 0) {
        return new Genotype(parameterCount);
    }

    Genotype* randomGenotype = new Genotype(parameterCount);
    randomGenotype->setRandomParameters(minValue, maxValue);
    return randomGenotype;
}

void Genotype::outputToConsole() {
    for (int i = 0; i < this->parameters.size(); i++) {
        std::cout << "parameters[" << i << "] -> " << parameters[i] << std::endl;
    }
}

void Genotype::setParameter(int index, float value) {
    parameters[index] = value;
}

int Genotype::getParameterCount() {
    return parameters.size();
}
