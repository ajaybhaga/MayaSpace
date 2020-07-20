//
// C++ Implementation by Ajay Bhaga
//
// Based on Deep Learning Cars design of Samuel Arzt (March 2017)
// https://arztsamuel.github.io/en/projects/unity/deepCars/deepCars.html
//

#pragma once

#include "genotype.h"
#include "../util/event.h"
#include "evolution_manager.h"

// Default min value of initial population parameters.
static const float DefInitParamMin = -1.0f;

// Default max value of initial population parameters.
static const float DefInitParamMax = 1.0f;

// Default probability of a parameter being swapped during crossover.
static const float DefCrossSwapProb = 0.6f;

// Default probability of a parameter being mutated.
static const float DefMutationProb = 0.3f;

// Default amount by which parameters may be mutated.
static const float DefMutationAmount = 2.0f;

// Default percent of genotypes in a new population that are mutated.
static const float DefMutationPerc = 1.0f;

static const int RestartAfter = 100;

// Forward declarations
class EvolutionManager;

class GeneticAlgorithm {
public:

    GeneticAlgorithm(int genotypeParamCount, int populationSize);
    //GeneticAlgorithm(int genotypeParamCount, int populationSize);
    ~GeneticAlgorithm();

    static SimpleEvent::Event terminationCriterion;
    static SimpleEvent::Event algorithmTerminated;
    static SimpleEvent::Event fitnessCalculationFinished;

    void start();
    void evaluationFinished();
    void terminate();

    // Static methods
     static void defaultPopulationInitialization(std::vector<Genotype*> population);
     static void asyncEvaluation(std::vector<Genotype*> currentPopulation);
     static void defaultFitnessCalculation(std::vector<Genotype*> currentPopulation);
     static std::vector<Genotype*> *defaultSelectionOperator(std::vector<Genotype*> currentPopulation);
     static std::vector<Genotype*> *defaultRecombinationOperator(std::vector<Genotype*> intermediatePopulation, int newPopulationSize);

     static void defaultMutationOperator(std::vector<Genotype*> newPopulation);
     static void completeCrossover(Genotype *parent1, Genotype *parent2, float swapChance, Genotype* &offspring1, Genotype* &offspring2);
     static void mutateGenotype(Genotype *genotype, float mutationProb, float mutationAmount);
     static bool defaultTermination(std::vector<Genotype*> currentPopulation);

    // Use to initialize the initial population.
    typedef std::function<void (std::vector<Genotype*> initialPopulation)> InitializationOperator;

    // Used to evaluate (or start the evaluation process of) the current population.
    typedef std::function<void (std::vector<Genotype*> currentPopulation)> EvaluationOperator;

    // Used to calculate the fitness value of each genotype of the current population.
    typedef std::function<void (std::vector<Genotype*> currentPopulation)> FitnessCalculation;

    // Used to select genotypes of the current population and create the intermediate population.
    typedef std::function<std::vector<Genotype*> *(std::vector<Genotype*> currentPopulation)> SelectionOperator;

    // Used to recombine the intermediate population to generate a new population.
    typedef std::function<std::vector<Genotype*> *(std::vector<Genotype*> intermediatePopulation, int newPopulationSize)> RecombinationOperator;

    // Used to mutate the new population.
    typedef std::function<void (std::vector<Genotype*> newPopulation)> MutationOperator;

    // Used to check whether any termination criterion has been met.
    typedef std::function<bool (std::vector<Genotype*> currentPopulation)> CheckTerminationCriterion;

    // std::function<void(int)> f1 = [](int x){ return C::f(x); };
    // Operators
    InitializationOperator initializePopulation = defaultPopulationInitialization;
    EvaluationOperator evaluation = asyncEvaluation;
    FitnessCalculation fitnessCalculationMethod = defaultFitnessCalculation;
    SelectionOperator selection = defaultSelectionOperator;
    RecombinationOperator recombination = defaultRecombinationOperator;
    MutationOperator mutation = defaultMutationOperator;
    CheckTerminationCriterion checkTermination = defaultTermination;

    // The amount of genotypes in a population.
    int populationSize;

    // The amount of generations that have already passed.
    int generationCount = 1;

    // Whether the current population shall be sorted before calling the termination criterion operator.
    bool sortPopulation;

    // Whether the genetic algorithm is currently running.
    bool running;

private:
    std::vector<Genotype*> currentPopulation;
public:
    const std::vector<Genotype*> &getCurrentPopulation() const;
};
