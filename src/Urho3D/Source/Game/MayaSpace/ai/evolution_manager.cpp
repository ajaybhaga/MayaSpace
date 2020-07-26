//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//
#include "evolution_manager.h"
#include "../shared_libs.h"

#include <Urho3D/Math/MathDefs.h>

// Forward declarations
int EvolutionManager::agentsAliveCount;
bool EvolutionManager::saveStatistics;
std::string EvolutionManager::statisticsFileName;
std::ofstream EvolutionManager::statisticsFile;

int EvolutionManager::saveFirstNGenotype;
int EvolutionManager::genotypesSaved;
int EvolutionManager::populationSize;
int EvolutionManager::restartAfter;
bool EvolutionManager::elitistSelection;
EvolutionManager *EvolutionManager::instance = NULL;

// Topology of the agent's FNN
int *EvolutionManager::ffnTopology;
// The current population agents.
std::vector<Agent *> EvolutionManager::agents;
// The current population agents.
std::vector<AgentController *> EvolutionManager::agentControllers;
GeneticAlgorithm *EvolutionManager::geneticAlgorithm;

// Event for when all agents have died.
SimpleEvent::Event EvolutionManager::allAgentsDied;


bool directoryExists(const char *dname) {
    DIR *di = opendir(dname); // open the directory
    if (di) return true; // can open=>return true
    else return false; // otherwise return false
    closedir(di);
}

EvolutionManager::EvolutionManager() {
    saveStatistics = true;

    agentsAliveCount = 0;

    // How many of the first to finish the course should be saved to file
    saveFirstNGenotype = 0;
    genotypesSaved = 0;

    // Population size
    populationSize = 10;

    // After how many generations should the genetic algorithm be restarted (0 for never)
    restartAfter = 100;

    // Whether to use elitist selection or remainder stochastic sampling
    elitistSelection = false;

}

void EvolutionManager::instantiate() {
    instance = new EvolutionManager();
}

void EvolutionManager::clean() {
    if (instance)
        delete instance;
}

EvolutionManager::~EvolutionManager() {
    agents.clear();
    agentControllers.clear();

//    if (ffnTopology)
//        delete ffnTopology;


    //   if (geneticAlgorithm)
    //       delete geneticAlgorithm;
}

/*

    if (!EvolutionManager::instance) {
        EvolutionManager::instance = new EvolutionManager();
        // Initialize
 //       statisticsFileName = "stats.txt";
    }

    return 0;//EvolutionManager::instance;
}*/

// The age of the current generation.
int EvolutionManager::getGenerationCount() {
    return getGeneticAlgorithm()->generationCount;
}

void EvolutionManager::evalFinished() {
    getGeneticAlgorithm()->evaluationFinished();
}

void EvolutionManager::startEvolution() {

    // Build Neural Network.

    // Create neural layer array (NUM_NEURAL_LAYERS = 4)
    ffnTopology = new int[NUM_NEURAL_LAYERS + 1];

    // It comprises 4 layers: an input layer with 9 neurons, two hidden layers with 10 and 8 neurons respectively
    // and an output layer with 4 neurons.

    /*
     * Input Map:
     *
     *  1: x pos
     *  2: y pos
     *  3: z pos
     *  4: x vel
     *  5: y vel
     *  6: z vel
     *  7: est distance
     *  8: est risk (+1 helping, -1 attacking)
     *  9: health (+1 helping, -1 attacking)
     */

    // Input layer
    ffnTopology[0] = 9;

    // Hidden layers
    ffnTopology[1] = 10;
    ffnTopology[2] = 8;

    /*
     * Output Map:
     *
     *  1: x acc (horizontal)
     *  2: y acc (vertical)
     *  3: action state
     */

    // Output layer
    ffnTopology[3] = 3;
    ffnTopology[4] = 3;

    // Create neural network to determine parameter count (only used for config setting sized same as agent ffn)
    NeuralNetwork *nn = new NeuralNetwork(ffnTopology, NUM_NEURAL_LAYERS);

    // Setup genetic algorithm
    geneticAlgorithm = new GeneticAlgorithm(nn->weightCount, populationSize);
    genotypesSaved = 0;

    if (nn)
        delete nn;

    // Assign evaluation function to GA
    geneticAlgorithm->evaluation = startEvaluation;

    if (elitistSelection) {

        // Second configuration
        geneticAlgorithm->selection = geneticAlgorithm->defaultSelectionOperator;
        geneticAlgorithm->recombination = randomRecombination;
        geneticAlgorithm->mutation = mutateAllButBestTwo;

    } else {

        // First configuration
        geneticAlgorithm->selection = remainderStochasticSampling;
        geneticAlgorithm->recombination = randomRecombination;
        geneticAlgorithm->mutation = mutateAllButBestTwo;
    }

    allAgentsDied += evalFinished;

    char buffer[80];
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Statistics
    if (saveStatistics) {
        strftime(buffer, sizeof(buffer), "%d-%m-%Y_%H:%M:%S", timeinfo);
        std::string str(buffer);
        statisticsFileName = std::string("evaluation-") + buffer;
        writeStatisticsFileStart();
        geneticAlgorithm->fitnessCalculationFinished += writeStatisticsToFile;
        std::cout << "[" << currentDateTime()
                  << "] Evolution Manager - saveStatistics enabled -> adding handler to fitnessCalculationFinished."
                  << std::endl << std::flush;
    }

    geneticAlgorithm->fitnessCalculationFinished += checkForTrackFinished;

    //Restart logic
    if (restartAfter > 0) {

        geneticAlgorithm->terminationCriterion += checkGenerationTermination;
        geneticAlgorithm->algorithmTerminated += onGATermination;

    }

    geneticAlgorithm->start();
}


void EvolutionManager::writeStatisticsFileStart() {

    std::cout << "[" << currentDateTime() << "] Evolution Manager - writeStatisticsFileStart()..." << std::endl
              << std::flush;

    std::string dirPath = TRAINING_DATA_DIR;
    std::string fullPath = dirPath + statisticsFileName;

    std::string outText;
    std::string trackName = "default";

    statisticsFile.open(fullPath);
    if (statisticsFile.is_open()) {
        outText += "Evaluation of a population with size: ";
        outText += std::to_string(populationSize);
        outText += " on Track " + trackName + ".\n";
        statisticsFile << outText;
        statisticsFile.close();
    }
}

void EvolutionManager::writeStatisticsToFile() {

    std::cout << "[" << currentDateTime() << "] Evolution Manager - writeStatisticsToFile()..." << std::endl
              << std::flush;

    std::string outText;

    std::vector<Genotype *> currentPopulation = getGeneticAlgorithm()->getCurrentPopulation();

    for (int i = 0; i < currentPopulation.size(); i++) {
        /*
        outText += "Generation Count -> ";
        outText += getGeneticAlgorithm()->generationCount;
        outText += "\n";
        outText += "Genotype Evaluation: ";
        outText += currentPopulation[i]->evaluation;
        outText += "\n";
        */
        std::cout << "[" << currentDateTime()
                  << "] Evolution Manager - Generation Count -> " + getGeneticAlgorithm()->generationCount << "\n"
                  << "Genotype Evaluation: " << currentPopulation[i]->evaluation << std::endl << std::flush;
    }
}

// Checks the current population and saves genotypes to a file if their evaluation is greater than or equal to 1.
void EvolutionManager::checkForTrackFinished() {

    std::cout << "[" << currentDateTime() << "] Evolution Manager - checkForTrackFinished()..." << std::endl
              << std::flush;

    if (genotypesSaved >= saveFirstNGenotype) return;

    std::string saveFolder = statisticsFileName + "/";
    std::vector<Genotype *> currentPopulation = getGeneticAlgorithm()->getCurrentPopulation();

    for (int i = 0; i < currentPopulation.size(); i++) {

        if (currentPopulation[i]->evaluation >= 1) {

            // Check if directory exist
            if (directoryExists(saveFolder.data())) {

                // Create directory
                const int dir_err = mkdir(saveFolder.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                if (-1 == dir_err) {
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

    // Instantiates Genetic Algorithm and passes Evolution Manager
    startEvolution();
}

// Starts the evaluation by first creating new agents from the current population and then restarting the track manager.
void EvolutionManager::startEvaluation(std::vector<Genotype *> currentPopulation) {

    // Create new agents from currentPopulation
    agents.clear();
    agentControllers.clear();
    agentsAliveCount = 0;

    // Iterate through genotypes
    //for (auto it = currentPopulation.begin(); it != currentPopulation.end(); ++it) {

    for (int i = 0; i < currentPopulation.size(); i++) {

        Agent *agent = new Agent(currentPopulation[i], MathHelper::softSignFunction, ffnTopology);
        AgentController *agentController = new AgentController(i);
        agents.emplace_back(agent);
        agentControllers.emplace_back(agentController);
        agent->agentDied += onAgentDied;
        agentsAliveCount++;
    }

    // TrackManager.Instance.setCarAmount(agents.Count);

    // Iterate through agent controllers
//    for (auto it = agentControllers.begin(); it != agentControllers.end(); ++it) {
//    }

    // track manager -> Instance restart

}

void EvolutionManager::onAgentDied() {

    agentsAliveCount--;

    // Check if all agents have died and signal if so
    if (agentsAliveCount == 0) {
        allAgentsDied();
    }

}

// Mutates all members of the new population with the default probability, while leaving the first 2 genotypes in the list.
void EvolutionManager::mutateAllButBestTwo(std::vector<Genotype *> newPopulation) {
    std::cout << "Mutating all population but best two.";

    int i = 0;
    for (int i = 2; i < newPopulation.size(); i++) {

        if (Urho3D::Random(0.0f, 1.0f) < DefMutationProb) {
            getGeneticAlgorithm()->mutateGenotype(newPopulation[i], DefMutationProb, DefMutationAmount);
        }
    }
}

void EvolutionManager::mutateAll(std::vector<Genotype *> newPopulation) {

    for (int i = 0; i < newPopulation.size(); i++) {
        if (Urho3D::Random(0.0f, 1.0f) < DefMutationProb) {
            getGeneticAlgorithm()->mutateGenotype(newPopulation[i], DefMutationProb, DefMutationAmount);
        }
    }
}

std::vector<Genotype *> *
EvolutionManager::randomRecombination(std::vector<Genotype *> intermediatePopulation, int newPopulationSize) {

    if (intermediatePopulation.size() < 2) {

        std::cout << "The intermediate population has to be at least of size 2 for this operator.";
        return nullptr;
    }

    std::vector<Genotype *> *newPopulation = new std::vector<Genotype *>();

    if (newPopulation->size() < newPopulationSize) {

        Genotype *offspring1;
        Genotype *offspring2;

        // Get first 2 list items (top 2)
        size_t n = 2;
        auto end = std::next(intermediatePopulation.begin(), std::min(n, intermediatePopulation.size()));
        std::vector<Genotype *> b(intermediatePopulation.begin(), end);

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
            int randomIndex1 = (int) Urho3D::Random(0.0, (float) std::round(intermediatePopulation.size()));
            int randomIndex2;

            do {
                randomIndex2 = (int) Urho3D::Random(0.0, (float) std::round(intermediatePopulation.size()));
            } while (randomIndex2 == randomIndex1);

            getGeneticAlgorithm()->completeCrossover(intermediatePopulation[randomIndex1],
                                                     intermediatePopulation[randomIndex2],
                                                     DefCrossSwapProb, offspring1, offspring2);

            newPopulation->emplace_back(offspring1);
            if (newPopulation->size() < newPopulationSize) {
                newPopulation->emplace_back(offspring2);
            }
        }
    }

    return newPopulation;
}

std::vector<Genotype *> *EvolutionManager::remainderStochasticSampling(std::vector<Genotype *> currentPopulation) {

    std::vector<Genotype *> *intermediatePopulation = new std::vector<Genotype *>();
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
        if (Urho3D::Random(0.0f, 1.0f) < remainder) {
            Genotype *g = new Genotype(currentPopulation[i]->getParameterCopy());
            intermediatePopulation->emplace_back(g);
        }
    }

    return intermediatePopulation;
}

GeneticAlgorithm *EvolutionManager::getGeneticAlgorithm() {
    return geneticAlgorithm;
}

const std::vector<Agent *> &EvolutionManager::getAgents() const {
    return agents;
}

const std::vector<AgentController *> &EvolutionManager::getAgentControllers() const {
    return agentControllers;
}

EvolutionManager *EvolutionManager::getInstance() {
    if (!instance) {
        instantiate();
    }
    return instance;
}