//
// C++ Implementation by Ajay Bhaga
//

#ifndef EANN_SIMPLE_AGENT_FSM_H
#define EANN_SIMPLE_AGENT_FSM_H

#include "fsm_event_data.h"
#include "fsm.h"

// Structure to hold event data passed into state machine
struct AgentData : public FSMEventData {
    int speed;
};

// Agent Finite State Machine class
class AgentFSM : public FiniteStateMachine {
public:
    AgentFSM() : FiniteStateMachine(ST_MAX_STATES) {};

    // External events taken by this state machine
    void halt();
    void setSpeed(AgentData*);
    void setRotation(AgentData*);
    void jump(AgentData*);
    void attack(AgentData*);

private:
    // State machine state functions
    void stIdle();
    void stStop();
    void stStart(AgentData*);
    void stChangeSpeed(AgentData*);
    void stChangeRotation(AgentData*);
    void stJump(AgentData*);
    void stAttack(AgentData*);

    // State map to define state function order
    BEGIN_STATE_MAP
        STATE_MAP_ENTRY(&AgentFSM::stIdle)
        STATE_MAP_ENTRY(&AgentFSM::stStop)
        STATE_MAP_ENTRY(&AgentFSM::stStart)
        STATE_MAP_ENTRY(&AgentFSM::stChangeSpeed)
        STATE_MAP_ENTRY(&AgentFSM::stChangeRotation)
        STATE_MAP_ENTRY(&AgentFSM::stJump)
        STATE_MAP_ENTRY(&AgentFSM::stAttack)
    END_STATE_MAP

    // State enumeration order must match the order of state method entries in the state map
    enum e_states {
        ST_IDLE = 0,
        ST_STOP,
        ST_START,
        ST_CHANGE_SPEED,
        ST_CHANGE_ROTATION,
        ST_JUMP,
        ST_ATTACK,
        ST_MAX_STATES
    };
};

#endif //EANN_SIMPLE_AGENT_FSM_H
