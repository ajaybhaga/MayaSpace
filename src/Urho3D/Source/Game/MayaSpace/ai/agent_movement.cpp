//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//

#include "../shared_libs.h"

AgentMovement::AgentMovement(AgentController *agentController) {
    //this->agentController.make_shared(agentController);
    this->agentController = std::make_shared<AgentController>(*agentController);
}

AgentMovement::~AgentMovement() {

}

void AgentMovement::update(float deltaTime) {
    checkInput();
    applyInput(deltaTime);
    applyVelocity(deltaTime);
    applyFriction(deltaTime);

}

void AgentMovement::checkInput() {
}

void AgentMovement::applyInput(float deltaTime) {
    // Clamp input
    if (verticalInput > 1) {
        verticalInput = 1;
    } else if (verticalInput < -1) {
     verticalInput = -1;
    }

    if (horizontalInput > 1) {
        horizontalInput = 1;
    } else if (horizontalInput < -1) {
        horizontalInput = -1;
    }

    // Agent can only accelerate further if velocity is lower than engineForce * MAX_VEL
    bool canAccelerate = true;
    if (verticalInput < 0) {
        canAccelerate = velocity.y_ > verticalInput * MAX_VEL;
    } else if (verticalInput > 0) {
        canAccelerate = velocity.y_ < verticalInput * MAX_VEL;
    }

    // Set velocity
    if (canAccelerate) {
        velocity.y_ += (float) verticalInput * ACCELERATION * deltaTime;

        // Clamp velocity
        if (velocity.y_ > MAX_VEL) {
            velocity.y_ = MAX_VEL;
        } else if (velocity.y_ < -MAX_VEL) {
            velocity.y_ = -MAX_VEL;
        }
    }

    // Set rotation
    this->rotation = agentController->agent->getRotation();
    this->rotation *= (float)-horizontalInput * TURN_SPEED * deltaTime;
}

void AgentMovement::setInputs(double *input) {

    horizontalInput = input[0];
    verticalInput = input[1];
}

// Apply the current velocity to the position of the agent.
void AgentMovement::applyVelocity(float deltaTime) {
    Vector3 direction = Vector3(0, 1, 0);
    Quaternion currRot = this->agentController->agent->getRotation();

    this->agentController->agent->setRotation(rotation);

    Quaternion dirQ = Quaternion(0.0f, 90.0f, 0.0f);
    ;
//    dirQ.addScaledVector(direction, 1.0);
    //dirQ.rotateByVector(velocity * deltaTime);
    //dirQ *= currRot * velocity * deltaTime;

/*    velocity[0] += rdx();
    velocity.y_ += rdx();
    velocity[2] += rdx();
*/

    // Update position
    this->agentController->agent->setPosition(Vector3(this->agentController->agent->getPosition() + (velocity * deltaTime)));
}

// Apply some friction to the velocity.
void AgentMovement::applyFriction(float deltaTime) {

    if (verticalInput == 0) {
        if (velocity.y_ > 0) {
            velocity.y_ -= VEL_FRICT * deltaTime;
            if (velocity.y_ < 0) {
                velocity.y_ = 0;
            }
        }

        if (velocity.y_ < 0) {
            velocity.y_ += VEL_FRICT * deltaTime;
            if (velocity.y_ > 0) {
                velocity.y_ = 0;
            }
        }
    }

}

void AgentMovement::stop() {
    velocity = Vector3(0, 0, 0);
    rotation = Quaternion(0, Vector3(0, 0, 1));
}

double AgentMovement::getHorizontalInput() const {
    return horizontalInput;
}

void AgentMovement::setHorizontalInput(double horizontalInput) {
    AgentMovement::horizontalInput = horizontalInput;
}

double AgentMovement::getVerticalInput() const {
    return verticalInput;
}

void AgentMovement::setVerticalInput(double verticalInput) {
    AgentMovement::verticalInput = verticalInput;
}
