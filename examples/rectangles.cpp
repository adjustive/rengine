/*
    Copyright (c) 2015, Gunnar Sletta <gunnar@sletta.org>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "rengine.h"
#include "examples.h"

class MyWindow : public StandardSurfaceInterface
{
public:
    MyWindow()
        : m_layer(0)
    {
    }

    ~MyWindow()
    {
        delete m_layer;
    }

    Node *update(Node *old) {
        if (old)
            return old;

        vec2 s = surface()->size();

        m_layer = rengine_loadImage(renderer(), "walker.png");

        renderer()->setFillColor(vec4(0.0, 0, 0, 0.0));

        // Root has origin in screen center
        TransformNode *root = TransformNode::create();
        root->setMatrix(mat4::translate2D(s.x * 0.5, s.y * 0.5));

        bool flipOrientation = false;
        if (s.x < s.y) {
            flipOrientation = true;
            s = vec2(s.y, s.x);
        }

        TransformNode *xnode = TransformNode::create();
        xnode->setProjectionDepth(1000);
        xnode->setMatrix(mat4::rotateAroundX(-0.15));

        TransformNode *rotationNode = TransformNode::create();
        xnode->append(rotationNode);

        const int count = 20;
        for (int i=0; i<count; ++i) {
            float c = i / float(count - 1);
            float w = s.x * 0.1;
            float h = s.y * 0.2;

            TransformNode *tn = TransformNode::create();

            tn->setMatrix(mat4::rotateAroundY(M_PI * 2.0 * (i / float(count)))
                          * mat4::translate(s.x * 0.33, 0, 0)
                          * mat4::rotateAroundY(M_PI/2.0));

            *tn << RectangleNode::create(rect2d::fromXywh(-w/2, -h/2, w, h), vec4(c, 0.5, 1-c, 0.8));
            rotationNode->append(tn);

            TransformNode *depthAdjustment = TransformNode::create();
            depthAdjustment->setMatrix(mat4::rotateAroundY(M_PI * 2.0 * (i / float(count)))
                                       * mat4::translate(s.x * 0.33 + 2, 0, 0)
                                       * mat4::rotateAroundY(M_PI/2.0));
            *depthAdjustment << TextureNode::create(rect2d::fromXywh(-w/2, -h/2, w, h), m_layer);
            rotationNode->append(depthAdjustment);
        }

        if (flipOrientation) {
            *root << &(*TransformNode::create(mat4::rotate2D(M_PI / 2.0))
                       << xnode);
        } else {
            root->append(xnode);
        }

        AnimationClosure<TransformNode, SmoothedTimingFunction> *anim = new AnimationClosure<TransformNode, SmoothedTimingFunction>(rotationNode);
        anim->setDuration(4);
        anim->setIterations(-1);
        anim->keyFrames.times() << 0 << 1;
        anim->keyFrames.addValues<double, TransformNode_rotateAroundY>() << 0 << M_PI * 2.0;
        animationManager()->startAnimation(anim);

        return root;
    }

private:
    Texture *m_layer;
};

RENGINE_MAIN(MyWindow)