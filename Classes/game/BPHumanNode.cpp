//
//  BPHumanNode.cpp
//  bankPanic
//
//  Created by forestj on 2014. 4. 29..
//  Copyright (c) 2014년 __MyCompanyName__. All rights reserved.
//

#include "BPHumanNode.h"


BPHumanNode::~BPHumanNode()
{
}


bool BPHumanNode::init()
{
    if (!CCNode::init()) {
        return false;
    }
	return true;
}

void BPHumanNode::onEnter()
{
    CCNode::onEnter();
}

void BPHumanNode::onExit()
{
    CCNode::onExit();
}

#pragma mark functions
void BPHumanNode::appearHuman()
{
    if (!m_pHumanSprite) {
        m_pHumanSprite = CCSprite::createWithSpriteFrameName("citizen_01.png");
        addChild(m_pHumanSprite);
    }
    
    int rand = arc4random() % 3;
    m_eHumanType = (eHUMAN_TYPE)rand;
    CCSpriteFrame *pFrame = NULL;
    CCSpriteFrameCache *pCache = CCSpriteFrameCache::sharedSpriteFrameCache();
    
    switch (m_eHumanType) {
        case kHUMAN_TYPE_CITIZEN:
        default:
        {
            pFrame = pCache->spriteFrameByName("citizen_01.png");
        }
            break;
        case kHUMAN_TYPE_GUNMEN:
        {
            pFrame = pCache->spriteFrameByName("enemy_01.png");
        }
            break;
        case kHUMAN_TYPE_WANTED:
        {
            pFrame = pCache->spriteFrameByName("enemy_01.png");
        }
            break;
    }
    
    m_pHumanSprite->setDisplayFrame(pFrame);
}