#ifndef EANN_SIMPLE_FSM_H
#define EANN_SIMPLE_FSM_H

#include <stdio.h>
#include "fsm_event_data.h"
#include "../util/semaphore.h"

struct StateStruct;

// Base class for finite state machines
class FiniteStateMachine {
public:
    FiniteStateMachine(int maxStates);
    virtual ~FiniteStateMachine() {};

    Semaphore s;

protected:
    enum { EVENT_IGNORED = 0xFE, CANNOT_HAPPEN };
    unsigned char currentState;
    void externalEvent(unsigned char, FSMEventData* = NULL);
    void internalEvent(unsigned char, FSMEventData* = NULL);
    virtual const StateStruct* getStateMap() = 0;

private:
    const int maxStates;
    bool eventGenerated;
    FSMEventData* pEventData;
    void stateEngine();
};

typedef void (FiniteStateMachine::*StateFunc)(FSMEventData *);
struct StateStruct {
    StateFunc pStateFunc;
};

#define BEGIN_STATE_MAP \
public:\
const StateStruct* getStateMap() {\
    static const StateStruct StateMap[] = {

#define STATE_MAP_ENTRY(entry) \
    { reinterpret_cast<StateFunc>(entry) },

#define END_STATE_MAP \
    { static_cast<StateFunc>(NULL) }\
    }; \
    return &StateMap[0]; };

#define BEGIN_TRANSITION_MAP \
    static const unsigned char TRANSITIONS[] = {\

#define TRANSITION_MAP_ENTRY(entry) \
    entry,

#define END_TRANSITION_MAP(data) \
    0 };\
    externalEvent(TRANSITIONS[currentState], data);

#endif //EANN_SIMPLE_FSM_H
