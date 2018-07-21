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
#include "../Graphics/Camera.h"
#include "../Graphics/Material.h"
#include "../Graphics/Texture2D.h"
#include "../Scene/Scene.h"
#include "../Urho2D/Drawable3D.h"
#include "../Urho2D/Renderer3D.h"

#include "../DebugNew.h"

namespace Urho3D
{

SourceBatch3D::SourceBatch3D() :
    distance_(0.0f),
    drawOrder_(0)
{
}

Drawable3D::Drawable3D(Context* context) :
    Drawable(context, DRAWABLE_GEOMETRY2D),
    layer_(0),
    orderInLayer_(0),
    sourceBatchesDirty_(true)
{
}

Drawable3D::~Drawable3D()
{
    if (renderer_)
        renderer_->RemoveDrawable(this);
}

void Drawable3D::RegisterObject(Context* context)
{
    URHO3D_ACCESSOR_ATTRIBUTE("Layer", GetLayer, SetLayer, int, 0, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Order in Layer", GetOrderInLayer, SetOrderInLayer, int, 0, AM_DEFAULT);
    URHO3D_ATTRIBUTE("View Mask", int, viewMask_, DEFAULT_VIEWMASK, AM_DEFAULT);
}

void Drawable3D::OnSetEnabled()
{
    bool enabled = IsEnabledEffective();

    if (enabled && renderer_)
        renderer_->AddDrawable(this);
    else if (!enabled && renderer_)
        renderer_->RemoveDrawable(this);
}

void Drawable3D::SetLayer(int layer)
{
    if (layer == layer_)
        return;

    layer_ = layer;

    OnDrawOrderChanged();
    MarkNetworkUpdate();
}

void Drawable3D::SetOrderInLayer(int orderInLayer)
{
    if (orderInLayer == orderInLayer_)
        return;

    orderInLayer_ = orderInLayer;

    OnDrawOrderChanged();
    MarkNetworkUpdate();
}

const Vector<SourceBatch3D>& Drawable3D::GetSourceBatches()
{
    if (sourceBatchesDirty_)
        UpdateSourceBatches();

    return sourceBatches_;
}

void Drawable3D::OnSceneSet(Scene* scene)
{


    // AB: DO WE WANT TO PUT for drawables in the octree?     
    // Do not call Drawable::OnSceneSet(node), as 2D drawable components should not be added to the octree
    // but are instead rendered through Renderer2D

    if (scene)
    {
        renderer_ = scene->GetOrCreateComponent<Renderer3D>();

        if (IsEnabledEffective())
            renderer_->AddDrawable(this);
    }
    else
    {
        if (renderer_)
            renderer_->RemoveDrawable(this);
    }
}

void Drawable3D::OnMarkedDirty(Node* node)
{
    Drawable::OnMarkedDirty(node);

    sourceBatchesDirty_ = true;
}

}
