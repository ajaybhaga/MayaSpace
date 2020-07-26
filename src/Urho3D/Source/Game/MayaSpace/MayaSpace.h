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

#include <MayaSpace/ai/evolution_manager.h>
#include "Game.h"
#include "Sample2D.h"

class Character2D;
class Sample2D;
class EvolutionManager;

#define MAX_AGENTS 1024 // Set max limit for agents (used for storage)

struct ParticlePool {
    bool used; // Is particle emitter used?
    int usedBy; // Node id using particle emitter
    SharedPtr<Node> node; // Scene node
    float lastEmit;
    float currEmit;
    float timeout;
};

/// Urho2D platformer example.
/// This sample demonstrates:
///    - Creating an orthogonal 2D scene from tile map file
///    - Displaying the scene using the Renderer subsystem
///    - Handling keyboard to move a character and zoom 2D camera
///    - Generating physics shapes from the tmx file's objects
///    - Mixing physics and translations to move the character
///    - Using Box2D Contact listeners to handle the gameplay
///    - Displaying debug geometry for physics and tile map
/// Note that this sample uses some functions from Sample2D utility class.
class MayaSpace : public Game
{
    URHO3D_OBJECT(MayaSpace, Game);

public:
    /// Construct.
    explicit MayaSpace(Context* context);

    /// Setup after engine initialization and before running the main loop.
    void Start() override;
    /// Setup before engine initialization. Modifies the engine parameters.
    void Setup() override;

    void Stop() override;

private:
    /// Construct the scene content.
    void CreateScene();
    /// Subscribe to application-wide logic update events.
    void SubscribeToEvents();
    /// Handle the logic update event.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the logic post update event.
    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the post render update event.
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the end rendering event.
    void HandleSceneRendered(StringHash eventType, VariantMap& eventData);
    void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
    /// Handle the contact begin event (Box2D contact listener).
    void HandleCollisionBegin(StringHash eventType, VariantMap& eventData);
    /// Handle the contact end event (Box2D contact listener).
    void HandleCollisionEnd(StringHash eventType, VariantMap& eventData);
    /// Handle reloading the scene.
    void ReloadScene(bool reInit);
    /// Handle 'PLAY' button released event.
    void HandlePlayButton(StringHash eventType, VariantMap& eventData);

    void SetParticleEmitter(int hitId, float contactX, float contactY, int type, float timeStep);
    void HandleUpdateParticlePool(float timeStep);

    // Init Genetic Algorithm sprite generator
    void InitEvolutionSpriteGenerator();

    // Display Genetic Algorithm Evolution Manager statistics
    void ShowEvolutionManagerStats();

    /// The controllable character component.
    WeakPtr<Character2D> player_;
    WeakPtr<Character2D> agents_[MAX_AGENTS];

    /// Flag for drawing debug geometry.
    bool drawDebug_{};
    /// Scaling factor based on tiles' aspect ratio.
    float moveSpeedScale_{};

    /// Sample2D utility object.
    SharedPtr<Sample2D> sample2D_;
    /// Powerbar P1 sprite.
    SharedPtr<Sprite> powerbarP1Sprite_;
    /// Powerbar Bkg P1 sprite.
    SharedPtr<Sprite> powerbarBkgP1Sprite_;

    /// Particle pool
    ParticlePool particlePool_[20];

    #define NUM_DEBUG_FIELDS 8
    // Debug text
    Text* debugText_[NUM_DEBUG_FIELDS];
};