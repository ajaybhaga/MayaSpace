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
#include <Urho3D/Graphics/BillboardSet.h>

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;

const float MOVE_SPEED = 30.0f;
const float YAW_SENSITIVITY = 0.1f;
const int LIFES = 3;

struct PlayerState {
    bool onGround;
    bool jump;
    float lastJump;
    bool walk;
    float lastWalk;
    bool kick;
    float lastKick;
    Vector3 moveDir;
};

/// Character2D component controlling Imp behavior.
class Character2D : public LogicComponent
{
    URHO3D_OBJECT(Character2D, LogicComponent);

public:
    /// Construct.
    explicit Character2D(Context* context);

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

    /// Handle update. Called by LogicComponent base class.
    void Update(float timeStep) override;
    /// Handle player state/behavior when wounded.
    void HandleWoundedState(float timeStep);
    /// Handle death of the player.
    void HandleDeath();
    /// Handle controllers
    PlayerState HandleP1Controller(float timeStep);
    PlayerState HandleAIController(float timeStep);

    // Previous state
    PlayerState prevState_;
    // Current state
    PlayerState currState_;

    /// Movement controls. Assigned by the main program each frame.
    Controls controls_;

    /// Flag when player is facing forward.
    bool forward_;
    float heading_;

    int id_; 
    int type_;

    bool isReady_; 
    int life_;

    bool doJump_;
    AnimationState* walkState_; 
    AnimationState* idleState_;
    AnimationState* jumpState_;
    AnimationState* kickState_;

    /// Flag when player is wounded.
    bool wounded_;
    /// Flag when player is dead.
    bool killed_;
    /// Timer for particle emitter duration.
    float timer_;
    /// Number of coins in the current level.
    int maxCoins_;
    /// Counter for remaining coins to pick.
    int remainingCoins_;
    /// Counter for remaining lifes.
    int remainingLifes_;
    /// Indicate when the player is climbing a ladder or a rope.
    bool isClimbing_;
    /// Used only for ropes, as they are split into 2 shapes.
    bool climb2_;
    /// Indicate when the player is above a climbable object, so we can still jump anyway.
    bool aboveClimbable_;
    /// Indicate when the player is climbing a slope, so we can apply force to its body.
    bool onSlope_;
    bool isAI_;

    Vector3 playerPos_;
    bool doMove_;
    bool chooseMove_;
    float lastMove_;
    float currMove_;

    int agentIndex;

    SharedPtr<Node> genotypeNode_; // Scene node displaying genotype
    SharedPtr<BillboardSet> genotypeBBSet_; // Billboard set for genotype

    SharedPtr<Node> powerbarNode_; // Scene node displaying powerbar
    SharedPtr<BillboardSet> powerbarBBSet_; // Billboard set for powerbar

};
