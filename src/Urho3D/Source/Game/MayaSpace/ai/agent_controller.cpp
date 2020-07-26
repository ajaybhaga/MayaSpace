//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//

#include "../shared_libs.h"
#include "agent_fsm.h"
#include "../util/math_helper.h"

class EvolutionManager;
//class MathHelper;
//Vector3 translateGLToWindowCoordinates(GLdouble x, GLdouble y, GLdouble z)

AgentController::AgentController(int index) {
    this->agentIndex = index;
    this->movement = new AgentMovement(this);
    this->fsm = new AgentFSM();

    int numSensors = 3;
    for (int i = 0; i < numSensors; i++) {
        Sensor *s = new Sensor(agentIndex);
        sensors.emplace_back(*s);
    }

    sensors[0].setOffset(Urho3D::Vector3(0.0f, 0.0f, 0.0f));
    sensors[0].setDirection(Urho3D::Vector3(Urho3D::Vector3(1.0f, 0.0f, 0.0f)));

    sensors[1].setOffset(Urho3D::Vector3(0.0f, 0.0f, 0.0f));
    sensors[1].setDirection(Urho3D::Vector3(Urho3D::Vector3(0.0f, 1.0f, 0.0f)));

    sensors[2].setOffset(Urho3D::Vector3(0.0f, 0.0f, 0.0f));
    sensors[2].setDirection(Urho3D::Vector3(Urho3D::Vector3(0.0f, 0.0f, 1.0f)));

        /*
        sensors[1].setCenter(Vector3(agent->getPosition()+Vector3(0.1f, 0.0f, 0.0f)));
        sensors[1].setTarget(Vector3(sensors[1].getCenter()+Vector3(0.0f, 0.1f, 0.0f)));

        sensors[2].setCenter(Vector3(agent->getPosition()+Vector3(0.1f, 0.0f, 0.0f)));
        sensors[2].setTarget(Vector3(sensors[2].getCenter()+Vector3(0.1f, 0.0f, 0.1f)));
    */

/*
    sensors[1].setTarget(Vector3(agent->getPosition()+Vector3(0.0f, 1.0f, 0.0f)));
    sensors[2].setTarget(Vector3(agent->getPosition()+Vector3(0.0f, 0.0f, 1.0f)));
    sensors[3].setTarget(Vector3(agent->getPosition()+Vector3(-1.0f, 0.0f, 0.0f)));
    sensors[4].setTarget(Vector3(agent->getPosition()+Vector3(0.0f, -1.0f, 0.0f)));
    sensors[5].setTarget(Vector3(agent->getPosition()+Vector3(0.0f, 0.0f, -1.0f)));
*/
 }

AgentController::~AgentController() {

    if (this->movement) {
        delete[] this->movement;
    }

    if (this->fsm) {
        delete[] this->fsm;
    }
}

void AgentController::awake() {

    // Cache components
    // movement -> CarMovement
    // spriteRender
    // sensors
}

void AgentController::start() {

    // When agent controller hits wall -> die action
//    this->movement->hitWall += die;

    auto start = std::chrono::high_resolution_clock::now();
}

void AgentController::restart() {

    timeSinceLastCheckpoint = 0;
    startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    for (int i = 0; i < this->sensors.size(); i++) {
        sensors[i].show();
    }

    EvolutionManager::getInstance()->getAgents()[agentIndex]->reset();
}

void AgentController::update(float duration) {
    timeSinceLastCheckpoint += duration;

    // Get readings from sensors
    double *sensorOutput = new double[sensors.size()];
    for (int i = 0; i < sensors.size(); i++) {
        sensors[i].update();
        sensorOutput[i] = sensors[i].output;
    }

    // Process sensor inputs through ffn
    double *controlInputs = EvolutionManager::getInstance()->getAgents()[agentIndex]->ffn->processInputs(sensorOutput);

    // Resultant data from sensor processing is used for controlling the agent movement

    //std::cout << "controlInputs[0]:" << controlInputs[0] << "," << "controlInputs[1]:" << controlInputs[1] << std::endl;
    // Apply inputs to agent movement (two dimension array)
    this->movement->setInputs(controlInputs);
    this->movement->update(duration);


    // TODO: Integrate with Urho display engine
    // Update screen coordinates of agent
//    this->agent->setWinPos(MathHelper::translateGLToWindowCoordinates(this->agent->getPosition().x, this->agent->getPosition().y, this->agent->getPosition().z));

    // Agent timed out, death by timeout
    if (timeSinceLastCheckpoint > MAX_CHECKPOINT_DELAY) {
        die();
    }
}

void AgentController::die() {

    this->movement->stop();

    for (int i = 0; i < sensors.size(); i++) {
        sensors[i].hide();
    }

    EvolutionManager::getInstance()->getAgents()[agentIndex]->kill();
}

void AgentController::checkpointCaptured() {
    timeSinceLastCheckpoint = 0;
}

float AgentController::getCurrentCompletionReward() {
    if (EvolutionManager::getInstance()->getAgents()[agentIndex])
        return EvolutionManager::getInstance()->getAgents()[agentIndex]->genotype->evaluation;
    else
        return -1.0;
}

void AgentController::setCurrentCompletionReward(float reward) {
    if (EvolutionManager::getInstance()->getAgents()[agentIndex])
        EvolutionManager::getInstance()->getAgents()[agentIndex]->genotype->evaluation = reward;
}

float AgentController::getTimeSinceLastCheckpoint() const {
    return timeSinceLastCheckpoint;
}

const std::vector<Sensor> &AgentController::getSensors() const {
    return sensors;
}
