//
// C++ Implementation by Ajay Bhaga
//

#pragma once

#include <string>
#include <stdlib.h>     /* abs */
#include "../util/math_helper.h"
#include "agent.h"
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Quaternion.h>

// Class representing a sensor reading the distance to the nearest obstacle in a specified direction.
class Sensor {
public:
    Sensor(int index);
    ~Sensor();

    void start();
    void update();
    void hide();
    void show();

    const Urho3D::Vector3 &getDirection() const;
    const Urho3D::Vector3 &getOffset() const;
    void setOffset(const Urho3D::Vector3 &offset);
    void setDirection(const Urho3D::Vector3 &direction);
    const Urho3D::Vector3 &getTarget() const;
    const Urho3D::Vector3 &getCenter() const;
    void setCenter(const Urho3D::Vector3 &center);
/*    CollisionSphere getCollisionSphere(const Vector3 &position,
                                                        const Vector3 &extents);
*/
    bool isHit() const;
    void setHit(bool hit);
    long getLastHit() const;
    void setLastHit(long lastHit);

    // The current sensor readings in percent of maximum distance.
    float output;

private:
    bool visibility;

    const long AGENT_HIT_TIMEOUT = 3000;
    const float MAX_DIST = 10.0f;
    const float MIN_DIST = 0.01f;

    // Agent index
    int agentIndex;

    // Offset from center of agent
    Urho3D::Vector3 offset;
    // Sensor center
    Urho3D::Vector3 center;
    // Sensor target
    Urho3D::Vector3 target;

    // Sensor direction
    Urho3D::Vector3 direction;

    Urho3D::Vector3 halfSize;

    bool hit = false;
    long lastHit = 0;

};
