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

#include "../Precompiled.h"

#include "../Core/Context.h"
#include "../Graphics/DebugRenderer.h"
#include "../Resource/ResourceCache.h"
#include "../Scene/Node.h"
#include "../Urho2D/StaticSprite2D.h"
#include "../Urho2D/StaticSprite3D.h"
#include "../Urho2D/TileMap3D.h"
#include "../Urho2D/TileMapLayer3D.h"
#include "../Urho2D/TmxFile2D.h"

#include "../Graphics/Material.h"
#include "../Graphics/Model.h"
#include "../Graphics/StaticModel.h"
#include "../Graphics/Octree.h"

#include "..//IO/File.h"
#include "../IO/FileSystem.h"


#include "../DebugNew.h"
#include <string>

namespace Urho3D
{

TileMapLayer3D::TileMapLayer3D(Context* context) :
    Component(context)
{
}

TileMapLayer3D::~TileMapLayer3D() = default;

void TileMapLayer3D::RegisterObject(Context* context)
{
    context->RegisterFactory<TileMapLayer3D>();
}

// Transform vector from node-local space to global space
static Vector2 TransformNode2D(const Matrix3x4& transform, Vector2 local)
{
    Vector3 transformed = transform * Vector4(local.x_, local.y_, 0.f, 1.f);
    return Vector2(transformed.x_, transformed.y_);
}

void TileMapLayer3D::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
    if (!debug)
        return;

    if (objectGroup_)
    {
        const Matrix3x4 transform = GetTileMap()->GetNode()->GetTransform();
        for (unsigned i = 0; i < objectGroup_->GetNumObjects(); ++i)
        {
            TileMapObject2D* object = objectGroup_->GetObject(i);
            const Color& color = Color::YELLOW;
            const Vector2& size = object->GetSize();
            const TileMapInfo2D& info = tileMap_->GetInfo();


            switch (object->GetObjectType())
            {
            case OT_RECTANGLE:
                {
                    Vector<Vector2> points;

                    switch (info.orientation_)
                    {
                    case O_ORTHOGONAL:
                    case O_HEXAGONAL:
                    case O_STAGGERED:
                        {
                            points.Push(Vector2::ZERO);
                            points.Push(Vector2(size.x_, 0.0f));
                            points.Push(Vector2(size.x_, -size.y_));
                            points.Push(Vector2(0.0f, -size.y_));
                            break;
                        }
                    case O_ISOMETRIC:
                        {
                            float ratio = (info.tileWidth_ / info.tileHeight_) * 0.5f;
                            points.Push(Vector2::ZERO);
                            points.Push(Vector2(size.y_ * ratio, size.y_ * 0.5f));
                            points.Push(Vector2((size.x_ + size.y_) * ratio, (-size.x_ + size.y_) * 0.5f));
                            points.Push(Vector2(size.x_ * ratio, -size.x_ * 0.5f));
                            break;
                        }
                    }

                    for (unsigned j = 0; j < points.Size(); ++j)
                        debug->AddLine(Vector3(TransformNode2D(transform, points[j] + object->GetPosition())),
                            Vector3(TransformNode2D(transform, points[(j + 1) % points.Size()] + object->GetPosition())), color,
                            depthTest);
                }
                break;

            case OT_ELLIPSE:
                {
                    const Vector2 halfSize = object->GetSize() * 0.5f;
                    float ratio = (info.tileWidth_ / info.tileHeight_) * 0.5f; // For isometric only

                    Vector2 pivot = object->GetPosition();
                    if (info.orientation_ == O_ISOMETRIC)
                    {
                        pivot += Vector2((halfSize.x_ + halfSize.y_) * ratio, (-halfSize.x_ + halfSize.y_) * 0.5f);
                    }
                    else
                    {
                        pivot += halfSize;
                    }

                    for (unsigned i = 0; i < 360; i += 30)
                    {
                        unsigned j = i + 30;
                        float x1 = halfSize.x_ * Cos((float)i);
                        float y1 = halfSize.y_ * Sin((float)i);
                        float x2 = halfSize.x_ * Cos((float)j);
                        float y2 = halfSize.y_ * Sin((float)j);
                        Vector2 point1 = Vector2(x1, - y1);
                        Vector2 point2 = Vector2(x2, - y2);

                        if (info.orientation_ == O_ISOMETRIC)
                        {
                            point1 = Vector2((point1.x_ + point1.y_) * ratio, (point1.y_ - point1.x_) * 0.5f);
                            point2 = Vector2((point2.x_ + point2.y_) * ratio, (point2.y_ - point2.x_) * 0.5f);
                        }

                        debug->AddLine(Vector3(TransformNode2D(transform, pivot + point1)),
                            Vector3(TransformNode2D(transform, pivot + point2)), color, depthTest);
                    }
                }
                break;

            case OT_POLYGON:
            case OT_POLYLINE:
                {
                    for (unsigned j = 0; j < object->GetNumPoints() - 1; ++j)
                        debug->AddLine(Vector3(TransformNode2D(transform, object->GetPoint(j))),
                            Vector3(TransformNode2D(transform, object->GetPoint(j + 1))), color, depthTest);

                    if (object->GetObjectType() == OT_POLYGON)
                        debug->AddLine(Vector3(TransformNode2D(transform, object->GetPoint(0))),
                            Vector3(TransformNode2D(transform, object->GetPoint(object->GetNumPoints() - 1))), color, depthTest);
                    // Also draw a circle at origin to indicate direction
                    else
                        debug->AddCircle(Vector3(TransformNode2D(transform, object->GetPoint(0))), Vector3::FORWARD, 0.05f, color,
                            64, depthTest);
                }
                break;

            default: break;
            }
        }
    }
}

void TileMapLayer3D::Initialize(TileMap3D* tileMap, const TmxLayer2D* tmxLayer)
{
    if (tileMap == tileMap_ && tmxLayer == tmxLayer_)
        return;

    if (tmxLayer_)
    {
        for (unsigned i = 0; i < nodes_.Size(); ++i)
        {
            if (nodes_[i])
                nodes_[i]->Remove();
        }

        nodes_.Clear();
    }

    tileLayer_ = nullptr;
    objectGroup_ = nullptr;
    imageLayer_ = nullptr;

    tileMap_ = tileMap;
    tmxLayer_ = tmxLayer;

    if (!tmxLayer_)
        return;

    switch (tmxLayer_->GetType())
    {
    case LT_TILE_LAYER:
        SetTileLayer((const TmxTileLayer2D*)tmxLayer_);
        break;

    case LT_OBJECT_GROUP:
        SetObjectGroup((const TmxObjectGroup2D*)tmxLayer_);
        break;

    case LT_IMAGE_LAYER:
        SetImageLayer((const TmxImageLayer2D*)tmxLayer_);
        break;

    default:
        break;
    }

    SetVisible(tmxLayer_->IsVisible());
}

void TileMapLayer3D::SetDrawOrder(int drawOrder)
{
    if (drawOrder == drawOrder_)
        return;

    drawOrder_ = drawOrder;

    for (unsigned i = 0; i < nodes_.Size(); ++i)
    {
        if (!nodes_[i])
            continue;

        auto* staticSprite = nodes_[i]->GetComponent<StaticSprite3D>();
        if (staticSprite)
            staticSprite->SetLayer(drawOrder_);
    }
}

void TileMapLayer3D::SetVisible(bool visible)
{
    if (visible == visible_)
        return;

    visible_ = visible;

    for (unsigned i = 0; i < nodes_.Size(); ++i)
    {
        if (nodes_[i])
            nodes_[i]->SetEnabled(visible_);
    }
}

TileMap3D* TileMapLayer3D::GetTileMap() const
{
    return tileMap_;
}

bool TileMapLayer3D::HasProperty(const String& name) const
{
    if (!tmxLayer_)
        return false;

    return tmxLayer_->HasProperty(name);
}

const String& TileMapLayer3D::GetProperty(const String& name) const
{
    if (!tmxLayer_)
        return String::EMPTY;
    return tmxLayer_->GetProperty(name);
}

TileMapLayerType2D TileMapLayer3D::GetLayerType() const
{
    return tmxLayer_ ? tmxLayer_->GetType() : LT_INVALID;
}

int TileMapLayer3D::GetWidth() const
{
    return tmxLayer_ ? tmxLayer_->GetWidth() : 0;
}

int TileMapLayer3D::GetHeight() const
{
    return tmxLayer_ ? tmxLayer_->GetHeight() : 0;
}

Tile2D* TileMapLayer3D::GetTile(int x, int y) const
{
    if (!tileLayer_)
        return nullptr;

    return tileLayer_->GetTile(x, y);
}

Node* TileMapLayer3D::GetTileNode(int x, int y) const
{
    if (!tileLayer_)
        return nullptr;

    if (x < 0 || x >= tileLayer_->GetWidth() || y < 0 || y >= tileLayer_->GetHeight())
        return nullptr;

    return nodes_[y * tileLayer_->GetWidth() + x];
}

unsigned TileMapLayer3D::GetNumObjects() const
{
    if (!objectGroup_)
        return 0;

    return objectGroup_->GetNumObjects();
}

TileMapObject2D* TileMapLayer3D::GetObject(unsigned index) const
{
    if (!objectGroup_)
        return nullptr;

    return objectGroup_->GetObject(index);
}

Node* TileMapLayer3D::GetObjectNode(unsigned index) const
{
    if (!objectGroup_)
        return nullptr;

    if (index >= nodes_.Size())
        return nullptr;

    return nodes_[index];
}

Node* TileMapLayer3D::GetImageNode() const
{
    if (!imageLayer_ || nodes_.Empty())
        return nullptr;

    return nodes_[0];
}

void TileMapLayer3D::SetTileLayer(const TmxTileLayer2D* tileLayer)
{
    bool land = false;
    bool plant = false;
    bool building = false;

    auto* cache = GetSubsystem<ResourceCache>();

    tileLayer_ = tileLayer;

    if (tileLayer_->GetName() == "Land")
        land = true;
    
    if (tileLayer_->GetName() == "Plant")
        plant = true;

    if (tileLayer_->GetName() == "Building")
        building = true;

    int width = tileLayer->GetWidth();
    int height = tileLayer->GetHeight();
    nodes_.Resize((unsigned)(width * height));

    const TileMapInfo2D& info = tileMap_->GetInfo();
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const Tile2D* tile = tileLayer->GetTile(x, y);
            if (!tile)
                continue;

            SharedPtr<Node> tileNode(GetNode()->CreateTemporaryChild("Tile"));
            auto* staticObject = tileNode->CreateComponent<StaticModel>();
            unsigned int tileId = tile->GetGid();


            std::string path = "Models/";
            std::string filler = "";
            std::string terrainType = "Land";
            if (tileId > 9)
                filler = ""; 
            else 
                filler = "0";
//            std::string tileStr = path + terrainType + "Tile" + filler + "%d.mdl";       
            std::string tileStr = path + "AssetPack/elephant.mdl";
            std::string matStr = "";       
//            std::string tileStr;
            float xoffset;
            float height;
            float depth;
            float scale = 0.17f;

            // Set tile location
//            tileNode->SetPosition(Vector3(info.TileIndexToPosition(x, y))+tile->GetModelOffset()+Vector3(0,0,-1));
            if (land) { xoffset = 0.3f; depth = 1.0f; }
            if (plant) { xoffset = 0.0f; depth = -0.5f; height = 0.0f; }
            if (building) { xoffset = 0.0f; depth = 1.5f; height = -0.6f; }

            if (land) {
                switch(tileId) {
                    case 1:
                        tileStr = path + "AssetPack/castle-wall_stone.mdl";  
                        matStr = path + "AssetPack/castle-wall_stone.txt";  
                        scale = 0.12f;
                    break;
                    case 2:
                        tileStr = path + "AssetPack/terrain-world-plain.mdl";
                        matStr = path + "AssetPack/terrain-world-plain.txt";
                        scale = 0.07f;
                        height = 0.7f;
                        depth = -0.9f;
                    break;
                    case 3:
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
            }
                    
            if (plant) {                  
                switch(tileId) {
                    case 33:
                        tileStr = path + "AssetPack/tree-forest.mdl";  
                        matStr = path + "AssetPack/tree-forest.txt";  
                    break;
                    case 34:
                        tileStr = path + "AssetPack/tree-baobab.mdl";  
                        matStr = path + "AssetPack/tree-baobab.txt";  
                    break;
                    case 35:
                        tileStr = path + "AssetPack/tree-birch02.mdl";  
                        matStr = path + "AssetPack/tree-birch02.txt";  
                    break;
                    case 36:
                        tileStr = path + "AssetPack/tree-oak_T.mdl";
                        matStr = path + "AssetPack/tree-oak_T.txt";
                    break;
                    case 37:
                        tileStr = path + "AssetPack/tree-lime.mdl";
                        matStr = path + "AssetPack/tree-lime.txt";
                    break;
                    case 38:
                        tileStr = path + "AssetPack/grass01.mdl";
                        matStr = path + "AssetPack/grass01.txt";
                    break;
                    case 39:
                        tileStr = path + "AssetPack/flower01.mdl";
                        matStr = path + "AssetPack/flower01.txt";
                    break;
                    case 40:
                        tileStr = path + "AssetPack/flower02.mdl";
                        matStr = path + "AssetPack/flower02.txt";
                    break;
                };
            }

            if (building) {
                switch(tileId) {
                    case 9:
                        tileStr = path + "AssetPack/castle-tower.mdl";  
                        matStr = path + "AssetPack/castle-tower.txt";  
                        scale = 0.17f;
                    break;
                    case 10:
                        tileStr = path + "AssetPack/castle-tower-square.mdl";  
                        matStr = path + "AssetPack/castle-tower-square.txt";  
                        scale = 0.17f;
                    break;
                    case 11:
                        tileStr = path + "AssetPack/castle-gate_small.mdl";  
                        matStr = path + "AssetPack/castle-gate_small.txt";  
                        scale = 0.17f;
                    break;
                    case 12:
                        tileStr = path + "AssetPack/castle.mdl";  
                        matStr = path + "AssetPack/castle.txt";  
                        scale = 0.15f;
                    break;
                    case 13:
                        tileStr = path + "AssetPack/castle-gate.mdl";
                        matStr = path + "AssetPack/castle-gate.txt";
                        scale = 0.17f;
                    break;
                    case 14:
                    break;
                    case 15:
                    break;
                    case 16:
                    break;
                };
            }

            const char *cstr = tileStr.c_str();

            char buffer[100];
            sprintf(buffer, cstr, tileId);

    
            tileNode->SetPosition(Vector3(info.TileIndexToPosition(x, y))*1.0f+Vector3(xoffset,height,depth));
            tileNode->SetScale(Vector3(scale, scale, scale));
            tileNode->SetRotation(Quaternion(180.0f,90.0f,90.0f));

            staticObject->SetModel(cache->GetResource<Model>(buffer));
            String matFile = GetSubsystem<FileSystem>()->GetProgramDir() + "Data/" + matStr.c_str();
 //           staticObject->SetMaterial(cache->GetResource<Material>("Materials/LOWPOLY-COLORS.xml"));
            staticObject->ApplyMaterialList(matFile);

           // staticObject->SetMaterial(cache->GetResource<Material>("Materials/BROWN-DARK.xml"));
          //  staticObject->SetMaterial(cache->GetResource<Material>("Materials/GREEN.xml"));

//
/*
        auto* modelObject = modelNode->CreateComponent<AnimatedModel>();
        modelObject->SetModel(cache->GetResource<Model>("Models/Kachujin/Kachujin.mdl"));
        modelObject->SetMaterial(cache->GetResource<Material>("Materials/LOWPOLY-COLORS.xml"));
        modelObject->SetCastShadows(true);
*/



            //terrain-world-plain.mdl

//            staticObject->SetMaterial(cache->GetResource<Material>("Models/3dtile01.mtl"));

 //             tileMap->SetTmxFile(cache->GetResource<TmxFile2D>("Models/3dtile01.obj"));
 

/*
            staticSprite->SetSprite(tile->GetSprite());
            staticSprite->SetFlip(tile->GetFlipX(), tile->GetFlipY(), tile->GetSwapXY());
            staticSprite->SetLayer(drawOrder_);
            staticSprite->SetOrderInLayer(y * width + x);*/

      /*      Node* mushroomNode = scene_->CreateChild("Mushroom");
            mushroomNode->SetPosition(Vector3(Random(90.0f) - 45.0f, 0.0f, Random(90.0f) - 45.0f));
            mushroomNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
            mushroomNode->SetScale(0.5f + Random(2.0f));*/
            nodes_[y * width + x] = tileNode;
        }
    }
}

void TileMapLayer3D::SetObjectGroup(const TmxObjectGroup2D* objectGroup)
{
    objectGroup_ = objectGroup;

    TmxFile2D* tmxFile = objectGroup->GetTmxFile();
    nodes_.Resize(objectGroup->GetNumObjects());

    for (unsigned i = 0; i < objectGroup->GetNumObjects(); ++i)
    {
        const TileMapObject2D* object = objectGroup->GetObject(i);

        // Create dummy node for all object
        SharedPtr<Node> objectNode(GetNode()->CreateTemporaryChild("Object"));
        objectNode->SetPosition(Vector3(object->GetPosition()));

        // If object is tile, create static sprite component
        if (object->GetObjectType() == OT_TILE && object->GetTileGid() && object->GetTileSprite())
        {
            auto* staticSprite = objectNode->CreateComponent<StaticSprite3D>();
            staticSprite->SetSprite(object->GetTileSprite());
            staticSprite->SetFlip(object->GetTileFlipX(), object->GetTileFlipY(), object->GetTileSwapXY());
            staticSprite->SetLayer(drawOrder_);
            staticSprite->SetOrderInLayer((int)((10.0f - object->GetPosition().y_) * 100));

            if (tmxFile->GetInfo().orientation_ == O_ISOMETRIC)
            {
                staticSprite->SetUseHotSpot(true);
                staticSprite->SetHotSpot(Vector2(0.5f, 0.0f));
            }
        }

        nodes_[i] = objectNode;
    }
}

void TileMapLayer3D::SetImageLayer(const TmxImageLayer2D* imageLayer)
{
    imageLayer_ = imageLayer;

    if (!imageLayer->GetSprite())
        return;

    SharedPtr<Node> imageNode(GetNode()->CreateTemporaryChild("Tile"));
    imageNode->SetPosition(Vector3(imageLayer->GetPosition()));

    auto* staticSprite = imageNode->CreateComponent<StaticSprite3D>();
    staticSprite->SetSprite(imageLayer->GetSprite());
    staticSprite->SetOrderInLayer(0);

    nodes_.Push(imageNode);
}

}
