//
// Copyright (c) 2008-2018 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Scene/LogicComponent.h>

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;

struct ObjectState {
    bool active;
    bool onGround;
    Vector3 moveDir;
};

/// Object component controling Imp behavior.
class Object2D : public LogicComponent
{
    URHO3D_OBJECT(Object2D, LogicComponent);

public:
    /// Construct.
    explicit Object2D(Context* context);

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

    /// Handle update. Called by LogicComponent base class.
    void Update(float timeStep) override;
    /// Update object state
    ObjectState UpdateState(float timeStep);

    // Previous state
    ObjectState prevState_;
    // Current state
    ObjectState currState_;

    /// Movement controls. Assigned by the main program each frame.
    Controls controls_;

    /// Flag when player is facing forward.
    bool forward_;
    float heading_;

    int id_; 
    int type_;

    bool isReady_; 

    /// Flag when player is wounded.
    bool wounded_;
    /// Flag when player is dead.
    bool killed_;
    /// Timer for particle emitter duration.
    float timer_;

    Vector3 playerPos_;
    bool doMove_;
    bool chooseMove_;
    float lastMove_;
    float currMove_;
};
