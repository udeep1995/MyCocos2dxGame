//
//  Actor.cpp
//  Tone_Skipper
//
//  Created by wang haibo on 15/8/26.
//
//

#include "Actor.h"
USING_NS_CC;

float Actor::m_sfInitHue = 0;
Actor::Actor()
{
    m_pSprite = nullptr;
    m_Velocity = Vec2::ZERO;
    m_bOnLand = true;
    m_bOnLadder = false;
    m_fMaxXSpeed = 0.5f;
    m_fMaxYSpeed = 3.0f;
    m_pDebugDrawNode = nullptr;
    setCascadeOpacityEnabled(true);
    setCascadeColorEnabled(true);
}
Actor::~Actor()
{
}
float Actor::getHue()
{
    if(!m_pSprite)
        return 0;
    float hue = m_pSprite->getHue()/(2*M_PI);
    return hue - (int)hue;
}
void Actor::setHue(float hue)
{
    if(!m_pSprite)
        return;
    CCASSERT(hue >= 0 && hue<=1, "hue value must between 0 and 1!");
    m_fHueNoChangeTime = 0;
    m_sfInitHue = hue;
    m_pSprite->setHue(2*M_PI*hue);
}
void Actor::setFlipX(bool fliped)
{
    if(m_pSprite)
        m_pSprite->setFlippedX(fliped);
}
void Actor::showDebug(bool debug)
{
    if(debug)
    {
        if(!m_pDebugDrawNode)
        {
            m_pDebugDrawNode = DrawNode::create();
            addChild(m_pDebugDrawNode);
        }
        if(m_pSprite)
        {
            m_pDebugDrawNode->clear();
            cocos2d::Rect rect = getBoundingBox();
            Vec2 vertices[4] = {
                Vec2( rect.getMinX(), rect.getMinY() ),
                Vec2( rect.getMaxX(), rect.getMinY() ),
                Vec2( rect.getMaxX(), rect.getMaxY() ),
                Vec2( rect.getMinX(), rect.getMaxY() ),
            };
            m_pDebugDrawNode->drawPoly(vertices, 4, true, Color4F::WHITE);
        }
    }
    else
    {
        if(m_pDebugDrawNode)
        {
            m_pDebugDrawNode->clear();
            m_pDebugDrawNode->removeFromParentAndCleanup(true);
            m_pDebugDrawNode = nullptr;
        }
    }
}