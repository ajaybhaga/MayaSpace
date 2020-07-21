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

#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimationState.h>
#include <Urho3D/Urho2D/AnimatedSprite2D.h>
#include <Urho3D/Urho2D/AnimationSet2D.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Urho2D/CollisionBox2D.h>
#include <Urho3D/Urho2D/CollisionChain2D.h>
#include <Urho3D/Urho2D/CollisionCircle2D.h>
#include <Urho3D/Urho2D/CollisionPolygon2D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Urho2D/ParticleEffect2D.h>
#include <Urho3D/Urho2D/ParticleEmitter2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Urho2D/TileMap2D.h>
#include <Urho3D/Urho2D/TileMapLayer2D.h>
#include <Urho3D/Urho2D/TileMap3D.h>
#include <Urho3D/Urho2D/TileMapLayer3D.h>
#include <Urho3D/Urho2D/TmxFile2D.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Scene/ValueAnimation.h>
#include <Urho3D/UI/Window.h>


#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Resource/ResourceCache.h>

#include <string>
#include "Utilities2D/Mover.h"
#include "Sample2D.h"
#include "Character2D.h"

Sample2D::Sample2D(Context* context) :
    Object(context)
{
}

void Sample2D::CreateCollisionShapesFromTMXObjects(Node* tileMapNode, TileMapLayer3D* tileMapLayer, TileMapInfo2D info)
{
    // Create rigid body to the root node
    auto* body = tileMapNode->CreateComponent<RigidBody2D>();
    body->SetBodyType(BT_STATIC);

    // Generate physics collision shapes and rigid bodies from the tmx file's objects located in "Physics" layer
    for (int i = 0; i < tileMapLayer->GetNumObjects(); ++i)
    {
        TileMapObject2D* tileMapObject = tileMapLayer->GetObject(i); // Get physics objects

        // Create collision shape from tmx object
        switch (tileMapObject->GetObjectType())
        {
            case OT_RECTANGLE:
            {
                CreateRectangleShape(tileMapNode, tileMapObject, tileMapObject->GetSize(), info);
            }
            break;

            case OT_ELLIPSE:
            {
                CreateCircleShape(tileMapNode, tileMapObject, tileMapObject->GetSize().x_ / 2, info); // Ellipse is built as a Circle shape as it doesn't exist in Box2D
            }
            break;

            case OT_POLYGON:
            {
                CreatePolygonShape(tileMapNode, tileMapObject);
            }
            break;

            case OT_POLYLINE:
            {
                CreatePolyLineShape(tileMapNode, tileMapObject);
            }
            break;
        }
    }
}

CollisionBox2D* Sample2D::CreateRectangleShape(Node* node, TileMapObject2D* object, Vector2 size, TileMapInfo2D info)
{
    auto* shape = node->CreateComponent<CollisionBox2D>();
    shape->SetSize(size);
    if (info.orientation_ == O_ORTHOGONAL)
    {
          shape->SetCenter(object->GetPosition() + size / 2);
    }
    else
    {
        shape->SetCenter(object->GetPosition() + Vector2(info.tileWidth_ / 2, 0.0f));
        shape->SetAngle(45.0f); // If our tile map is isometric then shape is losange
    }
    shape->SetFriction(0.8f);
    if (object->HasProperty("Friction"))
        shape->SetFriction(ToFloat(object->GetProperty("Friction")));
    return shape;
}

CollisionCircle2D* Sample2D::CreateCircleShape(Node* node, TileMapObject2D* object, float radius, TileMapInfo2D info)
{
    auto* shape = node->CreateComponent<CollisionCircle2D>();
    Vector2 size = object->GetSize();
    if (info.orientation_ == O_ORTHOGONAL)
        shape->SetCenter(object->GetPosition() + size / 2);
    else
    {
        shape->SetCenter(object->GetPosition() + Vector2(info.tileWidth_ / 2, 0.0f));
    }

    shape->SetRadius(radius);
    shape->SetFriction(0.8f);
    if (object->HasProperty("Friction"))
        shape->SetFriction(ToFloat(object->GetProperty("Friction")));
    return shape;
}

CollisionPolygon2D* Sample2D::CreatePolygonShape(Node* node, TileMapObject2D* object)
{
    auto* shape = node->CreateComponent<CollisionPolygon2D>();
    int numVertices = object->GetNumPoints();
    shape->SetVertexCount(numVertices);
    for (int i = 0; i < numVertices; ++i)
        shape->SetVertex(i, object->GetPoint(i));
    shape->SetFriction(0.8f);
    if (object->HasProperty("Friction"))
        shape->SetFriction(ToFloat(object->GetProperty("Friction")));
    return shape;
}

CollisionChain2D* Sample2D::CreatePolyLineShape(Node* node, TileMapObject2D* object)
{
    auto* shape = node->CreateComponent<CollisionChain2D>();
    int numVertices = object->GetNumPoints();
    shape->SetVertexCount(numVertices);
    for (int i = 0; i < numVertices; ++i)
        shape->SetVertex(i, object->GetPoint(i));
    shape->SetFriction(0.8f);
    if (object->HasProperty("Friction"))
        shape->SetFriction(ToFloat(object->GetProperty("Friction")));
    return shape;
}

Node* Sample2D::CreateCharacter(TileMapInfo2D info, float friction, Vector3 position, float scale, int type)
{
    auto* cache = GetSubsystem<ResourceCache>();
    Node* modelNode = scene_->CreateChild("CharNode");

    modelNode->SetPosition(position);
    modelNode->SetScale(scale);

    // Create animated models
    const float MODEL_MOVE_SPEED = 2.0f;
    const float MODEL_ROTATE_SPEED = 100.0f;


    // rotate model by 180 ****************************
    Node* adjustNode = modelNode->CreateChild("AdjNode");
    Quaternion qAdjRot(90, Vector3(1,0,0) ); // rotate it by 180
    adjustNode->SetRotation( qAdjRot );

    // Create the rendering component + animation controller
    auto* modelObject = adjustNode->CreateComponent<AnimatedModel>();
    //auto* modelObject = modelNode->CreateComponent<AnimatedModel>();

    ///code/dev/MayaSpace/src/Urho3D/bin/Data/Models/spriteBase/Models/BodyFrame.002.mdl
        switch (type) {
            case 1:
            modelObject->SetModel(cache->GetResource<Model>("Models/spriteBase/Models/spriteBase01.mdl"));
            modelObject->SetMaterial(cache->GetResource<Material>("Material;Models/spriteBase/Materials/Material.011.xml;Models/spriteBase/Materials/Material.012.xml;Models/spriteBase/Materials/Material.019.xml"));

            break;
            case 2:
            modelObject->SetModel(cache->GetResource<Model>("Models/spriteBase/Models/spriteBase01.mdl"));
            //modelObject->SetMaterial(cache->GetResource<Material>("Models/bear2/Materials/Material.xml"));
            modelObject->SetMaterial(cache->GetResource<Material>("Material;Models/spriteBase/Materials/Material.011.xml;Models/spriteBase/Materials/Material.012.xml;Models/spriteBase/Materials/Material.019.xml"));

            break;
            case 3:
            modelObject->SetModel(cache->GetResource<Model>("Models/spriteBase/Models/spriteBase01.mdl"));
            //modelObject->SetMaterial(cache->GetResource<Material>("Models/bear3/Materials/Material.xml"));
            modelObject->SetMaterial(cache->GetResource<Material>("Material;Models/spriteBase/Materials/Material.011.xml;Models/spriteBase/Materials/Material.012.xml;Models/spriteBase/Materials/Material.019.xml"));

            break;
            case 4:
            modelObject->SetModel(cache->GetResource<Model>("Models/spriteBase/Models/spriteBase01.mdl"));
            //modelObject->SetMaterial(cache->GetResource<Material>("Models/bear4/Materials/Material.xml"));
            modelObject->SetMaterial(cache->GetResource<Material>("Material;Models/spriteBase/Materials/Material.011.xml;Models/spriteBase/Materials/Material.012.xml;Models/spriteBase/Materials/Material.019.xml"));
            break;
        }


        modelObject->SetCastShadows(true);


    auto* impBody = modelNode->CreateComponent<RigidBody2D>();
    //impBody->SetBodyType(BT_STATIC);
//    impBody->SetMassCenter()
    impBody->GetNode()->SetName("hit r");
    impBody->SetBodyType(BT_DYNAMIC);
    impBody->SetAllowSleep(false);
    URHO3D_LOGINFOF("CREATE BODY HIT id=%d, name=%s", impBody->GetNode()->GetID(), impBody->GetNode()->GetName());


    auto* shape = modelNode->CreateComponent<CollisionCircle2D>();
    /*
    shape = modelNode->CreateComponent<CollisionCircle2D>();
    shape->GetNode()->SetName("hit a");
    //shape->SetCenter(Vector2(position.x_, position.y_)); 
    shape->SetRadius(0.04f); // Set shape size
    shape->SetFriction(friction); // Set friction
    shape->SetRestitution(0.1f); // Bounce
    URHO3D_LOGINFOF("CREATE HIT id=%d, name=%s", shape->GetNode()->GetID(), shape->GetNode()->GetName());

    shape = modelNode->CreateComponent<CollisionCircle2D>();
    shape->GetNode()->SetName("hit b");
    shape->SetCenter(shape->GetCenter()+Vector2(0,-1.2f));
    shape->SetRadius(4.12f); // Set shape size
    shape->SetFriction(friction); // Set friction
    shape->SetRestitution(0.1f); // Bounce
    URHO3D_LOGINFOF("CREATE HIT id=%d, name=%s", shape->GetNode()->GetID(), shape->GetNode()->GetName());
*/
        switch (type) {
            case 1:
            // player
            // Main bounding circle
            shape = modelNode->CreateComponent<CollisionCircle2D>();
            shape->GetNode()->SetName("hit c");
            shape->SetCenter(shape->GetCenter()+Vector2(0.0f,0.0f));
            shape->SetRadius(2.0f); // Set shape size
            shape->SetFriction(friction); // Set friction
            shape->SetRestitution(0.1f); // Bounce
            //URHO3D_LOGINFOF("CREATE HIT id=%d, name=%s", shape->GetNode()->GetID(), shape->GetNode()->GetName());

            break;

            case 2:
            // enemy
            // Main bounding circle
            shape = modelNode->CreateComponent<CollisionCircle2D>();
            shape->GetNode()->SetName("hit c");
//            shape->SetCenter(shape->GetCenter()+Vector2(0,-0.8f)); 
            shape->SetCenter(shape->GetCenter()+Vector2(0,0.0f));

            shape->SetRadius(2.0); // Set shape size
            shape->SetFriction(friction); // Set friction
            shape->SetRestitution(0.1f); // Bounce
            //URHO3D_LOGINFOF("CREATE HIT id=%d, name=%s", shape->GetNode()->GetID(), shape->GetNode()->GetName());

            break;
        }




    Vector2 min, max, center;
    auto& bones = modelObject->GetSkeleton().GetBones();
		for (auto& bone : bones) {
			auto& name = bone.name_;
            
                //URHO3D_LOGINFOF("bone=%d", bone.name_);
//            center = Vector2(bone.boundingBox_.max_.x_-bone.boundingBox_.min_.x_, bone.boundingBox_.max_.y_-bone.boundingBox_.min_.y_);
/*
             center = Vector2(bone.boundingBox_.max_.x_-bone.boundingBox_.min_.x_, bone.boundingBox_.max_.y_-bone.boundingBox_.min_.y_);
            max = Vector2(std::max(max.x_, bone.boundingBox_.max_.x_), std::max(max.y_, bone.boundingBox_.max_.y_));
            min = Vector2(std::min(min.x_, bone.boundingBox_.min_.x_), std::min(min.y_, bone.boundingBox_.min_.y_));
*/

            center = Vector2(bone.boundingBox_.max_.x_-bone.boundingBox_.min_.x_, bone.boundingBox_.max_.z_-bone.boundingBox_.min_.z_);
            max = Vector2(std::max(max.x_, bone.boundingBox_.max_.x_), std::max(max.y_, bone.boundingBox_.max_.z_));
            min = Vector2(std::min(min.x_, bone.boundingBox_.min_.x_), std::min(min.y_, bone.boundingBox_.min_.z_));

                //URHO3D_LOGINFOF("center=(%f, %f)", center.x_, center.y_);

        //        bone.boundingBox_.max_.x_ - , bone.boundingBox_.max_.y_ - bone.boundingBox_.min_.y_)
        }


/* PLAYER HIT TESTS
                shape = modelNode->CreateComponent<CollisionCircle2D>();
                shape->GetNode()->SetName("hit-right");
                shape->SetCenter(Vector2((max.x_),center.y_+0.4f));
                shape->SetRadius(0.3f); // Set shape size
                shape->SetFriction(friction); // Set friction
                shape->SetRestitution(0.1f); // Bounce
                URHO3D_LOGINFOF("CREATE HIT id=%d, name=%s", shape->GetNode()->GetID(), shape->GetNode()->GetName());

                shape = modelNode->CreateComponent<CollisionCircle2D>();
                shape->GetNode()->SetName("hit-left");
                shape->SetCenter(Vector2((center.x_-max.x_),center.y_+0.4f));
                shape->SetRadius(0.3f); // Set shape size
                shape->SetFriction(friction); // Set friction
                shape->SetRestitution(0.1f); // Bounce
                URHO3D_LOGINFOF("CREATE HIT id=%d, name=%s", shape->GetNode()->GetID(), shape->GetNode()->GetName());

                */



//        boneNode1->SetPosition(skeleton.GetRootBone()-->initialPosition_);
/*
    auto& bones = modelObject->GetSkeleton().GetBones();
		for (auto& bone : bones) {
			auto& name = bone.name_;
//                URHO3D_LOGINFOF("bone=%s", name);

			if (name == "Bip01_Head" || name == "Bip01_L_UpperArm" ||
				name == "Bip01_L_Forearm" || name == "Bip01_L_Thigh" ||
				name == "Bip01_L_Calf") {
				//CreateRagdollBone(name, SHAPE_BOX, Vector3(bone.boundingBox_.max_.x_ - bone.boundingBox_.min_.x_, bone.boundingBox_.max_.y_ - bone.boundingBox_.min_.y_, bone.boundingBox_.max_.z_ - bone.boundingBox_.min_.z_), bone.initialPosition_, bone.initialRotation_);
                auto* shape = modelNode->CreateComponent<CollisionCircle2D>();
                shape->SetCenter(Vector2(bone.boundingBox_.max_.x_ - bone.boundingBox_.min_.x_, bone.boundingBox_.max_.y_ - bone.boundingBox_.min_.y_)); 
                shape->SetRadius(0.04f); // Set shape size
                shape->SetFriction(friction); // Set friction
                shape->SetRestitution(0.1f); // Bounce


			//}
//		}
*/
    // Create rigid body to the root node
 //   auto* body = modelNode->CreateComponent<RigidBody2D>();
 //   body->SetBodyType(BT_STATIC);
//    body->SetBodyType(BT_DYNAMIC);
//    body->SetAllowSleep(false);

/*
    auto* shape = modelNode->CreateComponent<CollisionCircle2D>();
    shape->SetCenter(Vector2(position.x_, position.y_)); 
    shape->SetRadius(0.04f);
    shape->SetFriction(0.8f);

*/
/*
    auto* impBody2 = boneNode1->CreateComponent<RigidBody2D>();
    impBody2->SetBodyType(BT_DYNAMIC);
    impBody2->SetAllowSleep(false);
*/
/*
    auto* shape2 = modelNode->CreateComponent<CollisionCircle2D>();
    shape2->SetRadius(1.00f); // Set shape size
    shape2->SetFriction(friction); // Set friction
    shape2->SetRestitution(0.1f); // Bounce
*/

    return modelNode;
}

Node* Sample2D::CreateObject(TileMapInfo2D info, float friction, Vector3 position, float scale, int type)
{
    auto* cache = GetSubsystem<ResourceCache>();
    Node* modelNode = scene_->CreateChild("ObjNode");

    modelNode->SetPosition(position);

    // Create animated models
    const float MODEL_MOVE_SPEED = 2.0f;
    const float MODEL_ROTATE_SPEED = 100.0f;
    const BoundingBox bounds(Vector3(-20.0f, 0.0f, -20.0f), Vector3(20.0f, 0.0f, 20.0f));

    // rotate model by 180 ****************************
    Node* adjustNode = modelNode->CreateChild("AdjNode");
    Quaternion qAdjRot(180, Vector3(0,1,0) ); // rotate it by 180
    adjustNode->SetRotation( qAdjRot );

    std::string path = "Models/";
    std::string filler = "";
    std::string terrainType = "Land";
//            std::string tileStr = path + terrainType + "Tile" + filler + "%d.mdl";       
    std::string tileStr = path + "AssetPack/elephant.mdl";
    std::string matStr = "";       
//            std::string tileStr;
    float xoffset;
    float height;
    float depth;
    float boundingScale;
            // Set tile location
//            tileNode->SetPosition(Vector3(info.TileIndexToPosition(x, y))+tile->GetModelOffset()+Vector3(0,0,-1));
            //if (land) { xoffset = 0.3f; depth = 1.0f; }
            //if (plant) { xoffset = 0.0f; depth = -3.0f; height = 0.0f; }
            //if (building) { xoffset = 0.0f; depth = 1.2f; height = -0.6f; }

    switch(type) {
        case 1:
            tileStr = path + "AssetPack/balloon.mdl";  
            matStr = path + "AssetPack/balloon.txt";  
            scale = 0.12f;
            boundingScale = 1.2f;
        break;
        case 2:
            tileStr = path + "AssetPack/cloud02.mdl";  
            matStr = path + "AssetPack/cloud02.txt";  
            scale = 0.12f;
            boundingScale = 1.2f;
        break;
        case 3:
            tileStr = path + "AssetPack/pumkin.mdl";  
            matStr = path + "AssetPack/pumkin.txt";  
            scale = 0.19f;
            boundingScale = 0.2f;
        break;
        case 4:
        break;
        case 5:
        break;
        case 6:
        break;
        case 7:
        break;
        case 8:
        break;
    };        
    modelNode->SetScale(scale);
            

    const char *cstr = tileStr.c_str();

    char buffer[100];
    sprintf(buffer, cstr, type);

    auto* staticObject = adjustNode->CreateComponent<StaticModel>();

//            staticObject->SetPosition(Vector3(info.TileIndexToPosition(x, y))*1.0f+Vector3(xoffset,height,depth));
//            staticObject->SetScale(Vector3(scale, scale, scale));
            staticObject->GetNode()->SetRotation(Quaternion(180.0f,90.0f,90.0f));

            staticObject->SetModel(cache->GetResource<Model>(buffer));
            String matFile = GetSubsystem<FileSystem>()->GetProgramDir() + "Data/" + matStr.c_str();
 //           staticObject->SetMaterial(cache->GetResource<Material>("Materials/LOWPOLY-COLORS.xml"));
            staticObject->ApplyMaterialList(matFile);

            staticObject->SetCastShadows(true);


    auto* impBody = modelNode->CreateComponent<RigidBody2D>();
    //impBody->SetBodyType(BT_STATIC);
//    impBody->SetMassCenter()
    impBody->GetNode()->SetName("hit r");
    impBody->SetBodyType(BT_DYNAMIC);
    impBody->SetAllowSleep(false);
    URHO3D_LOGINFOF("CREATE BODY HIT id=%d, name=%s", impBody->GetNode()->GetID(), impBody->GetNode()->GetName());


    // Main bounding circle
    auto* shape = modelNode->CreateComponent<CollisionCircle2D>();
    shape->GetNode()->SetName("hit c");
    shape->SetCenter(shape->GetCenter()+Vector2(0,0.2)); 
    shape->SetRadius(boundingScale); // Set shape size
    shape->SetFriction(friction); // Set friction
    shape->SetRestitution(0.1f); // Bounce
    URHO3D_LOGINFOF("CREATE HIT id=%d, name=%s", shape->GetNode()->GetID(), shape->GetNode()->GetName());

    return modelNode;
}

Node* Sample2D::CreateTrigger()
{
    Node* node = scene_->CreateChild(); // Clones will be renamed according to object type
    auto* body = node->CreateComponent<RigidBody2D>();
    body->SetBodyType(BT_STATIC);
    auto* shape = node->CreateComponent<CollisionBox2D>(); // Create box shape
    shape->SetTrigger(true);
    return node;
}

/*
Node* Sample2D::CreateEnemy()
{
    auto* cache = GetSubsystem<ResourceCache>();
    Node* node = scene_->CreateChild("Enemy");
    auto* staticSprite = node->CreateComponent<StaticSprite2D>();
    staticSprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/Aster.png"));
    auto* body = node->CreateComponent<RigidBody2D>();
    body->SetBodyType(BT_STATIC);
    auto* shape = node->CreateComponent<CollisionCircle2D>(); // Create circle shape
    shape->SetRadius(0.25f); // Set radius
    return node;
}

Node* Sample2D::CreateOrc()
{
    auto* cache = GetSubsystem<ResourceCache>();
    Node* node = scene_->CreateChild("Orc");
    node->SetScale(scene_->GetChild("Imp", true)->GetScale());
    auto* animatedSprite = node->CreateComponent<AnimatedSprite2D>();
    auto* animationSet = cache->GetResource<AnimationSet2D>("Urho2D/Orc/Orc.scml");
    animatedSprite->SetAnimationSet(animationSet);
    animatedSprite->SetAnimation("run"); // Get scml file and Play "run" anim
    animatedSprite->SetLayer(2); // Make orc always visible
    auto* body = node->CreateComponent<RigidBody2D>();
    auto* shape = node->CreateComponent<CollisionCircle2D>();
    shape->SetRadius(1.3f); // Set shape size
    shape->SetTrigger(true);
    return node;
}

Node* Sample2D::CreateCoin()
{
    auto* cache = GetSubsystem<ResourceCache>();
    Node* node = scene_->CreateChild("Coin");
    node->SetScale(0.5);
    auto* animatedSprite = node->CreateComponent<AnimatedSprite2D>();
    auto* animationSet = cache->GetResource<AnimationSet2D>("Urho2D/GoldIcon.scml");
    animatedSprite->SetAnimationSet(animationSet); // Get scml file and Play "idle" anim
    animatedSprite->SetAnimation("idle");
    animatedSprite->SetLayer(4);
    auto* body = node->CreateComponent<RigidBody2D>();
    body->SetBodyType(BT_STATIC);
    auto* shape = node->CreateComponent<CollisionCircle2D>(); // Create circle shape
    shape->SetRadius(0.32f); // Set radius
    shape->SetTrigger(true);
    return node;
}

Node* Sample2D::CreateMovingPlatform()
{
    auto* cache = GetSubsystem<ResourceCache>();
    Node* node = scene_->CreateChild("MovingPlatform");
    node->SetScale(Vector3(3.0f, 1.0f, 0.0f));
    auto* staticSprite = node->CreateComponent<StaticSprite2D>();
    staticSprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/Box.png"));
    auto* body = node->CreateComponent<RigidBody2D>();
    body->SetBodyType(BT_STATIC);
    auto* shape = node->CreateComponent<CollisionBox2D>(); // Create box shape
    shape->SetSize(Vector2(0.32f, 0.32f)); // Set box size
    shape->SetFriction(0.8f); // Set friction
    return node;
}*/

void Sample2D::PopulateMovingEntities(TileMapLayer3D* movingEntitiesLayer)
{
    // Create enemy (will be cloned at each placeholder)
   // Node* enemyNode = CreateEnemy();
   // Node* orcNode = CreateOrc();
   // Node* platformNode = CreateMovingPlatform();
/*
    // Instantiate enemies and moving platforms at each placeholder (placeholders are Poly Line objects defining a path from points)
    for (int i=0; i < movingEntitiesLayer->GetNumObjects(); ++i)
    {
        // Get placeholder object
        TileMapObject2D* movingObject = movingEntitiesLayer->GetObject(i); // Get placeholder object
        if (movingObject->GetObjectType() == OT_POLYLINE)
        {
            // Clone the enemy and position it at placeholder point
            Node* movingClone;
            Vector2 offset = Vector2(0.0f, 0.0f);
            if (movingObject->GetType() == "Enemy")
            {
                movingClone = enemyNode->Clone();
                offset = Vector2(0.0f, -0.32f);
            }
            else if (movingObject->GetType() == "Orc")
                movingClone = orcNode->Clone();
            else if (movingObject->GetType() == "MovingPlatform")
                movingClone = platformNode->Clone();
            else
                continue;
            movingClone->SetPosition2D(movingObject->GetPoint(0) + offset);

            // Create script object that handles entity translation along its path
            auto* mover = movingClone->CreateComponent<Mover>();

            // Set path from points
            PODVector<Vector2> path = CreatePathFromPoints(movingObject, offset);
            mover->path_ = path;

            // Override default speed
            if (movingObject->HasProperty("Speed"))
                mover->speed_ = ToFloat(movingObject->GetProperty("Speed"));
        }
    }

    // Remove nodes used for cloning purpose
    enemyNode->Remove();
    orcNode->Remove();
    platformNode->Remove();*/
}

void Sample2D::PopulateCoins(TileMapLayer3D* coinsLayer)
{
    // Create coin (will be cloned at each placeholder)
/*    Node* coinNode = CreateCoin();

    // Instantiate coins to pick at each placeholder
    for (int i=0; i < coinsLayer->GetNumObjects(); ++i)
    {
        TileMapObject2D* coinObject = coinsLayer->GetObject(i); // Get placeholder object
        Node* coinClone = coinNode->Clone();
        coinClone->SetPosition2D(coinObject->GetPosition() + coinObject->GetSize() / 2 + Vector2(0.0f, 0.16f));

    }

    // Remove node used for cloning purpose
    coinNode->Remove();*/
}

void Sample2D::PopulateTriggers(TileMapLayer3D* triggersLayer)
{
    // Create trigger node (will be cloned at each placeholder)
    Node* triggerNode = CreateTrigger();

    // Instantiate triggers at each placeholder (Rectangle objects)
    for (int i=0; i < triggersLayer->GetNumObjects(); ++i)
    {
        TileMapObject2D* triggerObject = triggersLayer->GetObject(i); // Get placeholder object
        if (triggerObject->GetObjectType() == OT_RECTANGLE)
        {
            Node* triggerClone = triggerNode->Clone();
            triggerClone->SetName(triggerObject->GetType());
            auto* shape = triggerClone->GetComponent<CollisionBox2D>();
            shape->SetSize(triggerObject->GetSize());
            triggerClone->SetPosition2D(triggerObject->GetPosition() + triggerObject->GetSize() / 2);
        }
    }
}

float Sample2D::Zoom(Camera* camera)
{
    
    auto* input = GetSubsystem<Input>();
    float zoom_ = camera->GetZoom();

    if (input->GetMouseMoveWheel() != 0)
    {
        zoom_ = Clamp(zoom_ + input->GetMouseMoveWheel() * 0.1f, CAMERA_MIN_DIST, CAMERA_MAX_DIST);
        camera->SetZoom(zoom_);
    }

    if (input->GetKeyDown(KEY_PAGEUP))
    {
        zoom_ = Clamp(zoom_ * 1.01f, CAMERA_MIN_DIST, CAMERA_MAX_DIST);
        camera->SetZoom(zoom_);
    }

    if (input->GetKeyDown(KEY_PAGEDOWN))
    {
        zoom_ = Clamp(zoom_ * 0.99f, CAMERA_MIN_DIST, CAMERA_MAX_DIST);
        camera->SetZoom(zoom_);
    }

    return zoom_;
}



float Sample2D::MoveCamera(Camera* camera)
{
    auto* input = GetSubsystem<Input>();
    float zoom_ = camera->GetZoom();

    return zoom_;
}

PODVector<Vector2> Sample2D::CreatePathFromPoints(TileMapObject2D* object, Vector2 offset)
{
    PODVector<Vector2> path;
    for (int i=0; i < object->GetNumPoints(); ++i)
        path.Push(object->GetPoint(i) + offset);
    return path;
}

void Sample2D::CreateUIContent(const String& demoTitle, int remainingLifes, int remainingCoins)
{
    auto* cache = GetSubsystem<ResourceCache>();
    auto* ui = GetSubsystem<UI>();

    // Set the default UI style and font
    ui->GetRoot()->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
    auto* font = cache->GetResource<Font>("Fonts/Anonymous Pro.ttf");

    // We create in-game UIs (coins and lifes) first so that they are hidden by the fullscreen UI (we could also temporary hide them using SetVisible)

    // Create the UI for displaying the remaining coins
    auto* coinsUI = ui->GetRoot()->CreateChild<BorderImage>("Coins");
    coinsUI->SetTexture(cache->GetResource<Texture2D>("Urho2D/GoldIcon.png"));
    coinsUI->SetSize(50, 50);
    coinsUI->SetImageRect(IntRect(0, 64, 60, 128));
    coinsUI->SetAlignment(HA_LEFT, VA_TOP);
    coinsUI->SetPosition(5, 5);
    coinsUI->SetVisible(false);
    auto* coinsText = coinsUI->CreateChild<Text>("CoinsText");
    coinsText->SetAlignment(HA_CENTER, VA_CENTER);
    coinsText->SetFont(font, 24);
    coinsText->SetTextEffect(TE_SHADOW);
    coinsText->SetText(String(remainingCoins));
    coinsText->SetVisible(false);
    
    // Create the UI for displaying the remaining lifes
    auto* lifeUI = ui->GetRoot()->CreateChild<BorderImage>("Life");
    lifeUI->SetTexture(cache->GetResource<Texture2D>("Urho2D/imp/imp_all.png"));
    lifeUI->SetSize(70, 80);
    lifeUI->SetAlignment(HA_RIGHT, VA_TOP);
    lifeUI->SetPosition(-5, 5);
    lifeUI->SetVisible(false);

    auto* lifeText = lifeUI->CreateChild<Text>("LifeText");
    lifeText->SetAlignment(HA_CENTER, VA_CENTER);
    lifeText->SetFont(font, 24);
    lifeText->SetTextEffect(TE_SHADOW);
    lifeText->SetText(String(remainingLifes));
    lifeText->SetVisible(false);

    // Create the fullscreen UI for start/end
    auto* fullUI = ui->GetRoot()->CreateChild<Window>("FullUI");
    fullUI->SetStyleAuto();
    fullUI->SetSize(ui->GetRoot()->GetWidth(), ui->GetRoot()->GetHeight());
    fullUI->SetEnabled(false); // Do not react to input, only the 'Exit' and 'Play' buttons will

    // Create the title
    auto* title = fullUI->CreateChild<BorderImage>("Title");
    title->SetMinSize(fullUI->GetWidth(), 50);
    //title->SetTexture(cache->GetResource<Texture2D>("Textures/HeightMap.png"));
    title->SetFullImageRect();
    title->SetAlignment(HA_CENTER, VA_TOP);
    auto* titleText = title->CreateChild<Text>("TitleText");
    titleText->SetAlignment(HA_CENTER, VA_CENTER);
    titleText->SetFont(font, 24);
    titleText->SetText(demoTitle);

    // Create the image
    auto* spriteUI = fullUI->CreateChild<BorderImage>("Sprite");
    spriteUI->SetTexture(cache->GetResource<Texture2D>("Textures/BZLogo-lg.png"));
    spriteUI->SetSize(700, 328);
    spriteUI->SetAlignment(HA_CENTER, VA_CENTER);
    spriteUI->SetPosition(0, - ui->GetRoot()->GetHeight() / 4);

    // Create the 'PLAY' button
    auto* playButton = ui->GetRoot()->CreateChild<Button>("PlayButton");
    playButton->SetStyleAuto();
    playButton->SetFocusMode(FM_RESETFOCUS);
    playButton->SetSize(100, 50);
    playButton->SetAlignment(HA_CENTER, VA_CENTER);
    playButton->SetPosition(-100, 0);
    auto* playText = playButton->CreateChild<Text>("PlayText");
    playText->SetAlignment(HA_CENTER, VA_CENTER);
    playText->SetFont(font, 24);
    playText->SetText("PLAY");
//  SubscribeToEvent(playButton, E_RELEASED, HANDLER(Urho2DPlatformer, HandlePlayButton));


    // Create the 'EXIT' button
    auto* exitButton = ui->GetRoot()->CreateChild<Button>("ExitButton");
    exitButton->SetStyleAuto();
    exitButton->SetFocusMode(FM_RESETFOCUS);
    exitButton->SetSize(100, 50);
    exitButton->SetAlignment(HA_CENTER, VA_CENTER);
    exitButton->SetPosition(100, 0);
    auto* exitText = exitButton->CreateChild<Text>("ExitText");
    exitText->SetAlignment(HA_CENTER, VA_CENTER);
    exitText->SetFont(font, 24);
    exitText->SetText("EXIT");
    SubscribeToEvent(exitButton, E_RELEASED, URHO3D_HANDLER(Sample2D, HandleExitButton));


    // Create the instructions
    auto* instructionText = ui->GetRoot()->CreateChild<Text>("Instructions");
    instructionText->SetText("Get ready.");
    instructionText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);
    instructionText->SetTextAlignment(HA_CENTER); // Center rows in relation to each other
    instructionText->SetAlignment(HA_CENTER, VA_CENTER);
    instructionText->SetPosition(0, ui->GetRoot()->GetHeight() / 4);


    // Show mouse cursor
    auto* input = GetSubsystem<Input>();
    input->SetMouseVisible(true);
}

void Sample2D::HandleExitButton(StringHash eventType, VariantMap& eventData)
{
    auto* engine = GetSubsystem<Engine>();
    engine->Exit();
}

void Sample2D::SaveScene(bool initial)
{
    String filename = demoFilename_;
    if (!initial)
        filename += "InGame";
    File saveFile(context_, GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/" + filename + ".xml", FILE_WRITE);
    scene_->SaveXML(saveFile);
}


void Sample2D::CreateBackgroundSprite(TileMapInfo2D info, float scale, const String& texture, bool animate)
{
    auto* cache = GetSubsystem<ResourceCache>();
    Node* node = scene_->CreateChild("Background");
    node->SetPosition(Vector3(info.GetMapWidth(), info.GetMapHeight(), 20.0f) / 2);
    node->SetScale(scale);
    auto* sprite = node->CreateComponent<StaticSprite2D>();
    sprite->SetSprite(cache->GetResource<Sprite2D>(texture));
    SetRandomSeed(Time::GetSystemTime()); // Randomize from system clock
    sprite->SetColor(Color(Random(0.6f, 0.9f), Random(0.8f, 1.0f), Random(0.9f, 1.0f), 1.0f));

    // Create rotation animation
    if (animate)
    {
        SharedPtr<ValueAnimation> animation(new ValueAnimation(context_));
        animation->SetKeyFrame(0, Variant(Quaternion(0.0f, 0.0f, 0.0f)));
        animation->SetKeyFrame(1, Variant(Quaternion(0.0f, 0.0f, 180.0f)));
        animation->SetKeyFrame(2, Variant(Quaternion(0.0f, 0.0f, 0.0f)));
        node->SetAttributeAnimation("Rotation", animation, WM_LOOP, 0.02f);
    }
}

void Sample2D::SpawnEffect(Node* node)
{
    auto* cache = GetSubsystem<ResourceCache>();
    Node* particleNode = node->CreateChild("Emitter");
    particleNode->SetScale(0.5 / node->GetScale().x_);
    auto* particleEmitter = particleNode->CreateComponent<ParticleEmitter2D>();
    particleEmitter->SetLayer(2);
    particleEmitter->SetEffect(cache->GetResource<ParticleEffect2D>("Urho2D/sun.pex"));
}

void Sample2D::PlaySoundEffect(const String& soundName)
{
    auto* cache = GetSubsystem<ResourceCache>();
    auto* source = scene_->CreateComponent<SoundSource>();
    auto* sound = cache->GetResource<Sound>("Sounds/" + soundName);
    if (sound != nullptr) {
        source->SetAutoRemoveMode(REMOVE_COMPONENT);
        source->Play(sound);
    }
}
