//
//  BPElevatorNode.cpp
//  bankPanic
//
//  Created by 전 홍림 on 2014. 4. 26..
//  Copyright (c) 2014년 __MyCompanyName__. All rights reserved.
//

#include "BPElevatorNode.h"
#include "ITGUtil.h"

BPElevatorNode::~BPElevatorNode()
{
}

BPElevatorNode* BPElevatorNode::createNode()
{
    BPElevatorNode *node = (BPElevatorNode*)ITGUtil::sharedUtils()->loadCustomNodeFromCCBI<BPElevatorNode>("BPElevatorNode", "BPElevatorNode.ccbi", NULL, NULL);
    if (node) {
        node->initObjects();
    }
    return node;
}

bool BPElevatorNode::init()
{
    if (!CCNode::init()) {
        return false;
    }
	return true;
}

void BPElevatorNode::onEnter()
{
    CCNode::onEnter();
}

void BPElevatorNode::onExit()
{
    CCNode::onExit();
}

#pragma mark functions
void BPElevatorNode::initObjects()
{
    m_pHumanSprite = CCSprite::createWithSpriteFrameName("appear_human.png");
    m_pHumanSprite->setAnchorPoint(ccp(0.5, 0));
    m_pHumanSprite->setVisible(false);
    m_pHumanSprite->setPosition(m_pSpawnPosition->getPosition());
    addChild(m_pHumanSprite, 10);
    
    m_pAlertBoardNode->setColor(ccWHITE);
    m_pAlertLabelNode->setFontFillColor(ccBLACK);
}

void BPElevatorNode::updateContents()
{
    
}

void BPElevatorNode::setElevatorNumber(uint32_t eNumber)
{
    m_iElevatorNumber = eNumber;
    CCString szTemp;
    szTemp.initWithFormat("%d", m_iElevatorNumber);
    m_pElevatorNumber->setString(szTemp.getCString());
}

void BPElevatorNode::startAppearHuman()
{
    if (!m_bHumanAppearUpdate && !m_bArrivedHuman) {
        if (m_pHumanSprite) {
            m_pHumanSprite->setPosition(m_pSpawnPosition->getPosition());
            m_pHumanSprite->setVisible(true);
        }
        m_fDelayStart = arc4random() % 20 + 2;
        m_tVelocity.y = -(float)(arc4random() % 3 + 1);

        m_bHumanAppearUpdate = true;
        scheduleUpdate();
    }
}

void BPElevatorNode::resetAppearHuman()
{
    m_pAlertBoardNode->setColor(ccWHITE);
    m_pAlertLabelNode->setFontFillColor(ccBLACK);

    m_bArrivedHuman = false;
    if (m_bHumanAppearUpdate) {
        unscheduleUpdate();
    }
    m_bHumanAppearUpdate = false;
}


void BPElevatorNode::update(float delta)
{
    m_fDelayStart -= delta;
    
    //  end delay
    if (m_fDelayStart < 0)
    {
        if (!m_pHumanSprite->isVisible()) {
            m_pHumanSprite->setVisible(true);
        }
        
        CCPoint p = m_pHumanSprite->getPosition();
        p = ccpAdd(p, m_tVelocity);
        m_pHumanSprite->setPosition(p);
        if (m_pTargetPosition->boundingBox().containsPoint(p))
        {
            m_bArrivedHuman = true;
            m_bHumanAppearUpdate = false;
            unscheduleUpdate();
            m_pHumanSprite->setVisible(false);
            m_pHumanSprite->setPosition(m_pSpawnPosition->getPosition());
            
            m_pAlertBoardNode->setColor(ccRED);
            m_pAlertLabelNode->setFontFillColor(ccWHITE);
            
            if (m_pDelegate) {
                m_pDelegate->arrivedHuman(this);
            }

        }
        
    }
}
