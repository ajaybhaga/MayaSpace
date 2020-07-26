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
/*

    Written by Ajay Bhaga 2019/2020

*/
#include <sstream>
#include <string>
#include <iostream>
#include <Urho3D/Audio/Audio.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/Urho2D/CollisionBox2D.h>
#include <Urho3D/Urho2D/CollisionChain2D.h>
#include <Urho3D/Urho2D/CollisionCircle2D.h>
#include <Urho3D/Urho2D/CollisionPolygon2D.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimationState.h>
#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>


#include <Urho3D/Urho2D/PhysicsEvents2D.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Urho2D/TileMap2D.h>
#include <Urho3D/Urho2D/TileMapLayer2D.h>
#include <Urho3D/Urho2D/TileMap3D.h>
#include <Urho3D/Urho2D/TileMapLayer3D.h>
#include <Urho3D/Urho2D/TmxFile2D.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Graphics/Zone.h>

#include <Urho3D/Urho2D/ParticleEffect2D.h>
#include <Urho3D/Urho2D/ParticleEmitter2D.h>


#include <Urho3D/DebugNew.h>

#include "GameController.h"
#include "Character2D.h"
#include "Object2D.h"
#include "Sample2D.h"
#include "Utilities2D/Mover.h"
#include "MayaSpace.h"
#include <MayaSpace/ai/evolution_manager.h>


// AgentSim shared libs
#include "shared_libs.h"

URHO3D_DEFINE_APPLICATION_MAIN(MayaSpace)

MayaSpace::MayaSpace(Context *context) :
        Game(context) {
    // Register factory for the Character2D component so it can be created via CreateComponent
    Character2D::RegisterObject(context);
    // Register factory for the Object2D component so it can be created via CreateComponent
    Object2D::RegisterObject(context);
    // Register factory and attributes for the Mover component so it can be created via CreateComponent, and loaded / saved
    Mover::RegisterObject(context);
}

void MayaSpace::Setup() {
    Game::Setup();
}

void MayaSpace::InitEvolutionSpriteGenerator() {

    std::cout << "Evolution Manager -> starting..." << std::endl;

    EvolutionManager::getInstance()->startEvolution();
    EvolutionManager *evolutionManager = EvolutionManager::getInstance();

    // Create shape for each agent
    std::vector<Agent *> agents = evolutionManager->getAgents();
    std::cout << "Evolution Manager -> number of agents = " << agents.size() << std::endl;

    for (int i = 0; i < agents.size(); i++) {
        // Randomly place agents
        //       agents[i]->setPosition(cyclone::Vector3(rd(), rd(), rd()));
    }

    // DISABLED CYCLONE PHYSICS CODE //
    /*
           // Create agent -> collision sphere
           cyclone::CollisionSphere cs;
           cs.body = new cyclone::RigidBody();
           agentCollSpheres.emplace_back(cs);

           cs.radius = 0.25f;
           cs.body->setMass(5.0f);
           cs.body->setDamping(0.9f, 0.9f);
           cyclone::Matrix3 it;
           it.setDiagonal(5.0f, 5.0f, 5.0f);
           cs.body->setPosition(agents[i]->getPosition());
           cs.body->setInertiaTensor(it);
           cs.body->setAcceleration(cyclone::Vector3::GRAVITY);

           cs.body->setCanSleep(false);
           cs.body->setAwake(true);
    */

    /*
        // Create the ball.
        agentCollSphere.body = new cyclone::RigidBody();
        ball.radius = 0.25f;
        ball.body->setMass(5.0f);
        ball.body->setDamping(0.9f, 0.9f);
        cyclone::Matrix3 it;
        it.setDiagonal(5.0f, 5.0f, 5.0f);
        ball.body->setInertiaTensor(it);
        ball.body->setAcceleration(cyclone::Vector3::GRAVITY);

        ball.body->setCanSleep(false);
        ball.body->setAwake(true);
    */
    /*
    // Test Case 1
    auto genotype = new Genotype();
    std::cout << "Generating random genotype..." << std::endl;
    genotype = genotype->generateRandom(10, 0.4, 20.4);
    genotype->outputToConsole();
    genotype->saveToFile("genotype01.data");
    std::cout << "Genotype saved to disk." << std::endl;

    std::cout << "Loading genotype from disk..." << std::endl;
    genotype = genotype->loadFromFile("genotype01.data");
    std::cout << "Loaded genotype from disk." << std::endl;
    */

}


void MayaSpace::ShowEvolutionManagerStats() {
    std::vector<Agent *> agents = EvolutionManager::getInstance()->getAgents();
    std::vector<AgentController *> controllers = EvolutionManager::getInstance()->getAgentControllers();

    char buffer[255];
    int aliveCount = EvolutionManager::getInstance()->agentsAliveCount;
    const int maxRows = 20;
    char *strText[maxRows];

    for (int i = 0; i < maxRows; i++) {
        strText[i] = new char[80];
    }

    for (int i = 0; i < maxRows; i++) {

        switch (i) {

            case maxRows - 1:
                sprintf(strText[i], "%d alive out of %d population.", aliveCount, agents.size());
                break;
            case maxRows - 2:
                sprintf(strText[i], "%d generation out of %d generations.",
                        EvolutionManager::getGeneticAlgorithm()->generationCount, RestartAfter);
                break;
            case maxRows - 3:
                sprintf(strText[i], "==========================================================================");
                break;
            case maxRows - 4:
                sprintf(strText[i], "agent[0].timeSinceLastCheckpoint: %f",
                        controllers[0]->getTimeSinceLastCheckpoint());
                break;

            case maxRows - 5:
                sprintf(strText[i], "agent[0].x: %f, agent[0].y: %f, agent[0].z: %f", agents[0]->getPosition().x_,
                        agents[0]->getPosition().y_, agents[0]->getPosition().z_);
                break;
            case maxRows - 6:
                sprintf(strText[i], "agent[0].winX: %f, agent[0].winY: %f, agent[0].winZ: %f",
                        agents[0]->getWinPos().x_, agents[0]->getWinPos().y_, agents[0]->getWinPos().z_);
                break;
            case maxRows - 7:
                sprintf(strText[i], "", agents[0]->getPosition().z_);
                break;

            case maxRows - 8:
                sprintf(strText[i], "agent[0].evaluation: %f", agents[0]->genotype->evaluation);
                break;
            case maxRows - 9:
                sprintf(strText[i], "agent[0].horizontalInput: %f", controllers[0]->movement->getHorizontalInput());
                break;
            case maxRows - 10:
                sprintf(strText[i], "agent[0].verticalInput: %f", controllers[0]->movement->getVerticalInput());
                break;


            default:
                sprintf(strText[i], "");
                break;

        }

        //       if (strText[i])
//            renderText(5, 5 + (10 * i), strText[i], NULL);

    }

    for (int i = 0; i < maxRows; i++) {
        delete strText[i];
    }

    char *c = new char[255];
    for (int i = 0; i < agents.size(); i++) {

        sprintf(c, "Agent:  %s\nagent[%d].x: %f\nagent[%d].y: %f\nagent[%d].z: %f",
                agents[i]->getName(),
                i, agents[i]->getPosition().x_,
                i, agents[i]->getPosition().y_,
                i, agents[i]->getPosition().z_);

        // renderPanel(agents[i]->getWinPos().x_, agents[i]->getWinPos().y_, 200.0f, 100.0f, c);

        if (agents[i]->genotype) {
            //     renderParameters(agents[i]->getWinPos().x_ + 90.0f, agents[i]->getWinPos().y_ - 80.0f,
            //                      agents[i]->genotype->getParameterCopy());
        }
    }
    delete[] c;
}

void MayaSpace::Start() {

    for (int i = 0; i < sizeof(particlePool_) / sizeof(*particlePool_); i++) {
        particlePool_[i].used = false;
        particlePool_[i].usedBy = -1;
    }

    // Execute base class startup
    Game::Start();

    // Initialize evolution sprite generator
    MayaSpace::InitEvolutionSpriteGenerator();

    sample2D_ = new Sample2D(context_);

    // Set filename for load/save functions
    sample2D_->demoFilename_ = "Platformer2D";

    context_->RegisterSubsystem(new GameController(context_));

    // Create the scene content
    CreateScene();

    UI *ui = GetSubsystem<UI>();

    // Create the UI content
    sample2D_->CreateUIContent("MayaSpace Game Engine v0.1", player_->remainingLifes_, player_->remainingCoins_);
//    auto* ui = GetSubsystem<UI>();
    Button *playButton = static_cast<Button *>(ui->GetRoot()->GetChild("PlayButton", true));
    SubscribeToEvent(playButton, E_RELEASED, URHO3D_HANDLER(MayaSpace, HandlePlayButton));

    // Hook up to the frame update events
    SubscribeToEvents();

}

void MayaSpace::Stop() {

    // Free evolution manager
    EvolutionManager::clean();

    // Dump engine resources
    Game::Stop();
}


void MayaSpace::CreateScene() {
    scene_ = new Scene(context_);
    sample2D_->scene_ = scene_;

    // Create the Octree, DebugRenderer and PhysicsWorld2D components to the scene
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();
    /*PhysicsWorld2D* physicsWorld =*/ scene_->CreateComponent<PhysicsWorld2D>();

    // Create camera
    cameraNode_ = scene_->CreateChild("Camera");
    auto *camera = cameraNode_->CreateComponent<Camera>();
//    camera->SetOrthographic(true);

    auto *graphics = GetSubsystem<Graphics>();
    //   camera->SetOrthoSize((float)graphics->GetHeight() * PIXEL_SIZE);
    camera->SetZoom(4.0f * Min((float) graphics->GetWidth() / 1280.0f, (float) graphics->GetHeight() /
                                                                       800.0f)); // Set zoom according to user's resolution to ensure full visibility (initial zoom (2.0) is set for full visibility at 1280x800 resolution)
    camera->SetFarClip(300.0f);

    // Setup the viewport for displaying the scene
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, camera));
    auto *renderer = GetSubsystem<Renderer>();
    renderer->SetViewport(0, viewport);

    ResourceCache *cache = GetSubsystem<ResourceCache>();
    UI *ui = GetSubsystem<UI>();

    // Set the default UI style and font
    //ui->GetRoot()->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
    auto *font = cache->GetResource<Font>("Fonts/Anonymous Pro.ttf");

    // Get powerbar texture
    Texture2D *powerbarTexture = cache->GetResource<Texture2D>("Textures/powerbar.png");
    if (!powerbarTexture)
        return;

    // Get powerbar background texture
    Texture2D *powerbarBkgTexture = cache->GetResource<Texture2D>("Textures/powerbar-bk.png");
    if (!powerbarBkgTexture)
        return;

    // Get genotype texture
    Texture2D *genotypeTexture = cache->GetResource<Texture2D>("Textures/genotype.png");
    if (!powerbarTexture)
        return;

    // Get genotype background texture
    Texture2D *genotypeBkgTexture = cache->GetResource<Texture2D>("Textures/genotype-bk.png");
    if (!powerbarBkgTexture)
        return;

    // Create sprite and add to the UI layout
    powerbarP1Sprite_ = ui->GetRoot()->CreateChild<Sprite>();
    powerbarBkgP1Sprite_ = ui->GetRoot()->CreateChild<Sprite>();

    // Set sprite texture
    powerbarP1Sprite_->SetTexture(powerbarTexture);
    powerbarBkgP1Sprite_->SetTexture(powerbarBkgTexture);

    int textureWidth;
    int textureHeight;

    textureWidth = powerbarTexture->GetWidth();
    textureHeight = powerbarTexture->GetHeight();

    powerbarP1Sprite_->SetScale(256.0f / textureWidth);
    powerbarP1Sprite_->SetSize(textureWidth, textureHeight);
    powerbarP1Sprite_->SetHotSpot(textureWidth, textureHeight);
    powerbarP1Sprite_->SetAlignment(HA_LEFT, VA_TOP);
    powerbarP1Sprite_->SetPosition(Vector2(300.0f, 50.0f));
    powerbarP1Sprite_->SetOpacity(1.0f);
    // Set a low priority so that other UI elements can be drawn on top
    powerbarP1Sprite_->SetPriority(-100);

    powerbarBkgP1Sprite_->SetScale(256.0f / textureWidth);
    powerbarBkgP1Sprite_->SetSize(textureWidth, textureHeight);
    powerbarBkgP1Sprite_->SetHotSpot(textureWidth, textureHeight);
    powerbarBkgP1Sprite_->SetAlignment(HA_LEFT, VA_TOP);
    powerbarBkgP1Sprite_->SetPosition(Vector2(300.0f, 50.0f));
    powerbarBkgP1Sprite_->SetOpacity(0.2f);
    // Set a low priority so that other UI elements can be drawn on top
    powerbarBkgP1Sprite_->SetPriority(-100);

    powerbarP1Sprite_->SetVisible(true);
    powerbarBkgP1Sprite_->SetVisible(true);

    // Create the UI for displaying the remaining lifes
    auto *lifeUI = ui->GetRoot()->CreateChild<BorderImage>("Life2");
    lifeUI->SetTexture(cache->GetResource<Texture2D>("Textures/jiva2d.png"));
    lifeUI->SetSize(60, 60);
    lifeUI->SetAlignment(HA_LEFT, VA_TOP);
    lifeUI->SetPosition(-5, 15);
    lifeUI->SetVisible(true);

    auto *lifeText = lifeUI->CreateChild<Text>("LifeText2");
    lifeText->SetAlignment(HA_CENTER, VA_CENTER);
    lifeText->SetPosition(2.0f, -30.0);
    lifeText->SetFont(font, 12);
    lifeText->SetTextEffect(TE_SHADOW);
    lifeText->SetText(String("Jiva"));
    lifeText->SetVisible(true);

    // Debug text

    for (int i = 0; i < NUM_DEBUG_FIELDS; i++) {
        debugText_[i] = lifeUI->CreateChild<Text>("DebugText1");
        debugText_[i]->SetAlignment(HA_LEFT, VA_CENTER);
        debugText_[i]->SetPosition(10.0f, 60.0 + (i * 20));
        debugText_[i]->SetFont(font, 8);
        debugText_[i]->SetTextEffect(TE_SHADOW);
        debugText_[i]->SetVisible(true);
        std::string debugData1;
        debugData1.append("-");
        debugText_[i]->SetText(debugData1.c_str());
    }


    /*
    auto* lifeText = ui->GetRoot()->CreateChild<Text>("LifeText2");
    lifeText->SetAlignment(HA_CENTER, VA_CENTER);
    lifeText->SetFont(font, 24);
    lifeText->SetTextEffect(TE_SHADOW);
    lifeText->SetText(String(3));
    lifeText->SetVisible(false);

*/
    using namespace std;



    // Set background color for the scene
    Zone *zone = renderer->GetDefaultZone();
    zone->SetFogColor(Color(0.2f, 0.2f, 0.2f));

    // Create tile map from tmx file
    SharedPtr<Node> tileMapNode(scene_->CreateChild("TileMap"));

//    auto* tileMap3d = tileMapNode->CreateComponent<TileMap3D>();
//    tileMap3d->SetTmxFile(cache->GetResource<TmxFile2D>("Urho2D/Tilesets/Ortho.tmx"

    TileMap3D *tileMap = tileMapNode->CreateComponent<TileMap3D>();
    URHO3D_LOGINFOF("tileMap=%x", tileMap);

    tileMap->SetTmxFile(cache->GetResource<TmxFile2D>("Urho2D/Tilesets/MayaSpace_Level0.tmx"));
    const TileMapInfo2D &info = tileMap->GetInfo();

/*
    // Create balloon object
    for (int i = 0; i < 4; i++) {
        Node* ballonNode = sample2D_->CreateObject(info, 0.0f, Vector3(Random(-0.0f,8.0f), 16.0f, 0.0f), 0.1f, 1);
        auto* obj_ = ballonNode->CreateComponent<Object2D>(); // Create a logic component to handle character behavior
        string name = "Balloon-P" + i;
        obj_->GetNode()->SetName(name.c_str());
        obj_->id_ = i;
        obj_->type_ = 1;
    }

    // Create cloud02 object
    for (int i = 0; i < 10; i++) {
        Node* cloudNode = sample2D_->CreateObject(info, 0.0f, Vector3(Random(-0.0f,8.0f), 16.0f, 0.0f), 0.1f, 2);
        auto* obj_ = cloudNode->CreateComponent<Object2D>(); // Create a logic component to handle character behavior
        string name = "Cloud02-P" + i;
        obj_->GetNode()->SetName(name.c_str());
        obj_->id_ = i;
        obj_->type_ = 2;
    }
*/

    // Create pumpkin object
    for (int i = 0; i < 10; i++) {
        Node *pumpkinNode = sample2D_->CreateObject(info, 0.0f, Vector3(Random(0.0f, 14.0f), 10.0f, 0.0f), 0.5f, 3);
        auto *obj_ = pumpkinNode->CreateComponent<Object2D>(); // Create a logic component to handle character behavior
        String name = String("Pumpkin-P") + String(i);
        obj_->GetNode()->SetName(name.CString());
        obj_->id_ = i;
        obj_->type_ = 3;
    }


    // Create player character
//    Node* modelNode = sample2D_->CreateCharacter(info, 0.0f, Vector3(2.5f, 16.0f, 0.0f), 1.0f, 1);
    Node *modelNode = sample2D_->CreateCharacter(info, 0.0f, Vector3(2.5f, 2.0f, 0.0f), 0.1f, 1);
    player_ = modelNode->CreateComponent<Character2D>(); // Create a logic component to handle character behavior
    player_->GetNode()->SetName("Bear-P1");
    player_->isAI_ = false;
    player_->life_ = 100;
    player_->id_ = 0;
    player_->type_ = 1;

    for (int i = 0; i < EvolutionManager::getInstance()->getAgents().size(); i++) {

        // Create AI player character
        modelNode = sample2D_->CreateCharacter(info, 0.0f, Vector3(3.5f + Random(-2.0f, 2.0f), 16.0f, 0.0f), 0.1f, 2);
        agents_[i] = modelNode->CreateComponent<Character2D>(); // Create a logic component to handle character behavior
        agents_[i]->agentIndex = i;
        String name = String("AI-Bear-P") + String(i);
        agents_[i]->GetNode()->SetName(name.CString());
        agents_[i]->isAI_ = true;
        agents_[i]->playerPos_ = player_->GetNode()->GetPosition();
        agents_[i]->id_ = 1 + i;
        agents_[i]->type_ = 2;

        agents_[i]->doMove_ = false;
        agents_[i]->chooseMove_ = false;
        agents_[i]->lastMove_ = agents_[i]->currMove_ = 0;

        agents_[i]->genotypeNode_ = scene_->CreateChild("Genotype " + i);
        agents_[i]->powerbarNode_ = scene_->CreateChild("Powerbar " + i);

        // Get AI position
        Vector3 aiPos = agents_[i]->GetNode()->GetPosition();

        // Powerbar
        // Create billboard sets (powerbars)
        //const unsigned NUM_BILLBOARDNODES = 10;//NUM_AI;


        // A single billboard for each parameter of genotype
        const unsigned NUM_BILLBOARDS = EvolutionManager::getInstance()->getAgents()[0]->genotype->getParameterCount();

        // BillboardSet* billboardObject;

        /*

                Node* pbNode = scene_->CreateChild("PowerBar");
//        smokeNode->SetPosition(Vector3(Random(200.0f) - 100.0f, Random(20.0f) + 10.0f, Random(200.0f) - 100.0f));
        //pbNode->SetPosition(Vector3(3.5f+Random(-2.0f,2.0f), 20.0f, 0.0f));
        pbNode->SetPosition(Vector3(-0.02f, 0.25f, 0.0f));
//        pbNode->SetScale(Vector3(0.5f,0.5f,0.5f));
        auto* billboardObject = pbNode->CreateComponent<BillboardSet>();
        billboardObject->SetNumBillboards(NUM_BILLBOARDS);
        billboardObject->SetMaterial(cache->GetResource<Material>("Materials/PowerBar.xml"));
        billboardObject->SetSorted(true);

        for (unsigned j = 0; j < NUM_BILLBOARDS; ++j)
        {
            Billboard* bb = billboardObject->GetBillboard(j);
//            bb->position_ = Vector3(Random(12.0f) - 6.0f, Random(8.0f) - 4.0f, -5.0f);
            bb->position_ = Vector3(aiPos.x_, aiPos.y_, 0.0f);
            bb->size_ = Vector2((256.0f/512.0f)*0.06f, (256.0f/144.0f)*0.06f);
            bb->rotation_ = 90.0f; //Random() * 360.0f;
            bb->enabled_ = true;

//            bb->uv_ = Rect(left,top,right,bottom);
            bb->uv_ = Rect(0.0,0.5,1.0,1.0);
            
            // After modifying the billboards, they need to be "committed" so that the BillboardSet updates its internals
            billboardObject->Commit();
        }
        */

        // Genotype

        //Node* gtNode = scene_->CreateChild("Genotype");
        //pbNode->SetPosition(Vector3(3.5f+Random(-2.0f,2.0f), 20.0f, 0.0f));

        agents_[i]->genotypeNode_->SetPosition(Vector3(-0.24f, 0.25f, 0.0f));
//        pbNode->SetScale(Vector3(0.5f,0.5f,0.5f));
        agents_[i]->genotypeBBSet_ = agents_[i]->genotypeNode_->CreateComponent<BillboardSet>();
        agents_[i]->genotypeBBSet_->SetNumBillboards(NUM_BILLBOARDS);
        agents_[i]->genotypeBBSet_->SetMaterial(cache->GetResource<Material>("Materials/Genotype.xml"));
        agents_[i]->genotypeBBSet_->SetSorted(true);

        // Draw billboard for each genotype parameter -> alpha based on value

        for (unsigned j = 0; j < NUM_BILLBOARDS; ++j) {
            Billboard *bb = agents_[i]->genotypeBBSet_->GetBillboard(j);

            // Get genotype parameters
            for (int g = 0; g < EvolutionManager::getInstance()->getAgents()[i]->genotype->getParameterCount(); g++) {
                float parameter = EvolutionManager::getInstance()->getAgents()[i]->genotype->getParameter(g);

                // Set alpha based on parameter
               //bb->color_ = Urho3D::Color(parameter, parameter, parameter);

                //URHO3D_LOGINFOF("Agent [%d]: genotype parameter[%d] -> %f", i, g, parameter);

                //float fx = 0.5;
                //	float fy = fx / (m_i_width / m_i_height);
                //	m_bb->size_ = Vector2(fx, fy);

                // Diminish height of genotype billboard by parameter value
                bb->size_ = Vector2((1.0f) * 0.05f * parameter, (0.1f) * 0.05f);
            }

//            bb->size_ = Vector2((256.0f/8.0f)*0.06f, (256.0f/144.0f)*0.06f);
        //    bb->size_ = Vector2((1.0f) * 0.05f, (0.1f) * 0.05f);

            //float fx = 0.5;
            //	float fy = fx / (m_i_width / m_i_height);
            //	m_bb->size_ = Vector2(fx, fy);

            bb->rotation_ = 90.0f; //Random() * 360.0f;
            bb->enabled_ = true;

//            bb->uv_ = Rect(left,top,right,bottom);
            bb->uv_ = Rect(0.0, 0.0, 1.0, 1.0);

            // After modifying the billboards, they need to be "committed" so that the BillboardSet updates its internals
            agents_[i]->genotypeBBSet_->Commit();
        }

        // Powerbar

        agents_[i]->powerbarNode_->SetPosition(Vector3(0.0, 0.25f, 0.0f));
        agents_[i]->powerbarBBSet_ = agents_[i]->powerbarNode_->CreateComponent<BillboardSet>();
        agents_[i]->powerbarBBSet_->SetNumBillboards(1);
        agents_[i]->powerbarBBSet_->SetMaterial(cache->GetResource<Material>("Materials/PowerBar.xml"));
        agents_[i]->powerbarBBSet_->SetSorted(true);

        // Draw billboard for each genotype parameter -> alpha based on value

        // Single billboard for power meter
        for (unsigned j = 0; j < 1; ++j) {
            Billboard *bb = agents_[i]->powerbarBBSet_->GetBillboard(j);

//            bb->size_ = Vector2((256.0f/8.0f)*0.06f, (256.0f/144.0f)*0.06f);
            bb->size_ = Vector2((1.0f) * 0.05f, (0.1f) * 0.05f);

            bb->rotation_ = 90.0f; //Random() * 360.0f;
            bb->enabled_ = true;

//            bb->uv_ = Rect(left,top,right,bottom);
            bb->uv_ = Rect(0.0, 0.0, 1.0, 1.0);

            // After modifying the billboards, they need to be "committed" so that the BillboardSet updates its internals
            agents_[i]->powerbarBBSet_->Commit();
        }
    }

// Generate physics collision shapes from the tmx file's objects located in "Physics" (top) layer
    TileMapLayer3D *tileMapLayer = tileMap->GetLayer(tileMap->GetNumLayers() - 1);
    sample2D_->CreateCollisionShapesFromTMXObjects(tileMapNode, tileMapLayer, info);

    // Create a directional light to the world so that we can see something. The light scene node's orientation controls the
    // light direction; we will use the SetDirection() function which calculates the orientation from a forward direction vector.
    // The light will use default settings (white light, no shadows)
    Node *lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetPosition(Vector3(1.0f, 8.0f, 0.0f));
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f)); // The direction vector does not need to be normalized
    auto *light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    // Set an initial position for the camera scene node above the plane
    //   mushroomNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
    // mushroomNode->SetScale(0.5f + Random(2.0f));
    // Instantiate enemies and moving platforms at each placeholder of "MovingEntities" layer (placeholders are Poly Line objects defining a path from points)
    //sample2D_->PopulateMovingEntities(tileMap->GetLayer(tileMap->GetNumLayers() - 2));

    // Instantiate coins to pick at each placeholder of "Coins" layer (placeholders for coins are Rectangle objects)
    //TileMapLayer3D* coinsLayer = tileMap->GetLayer(tileMap->GetNumLayers() - 3);
    //sample2D_->PopulateCoins(coinsLayer);

    // Init coins counters
    //player_->remainingCoins_ = coinsLayer->GetNumObjects();
    //player_->maxCoins_ = coinsLayer->GetNumObjects();

    //Instantiate triggers (for ropes, ladders, lava, slopes...) at each placeholder of "Triggers" layer (placeholders for triggers are Rectangle objects)
    //sample2D_->PopulateTriggers(tileMap->GetLayer(tileMap->GetNumLayers() - 4));

    // Create background
    sample2D_->CreateBackgroundSprite(info, 6.0, "Textures/HeightMap.png", true);



    // Check when scene is rendered
    SubscribeToEvent(E_ENDRENDERING, URHO3D_HANDLER(MayaSpace, HandleSceneRendered));
}

void MayaSpace::HandleSceneRendered(StringHash eventType, VariantMap &eventData) {
    UnsubscribeFromEvent(E_ENDRENDERING);
    // Save the scene so we can reload it later
    sample2D_->SaveScene(true);
    // Pause the scene as long as the UI is hiding it
    scene_->SetUpdateEnabled(false);
}

void MayaSpace::SubscribeToEvents() {
    // Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MayaSpace, HandleUpdate));

    // Subscribe HandlePostUpdate() function for processing post update events
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(MayaSpace, HandlePostUpdate));

    // Subscribe to PostRenderUpdate to draw debug geometry
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MayaSpace, HandlePostRenderUpdate));

    // Subscribe to Box2D contact listeners
    SubscribeToEvent(E_PHYSICSBEGINCONTACT2D, URHO3D_HANDLER(MayaSpace, HandleCollisionBegin));
    SubscribeToEvent(E_PHYSICSENDCONTACT2D, URHO3D_HANDLER(MayaSpace, HandleCollisionEnd));

    // If the node pointer is non-null, this component has been created into a scene node. Subscribe to physics collisions that
    // concern this scene node
//    SubscribeToEvent(E_NODEUPDATECONTACT2D, URHO3D_HANDLER(MayaSpace, HandleNodeCollision));

    // Unsubscribe the SceneUpdate event from base class to prevent camera pitch and yaw in 2D sample
    UnsubscribeFromEvent(E_SCENEUPDATE);
}

/*
void MayaSpace::HandleNodeCollision(StringHash eventType, VariantMap& eventData) {

    using namespace PhysicsBeginContact2D;

    Node* p1Node = scene_->GetChild("Bear-P1", true);
    Node* p2Node = scene_->GetChild("Bear-P2", true);

    // Get colliding node
    auto* hitNodeA = static_cast<Node*>(eventData[PhysicsBeginContact2D::P_NODEA].GetPtr());
    auto* hitNodeB = static_cast<Node*>(eventData[PhysicsBeginContact2D::P_NODEB].GetPtr());

    URHO3D_LOGINFOF("hitNodeA=%d, hitNodeB=%d", hitNodeA, hitNodeB);
    URHO3D_LOGINFOF("hitNodeA id=%d, hitNodeB id=%d", hitNodeA->GetID(), hitNodeB->GetName());
        Vector2 contactPosition; 

        MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());
        while (!contacts.IsEof()) {
            contactPosition = contacts.ReadVector2();
            auto contactNormal = contacts.ReadVector2();
            auto contactDistance = contacts.ReadFloat();
            auto contactImpulse = contacts.ReadFloat();
       //     std::cout << "contact position " << contactPosition.ToString().CString() << std::endl;
       //     std::cout << "contact normal " << contactNormal.ToString().CString() << std::endl;
       //     std::cout << "contact distance " << contactDistance << std::endl;
       //     std::cout << "contact impulse " << contactImpulse << std::endl;
        }

        std::cout << std::endl;

}*/

void MayaSpace::HandleCollisionBegin(StringHash eventType, VariantMap &eventData) {
    // Get colliding node
    auto *hitNode = static_cast<Node *>(eventData[PhysicsBeginContact2D::P_NODEA].GetPtr());
    if (hitNode->GetName() == "Bear-P1")
        hitNode = static_cast<Node *>(eventData[PhysicsBeginContact2D::P_NODEB].GetPtr());
    String nodeName = hitNode->GetName();
    Node *character2DNode = scene_->GetChild("Bear-P1", true);

//Pumpkin-P
    Node *p1Node = scene_->GetChild("Bear-P1", true);
    Node *p2Node = scene_->GetChild("Bear-P2", true);

    // Get colliding node
    auto *hitNodeA = static_cast<Node *>(eventData[PhysicsBeginContact2D::P_NODEA].GetPtr());
    auto *hitNodeB = static_cast<Node *>(eventData[PhysicsBeginContact2D::P_NODEB].GetPtr());

    // Skip tile map collisions
    if (!hitNodeA->GetName().Contains("TileMap")) {


        //hitNodeA id=Pumpkin-P4, hitNodeB id=Bear-P1
/*
        URHO3D_LOGINFOF("hitNodeA=%d, hitNodeB=%d", hitNodeA, hitNodeB);
        URHO3D_LOGINFOF("hitNodeA id=%s, hitNodeB id=%s", hitNodeA->GetName().CString(), hitNodeB->GetName().CString());*/

        Vector2 contactPosition;
        //
        MemoryBuffer contacts(eventData[PhysicsBeginContact2D::P_CONTACTS].GetBuffer());
        while (!contacts.IsEof()) {
            contactPosition = contacts.ReadVector2();
            auto contactNormal = contacts.ReadVector2();
            auto contactDistance = contacts.ReadFloat();
            auto contactImpulse = contacts.ReadFloat();
            //     std::cout << "contact position " << contactPosition.ToString().CString() << std::endl;
            //     std::cout << "contact normal " << contactNormal.ToString().CString() << std::endl;
            //     std::cout << "contact distance " << contactDistance << std::endl;
            //     std::cout << "contact impulse " << contactImpulse << std::endl;
        }

        std::cout << std::endl;

        String a1, a2, b1, b2;

        a1 = "Pumpkin";
        b1 = "Bear-P1";
        a2 = "Bear-P1";
        b2 = "Pumpkin";

        bool hit1 = hitNodeA->GetName().Contains(a1) && hitNodeB->GetName().Contains(b1);
        bool hit2 = hitNodeA->GetName().Contains(a2) && hitNodeB->GetName().Contains(b2);

        if (hit1 || hit2) {
            // Bear P1 collides with pumpkin
            //URHO3D_LOGINFOF("hit1=%d, hit2=%d", hit1, hit2);

            if (hit1) {
                String nodeName = hitNode->GetName();
                Node *pumpkinNode = scene_->GetChild(nodeName, true);
                player_->life_ += 20;
                if (pumpkinNode) {
                    pumpkinNode->Remove();
                }

                // Take the frame time step, which is stored as a float
                using namespace Update;
                float timeStep = eventData[P_TIMESTEP].GetFloat();
                SetParticleEmitter(hitNode->GetID(), contactPosition.x_, contactPosition.y_, 1, timeStep);
                sample2D_->PlaySoundEffect("Powerup.wav");


            }
        }


        a1 = "Bear-P";
        b1 = "Bear-P1";
        a2 = "Bear-P1";
        b2 = "Bear-P";

        bool hit3 = hitNodeA->GetName().Contains(a1) && hitNodeB->GetName().Contains(b1);
        bool hit4 = hitNodeA->GetName().Contains(a2) && hitNodeB->GetName().Contains(b2);

        if ((hit3 || hit4) && player_->isReady_) {

            // If hit node is an id more than the player, it's AI
//        if (hitNode->GetID() > character2DNode->GetID() && player_->isReady_) {
            player_->life_ -= 10;
            auto *body = character2DNode->GetComponent<RigidBody2D>();
            auto *body2 = hitNode->GetComponent<RigidBody2D>();

            //        Vector2 v = body->GetLinearVelocity()*-10.0f;
            //        float a = body->GetAngularVelocity();

            // Clear forces (should be performed by setting linear velocity to zero, but currently doesn't work)
            body->SetLinearVelocity(Vector2::ZERO);
            body->SetAwake(false);
            body->SetAwake(true);

            body2->SetLinearVelocity(Vector2::ZERO);
            body2->SetAwake(false);
            body2->SetAwake(true);

            //URHO3D_LOGINFOF("PLAYER HIT by=%d", hitNode->GetID());

            // Take the frame time step, which is stored as a float
            using namespace Update;
            float timeStep = eventData[P_TIMESTEP].GetFloat();
            SetParticleEmitter(hitNode->GetID(), contactPosition.x_, contactPosition.y_, 0, timeStep);
            sample2D_->PlaySoundEffect("explosion-sm.wav");
            sample2D_->PlaySoundEffect("bam-motherfucker.wav");


        }
    }


    // Handle ropes and ladders climbing
    if (nodeName == "Climb") {
        if (player_->isClimbing_) // If transition between rope and top of rope (as we are using split triggers)
            player_->climb2_ = true;
        else {
            player_->isClimbing_ = true;
            auto *body = character2DNode->GetComponent<RigidBody2D>();
            body->SetGravityScale(0.0f); // Override gravity so that the character doesn't fall
            // Clear forces so that the character stops (should be performed by setting linear velocity to zero, but currently doesn't work)
            body->SetLinearVelocity(Vector2(0.0f, 0.0f));
            body->SetAwake(false);
            body->SetAwake(true);
        }
    }

    //URHO3D_LOGINFOF("HIT id=%d, name=%s", hitNode->GetID(), hitNode->GetName());
    //URHO3D_LOGINFOF("character2DNode=%d", character2DNode->GetID());


    auto *body = character2DNode->GetComponent<RigidBody2D>();
    // body->SetGravityScale(0.0f); // Override gravity so that the character doesn't fall
    // Clear forces so that the character stops (should be performed by setting linear velocity to zero, but currently doesn't work)
//            body->SetLinearVelocity(body->GetLinearVelocity()*-1.0f);
//            body->SetAwake(false);
//            body->SetAwake(true);


    if (nodeName == "hit-body") {
        URHO3D_LOGINFOF("HIT BODY=%s", hitNode->GetName());
    }

    if (nodeName == "CanJump")
        player_->aboveClimbable_ = true;

    // Handle coins picking
    if (nodeName == "Coin") {
        hitNode->Remove();
        player_->remainingCoins_ -= 1;
        auto *ui = GetSubsystem<UI>();
        if (player_->remainingCoins_ == 0) {
            Text *instructions = static_cast<Text *>(ui->GetRoot()->GetChild("Instructions", true));
            instructions->SetText("!!! Go to the Exit !!!");
        }
        Text *coinsText = static_cast<Text *>(ui->GetRoot()->GetChild("CoinsText", true));
        coinsText->SetText(String(player_->remainingCoins_)); // Update coins UI counter
        sample2D_->PlaySoundEffect("Powerup.wav");
    }

/*
    // Handle interactions with enemies
    if (nodeName == "Enemy" || nodeName == "Orc")
    {
        auto* animatedSprite = character2DNode->GetComponent<AnimatedSprite2D>();
        float deltaX = character2DNode->GetPosition().x_ - hitNode->GetPosition().x_;

        // Orc killed if character is fighting in its direction when the contact occurs (flowers are not destroyable)
        if (nodeName == "Orc" && animatedSprite->GetAnimation() == "attack" && (deltaX < 0 == animatedSprite->GetFlipX()))
        {
            static_cast<Mover*>(hitNode->GetComponent<Mover>())->emitTime_ = 1;
            if (!hitNode->GetChild("Emitter", true))
            {
                hitNode->GetComponent("RigidBody2D")->Remove(); // Remove Orc's body
                sample2D_->SpawnEffect(hitNode);
                sample2D_->PlaySoundEffect("BigExplosion.wav");
            }
        }
        // Player killed if not fighting in the direction of the Orc when the contact occurs, or when colliding with a flower
        else
        {
            if (!character2DNode->GetChild("Emitter", true))
            {
                player_->wounded_ = true;
                if (nodeName == "Orc")
                {
                    auto* orc = static_cast<Mover*>(hitNode->GetComponent<Mover>());
                    orc->fightTimer_ = 1;
                }
                sample2D_->SpawnEffect(character2DNode);
                sample2D_->PlaySoundEffect("BigExplosion.wav");
            }
        }
    }*/

    // Handle exiting the level when all coins have been gathered
    if (nodeName == "Exit" && player_->remainingCoins_ == 0) {
        // Update UI
        auto *ui = GetSubsystem<UI>();
        Text *instructions = static_cast<Text *>(ui->GetRoot()->GetChild("Instructions", true));
        instructions->SetText("!!! WELL DONE !!!");
        instructions->SetPosition(IntVector2(0, 0));
        // Put the character outside of the scene and magnify him
        character2DNode->SetPosition(Vector3(-20.0f, 0.0f, 0.0f));
        character2DNode->SetScale(1.5f);
    }

    // Handle falling into lava
    if (nodeName == "Lava") {
        auto *body = character2DNode->GetComponent<RigidBody2D>();
        body->ApplyForceToCenter(Vector2(0.0f, 1000.0f), true);
        if (!character2DNode->GetChild("Emitter", true)) {
            player_->wounded_ = true;
            sample2D_->SpawnEffect(character2DNode);
            sample2D_->PlaySoundEffect("BigExplosion.wav");
        }
    }

    // Handle climbing a slope
    if (nodeName == "Slope")
        player_->onSlope_ = true;
}


void MayaSpace::SetParticleEmitter(int hitId, float contactX, float contactY, int type, float timeStep) {
    // CREATE
    auto *cache = GetSubsystem<ResourceCache>();
    ParticleEffect2D *particleEffect;
    Vector2 position;

    switch (type) {
        case 0:
            particleEffect = cache->GetResource<ParticleEffect2D>("Urho2D/sun.pex");
            position.x_ = contactX;
            position.y_ = contactY;
            break;
        case 1:
            particleEffect = cache->GetResource<ParticleEffect2D>("Urho2D/power.pex");
            position.x_ = contactX;
            position.y_ = contactY;
            break;
    }

    if (!particleEffect)
        return;

    for (int i = 0; i < sizeof(particlePool_) / sizeof(*particlePool_); i++) {
        if (!particlePool_[i].used) {
            particlePool_[i].used = true;
            particlePool_[i].usedBy = hitId;
            particlePool_[i].node = scene_->CreateChild("GreenSpiral");
            auto *particleEmitter = particlePool_[i].node->CreateComponent<ParticleEmitter2D>();
            particleEmitter->SetEffect(particleEffect);
            particlePool_[i].node->SetPosition(Vector3(position.x_, position.y_, 0.0));
            particlePool_[i].lastEmit = timeStep;
            particlePool_[i].currEmit = 0;
            particlePool_[i].timeout = 0.8f;

            break;
        }
    }

    URHO3D_LOGINFOF("PARTICLE EMITTER CREATED used by id=%d", hitId);
}

void MayaSpace::HandleUpdateParticlePool(float timeStep) {
    // CREATE
    auto *cache = GetSubsystem<ResourceCache>();
/*    auto* particleEffect = cache->GetResource<ParticleEffect2D>("Urho2D/sun.pex");
    if (!particleEffect)
        return;
*/
    for (int i = 0; i < sizeof(particlePool_) / sizeof(*particlePool_); i++) {
        if (particlePool_[i].used) {

            particlePool_[i].currEmit += timeStep;

            if (particlePool_[i].currEmit - particlePool_[i].lastEmit > particlePool_[i].timeout) {
                if (particlePool_[i].node) {
                    particlePool_[i].node->Remove();
                    particlePool_[i].used = false;
                    particlePool_[i].usedBy = -1;
                }
            }
        }
    }
}


void MayaSpace::HandleCollisionEnd(StringHash eventType, VariantMap &eventData) {
    // Get colliding node
    auto *hitNode = static_cast<Node *>(eventData[PhysicsEndContact2D::P_NODEA].GetPtr());
    if (hitNode->GetName() == "Bear-P1")
        hitNode = static_cast<Node *>(eventData[PhysicsEndContact2D::P_NODEB].GetPtr());
    String nodeName = hitNode->GetName();
    Node *character2DNode = scene_->GetChild("Bear-P1", true);

    // Handle leaving a rope or ladder
    if (nodeName == "Climb") {
        if (player_->climb2_)
            player_->climb2_ = false;
        else {
            player_->isClimbing_ = false;
            auto *body = character2DNode->GetComponent<RigidBody2D>();
            body->SetGravityScale(1.0f); // Restore gravity
        }
    }

    if (nodeName == "CanJump")
        player_->aboveClimbable_ = false;

    // Handle leaving a slope
    if (nodeName == "Slope") {
        player_->onSlope_ = false;
        // Clear forces (should be performed by setting linear velocity to zero, but currently doesn't work)
        auto *body = character2DNode->GetComponent<RigidBody2D>();
        body->SetLinearVelocity(Vector2::ZERO);
        body->SetAwake(false);
        body->SetAwake(true);
    }
}

void MayaSpace::HandleUpdate(StringHash eventType, VariantMap &eventData) {
    using namespace Update;
    auto *input = GetSubsystem<Input>();

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();
    HandleUpdateParticlePool(timeStep);

    float zoom_ = cameraNode_->GetComponent<Camera>()->GetZoom();
    float deltaSum;

    // Determine zoom by getting average distance from all players
    for (int i = 0; i < EvolutionManager::getInstance()->getAgents().size(); i++) {

        // Update player location for AI
        agents_[i]->playerPos_ = player_->GetNode()->GetPosition();

        Vector3 p1 = player_->GetNode()->GetPosition();
        p1.z_ = 0;
        Vector3 p2 = agents_[i]->GetNode()->GetPosition();
        p2.z_ = 0;
        float delta = p1.DistanceToPoint(p2);
        deltaSum += delta;
    }

    float avgDelta = ((float) deltaSum) / ((float) EvolutionManager::getInstance()->getAgents().size());
    float factor;

    if (avgDelta > 5.0f) {
        factor = 1.0f - avgDelta * 0.02f;
    } else {
        factor = 1.0f + avgDelta * 0.02f;
    }

    factor = 1.0f;

    zoom_ = Clamp(zoom_ * factor, CAMERA_MIN_DIST, CAMERA_MAX_DIST);
    cameraNode_->GetComponent<Camera>()->SetZoom(zoom_);

    //    URHO3D_LOGINFOF("delta=%f", delta);
    //    URHO3D_LOGINFOF("factor=%f", factor);


    // Zoom in/out
    if (cameraNode_)
        sample2D_->Zoom(cameraNode_->GetComponent<Camera>());

    // Toggle debug geometry with 'Z' key
    if (input->GetKeyPress(KEY_Z))
        drawDebug_ = !drawDebug_;


    // Check for loading / saving the scene
    if (input->GetKeyPress(KEY_F5))
        sample2D_->SaveScene(false);
    if (input->GetKeyPress(KEY_F7))
        ReloadScene(false);

    GameController *gameController = GetSubsystem<GameController>();

    if (player_) {
        gameController->UpdateControlInputs(player_->controls_);

        // **note** the buttons controls are handled in the character class update fn.

        // right stick - camera
        Variant rStick = player_->controls_.extraData_[VAR_AXIS_1];

        if (!rStick.IsEmpty()) {
            Vector2 axisInput = rStick.GetVector2();
            player_->controls_.yaw_ += axisInput.x_ * YAW_SENSITIVITY;
            player_->controls_.pitch_ += axisInput.y_ * YAW_SENSITIVITY;
        }

        // Limit pitch
        // player_->controls_.pitch_ = Clamp(player_->controls_.pitch_, -80.0f, 80.0f);
        // Set rotation already here so that it's updated every rendering frame instead of every physics frame
        //   player_->GetNode()->SetRotation(Quaternion(player_->controls_.yaw_, Vector3::UP));
        //  player_->GetNode()->SetRotation(Quaternion(0.0f, -180.0f-player_->heading_, 0.0f));

        player_->GetNode()->SetRotation(Quaternion(0.0f, player_->heading_, 0.0));
        auto *model_ = player_->GetNode()->GetComponent<AnimatedModel>(true);

        Skeleton &skeleton = model_->GetSkeleton();
        Bone *rootBone = skeleton.GetRootBone();
        Bone *startBone = rootBone;


    }

    // Clamp player life
    if (player_->life_ > 100) {
        player_->life_ = 100;
    }
    if (player_->life_ < 0) {
        player_->life_ = 0;
    }

    // AI
    if (agents_) {
        for (int i = 0; i < EvolutionManager::getInstance()->getAgents().size(); i++) {
            // Set rotation already here so that it's updated every rendering frame instead of every physics frame
            agents_[i]->GetNode()->SetRotation(Quaternion(agents_[i]->controls_.yaw_, Vector3::UP));
            //ai_[i]->GetNode()->SetRotation(Quaternion(0.0f, -180.0f-ai_[i]->heading_, 0.0f));
            //ai_[i]->GetNode()->SetRotation(Quaternion(-90.0f, ai_[i]->heading_+180.0f, 0.0f));
            agents_[i]->GetNode()->SetRotation(Quaternion(0.0f, agents_[i]->heading_, 0.0));

/*
        static int _sndCnt = 0;
        float r = Random(-0.0f,5.0f);
        if (r > 2.5f) {
            _sndCnt++;
        }

        if (_sndCnt > 5) {
            sample2D_->PlaySoundEffect("enemy01-laugh.wav");
        }*/

            // Update billboards (genotype, powerbar)

            const float BILLBOARD_ROTATION_SPEED = 50.0f;

            // Genotype

            // Rotate the individual billboards within the billboard sets, then recommit to make the changes visible
            for (unsigned j = 0; j < agents_[i]->genotypeBBSet_->GetNumBillboards(); ++j) {
                Billboard *bb = agents_[i]->genotypeBBSet_->GetBillboard(j);
                //  bb->rotation_ += BILLBOARD_ROTATION_SPEED * timeStep;
                if (agents_[i]) {
                    Vector3 aiPos = agents_[i]->GetNode()->GetPosition();
                    bb->position_ = Vector3(aiPos.x_ + (j * 0.02), aiPos.y_, 0.0f);

                    // Already set size in initialization (based on parameter)
                    //bb->size_ = Vector2((1.0f) * 0.05f, (0.1f) * 0.05f);


                    //       bb->position_ = Vector3(player_->GetNode()->GetPosition().x_, player_->GetNode()->GetPosition().y_, -5.0f);
                }

            }

            agents_[i]->genotypeBBSet_->Commit();


            // Powerbar

            // Rotate the individual billboards within the billboard sets, then recommit to make the changes visible
            for (unsigned j = 0; j < agents_[i]->powerbarBBSet_->GetNumBillboards(); ++j) {
                Billboard *bb = agents_[i]->powerbarBBSet_->GetBillboard(j);
                //  bb->rotation_ += BILLBOARD_ROTATION_SPEED * timeStep;
                if (agents_[i]) {
                    Vector3 aiPos = agents_[i]->GetNode()->GetPosition();
                    bb->position_ = Vector3(aiPos.x_ + (j * 0.02), aiPos.y_+0.2f, 0.0f);

                    bb->size_ = Vector2((0.4f) * 0.05f, (4.0f) * 0.05f);

                    //       bb->position_ = Vector3(player_->GetNode()->GetPosition().x_, player_->GetNode()->GetPosition().y_, -5.0f);
                }

            }

            agents_[i]->powerbarBBSet_->Commit();

        }
    }


    // Update player powerbar
    IntVector2 v = powerbarBkgP1Sprite_->GetSize();
    int power = int(((player_->life_) / 100.0f) * (float) v.x_);
    powerbarP1Sprite_->SetSize(power, v.y_);

    char str[40];

    Vector3 pos = player_->GetNode()->GetPosition();
    sprintf(str, "%f,%f,%f", pos.x_, pos.y_, pos.z_);
    int i = 0;
    std::string playerInfo;
    playerInfo.clear();
    playerInfo.append("Player position (x,y,z) -> ").append(str);
    debugText_[i]->SetText(playerInfo.c_str());

    i++;
    Vector3 vel = player_->currState_.moveDir;
    sprintf(str, "%f,%f,%f", vel.x_, vel.y_, vel.z_);

    playerInfo.clear();
    playerInfo.append("Player velocity (x,y,z) -> ").append(str);
    debugText_[i]->SetText(playerInfo.c_str());

    i++;
//    Vector3 vel = player_
    sprintf(str, "%d,%d,%d", player_->currState_.onGround, player_->currState_.jump, player_->currState_.kick);

    playerInfo.clear();
    playerInfo.append("Player state (onGround,jump,kick) -> ").append(str);
    debugText_[i]->SetText(playerInfo.c_str());


    i++;
    playerInfo.clear();
    sprintf(str, "%d", EvolutionManager::getInstance()->getGenerationCount());
    playerInfo.append("Evolution Manager: Generation -> ").append(str);
    debugText_[i]->SetText(playerInfo.c_str());


    i++;
    playerInfo.clear();
    sprintf(str, "%d", EvolutionManager::getInstance()->populationSize);
    playerInfo.append("Evolution Manager: populationSize -> ").append(str);
    debugText_[i]->SetText(playerInfo.c_str());

    i++;
    playerInfo.clear();
    sprintf(str, "%f", EvolutionManager::getInstance()->getAgents()[0]->genotype->evaluation);
    playerInfo.append("Evolution Manager: agent[0]->genotype->evaluation -> ").append(str);
    debugText_[i]->SetText(playerInfo.c_str());

    i++;
    playerInfo.clear();
    sprintf(str, "%.2f, %.2f, %.2f", EvolutionManager::getInstance()->getAgents()[0]->getPosition().x_,
            EvolutionManager::getInstance()->getAgents()[0]->getPosition().y_,
            EvolutionManager::getInstance()->getAgents()[0]->getPosition().z_);
    playerInfo.append("Evolution Manager: agent[0]->position -> ").append(str);
    debugText_[i]->SetText(playerInfo.c_str());


    i++;
    playerInfo.clear();
    sprintf(str, "%d", EvolutionManager::getInstance()->agentsAliveCount);
    playerInfo.append("Evolution Manager: agentsAliveCount -> ").append(str);
    debugText_[i]->SetText(playerInfo.c_str());


    //URHO3D_LOGINFOF("player_ position x=%f, y=%f, z=%f", player_->GetNode()->GetPosition().x_, player_->GetNode()->GetPosition().y_, player_->GetNode()->GetPosition().z_);

}

void MayaSpace::HandlePostUpdate(StringHash eventType, VariantMap &eventData) {
    if (!player_)
        return;

    Node *character2DNode = player_->GetNode();
    cameraNode_->SetPosition(Vector3(character2DNode->GetPosition().x_, character2DNode->GetPosition().y_,
                                     -10.0f)); // Camera tracks character
}

void MayaSpace::HandlePostRenderUpdate(StringHash eventType, VariantMap &eventData) {
    // Mesh and bones do not match -> bones are too big
    // Scale down bone
//    player_->
    auto *model_ = player_->GetNode()->GetComponent<AnimatedModel>(true);
    //node_->GetComponent<AnimatedModel>(true);


    //  if (!model_ || !animation_)
    //      return;

    Skeleton &skeleton = model_->GetSkeleton();
    Bone *rootBone = skeleton.GetRootBone();
    Bone *startBone = rootBone;

    if (!rootBone)
        return;

//    startBone->initialScale_(Vector3(0.01f, 0.01f, 0.01f));

    if (drawDebug_) {
        auto *physicsWorld = scene_->GetComponent<PhysicsWorld2D>();
        physicsWorld->DrawDebugGeometry();

        Node *tileMapNode = scene_->GetChild("TileMap", true);
        auto *map = tileMapNode->GetComponent<TileMap3D>();
        map->DrawDebugGeometry(scene_->GetComponent<DebugRenderer>(), false);

        // bones. Note that debug geometry has to be separately requested each frame. Disable depth test so that we can see the
        // bones properly
        GetSubsystem<Renderer>()->DrawDebugGeometry(false);
    }
}

void MayaSpace::ReloadScene(bool reInit) {
    String filename = sample2D_->demoFilename_;
    if (!reInit)
        filename += "InGame";

    File loadFile(context_, GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/" + filename + ".xml",
                  FILE_READ);
    scene_->LoadXML(loadFile);
    // After loading we have to reacquire the weak pointer to the Character2D component, as it has been recreated
    // Simply find the character's scene node by name as there's only one of them
    Node *character2DNode = scene_->GetChild("Bear-P1", true);
    if (character2DNode)
        player_ = character2DNode->GetComponent<Character2D>();

    // Set what number to use depending whether reload is requested from 'PLAY' button (reInit=true) or 'F7' key (reInit=false)
    int lifes = player_->remainingLifes_;
    int coins = player_->remainingCoins_;
    if (reInit) {
        lifes = LIFES;
        coins = player_->maxCoins_;
    }

    // Update lifes UI
    auto *ui = GetSubsystem<UI>();
    Text *lifeText = static_cast<Text *>(ui->GetRoot()->GetChild("LifeText", true));
    lifeText->SetText(String(lifes));

    // Update coins UI
    Text *coinsText = static_cast<Text *>(ui->GetRoot()->GetChild("CoinsText", true));
    coinsText->SetText(String(coins));
}

void MayaSpace::HandlePlayButton(StringHash eventType, VariantMap &eventData) {
//    sample2D_->PlaySoundEffect("enemy01-laugh.wav");
//    sample2D_->PlaySoundEffect("BAY-r1.wav");

    // Remove fullscreen UI and unfreeze the scene
    auto *ui = GetSubsystem<UI>();
    if (static_cast<Text *>(ui->GetRoot()->GetChild("FullUI", true))) {
        ui->GetRoot()->GetChild("FullUI", true)->Remove();
        scene_->SetUpdateEnabled(true);
    } else
        // Reload scene
        ReloadScene(true);

    // Hide Instructions and Play/Exit buttons
    Text *instructionText = static_cast<Text *>(ui->GetRoot()->GetChild("Instructions", true));
    instructionText->SetText("");
    Button *exitButton = static_cast<Button *>(ui->GetRoot()->GetChild("ExitButton", true));
    exitButton->SetVisible(false);
    Button *playButton = static_cast<Button *>(ui->GetRoot()->GetChild("PlayButton", true));
    playButton->SetVisible(false);

    // Hide mouse cursor
    auto *input = GetSubsystem<Input>();
    input->SetMouseVisible(false);

}
