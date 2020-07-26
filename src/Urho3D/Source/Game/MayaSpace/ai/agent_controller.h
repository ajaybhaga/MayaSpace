//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//

#pragma once

#include "genotype.h"
#include "neural_layer.h"
#include "genetic_algorithm.h"
#include "neural_network.h"
#include "sensor.h"
#include "agent_fsm.h"
#include "agent.h"
#include "agent_movement.h"
#include "../shared_libs.h"

// Forward declaration
class Agent;
class Sensor;

class AgentController {
public:

    AgentController(int index);
    ~AgentController();

    void awake();
    void start();
    void restart();
    void update(float duration);
    void die();
    void checkpointCaptured();

    float getCurrentCompletionReward();
    void setCurrentCompletionReward(float reward);
    const std::vector<Sensor> &getSensors() const;

    int agentIndex; // Agent index of the evolution manager agents array
    AgentMovement *movement;
    AgentFSM *fsm;

    bool useUserInput = false;
    float getTimeSinceLastCheckpoint() const;

    // Event for when all agents have died.
//    Event allAgentsDied;

private:
    // Maximum delay in seconds between the collection of two checkpoints until this agent dies.
    const float MAX_CHECKPOINT_DELAY = 7;
    std::vector<Sensor> sensors; // Vector of sensors
    float timeSinceLastCheckpoint = 0.0;
    long startTime;
    long lastTime = 0;

};
