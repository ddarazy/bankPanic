//
//  BPDoorNode.cpp
//  bankPanic
//
//  Created by 전 홍림 on 2014. 4. 24..
//  Copyright (c) 2014년 __MyCompanyName__. All rights reserved.
//

#include "BPDoorNode.h"
#include "ITGUtil.h"
#include "BPHumanNode.h"

BPDoorNode::~BPDoorNode()
{
    CC_SAFE_RELEASE_NULL(m_pAnimationMng);
}

BPDoorNode* BPDoorNode::createNode()
{
    BPDoorNode *node = (BPDoorNode*)ITGUtil::sharedUtils()->loadCustomNodeFromCCBI<BPDoorNode>("BPDoorNode", "BPDoorNode.ccbi", NULL, NULL);
    if (node) {
        node->m_pAnimationMng = (CCBAnimationManager*)node->getUserObject();
        node->m_pAnimationMng->retain();
        node->initObjects();
    }
    return node;
}

bool BPDoorNode::init()
{
    if (!CCNode::init()) {
        return false;
    }
    
    
	return true;
}

void BPDoorNode::onEnter()
{
    CCNode::onEnter();
}

void BPDoorNode::onExit()
{
    CCNode::onExit();
}

#pragma mark functions

void BPDoorNode::initObjects()
{
    m_pHumanNode = BPHumanNode::create();
    m_pHumanNode->setVisible(false);
    m_pEnemyAppearNode->addChild(m_pHumanNode);
}

void BPDoorNode::setOpenDoor(bool bOpen)
{
    if (m_bEnableOpenDoor) {
        m_bOpenDoor = bOpen;
        if (m_bOpenDoor) {
            if (m_pAnimationMng) {
                m_pAnimationMng->runAnimationsForSequenceNamed("opening");
            }
            if (m_pHumanNode) {
                m_pHumanNode->setVisible(true);
                m_pHumanNode->appearHuman();
            }
        }
        else
        {
            if (m_pAnimationMng) {
                m_pAnimationMng->runAnimationsForSequenceNamed("closing");
            }
            if (m_pHumanNode) {
                m_pHumanNode->setVisible(false);
            }
        }
    }
}

void BPDoorNode::setDoorNumber(uint32_t doorNumber)
{
    m_iDoorNumber = doorNumber;
    CCString szTemp;
    szTemp.initWithFormat("%d", doorNumber);
    m_pDoorNumber->setString(szTemp.getCString());
}
