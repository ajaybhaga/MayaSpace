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

#include "../Graphics/Drawable.h"
#include "./Drawable2D.h"
#include "../Graphics/GraphicsDefs.h"

namespace Urho3D
{

class Drawable3D;
class Renderer3D;
class Texture2D;
class VertexBuffer;

/// 3D source batch.
struct SourceBatch3D
{
    /// Construct.
    SourceBatch3D();

    /// Owner.
    WeakPtr<Drawable3D> owner_;
    /// Distance to camera.
    mutable float distance_;
    /// Draw order.
    int drawOrder_;
    /// Material.
    SharedPtr<Material> material_;
    /// Vertices.
    Vector<Vertex2D> vertices_;
};

/// Pixel size (equal 0.01f).
extern URHO3D_API const float PIXEL_SIZE;

/// Base class for 2D visible components.
class URHO3D_API Drawable3D : public Drawable
{
    URHO3D_OBJECT(Drawable3D, Drawable);

public:
    /// Construct.
    explicit Drawable3D(Context* context);
    /// Destruct.
    ~Drawable3D() override;
    /// Register object factory. Drawable must be registered first.
    static void RegisterObject(Context* context);

    /// Handle enabled/disabled state change.
    void OnSetEnabled() override;

    /// Set layer.
    void SetLayer(int layer);
    /// Set order in layer.
    void SetOrderInLayer(int orderInLayer);

    /// Return layer.
    int GetLayer() const { return layer_; }

    /// Return order in layer.
    int GetOrderInLayer() const { return orderInLayer_; }

    /// Return all source batches (called by Renderer3D).
    const Vector<SourceBatch3D>& GetSourceBatches();

protected:
    /// Handle scene being assigned.
    void OnSceneSet(Scene* scene) override;
    /// Handle node transform being dirtied.
    void OnMarkedDirty(Node* node) override;
    /// Handle draw order changed.
    virtual void OnDrawOrderChanged() = 0;
    /// Update source batches.
    virtual void UpdateSourceBatches() = 0;

    /// Return draw order by layer and order in layer.
    int GetDrawOrder() const { return layer_ << 16u | orderInLayer_; }

    /// Layer.
    int layer_;
    /// Order in layer.
    int orderInLayer_;
    /// Source batches.
    Vector<SourceBatch3D> sourceBatches_;
    /// Source batches dirty flag.
    bool sourceBatchesDirty_;
    /// Renderer3D.
    WeakPtr<Renderer3D> renderer_;
};

}
