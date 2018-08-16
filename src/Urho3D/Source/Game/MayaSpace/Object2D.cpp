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
#include "GameController.h"


#include "Object2D.h"

// Object2D logic component
Object2D::Object2D(Context* context) :
    LogicComponent(context),
    wounded_(false),
    killed_(false),
    timer_(0.0f) {

    // Face model forward (right)
    heading_ = 270.0f;
    // Set true once hit ground
    isReady_ = false;
    currState_.moveDir = Vector3::ZERO;
}

void Object2D::RegisterObject(Context* context)
{
    context->RegisterFactory<Object2D>();

    // These macros register the class attributes to the Context for automatic load / save handling.
    // We specify the 'Default' attribute mode which means it will be used both for saving into file, and network replication.
    URHO3D_ATTRIBUTE("Wounded", bool, wounded_, false, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Killed", bool, killed_, false, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Timer", float, timer_, 0.0f, AM_DEFAULT);
    // Note that we don't load/save isClimbing_ as the contact listener already sets this bool.
}

void Object2D::Update(float timeStep)
{
    // Handle wounded/killed states
    if (killed_)
        return;

    node_->SetRotation(Quaternion(0.0f, -180.0f-heading_, 0.0f));

    // Set temporary variables
    auto* body = GetComponent<RigidBody2D>();
    auto* staticModel = GetComponent<StaticModel>();

    // Collision detection (AABB query)
    Vector2 characterHalfSize = Vector2(0.16f, 0.16f);
    auto* physicsWorld = GetScene()->GetComponent<PhysicsWorld2D>();
    PODVector<RigidBody2D*> collidingBodies;
    physicsWorld->GetRigidBodies(collidingBodies, Rect(node_->GetWorldPosition2D() - characterHalfSize - Vector2(0.0f, 0.1f), node_->GetWorldPosition2D() + characterHalfSize));

    if (collidingBodies.Size() > 1) {
        currState_.onGround = true;
    }
   
//    URHO3D_LOGINFOF("OBJ[%d] POS [x=%f, y=%f]", id_, node_->GetWorldPosition2D().x_,node_->GetWorldPosition2D().y_);

    // Handle controller to update character state
    currState_ = UpdateState(timeStep);

    // Object-specific behaviour
    switch (type_) {
        case 1:
        // Balloon 
        heading_ += 5.5f * Random(1.5f,8.5f);
        if (heading_ > 360.f) {
            heading_ = 0.0f;
        }

        if (node_->GetWorldPosition2D().y_ < 3.0f)
           body->ApplyLinearImpulse(Vector2(0.0f, 0.005f) * 30.0f, body->GetMassCenter(), true);

        if (node_->GetWorldPosition2D().y_ < 1.0f)
            body->ApplyLinearImpulse(Vector2(0.0f, 1.0f) * 0.1f, body->GetMassCenter(), true);


        break;

        case 2:
        // Cloud
        if (node_->GetWorldPosition2D().y_ < 6.0f)
           body->ApplyLinearImpulse(Vector2(0.0f, 0.005f) * 11.0f, body->GetMassCenter(), true);

        if (node_->GetWorldPosition2D().y_ < 1.0f)
            body->ApplyLinearImpulse(Vector2(0.0f, 1.0f) * 0.1f, body->GetMassCenter(), true);

        break;
    }

    // General object behaviour
    if (node_->GetWorldPosition2D().x_ < 0.2f) {
        body->ApplyLinearImpulse(Vector2(1.0f, 0.0f) * 0.1f, body->GetMassCenter(), true);
    }

    if (node_->GetWorldPosition2D().x_ > 30.0f) {
        body->ApplyLinearImpulse(Vector2(-1.0f, 0.0f) * 0.1f, body->GetMassCenter(), true);
    }

    // Move character
    if (!currState_.moveDir.Equals(Vector3::ZERO))
    {


//        if (onSlope_)
  //          body->ApplyForceToCenter(moveDir * MOVE_SPEED / 2, true); // When climbing a slope, apply force (todo: replace by setting linear velocity to zero when will work)
    //    else
            node_->Translate(Vector3(currState_.moveDir.x_, currState_.moveDir.y_, currState_.moveDir.z_) * timeStep * 1.8f);
//            node_->Translate(Vector3(0, 0, 2.0) * timeStep * 1.0f);

     //   if (currState_.jump)
       //     body->ApplyLinearImpulse(Vector2(0.0f, 0.005f) * MOVE_SPEED, body->GetMassCenter(), true);
    }
}

ObjectState Object2D::UpdateState(float timeStep)
{
    auto* input = GetSubsystem<Input>();

    // Store previous state
    prevState_ = currState_;
    // Reset state
    currState_.onGround = false;

    currMove_ += timeStep;
//    URHO3D_LOGINFOF("[%f] ai[%d] -> diff time = %f", timeStep, id_,  currMove_-lastMove_);

            // Update AI timer for next move
            if (currMove_-lastMove_ > 2.0f) {
                chooseMove_ = true;
                lastMove_ = currMove_;
              //  URHO3D_LOGINFOF("ai[%d] = DO MOVE", id_);
            }

            if (playerPos_.x_ < GetNode()->GetPosition().x_) { 
                forward_ = false;
            } else {

                forward_ = true;
            }


            if (chooseMove_) {
                int r = Random(1,5);           
                switch (r) {
                    case 1:
                        currState_.moveDir.y_ -= 1.5f;
//                    currState_.walk = true;
                    case 2:
  //                  currState_.kick = true;
                    break;
                    case 3:
    //                doJump_ = true;
                    break;
                }
               // URHO3D_LOGINFOF("CHOSE MOVE %d -> AI STATE [forward=%d, walk=%d, jump=%d, kick=%d]", r, forward_, currState_.walk, currState_.jump, currState_.kick);
                chooseMove_ = false;
                doMove_ = true;
            }

            // Wait for move to complete
            if ((currMove_-lastMove_) > 2.0f) {
                // Reset timer
                lastMove_ = currMove_;
                doMove_ = false;
             //   URHO3D_LOGINFOF("[%f] ai = MOVE COMPLETE", (currMove_-lastMove_));
            } else {
      //          if (prevState_.walk)
        //            currState_.walk = true;

                // If the AI is close enough, put the AI in idle mode
          //     if ((abs(GetNode()->GetPosition().x_-playerPos_.x_)) < 0.05f) {
            //        currState_.walk = false;
              // }

            }
           


/*        if (isAI_) {
            // Slow down AI
            // Update movement direction
            currState_.moveDir = currState_.moveDir - Vector3::FORWARD * 0.3f;
        } else {
            // Update movement direction
            currState_.moveDir = currState_.moveDir - Vector3::FORWARD;
        }
    }*/

    // Return player state
    return currState_;
}