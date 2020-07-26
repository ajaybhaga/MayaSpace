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

#include <Urho3D/Core/Context.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimationState.h>


#include <Urho3D/DebugNew.h>
#include <MayaSpace/ai/agent.h>
#include "GameController.h"


#include "Character2D.h"

// Character2D logic component
Character2D::Character2D(Context *context) :
        LogicComponent(context),
        wounded_(false),
        killed_(false),
        timer_(0.0f),
        maxCoins_(0),
        remainingCoins_(0),
        remainingLifes_(3),
        isClimbing_(false),
        climb2_(false),
        aboveClimbable_(false),
        onSlope_(false) {
    // Face model forward (right)
    heading_ = 270.0f;
    life_ = 100;
    type_ = 1;
    // Set true once hit ground
    isReady_ = false;
    doJump_ = true;

}

void Character2D::RegisterObject(Context *context) {
    context->RegisterFactory<Character2D>();

    // These macros register the class attributes to the Context for automatic load / save handling.
    // We specify the 'Default' attribute mode which means it will be used both for saving into file, and network replication.
    URHO3D_ATTRIBUTE("Wounded", bool, wounded_, false, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Killed", bool, killed_, false, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Timer", float, timer_, 0.0f, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Coins In Level", int, maxCoins_, 0, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Remaining Coins", int, remainingCoins_, 0, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Remaining Lifes", int, remainingLifes_, 3, AM_DEFAULT);
    // Note that we don't load/save isClimbing_ as the contact listener already sets this bool.
    URHO3D_ATTRIBUTE("Is Climbing Rope", bool, climb2_, false, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Is Above Climbable", bool, aboveClimbable_, false, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Is On Slope", bool, onSlope_, false, AM_DEFAULT);
}

void Character2D::Update(float timeStep) {
    // Handle wounded/killed states
    if (killed_)
        return;

    if (wounded_) {
        HandleWoundedState(timeStep);
        return;
    }

    // Set temporary variables
    auto *body = GetComponent<RigidBody2D>();
    auto *animatedModel = GetComponent<AnimatedModel>();

    // Collision detection (AABB query)
    Vector2 characterHalfSize = Vector2(0.05f, 0.05f);
    auto *physicsWorld = GetScene()->GetComponent<PhysicsWorld2D>();
    PODVector<RigidBody2D *> collidingBodies;
    physicsWorld->GetRigidBodies(collidingBodies,
                                 Rect(node_->GetWorldPosition2D() - characterHalfSize - Vector2(0.0f, 0.1f),
                                      node_->GetWorldPosition2D() + characterHalfSize));

    if (collidingBodies.Size() > 1 && !isClimbing_) {
        currState_.onGround = true;
    }


//    URHO3D_LOGINFOF("CHAR[%d] POS [x=%f, y=%f]", id_, node_->GetWorldPosition2D().x_,node_->GetWorldPosition2D().y_);

    // Handle controller to update character state
    if (isAI_) {
        currState_ = HandleAIController(timeStep);
    } else {
        currState_ = HandleP1Controller(timeStep);
    }

    // Player mechanics
    if (currState_.onGround) {
        if (doJump_) {
            currState_.jump = true;
            doJump_ = false;
        }


        // Player ready when near ground
        if (node_->GetWorldPosition2D().y_ < 2.0f) {
            isReady_ = true;
        }
    }

    if (forward_) {
        heading_ = 270.0f;
        //  Update rotation of model to forward
        if (heading_ < 270.0f) { heading_ += 2.4f; };
        if (heading_ > 270.0f) { heading_ -= 2.4f; };
    } else {
        heading_ = 90.0f;
        //  Update rotation of model to back
        if (heading_ < 90.0f) { heading_ += 2.4f; };
        if (heading_ > 90.0f) { heading_ -= 2.4f; };
    }

    bool idle = (!currState_.walk && !currState_.jump);

    // Set animation state
    auto *cache = GetSubsystem<ResourceCache>();
    auto *model = node_->GetComponent<AnimatedModel>(true);

    String walkAnimStr = "";
    String idleAnimStr = "";
    String jumpAnimStr = "";
    String attackAnimStr = "";

    //sprite1_0008_Walking0010017_WushuKicks001.ani
    //sprite1_0008_Walking0010008_Walking001.ani
    //sprite1_0008_Walking0010018_DanceTurns001.ani

    switch (type_) {
        case 1:

            ///code/dev/MayaSpace/src/Urho3D/bin/Data/Models/spriteBase/Models/0008_Walking001_0008_Walking001_0018_DanceTurns001_0008_Walking.ani
            walkAnimStr = "Models/spriteBase/Models/Movements.ani";

            idleAnimStr = jumpAnimStr = attackAnimStr = walkAnimStr;
            //  walkAnimStr = "Models/spritePlayerA/sprite1_0008_Walking0010008_Walking001.ani";
//            idleAnimStr = "Models/spritePlayerA/sprite1_0008_Walking0010008_Walking001.ani";
            //           jumpAnimStr = "Models/spritePlayerA/sprite1_0008_Walking0010018_DanceTurns001.ani";
            //          attackAnimStr = "Models/spritePlayerA/sprite1_0008_Walking0010017_WushuKicks001.ani";
            break;
        case 2:
//            walkAnimStr = "Models/spriteBase/Models/0008_Walking001_0008_Walking001_0018_DanceTurns001_0008_Walking.ani";
            walkAnimStr = "Models/spriteBase/Models/Movements.ani";

            idleAnimStr = jumpAnimStr = attackAnimStr = walkAnimStr;

            // walkAnimStr = "Models/spritePlayerA/sprite1_0008_Walking0010008_Walking001.ani";
//            idleAnimStr = "Models/spritePlayerA/sprite1_0008_Walking0010008_Walking001.ani";
//            jumpAnimStr = "Models/spritePlayerA/sprite1_0008_Walking0010018_DanceTurns001.ani";
            //           attackAnimStr = "Models/spritePlayerA/sprite1_0008_Walking0010017_WushuKicks001.ani";
            break;

    }

    auto *walkAnimation = cache->GetResource<Animation>(walkAnimStr);
    auto *idleAnimation = cache->GetResource<Animation>(idleAnimStr);
    auto *jumpAnimation = cache->GetResource<Animation>(jumpAnimStr);
    auto *kickAnimation = cache->GetResource<Animation>(attackAnimStr);

    // model->RemoveAllAnimationStates(); // -> this clears the timesteps also

    // AddAnimationState includes an existence check
    walkState_ = model->AddAnimationState(walkAnimation);
    idleState_ = model->AddAnimationState(idleAnimation);
    jumpState_ = model->AddAnimationState(jumpAnimation);
    kickState_ = model->AddAnimationState(kickAnimation);


    if (currState_.walk) {

        // The state would fail to create (return null) if the animation was not found
        if (walkState_) {
            // Enable full blending weight and looping
            walkState_->SetWeight(1.0f);
            walkState_->AddTime(timeStep);
            walkState_->SetLooped(true);

        }
    } else {

        // The state would fail to create (return null) if the animation was not found
        if (walkState_) {
            // Enable full blending weight and looping
            walkState_->SetWeight(0.0f);
            walkState_->SetTime(0.0f);
        }
    }

    /*
    if (idle) {

        model->RemoveAllAnimationStates();
        idleState_ = model->AddAnimationState(idleAnimation);

        // The state would fail to create (return null) if the animation was not found
        if (idleState_)
        {
            // Enable full blending weight and looping
            idleState_->SetWeight(1.0f);
            idleState_->AddTime(timeStep);
            idleState_->SetLooped(true);
        }
    } else {

        // The state would fail to create (return null) if the animation was not found
        if (idleState_)
        {
            // Enable full blending weight and looping
            idleState_->SetWeight(1.0f);
            idleState_->SetTime(0.0f);
        }
    }


    if (currState_.jump) {

        model->RemoveAllAnimationStates();
        jumpState_ = model->AddAnimationState(jumpAnimation);

        // The state would fail to create (return null) if the animation was not found
        if (jumpState_)
        {
            // Enable full blending weight and looping
            jumpState_->SetWeight(1.0f);
            jumpState_->AddTime(timeStep);
        }
    } else {

        // The state would fail to create (return null) if the animation was not found
        if (jumpState_)
        {
            // Enable full blending weight and looping
            jumpState_->SetWeight(1.0f);
            jumpState_->SetTime(0.0f);
        }

    }

    if (currState_.kick) {

        model->RemoveAllAnimationStates();
        kickState_ = model->AddAnimationState(kickAnimation);

        // The state would fail to create (return null) if the animation was not found
        if (kickState_)
        {
            // Enable full blending weight and looping
            kickState_->SetWeight(1.0f);
            kickState_->AddTime(timeStep);
        }
    } else {

        model->RemoveAllAnimationStates();
        AnimationState* kickState_ = model->AddAnimationState(kickAnimation);
        // The state would fail to create (return null) if the animation was not found
        if (kickState_)
        {
            // Enable full blending weight and looping
            kickState_->SetWeight(1.0f);
            kickState_->SetTime(0.0f);
        }

    }*/

    // Move character
    if (!currState_.moveDir.Equals(Vector3::ZERO) || currState_.jump) {

        //URHO3D_LOGINFOF("AI[%d] MOVE DIR [x=%f, y=%f, z=%f]", id_, currState_.moveDir.x_,currState_.moveDir.y_, currState_.moveDir.z_);

//        if (onSlope_)
        //          body->ApplyForceToCenter(moveDir * MOVE_SPEED / 2, true); // When climbing a slope, apply force (todo: replace by setting linear velocity to zero when will work)
        //    else

        node_->Translate(
                Vector3(currState_.moveDir.x_, currState_.moveDir.y_, currState_.moveDir.z_) * timeStep * 1.8f);

        // Snap character back to z = 0
        Vector3 pos = node_->GetPosition();

        //  node_->SetPosition(Vector3(pos.x_, pos.y_, 0.0f));

//                node_->SetPosition(Vector3(-10.0f, 0.0f, 0.0f));
        //    node_->SetScale(0.0024f);
        //     node_->SetScale(0.4f);


        if (currState_.jump)
            body->ApplyLinearImpulse(Vector2(0.0f, 0.005f) * MOVE_SPEED, body->GetMassCenter(), true);
    }
}

PlayerState Character2D::HandleP1Controller(float timeStep) {
    auto *input = GetSubsystem<Input>();

    // Store previous state
    prevState_ = currState_;
    // Reset state
    currState_.jump = false;
    currState_.walk = false;

    // Let kick stay for 2 seconds
    if (currMove_ - currState_.lastKick > 2.0f) {
        currState_.kick = false;
    }
    currState_.moveDir = Vector3::ZERO;

    currMove_ += timeStep;
//    URHO3D_LOGINFOF("[%f] ai[%d] -> diff time = %f", timeStep, id_,  currMove_-lastMove_);

    if (timeStep > 0) {
        // GAME CONTROLS

        // Jump. Must release jump control between jumps
        if (controls_.IsDown(BUTTON_A) || input->GetKeyDown('J')) {
            doJump_ = true;
        }

        if (controls_.IsDown(BUTTON_X) || input->GetKeyDown('R')) {
            currState_.kick = true;

            // Progress animation
            auto *model = node_->GetComponent<AnimatedModel>(true);
            if (model->GetNumAnimationStates()) {
                AnimationState *state = model->GetAnimationStates()[0];
                state->AddTime(timeStep);
            }

        }

        // Dpad
        if (controls_.IsDown(BUTTON_DPAD_UP)) {
        }

        if (controls_.IsDown(BUTTON_DPAD_DOWN)) {
        }

        if (controls_.IsDown(BUTTON_DPAD_RIGHT)) {
        }

        if (input->GetKeyDown('A') || input->GetKeyDown(KEY_LEFT) || controls_.IsDown(BUTTON_DPAD_LEFT)) {
            walkState_->AddTime(timeStep);

            forward_ = false;
            currState_.walk = true;
        }

        if (input->GetKeyDown('D') || input->GetKeyDown(KEY_RIGHT) || controls_.IsDown(BUTTON_DPAD_RIGHT)) {
/*                auto *model = node_->GetComponent<AnimatedModel>(true);
                if (model->GetNumAnimationStates()) {
                    AnimationState *state = model->GetAnimationStates()[0];
                    state->AddTime(timeStep);

                }
*/
            walkState_->AddTime(timeStep);

            forward_ = true;
            currState_.walk = true;
        }

        // Jump
        if ((currState_.onGround || aboveClimbable_) &&
            (input->GetKeyPress('W') || input->GetKeyPress(KEY_UP) || controls_.IsDown(BUTTON_A)))
            currState_.jump = true;

        // END GAME CONTROLS


        if (currState_.walk) {
            // Update movement direction
            currState_.moveDir = currState_.moveDir - Vector3::FORWARD;
        }
    }

    // Return player state
    return currState_;
}

PlayerState Character2D::HandleAIController(float timeStep) {
    auto *input = GetSubsystem<Input>();

    // Store previous state
    prevState_ = currState_;
    // Reset state
    currState_.jump = false;
    currState_.walk = false;

    // TODO: Connect to Agent Movement which has calculated inputs

    // Iterate through agent controllers and apply update
    std::vector<Agent *> agents = EvolutionManager::getInstance()->getAgents();
    std::vector<AgentController *> controllers = EvolutionManager::getInstance()->getAgentControllers();

    if (!controllers.empty()) {
        // Get agent controller
        AgentController *controller = controllers[agentIndex];
        // Process sensor inputs through ffn and apply calculated inputs
        controller->update(timeStep);
        // Set agent evaluation (affects fitness calculation)
        controller->setCurrentCompletionReward(controller->getCurrentCompletionReward() + Random(0.0f, 1.0f));
    }

    // Let kick stay for 2 seconds
    if (currMove_ - currState_.lastKick > 2.0f) {
        currState_.kick = false;
    }
    currState_.moveDir = Vector3::ZERO;

    currMove_ += timeStep;
//    URHO3D_LOGINFOF("[%f] ai[%d] -> diff time = %f", timeStep, id_,  currMove_-lastMove_);

    if (timeStep > 0) {
        if (isAI_) {

            // Update AI timer for next move
            if (currMove_ - lastMove_ > 2.0f) {
                chooseMove_ = true;
                lastMove_ = currMove_;
                //              URHO3D_LOGINFOF("ai[%d] = DO MOVE", id_);
            }

            // Update frame
            auto *model = node_->GetComponent<AnimatedModel>(true);
            for (AnimationState *state : model->GetAnimationStates()) {
                state->AddTime(timeStep);
                //URHO3D_LOGINFOF("update frame -> timeStep %f", timeStep);

            }

            if (playerPos_.x_ < GetNode()->GetPosition().x_) {
                forward_ = false;
            } else {

                forward_ = true;
            }


            if (chooseMove_) {
                int r = Random(1, 5);
                switch (r) {
                    case 1:
                        currState_.walk = true;
                        // Store time
                        currState_.lastWalk = currMove_;
                    case 2:
                        currState_.kick = true;
                        URHO3D_LOGINFOF("AI KICK -> CHOSE MOVE %d -> AI STATE [forward=%d, walk=%d, jump=%d, kick=%d]",
                                        r, forward_, currState_.walk, currState_.jump, currState_.kick);
                        // Store time
                        currState_.lastKick = currMove_;
                        break;
                    case 3:
                        doJump_ = true;
                        // Store time
                        currState_.lastJump = currMove_;
                        break;
                }
                //       URHO3D_LOGINFOF("CHOSE MOVE %d -> AI STATE [forward=%d, walk=%d, jump=%d, kick=%d]", r, forward_, currState_.walk, currState_.jump, currState_.kick);
                chooseMove_ = false;
                doMove_ = true;
            }

            // Wait for move to complete
            if ((currMove_ - lastMove_) > 2.0f) {
                // Reset timer
                lastMove_ = currMove_;
                doMove_ = false;
                //      URHO3D_LOGINFOF("[%f] ai = MOVE COMPLETE", (currMove_-lastMove_));
            } else {
                if (prevState_.walk)
                    currState_.walk = true;

                // If the AI is close enough, put the AI in idle mode
                if ((abs(GetNode()->GetPosition().x_ - playerPos_.x_)) < 0.02f) {
                    currState_.walk = false;
                }

            }

        } else {
            // GAME CONTROLS

            // Jump. Must release jump control between jumps
            if (controls_.IsDown(BUTTON_A) || input->GetKeyDown('J')) {
                doJump_ = true;
            }

            if (controls_.IsDown(BUTTON_X) || input->GetKeyDown('R')) {
                currState_.kick = true;

                // Progress animation
                auto *model = node_->GetComponent<AnimatedModel>(true);
                if (model->GetNumAnimationStates()) {
                    AnimationState *state = model->GetAnimationStates()[0];
                    state->AddTime(timeStep);
                }

            }

            // Dpad
            if (controls_.IsDown(BUTTON_DPAD_UP)) {
            }

            if (controls_.IsDown(BUTTON_DPAD_DOWN)) {
            }

            if (controls_.IsDown(BUTTON_DPAD_RIGHT)) {
            }

            if (input->GetKeyDown('A') || input->GetKeyDown(KEY_LEFT) || controls_.IsDown(BUTTON_DPAD_LEFT)) {
                walkState_->AddTime(timeStep);

                forward_ = false;
                currState_.walk = true;
            }

            if (input->GetKeyDown('D') || input->GetKeyDown(KEY_RIGHT) || controls_.IsDown(BUTTON_DPAD_RIGHT)) {
/*                auto *model = node_->GetComponent<AnimatedModel>(true);
                if (model->GetNumAnimationStates()) {
                    AnimationState *state = model->GetAnimationStates()[0];
                    state->AddTime(timeStep);

                }
*/
                walkState_->AddTime(timeStep);

                forward_ = true;
                currState_.walk = true;
            }

            // Jump
            if ((currState_.onGround || aboveClimbable_) &&
                (input->GetKeyPress('W') || input->GetKeyPress(KEY_UP) || controls_.IsDown(BUTTON_A)))
                currState_.jump = true;

            // END GAME CONTROLS
        }

        if (currState_.walk) {

            if (isAI_) {
                // Slow down AI
                // Update movement direction
                currState_.moveDir = currState_.moveDir - Vector3::FORWARD * 0.3f;
            } else {
                // Update movement direction
                currState_.moveDir = currState_.moveDir - Vector3::FORWARD;
            }
        }
    }

    // Return player state
    return currState_;
}

void Character2D::HandleWoundedState(float timeStep) {
    auto *body = GetComponent<RigidBody2D>();
    // auto* animatedSprite = GetComponent<AnimatedSprite2D>();

/*
    // Play "hit" animation in loop
    if (animatedSprite->GetAnimation() != "hit")
        animatedSprite->SetAnimation("hit", LM_FORCE_LOOPED);
*/
    // Update timer
    timer_ += timeStep;

    if (timer_ > 2.0f) {
        // Reset timer
        timer_ = 0.0f;

        // Clear forces (should be performed by setting linear velocity to zero, but currently doesn't work)
        body->SetLinearVelocity(Vector2::ZERO);
        body->SetAwake(false);
        body->SetAwake(true);

        // Remove particle emitter
        node_->GetChild("Emitter", true)->Remove();

        // Update lifes UI and counter
        remainingLifes_ -= 1;
        auto *ui = GetSubsystem<UI>();
        Text *lifeText = static_cast<Text *>(ui->GetRoot()->GetChild("LifeText", true));
        lifeText->SetText(String(remainingLifes_)); // Update lifes UI counter

        // Reset wounded state
        wounded_ = false;

        // Handle death
        if (remainingLifes_ == 0) {
            HandleDeath();
            return;
        }

        // Re-position the character to the nearest point
        if (node_->GetPosition().x_ < 15.0f)
            node_->SetPosition(Vector3(1.0f, 8.0f, 0.0f));
        else
            node_->SetPosition(Vector3(18.8f, 9.2f, 0.0f));
    }
}

void Character2D::HandleDeath() {
    auto *body = GetComponent<RigidBody2D>();
    // auto* animatedSprite = GetComponent<AnimatedSprite2D>();

    // Set state to 'killed'
    killed_ = true;

    // Update UI elements
    auto *ui = GetSubsystem<UI>();
    Text *instructions = static_cast<Text *>(ui->GetRoot()->GetChild("Instructions", true));
    instructions->SetText("!!! GAME OVER !!!");
    static_cast<Text *>(ui->GetRoot()->GetChild("ExitButton", true))->SetVisible(true);
    static_cast<Text *>(ui->GetRoot()->GetChild("PlayButton", true))->SetVisible(true);

    // Show mouse cursor so that we can click
    auto *input = GetSubsystem<Input>();
    input->SetMouseVisible(true);

    // Put character outside of the scene and magnify him
//    node_->SetPosition(Vector3(-20.0f, 0.0f, 0.0f));
    node_->SetPosition(Vector3(-10.0f, 0.0f, 0.0f));
    node_->SetScale(1.2f);

    // Play death animation once
    /* if (animatedSprite->GetAnimation() != "dead2")
         animatedSprite->SetAnimation("dead2");*/
}
