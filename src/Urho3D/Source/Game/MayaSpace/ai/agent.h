//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//

#pragma once

#include "../shared_libs.h"
#include "../util/event.h"
#include <Urho3D/Math/MathDefs.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Quaternion.h>

class Agent {
public:

    Agent(Genotype *genotype, NeuralLayer::ActivationFunction defaultActivation, int *topology);
    ~Agent();
    void reset();
    void kill();
    int compareTo(Agent &other);

    // Agent id
    unsigned int id;
    std::string name;

    // Underlying genotype of this agent.
    Genotype *genotype;

    // The feed-forward neural network which was constructed from the genotype of this agent.
    NeuralNetwork *ffn;

    // Flag when the agent died (stopped participating in the simulation).
    SimpleEvent::Event agentDied;

    // Whether this agent is currently alive (actively participating in the simulation).
    bool isAlive();

    const Urho3D::Vector3 &getPosition() const;
    void setPosition(const Urho3D::Vector3 &position);
    const Urho3D::Quaternion &getRotation() const;
    void setRotation(const Urho3D::Quaternion &rotation);
    const Urho3D::Vector3 &getWinPos() const;
    void setWinPos(const Urho3D::Vector3 &winPos);
    const Urho3D::Vector3 &getColour() const;
    void setColour(const Urho3D::Vector3 &colour);
    const char* getName();
    unsigned generateId();
    bool isHit() const;
    void setHit(bool hit);
    long getLastHit() const;
    void setLastHit(long lastHit);

private:
    bool alive = false;
    bool hit = false;
    long lastHit = 0;

    Urho3D::Vector3 position;
    Urho3D::Vector3 winPos;
    Urho3D::Quaternion rotation;
    Urho3D::Vector3 colour;

};