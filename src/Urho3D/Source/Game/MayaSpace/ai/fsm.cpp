#include <assert.h>
#include "fsm.h"

FiniteStateMachine::FiniteStateMachine(int maxStates) :
    maxStates(maxStates),
    currentState(0),
    eventGenerated(0),
    pEventData(NULL)
{
}

// Generates an external event, called once per external event to start the machine executing.
void FiniteStateMachine::externalEvent(unsigned char newState, FSMEventData *pData) {

    // If we are supposed to ignore this event.
    if (newState == EVENT_IGNORED) {
        // Delete the event data, if any
        if (pData) {
            delete pData;
        }
    } else {
        // Generate the event and execute the state engine
        internalEvent(newState, pData);
        stateEngine();
    }
}

// Generates an internal event, called from within a state function to transition to a new state.
void FiniteStateMachine::internalEvent(unsigned char newState, FSMEventData *pData) {

    this->pEventData = pData;
    this->eventGenerated = true;
    this->currentState = newState;
}

// The state engine executes the state machine states
void FiniteStateMachine::stateEngine() {

    FSMEventData *pDataTemp = NULL;

    // Lock to prevent other threads from accessing this FSM object
    this->s.wait();

    // While events are being generated, keep executing states
    while (eventGenerated) {
        // Copy of event data pointer
        pDataTemp = pEventData;
        // Event data used up, reset pointer
        pEventData = NULL;
        // Reset flag
        eventGenerated = false;

        assert(currentState < maxStates);

        // Execute the state passing in event data, if any
        const StateStruct *pStateMap = getStateMap();
        (this->*pStateMap[currentState].pStateFunc)(pDataTemp);

        // If event data was used, then delete data
        if (pDataTemp) {
            delete pDataTemp;
            pDataTemp = NULL;
        }
    }

    // Unlock semaphore
    this->s.notify();
}