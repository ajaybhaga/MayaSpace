//
// C++ Implementation by Ajay Bhaga
//

#include "agent_fsm.h"

void AgentFSM::halt() {

    // Given the halt event, transition to a new state based upon the current state of the state machine
    BEGIN_TRANSITION_MAP                        // * Current State *
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_Idle
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)     // ST_Stop
        TRANSITION_MAP_ENTRY(ST_STOP)           // ST_Start
        TRANSITION_MAP_ENTRY(ST_STOP)           // ST_ChangeSpeed
        TRANSITION_MAP_ENTRY(ST_STOP)           // stChangeRotation
        TRANSITION_MAP_ENTRY(ST_STOP)           // stJump
        TRANSITION_MAP_ENTRY(ST_STOP)           // stAttack
    END_TRANSITION_MAP(NULL)

}

void AgentFSM::setSpeed(AgentData *pData) {

    BEGIN_TRANSITION_MAP                        // * Current State *
        TRANSITION_MAP_ENTRY(ST_START)          // ST_Idle
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)     // ST_Stop
        TRANSITION_MAP_ENTRY(ST_CHANGE_SPEED)   // ST_Start
        TRANSITION_MAP_ENTRY(ST_CHANGE_SPEED)   // ST_ChangeSpeed
        TRANSITION_MAP_ENTRY(ST_CHANGE_SPEED)   // stChangeRotation
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // stJump
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // stAttack
    END_TRANSITION_MAP(pData)
}

void AgentFSM::setRotation(AgentData *pData) {

    BEGIN_TRANSITION_MAP                                // * Current State *
            TRANSITION_MAP_ENTRY(ST_START)              // ST_Idle
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)         // ST_Stop
            TRANSITION_MAP_ENTRY(ST_CHANGE_ROTATION)    // ST_Start
            TRANSITION_MAP_ENTRY(ST_CHANGE_ROTATION)    // ST_ChangeSpeed
            TRANSITION_MAP_ENTRY(ST_CHANGE_ROTATION)    // stChangeRotation
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)         // stJump
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)         // stAttack
    END_TRANSITION_MAP(pData)
}

void AgentFSM::jump(AgentData *pData) {

    BEGIN_TRANSITION_MAP                            // * Current State *
            TRANSITION_MAP_ENTRY(ST_START)          // ST_Idle
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)     // ST_Stop
            TRANSITION_MAP_ENTRY(ST_JUMP)           // ST_Start
            TRANSITION_MAP_ENTRY(ST_JUMP)           // ST_ChangeSpeed
            TRANSITION_MAP_ENTRY(ST_JUMP)           // stChangeRotation
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // stJump
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // stAttack
    END_TRANSITION_MAP(pData)
}

void AgentFSM::attack(AgentData *pData) {

    BEGIN_TRANSITION_MAP                            // * Current State *
            TRANSITION_MAP_ENTRY(ST_START)          // ST_Idle
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)     // ST_Stop
            TRANSITION_MAP_ENTRY(ST_ATTACK)         // ST_Start
            TRANSITION_MAP_ENTRY(ST_ATTACK)         // ST_ChangeSpeed
            TRANSITION_MAP_ENTRY(ST_ATTACK)         // stChangeRotation
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // stJump
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // stAttack
    END_TRANSITION_MAP(pData)

}

// State machine sits here when agent is idle
void AgentFSM::stIdle() {
    //
}

void AgentFSM::stStop() {

    // Perform the stop agent processing here

    // Transition to ST_IDLE via an internal event
    internalEvent(ST_IDLE, NULL);
}

void AgentFSM::stStart(AgentData *pData) {

    // Set initial agent processing
}

void AgentFSM::stChangeSpeed(AgentData *pData) {

    // Perform the change agent speed to pData->speed
}

void AgentFSM::stChangeRotation(AgentData *pData) {

}

void AgentFSM::stJump(AgentData *pData) {

}

void AgentFSM::stAttack(AgentData *pData) {

}
