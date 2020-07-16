//
//

#ifndef EANN_SIMPLE_ENVIRONMENT_H
#define EANN_SIMPLE_ENVIRONMENT_H

#include "shared_libs.h"

/**
 * Environment class definition.
 */
class Environment : public Application
{
private:
    /** Resets the position of all the blocks. */
    virtual void reset();

    /** Processes the physics. */
    virtual void update();

    void showText();

public:
    /** Creates a new demo object. */
    Environment();

    /** Returns the window title for the demo. */
    virtual const char* getTitle();

    /** Display the particle positions. */
    virtual void display();
};

#endif //EANN_SIMPLE_ENVIRONMENT_H
