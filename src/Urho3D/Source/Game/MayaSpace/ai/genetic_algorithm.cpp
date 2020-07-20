//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on Deep Learning Cars design of Samuel Arzt (March 2017)
// https://arztsamuel.github.io/en/projects/unity/deepCars/deepCars.html
//

#include <algorithm>
#include "../shared_libs.h"
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Quaternion.h>

SimpleEvent::Event GeneticAlgorithm::terminationCriterion;
SimpleEvent::Event GeneticAlgorithm::algorithmTerminated;
SimpleEvent::Event GeneticAlgorithm::fitnessCalculationFinished;

GeneticAlgorithm::GeneticAlgorithm(int genotypeParamCount, int populationSize) {

    this->populationSize = populationSize;
    for (int i = 0; i < populationSize; i++) {
        Genotype* genotype = new Genotype(genotypeParamCount);
        currentPopulation.push_back(genotype);
    }

    generationCount = 1;
    sortPopulation = true;
    running = false;
}

GeneticAlgorithm::~GeneticAlgorithm() {

    /* TODO: Figure out how to do clean up without getting seg fault.
    for (int i = 0; i < currentPopulation.size(); i++) {
        Genotype* genotype = currentPopulation.at(i);
        if (genotype)
            delete genotype;
    }*/
}

void GeneticAlgorithm::start() {
    // Init
    generationCount = 1;
    running = true;
    initializePopulation(currentPopulation);
    evaluation(currentPopulation);
}

// Sort by genotype
bool sortByGenotype(const Genotype* lhs, const Genotype* rhs) { if ((!lhs) && (rhs)) { return rhs; } if ((!rhs) && (lhs)) { return lhs; } return lhs->fitness > rhs->fitness; }

void GeneticAlgorithm::evaluationFinished() {
    // Iterate through agent controllers and apply update
    std::vector<AgentController*> controllers = EvolutionManager().getInstance()->getAgentControllers();

    // Calculate fitness from evaluation
    fitnessCalculationMethod(currentPopulation);

    // Sort population if flag was set
    if (sortPopulation) {
        // Sort by genotype -> highest fitness is first element
        std::sort(currentPopulation.begin(), currentPopulation.end(), sortByGenotype);
    }

    // Fire fitness calculation finished event
    if (&fitnessCalculationFinished)
        fitnessCalculationFinished();

    // Check termination criterion
    if (checkTermination(currentPopulation)) {
        terminate();
        return;
    }

    // Apply selection
    std::vector<Genotype*> *intermediatePopulation = selection(currentPopulation);

    // Apply recombination
    std::vector<Genotype*> *newPopulation = recombination(*intermediatePopulation, populationSize);

    // Apply mutation
    mutation(*newPopulation);

    // Set current population to newly generated one and start evaluation again
    currentPopulation = *newPopulation;
    generationCount++;

    // Calls startEvaluation()
    evaluation(currentPopulation);
}

void GeneticAlgorithm::terminate() {

    running = false;
    algorithmTerminated();

}

void GeneticAlgorithm::defaultPopulationInitialization(std::vector<Genotype*> population) {

    int popCount = 0;
    // Set parameters to random values in set range
    for (int i = 0; i < population.size(); i++) {
        /* std::cout << *it; ... */
        //
        population[i]->setRandomParameters(DefInitParamMin, DefInitParamMax);
        //std::cout << "Generating genotype [" << (popCount + 1) << "]." << std::endl;
//        it->outputToConsole();
        popCount++;
    }
}

void GeneticAlgorithm::asyncEvaluation(std::vector<Genotype*> currentPopulation) {
    // At this point the async evaluation should be started and after it is finished EvaluationFinished should be called
    std::cout << "Reached async evaluation." << std::endl;
}

void GeneticAlgorithm::defaultFitnessCalculation(std::vector<Genotype*> currentPopulation) {

    // First calculate average evaluation of whole population
    int populationSize = 0;
    float overallEvaluation = 0;

    for (int i = 0; i < currentPopulation.size(); i++) {
        overallEvaluation += currentPopulation[i]->evaluation;
        populationSize++;
    }

    float averageEvaluation = overallEvaluation / populationSize;

    // Now assign fitness with formula fitness = evaluation / averageEvaluation
    for (int i = 0; i < currentPopulation.size(); i++) {
            currentPopulation[i]->fitness = currentPopulation[i]->evaluation / averageEvaluation;
        }
}

std::vector<Genotype*> *GeneticAlgorithm::defaultSelectionOperator(std::vector<Genotype*> currentPopulation) {

    std::vector<Genotype*>* intermediatePopulation = new std::vector<Genotype*>();

    // Get first 3 list items (top 3)
    size_t n = 3;
    auto end = std::next(currentPopulation.begin(), std::min(n, currentPopulation.size()));
    std::vector<Genotype*> b(currentPopulation.begin(), end);

    std::cout << "defaultSelectionOperator: " << currentPopulation.size() << std::endl;

    // Selects best three genotypes of the current population and copies them to the intermediate population.
    for (std::vector<Genotype*>::iterator it = b.begin(); it != b.end(); ++it) {
        intermediatePopulation->push_back(*it);
    }

    return intermediatePopulation;
}

// Simply crosses the first with the second genotype of the intermediate population until the new population is of desired size.
std::vector<Genotype*> *GeneticAlgorithm::defaultRecombinationOperator(std::vector<Genotype*> intermediatePopulation, int newPopulationSize) {

    if (intermediatePopulation.size() < 2) {
        std::cout << "Intermediate population size must be greater than 2 for this operator.";
        return nullptr;
    }

    std::vector<Genotype*> *newPopulation = new std::vector<Genotype*>();

    if (newPopulation->size() < newPopulationSize) {

        Genotype *offspring1;
        Genotype *offspring2;

        // Get first 2 list items (top 2)
        size_t n = 2;
        auto end = std::next(intermediatePopulation.begin(), std::min(n, intermediatePopulation.size()));
        std::vector<Genotype*> b(intermediatePopulation.begin(), end);

        completeCrossover(b[0], b[1], DefCrossSwapProb, offspring1, offspring2);

        newPopulation->push_back(offspring1);
        if (newPopulation->size() < newPopulationSize) {
            newPopulation->push_back(offspring2);
        }
    }

    return newPopulation;
}

void GeneticAlgorithm::defaultMutationOperator(std::vector<Genotype*> newPopulation) {

    for (std::vector<Genotype*>::iterator it = newPopulation.begin(); it != newPopulation.end(); ++it) {


        for (int i = 0; i < newPopulation.size(); i++) {
            if (Urho3D::Random(0.0f,1.0f) < DefMutationPerc) {
                mutateGenotype(newPopulation[i], DefMutationProb, DefMutationAmount);
            }
        }

    }
}

void GeneticAlgorithm::completeCrossover(Genotype *parent1, Genotype *parent2, float swapChance, Genotype* &offspring1,
                                         Genotype* &offspring2) {

    // Initialize new parameter vectors
    int parameterCount = parent1->getParameterCopy().size();
    float *off1Parameters = new float[parameterCount];
    float *off2Parameters = new float[parameterCount];

    // Iterate over all parameters randomly swapping
    for (int i = 0; i < parameterCount; i++) {

        if (Urho3D::Random(0.0f,1.0f) < swapChance) {
            // Swap parameters
            off1Parameters[i] = parent2->getParameter(i);
            off2Parameters[i] = parent1->getParameter(i);
        } else {
            // Dont swap parameters
            off1Parameters[i] = parent1->getParameter(i);
            off2Parameters[i] = parent2->getParameter(i);
        }
    }

    offspring1 = new Genotype(parameterCount, off1Parameters);
    offspring2 = new Genotype(parameterCount, off2Parameters);

}

void GeneticAlgorithm::mutateGenotype(Genotype *genotype, float mutationProb, float mutationAmount) {

    for (int i = 0; i < genotype->getParameterCount(); i++) {

        if (Urho3D::Random(0.0f,1.0f) < mutationProb) {
            // Mutate by random amount in range [-mutationAmount, mutationAmount]
            genotype->setParameter(i, genotype->getParameter(i) + (float)Urho3D::Random(0.0f,1.0f) * (mutationAmount * 2) - mutationAmount);
        }
    }
}

bool GeneticAlgorithm::defaultTermination(std::vector<Genotype*> currentPopulation) {

    //std::cout << "Generation count: " << EvolutionManager::getInstance()->getGenerationCount() << std::endl;

    return (EvolutionManager().getInstance()->getGenerationCount() >= RestartAfter);
}

const std::vector<Genotype*> &GeneticAlgorithm::getCurrentPopulation() const {
    return currentPopulation;
}
