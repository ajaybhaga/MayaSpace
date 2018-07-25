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
#include "../Graphics/Material.h"
#include "../Graphics/Texture2D.h"
#include "../Resource/ResourceCache.h"
#include "../Scene/Scene.h"
#include "../Urho2D/Renderer3D.h"
#include "../Urho2D/Sprite3D.h"
#include "../Urho2D/StaticSprite3D.h"

#include "../DebugNew.h"

namespace Urho3D
{

extern const char* URHO2D_CATEGORY;
extern const char* blendModeNames[];

StaticSprite3D::StaticSprite3D(Context* context) :
    Drawable3D(context),
    blendMode_(BLEND_ALPHA),
    flipX_(false),
    flipY_(false),
    swapXY_(false),
    color_(Color::WHITE),
    useHotSpot_(false),
    useDrawRect_(false),
    useTextureRect_(false),
    hotSpot_(0.5f, 0.5f),
    drawRect_(Rect::ZERO),
    textureRect_(Rect::ZERO)
{
    sourceBatches_.Resize(1);
    sourceBatches_[0].owner_ = this;
}

StaticSprite3D::~StaticSprite3D() = default;

void StaticSprite3D::RegisterObject(Context* context)
{
    context->RegisterFactory<StaticSprite3D>(URHO2D_CATEGORY);

    URHO3D_ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, AM_DEFAULT);
    URHO3D_COPY_BASE_ATTRIBUTES(Drawable3D);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Sprite", GetSpriteAttr, SetSpriteAttr, ResourceRef, ResourceRef(Sprite3D::GetTypeStatic()),
        AM_DEFAULT);
    URHO3D_ENUM_ACCESSOR_ATTRIBUTE("Blend Mode", GetBlendMode, SetBlendMode, BlendMode, blendModeNames, BLEND_ALPHA, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Flip X", GetFlipX, SetFlipX, bool, false, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Flip Y", GetFlipY, SetFlipY, bool, false, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Color", GetColor, SetColor, Color, Color::WHITE, AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Custom material", GetCustomMaterialAttr, SetCustomMaterialAttr, ResourceRef,
        ResourceRef(Material::GetTypeStatic()), AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Hot Spot", GetHotSpot, SetHotSpot, Vector2, Vector2(0.5f, 0.5f), AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Use Hot Spot", GetUseHotSpot, SetUseHotSpot, bool, false, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Draw Rectangle", GetDrawRect, SetDrawRect, Rect, Rect::ZERO, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Use Draw Rectangle", GetUseDrawRect, SetUseDrawRect, bool, false, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Texture Rectangle", GetTextureRect, SetTextureRect, Rect, Rect::ZERO, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Use Texture Rectangle", GetUseTextureRect, SetUseTextureRect, bool, false, AM_DEFAULT);
}

void StaticSprite3D::SetSprite(Sprite2D* sprite)
{
    if (sprite == sprite2d_)
        return;

    // Store 2d sprite
    sprite2d_ = sprite;

    // TODO: Need to set sprite_
    //sprite_ = sprite;
    UpdateMaterial();

    sourceBatchesDirty_ = true;
    MarkNetworkUpdate();

    UpdateDrawRect();
}

void StaticSprite3D::SetDrawRect(const Rect& rect)
{
    drawRect_ = rect;

    if(useDrawRect_)
    {
        sourceBatchesDirty_ = true;
    }
}

void StaticSprite3D::SetTextureRect(const Rect& rect)
{
    textureRect_ = rect;

    if(useTextureRect_)
    {
        sourceBatchesDirty_ = true;
    }
}

void StaticSprite3D::SetBlendMode(BlendMode blendMode)
{
    if (blendMode == blendMode_)
        return;

    blendMode_ = blendMode;

    UpdateMaterial();
    MarkNetworkUpdate();
}

void StaticSprite3D::SetFlip(bool flipX, bool flipY, bool swapXY)
{
    if (flipX == flipX_ && flipY == flipY_ && swapXY == swapXY_)
        return;

    flipX_ = flipX;
    flipY_ = flipY;
    swapXY_ = swapXY;
    sourceBatchesDirty_ = true;

    MarkNetworkUpdate();
}

void StaticSprite3D::SetFlipX(bool flipX)
{
    SetFlip(flipX, flipY_, swapXY_);
}

void StaticSprite3D::SetFlipY(bool flipY)
{
    SetFlip(flipX_, flipY, swapXY_);
}

void StaticSprite3D::SetSwapXY(bool swapXY)
{
    SetFlip(flipX_, flipY_, swapXY);
}

void StaticSprite3D::SetColor(const Color& color)
{
    if (color == color_)
        return;

    color_ = color;
    sourceBatchesDirty_ = true;
    MarkNetworkUpdate();
}

void StaticSprite3D::SetAlpha(float alpha)
{
    if (alpha == color_.a_)
        return;

    color_.a_ = alpha;
    sourceBatchesDirty_ = true;
    MarkNetworkUpdate();
}

void StaticSprite3D::SetUseHotSpot(bool useHotSpot)
{
    if (useHotSpot == useHotSpot_)
        return;

    useHotSpot_ = useHotSpot;
    sourceBatchesDirty_ = true;
    MarkNetworkUpdate();
    UpdateDrawRect();
}

void StaticSprite3D::SetUseDrawRect(bool useDrawRect)
{
    if (useDrawRect == useDrawRect_)
        return;

    useDrawRect_ = useDrawRect;
    sourceBatchesDirty_ = true;
    MarkNetworkUpdate();
    UpdateDrawRect();
}

void StaticSprite3D::SetUseTextureRect(bool useTextureRect)
{
    if (useTextureRect == useTextureRect_)
        return;

    useTextureRect_ = useTextureRect;
    sourceBatchesDirty_ = true;
    MarkNetworkUpdate();
}

void StaticSprite3D::SetHotSpot(const Vector2& hotspot)
{
    if (hotspot == hotSpot_)
        return;

    hotSpot_ = hotspot;

    if (useHotSpot_)
    {
        sourceBatchesDirty_ = true;
        MarkNetworkUpdate();
    }

    UpdateDrawRect();
}

void StaticSprite3D::SetCustomMaterial(Material* customMaterial)
{
    if (customMaterial == customMaterial_)
        return;

    customMaterial_ = customMaterial;
    sourceBatchesDirty_ = true;

    UpdateMaterial();
    MarkNetworkUpdate();
}

Sprite3D* StaticSprite3D::GetSprite() const
{
    return sprite_;
}


Material* StaticSprite3D::GetCustomMaterial() const
{
    return customMaterial_;
}

void StaticSprite3D::SetSpriteAttr(const ResourceRef& value)
{
    Sprite2D* sprite = Sprite3D::LoadFromResourceRef(this, value);
    if (sprite)
        SetSprite(sprite);
}

ResourceRef StaticSprite3D::GetSpriteAttr() const
{
    return Sprite3D::SaveToResourceRef(sprite2d_);
}

void StaticSprite3D::SetCustomMaterialAttr(const ResourceRef& value)
{
    auto* cache = GetSubsystem<ResourceCache>();
    SetCustomMaterial(cache->GetResource<Material>(value.name_));
}

ResourceRef StaticSprite3D::GetCustomMaterialAttr() const
{
    return GetResourceRef(customMaterial_, Material::GetTypeStatic());
}

void StaticSprite3D::OnSceneSet(Scene* scene)
{
    Drawable3D::OnSceneSet(scene);

    UpdateMaterial();
}

void StaticSprite3D::OnWorldBoundingBoxUpdate()
{
    boundingBox_.Clear();
    worldBoundingBox_.Clear();

    const Vector<SourceBatch3D>& sourceBatches = GetSourceBatches();
    for (unsigned i = 0; i < sourceBatches[0].vertices_.Size(); ++i)
        worldBoundingBox_.Merge(sourceBatches[0].vertices_[i].position_);

    boundingBox_ = worldBoundingBox_.Transformed(node_->GetWorldTransform().Inverse());
}

void StaticSprite3D::OnDrawOrderChanged()
{
    sourceBatches_[0].drawOrder_ = GetDrawOrder();
}

void StaticSprite3D::UpdateSourceBatches()
{
    if (!sourceBatchesDirty_)
        return;

    Vector<Vertex2D>& vertices = sourceBatches_[0].vertices_;
    vertices.Clear();

    if (!sprite2d_)
        return;

    if (!useTextureRect_)
    {
        if (!sprite2d_->GetTextureRectangle(textureRect_, flipX_, flipY_))
            return;
    }

    float minZ = -1.5f;
    float maxZ = 0.1f;

    /*
    V1---------V2
    |         / |
    |       /   |
    |     /     |
    |   /       |
    | /         |
    V0---------V3
    */
    Vertex2D vertex0;
    Vertex2D vertex1;
    Vertex2D vertex2;
    Vertex2D vertex3;

    // Convert to world space
    const Matrix3x4& worldTransform = node_->GetWorldTransform();


// fix -> top and front 

    // Face 1 (INNER FRONT)
    vertex0.position_ = worldTransform * Vector3(drawRect_.min_.x_, drawRect_.min_.y_, maxZ);
    vertex1.position_ = worldTransform * Vector3(drawRect_.min_.x_, drawRect_.max_.y_, maxZ);
    vertex2.position_ = worldTransform * Vector3(drawRect_.max_.x_, drawRect_.max_.y_, maxZ);
    vertex3.position_ = worldTransform * Vector3(drawRect_.max_.x_, drawRect_.min_.y_, maxZ);

    vertex0.uv_ = textureRect_.min_;
    (swapXY_ ? vertex3.uv_ : vertex1.uv_) = Vector2(textureRect_.min_.x_, textureRect_.max_.y_);
    vertex2.uv_ = textureRect_.max_;
    (swapXY_ ? vertex1.uv_ : vertex3.uv_) = Vector2(textureRect_.max_.x_, textureRect_.min_.y_);

    vertex0.color_ = vertex1.color_ = vertex2.color_ = vertex3.color_ = color_.ToUInt();

    vertices.Push(vertex0);
    vertices.Push(vertex1);
    vertices.Push(vertex2);
    vertices.Push(vertex3);


    // Face 2 (OUTER FRONT)
    vertex0.position_ = worldTransform * Vector3(drawRect_.min_.x_, drawRect_.min_.y_, minZ);
    vertex1.position_ = worldTransform * Vector3(drawRect_.min_.x_, drawRect_.max_.y_, minZ);
    vertex2.position_ = worldTransform * Vector3(drawRect_.max_.x_, drawRect_.max_.y_, minZ);
    vertex3.position_ = worldTransform * Vector3(drawRect_.max_.x_, drawRect_.min_.y_, minZ);

    vertex0.uv_ = textureRect_.min_;
    (swapXY_ ? vertex3.uv_ : vertex1.uv_) = Vector2(textureRect_.min_.x_, textureRect_.max_.y_);
    vertex2.uv_ = textureRect_.max_;
    (swapXY_ ? vertex1.uv_ : vertex3.uv_) = Vector2(textureRect_.max_.x_, textureRect_.min_.y_);

    vertex0.color_ = vertex1.color_ = vertex2.color_ = vertex3.color_ = color_.ToUInt();

    vertices.Push(vertex0);
    vertices.Push(vertex1);
    vertices.Push(vertex2);
    vertices.Push(vertex3);

    // Face 3 (bottom)
    vertex0.position_ = worldTransform * Vector3(drawRect_.min_.x_, drawRect_.min_.y_, minZ);
    vertex1.position_ = worldTransform * Vector3(drawRect_.min_.x_, drawRect_.min_.y_, maxZ);
    vertex2.position_ = worldTransform * Vector3(drawRect_.max_.x_, drawRect_.min_.y_, maxZ);
    vertex3.position_ = worldTransform * Vector3(drawRect_.max_.x_, drawRect_.min_.y_, minZ);


    vertex0.uv_ = textureRect_.min_;
    (swapXY_ ? vertex3.uv_ : vertex1.uv_) = Vector2(textureRect_.min_.x_, textureRect_.max_.y_);
    vertex2.uv_ = textureRect_.max_;
    (swapXY_ ? vertex1.uv_ : vertex3.uv_) = Vector2(textureRect_.max_.x_, textureRect_.min_.y_);

    vertex0.color_ = vertex1.color_ = vertex2.color_ = vertex3.color_ = color_.ToUInt();

    vertices.Push(vertex3);
    vertices.Push(vertex2);
    vertices.Push(vertex1);
    vertices.Push(vertex0);

/*
    // Face 4 (top)
    vertex0.position_ = worldTransform * Vector3(drawRect_.min_.x_, drawRect_.max_.y_, minZ);
    vertex1.position_ = worldTransform * Vector3(drawRect_.min_.x_, drawRect_.max_.y_, maxZ);
    vertex2.position_ = worldTransform * Vector3(drawRect_.max_.x_, drawRect_.max_.y_, maxZ);
    vertex3.position_ = worldTransform * Vector3(drawRect_.max_.x_, drawRect_.max_.y_, minZ);

    vertex0.uv_ = textureRect_.min_;
    (swapXY_ ? vertex3.uv_ : vertex1.uv_) = Vector2(textureRect_.min_.x_, textureRect_.max_.y_);
    vertex2.uv_ = textureRect_.max_;
    (swapXY_ ? vertex1.uv_ : vertex3.uv_) = Vector2(textureRect_.max_.x_, textureRect_.min_.y_);

    vertex0.color_ = vertex1.color_ = vertex2.color_ = vertex3.color_ = color_.ToUInt();
/*
    vertices.Push(vertex0);
    vertices.Push(vertex1);
    vertices.Push(vertex2);
    vertices.Push(vertex3);
*/
/*


    // Face 5
    vertex0.position_ = worldTransform * Vector3(drawRect_.min_.x_, drawRect_.min_.y_, maxZ);
    vertex1.position_ = worldTransform * Vector3(drawRect_.min_.x_, drawRect_.max_.y_, maxZ);
    vertex2.position_ = worldTransform * Vector3(drawRect_.min_.x_, drawRect_.max_.y_, minZ);
    vertex3.position_ = worldTransform * Vector3(drawRect_.min_.x_, drawRect_.min_.y_, minZ);

    vertex0.uv_ = textureRect_.min_;
    (swapXY_ ? vertex3.uv_ : vertex1.uv_) = Vector2(textureRect_.min_.x_, textureRect_.max_.y_);
    vertex2.uv_ = textureRect_.max_;
    (swapXY_ ? vertex1.uv_ : vertex3.uv_) = Vector2(textureRect_.max_.x_, textureRect_.min_.y_);

    vertex0.color_ = vertex1.color_ = vertex2.color_ = vertex3.color_ = color_.ToUInt();

    vertices.Push(vertex0);
    vertices.Push(vertex1);
    vertices.Push(vertex2);
    vertices.Push(vertex3);

    // Face 6
    vertex0.position_ = worldTransform * Vector3(drawRect_.max_.x_, drawRect_.min_.y_, minZ);
    vertex1.position_ = worldTransform * Vector3(drawRect_.max_.x_, drawRect_.min_.y_, maxZ);
    vertex2.position_ = worldTransform * Vector3(drawRect_.max_.x_, drawRect_.max_.y_, maxZ);
    vertex3.position_ = worldTransform * Vector3(drawRect_.max_.x_, drawRect_.max_.y_, minZ);

    vertex0.uv_ = textureRect_.min_;
    (swapXY_ ? vertex3.uv_ : vertex1.uv_) = Vector2(textureRect_.min_.x_, textureRect_.max_.y_);
    vertex2.uv_ = textureRect_.max_;
    (swapXY_ ? vertex1.uv_ : vertex3.uv_) = Vector2(textureRect_.max_.x_, textureRect_.min_.y_);

    vertex0.color_ = vertex1.color_ = vertex2.color_ = vertex3.color_ = color_.ToUInt();

    vertices.Push(vertex3);
    vertices.Push(vertex2);
    vertices.Push(vertex1);
    vertices.Push(vertex0);

*/

    sourceBatchesDirty_ = false;
}

void StaticSprite3D::UpdateMaterial()
{
    if (customMaterial_) {
        sourceBatches_[0].material_ = customMaterial_;
        sourceBatches_[0].material_ ->SetFillMode(FillMode::FILL_WIREFRAME);
    }
    else
    {
        if (sprite2d_ && renderer_) {
            sourceBatches_[0].material_ = renderer_->GetMaterial(sprite2d_->GetTexture(), blendMode_);
            sourceBatches_[0].material_ ->SetFillMode(FillMode::FILL_WIREFRAME);
        } else
            sourceBatches_[0].material_ = nullptr;
    }

}

void StaticSprite3D::UpdateDrawRect()
{
    if (!useDrawRect_)
    {
        if (useHotSpot_)
        {
            if (sprite2d_ && !sprite2d_->GetDrawRectangle(drawRect_, hotSpot_, flipX_, flipY_))
                return;
        }
        else
        {
            if (sprite2d_ && !sprite2d_->GetDrawRectangle(drawRect_, flipX_, flipY_))
                return;
        }
    }
}

}
