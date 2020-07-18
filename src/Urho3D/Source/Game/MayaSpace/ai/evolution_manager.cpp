//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//
#include "evolution_manager.h"
#include "../shared_libs.h"

#include <Urho3D/Math/MathDefs.h>

bool directoryExists(const char *dname){
    DIR *di = opendir(dname); // open the directory
    if(di) return true; // can open=>return true
    else return false; // otherwise return false
    closedir(di);
}

EvolutionManager::EvolutionManager() {
    saveStatistics = true;
}

EvolutionManager::~EvolutionManager() {

    for (auto it = agents.begin(); it != agents.end(); ++it) {
        delete &it;
    }

    for (auto it = agentControllers.begin(); it != agentControllers.end(); ++it) {
        delete &it;
    }
}

EvolutionManager *EvolutionManager::getInstance() {
    return this;
}
/*

    if (!EvolutionManager::instance) {
        EvolutionManager::instance = new EvolutionManager();
        // Initialize
 //       getInstance()->statisticsFileName = "stats.txt";
    }

    return 0;//EvolutionManager::instance;
}*/

// The age of the current generation.
int EvolutionManager::getGenerationCount() {
    return getInstance()->getGeneticAlgorithm()->generationCount;
}

void EvolutionManager::evalFinished() {
    getGeneticAlgorithm()->evaluationFinished();
}

void EvolutionManager::startEvolution() {

    // Build Neural Network.

    // Create neural layer array (NUM_NEURAL_LAYERS = 4)
    getInstance()->ffnTopology = new int[NUM_NEURAL_LAYERS+1];

    // It comprises 4 layers: an input layer with 5 neurons, two hidden layers with 4 and 3 neurons respectively
    // and an output layer with 2 neurons.

    // Input layer
    getInstance()->ffnTopology[0] = 5;

    // Hidden layers
    getInstance()->ffnTopology[1] = 4;
    getInstance()->ffnTopology[2] = 3;

    // Output layer
    getInstance()->ffnTopology[3] = 2;
    getInstance()->ffnTopology[4] = 2;

    // Create neural network to determine parameter count
    NeuralNetwork *nn = new NeuralNetwork(getInstance()->ffnTopology, NUM_NEURAL_LAYERS);

    // Setup genetic algorithm
    getInstance()->geneticAlgorithm = new GeneticAlgorithm(this, nn->weightCount, getInstance()->populationSize);
    getInstance()->genotypesSaved = 0;

    getInstance()->geneticAlgorithm->evaluation = startEvaluation;

    if (getInstance()->elitistSelection) {

        // Second configuration
        getInstance()->geneticAlgorithm->selection = getInstance()->geneticAlgorithm->defaultSelectionOperator;
        getInstance()->geneticAlgorithm->recombination = randomRecombination;
        getInstance()->geneticAlgorithm->mutation = mutateAllButBestTwo;

    } else {

        // First configuration
        getInstance()->geneticAlgorithm->selection = remainderStochasticSampling;
        getInstance()->geneticAlgorithm->recombination = randomRecombination;
        getInstance()->geneticAlgorithm->mutation = mutateAllButBestTwo;
    }

    getInstance()->allAgentsDied += evalFinished;

    char buffer[80];
    time_t rawtime;
    struct tm *timeinfo;
    time (&rawtime);
    timeinfo = localtime(&rawtime);

    // Statistics
    if (getInstance()->saveStatistics) {
        strftime(buffer,sizeof(buffer),"%d-%m-%Y_%H:%M:%S",timeinfo);
        std::string str(buffer);
        getInstance()->statisticsFileName = std::string("evaluation-") + buffer;
        writeStatisticsFileStart();
        getInstance()->geneticAlgorithm->fitnessCalculationFinished += writeStatisticsToFile;
    }

    getInstance()->geneticAlgorithm->fitnessCalculationFinished += checkForTrackFinished;

    //Restart logic
    if (getInstance()->restartAfter > 0) {

        getInstance()->geneticAlgorithm->terminationCriterion += checkGenerationTermination;
        getInstance()->geneticAlgorithm->algorithmTerminated += onGATermination;

    }

    getInstance()->geneticAlgorithm->start();
}


void EvolutionManager::writeStatisticsFileStart() {
    std::string dirPath = TRAINING_DATA_DIR;
    std::string fullPath = dirPath + statisticsFileName;

    std::string outText;
    std::string trackName = "default";

    statisticsFile.open(fullPath);
    if (statisticsFile.is_open())
    {
        outText += "Evaluation of a population with size: ";
        outText += std::to_string(populationSize);
        outText += " on Track " + trackName + ".\n";
        statisticsFile << outText;
        statisticsFile.close();
    }
}

void EvolutionManager::writeStatisticsToFile() {

    std::string outText;

    std::vector<Genotype*> currentPopulation = getGeneticAlgorithm()->getCurrentPopulation();

    for (int i = 0; i < currentPopulation.size(); i++) {
        outText += "Generation Count -> ";
        outText += getGeneticAlgorithm()->generationCount;
        outText += "\n";
        outText += "Genotype Evaluation: ";
        outText += currentPopulation[i]->evaluation;
        outText += "\n";
    }
}

// Checks the current population and saves genotypes to a file if their evaluation is greater than or equal to 1.
void EvolutionManager::checkForTrackFinished() {

    if (genotypesSaved >= saveFirstNGenotype) return;

    std::string saveFolder = statisticsFileName + "/";
    std::vector<Genotype*> currentPopulation = getGeneticAlgorithm()->getCurrentPopulation();

    for (int i = 0; i < currentPopulation.size(); i++) {

        if (currentPopulation[i]->evaluation >= 1) {

            // Check if directory exist
            if (directoryExists(saveFolder.data())) {

                // Create directory
                const int dir_err = mkdir(saveFolder.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                if (-1 == dir_err)
                {
                    printf("Error creating directory!n");
                    exit(1);
                }

                std::string a = saveFolder;
                a += "Genotype - Finished as ";
                a += (++(genotypesSaved));
                a += ".txt";
                currentPopulation[i]->saveToFile(a.data());

//                std::ofstream("sandbox/file"); // create regular file

                if (genotypesSaved >= saveFirstNGenotype) return;
            }
        } else
            return; // List should be sorted, so we can exit here.
    }
}

bool EvolutionManager::checkGenerationTermination() {
    return getGeneticAlgorithm()->checkTermination(getGeneticAlgorithm()->getCurrentPopulation());
}

void EvolutionManager::onGATermination() {

    allAgentsDied -= evalFinished;
    restartAlgorithm(5.0f);
}

// Restart the algorithm after a specific wait time
void EvolutionManager::restartAlgorithm(float wait) {

    // TODO: implement wait, ignoring wait

    std::cout << "[" << currentDateTime() << "] Evolution Manager - restarting algorithm..." << std::endl << std::flush;

    startEvolution();
}

// Starts the evaluation by first creating new agents from the current population and then restarting the track manager.
 void EvolutionManager::startEvaluation(std::vector<Genotype*> currentPopulation) {

    // Create new agents from currentPopulation
    getInstance()->agents.clear();
    getInstance()->agentControllers.clear();
    getInstance()->agentsAliveCount = 0;

    // Iterate through genotypes
    //for (auto it = currentPopulation.begin(); it != currentPopulation.end(); ++it) {

    for (int i = 0; i < currentPopulation.size(); i++) {

        Agent* agent = new Agent(currentPopulation[i], MathHelper::softSignFunction, getInstance()->ffnTopology);
        AgentController *agentController = new AgentController(agent);
        getInstance()->agents.emplace_back(agent);
        getInstance()->agentControllers.emplace_back(agentController);
        agentController->agent->agentDied += onAgentDied;
        getInstance()->agentsAliveCount++;
    }

    // TrackManager.Instance.setCarAmount(agents.Count);

    // Iterate through agent controllers
//    for (auto it = getInstance()->agentControllers.begin(); it != getInstance()->agentControllers.end(); ++it) {
//    }

    // track manager -> Instance restart

}

void EvolutionManager::onAgentDied() {

    getInstance()->agentsAliveCount--;

    // Check if all agents have died and signal if so
    if (getInstance()->agentsAliveCount == 0) {
        getInstance()->allAgentsDied();
    }

}

// Mutates all members of the new population with the default probability, while leaving the first 2 genotypes in the list.
void EvolutionManager::mutateAllButBestTwo(std::vector<Genotype*> newPopulation) {
    std::cout << "Mutating all population but best two.";

    int i = 0;
    for (int i = 2; i < newPopulation.size(); i++) {

        if (Urho3D::Random(0.0f,1.0f) < DefMutationProb) {
            getInstance()->getGeneticAlgorithm()->mutateGenotype(newPopulation[i], DefMutationProb, DefMutationAmount);
        }
    }
}

void EvolutionManager::mutateAll(std::vector<Genotype*> newPopulation) {

     for (int i = 0; i < newPopulation.size(); i++) {
         if (Urho3D::Random(0.0f,1.0f) < DefMutationProb) {
             getInstance()->getGeneticAlgorithm()->mutateGenotype(newPopulation[i], DefMutationProb, DefMutationAmount);
         }
     }
}

std::vector<Genotype*> *EvolutionManager::randomRecombination(std::vector<Genotype*> intermediatePopulation, int newPopulationSize) {

    if (intermediatePopulation.size() < 2) {

        std::cout << "The intermediate population has to be at least of size 2 for this operator.";
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

        Genotype *intermediatePopulation0;
        Genotype *intermediatePopulation1;

        int count = 0;
        for (int i = 0; i < b.size(); i++) {

            switch (i) {
                case 0:
                    intermediatePopulation0 = b[i];
                    break;
                case 1:
                    intermediatePopulation1 = b[i];
                    break;

            }
        }

        // Always add best two (unmodified)
        newPopulation->emplace_back(intermediatePopulation0);
        newPopulation->emplace_back(intermediatePopulation1);

        while (newPopulation->size() < newPopulationSize) {

            // Get two random indices that are not the same.
            int randomIndex1 = (int) Urho3D::Random(0.0, (float)  std::round(intermediatePopulation.size()));
            int randomIndex2;

            do {
                randomIndex2 = (int) Urho3D::Random(0.0, (float) std::round(intermediatePopulation.size()));
            } while (randomIndex2 == randomIndex1);

            getInstance()->getGeneticAlgorithm()->completeCrossover(intermediatePopulation[randomIndex1], intermediatePopulation[randomIndex2],
                    DefCrossSwapProb, offspring1, offspring2);

            newPopulation->emplace_back(offspring1);
            if (newPopulation->size() < newPopulationSize) {
                newPopulation->emplace_back(offspring2);
            }
        }
    }

    return newPopulation;
}

std::vector<Genotype*> *EvolutionManager::remainderStochasticSampling(std::vector<Genotype*> currentPopulation) {

    std::vector<Genotype*> *intermediatePopulation = new std::vector<Genotype*>();
    // Put integer portion of genotypes into intermediatePopulation
    // Assumes that currentPopulation is already sorted

    //std::cout << "selection -> remainderStochasticSampling(): " << currentPopulation.size() << std::endl;

    for (int i = 0; i < currentPopulation.size(); i++) {

        if (currentPopulation[i]->fitness < 1) {
            break;
        } else {
            for (int i = 0; i < (int) currentPopulation[i]->fitness; i++) {
                Genotype *g = new Genotype(currentPopulation[i]->getParameterCopy());
                intermediatePopulation->emplace_back(g);
            }
        }
    }

    // Put remainder portion of genotypes into intermediatePopulation
    for (int i = 0; i < currentPopulation.size(); i++) {

            float remainder = currentPopulation[i]->fitness - (int) currentPopulation[i]->fitness;
        if (Urho3D::Random(0.0f,1.0f) < remainder) {
            Genotype *g = new Genotype(currentPopulation[i]->getParameterCopy());
            intermediatePopulation->emplace_back(g);
        }
    }

    return intermediatePopulation;
}

GeneticAlgorithm *EvolutionManager::getGeneticAlgorithm() {
    return getInstance()->geneticAlgorithm;
}

const std::vector<Agent*> &EvolutionManager::getAgents() const {
    return agents;
}

const std::vector<AgentController*> &EvolutionManager::getAgentControllers() const {
    return agentControllers;
}
