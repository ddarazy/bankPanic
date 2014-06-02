//
//  ITGTypingLabelNode.cpp
//  Conan
//
//  Created by forestj on 13. 10. 18..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#include "ITGTypingLabelNode.h"


ITGTypingLabelNode::~ITGTypingLabelNode()
{
}


bool ITGTypingLabelNode::init()
{
    if (!CCNode::init()) {
        return false;
    }
    
    m_fTypingInterval   = 0.1f;
	return true;
}

void ITGTypingLabelNode::onEnter()
{
    CCNode::onEnter();
//    CCLog("ITGTypingLabelNode::onEnter");
}

void ITGTypingLabelNode::onExit()
{
    CCNode::onExit();

}

ITGTypingLabelNode* ITGTypingLabelNode::create(const char *string, const char *fontName, float fontSize)
{
    ITGTypingLabelNode *node = new ITGTypingLabelNode();

    if (node && node->init()) {
        node->m_pLabel = CCLabelTTF::create(string, fontName, fontSize);
        node->m_pLabel->setAnchorPoint(ccp(0, 0));
        node->m_pLabel->setColor(ccc3(88,62,47));
        node->m_pLabel->setHorizontalAlignment(kCCTextAlignmentLeft);
        node->addChild(node->m_pLabel);
    }
    
    return node;
}

void ITGTypingLabelNode::setDimentionSize(cocos2d::CCSize &tDimention)
{
    m_pLabel->setDimensions(tDimention);
    setContentSize(tDimention);
}


void ITGTypingLabelNode::setTypingString(const char *label)
{
    if (!m_bTyping) {
        m_pCurLabelString = label;
        m_iCurLabelCount = 0;
        m_iTotalLabelCount = m_pCurLabelString.length();
        
        schedule(schedule_selector(ITGTypingLabelNode::update), m_fTypingInterval, kCCRepeatForever, 0);
        m_bTyping = true;

    }
    else
    {
        if (m_pCurLabelString == label)
        {
            typingStringEnd();
        }
        else
        {
            m_pCurLabelString = label;
            m_iCurLabelCount = 0;
            m_iTotalLabelCount = m_pCurLabelString.length();
            
            schedule(schedule_selector(ITGTypingLabelNode::update), m_fTypingInterval, kCCRepeatForever, 0);
            m_bTyping = true;
            
        }
        
    }
}

void ITGTypingLabelNode::update(float delta)
{
    unsigned char ch;

    m_fCurDelta += delta;
    if (m_fCurDelta > m_fTypingInterval) {

        ch = m_pCurLabelString[m_iCurLabelCount];
        string sub;
        if ((ch & 0x80) == 0x80)
        {
            sub = m_pCurLabelString.substr(0, m_iCurLabelCount + 3);
            //  korean
            m_iCurLabelCount += 3;
        }
        else
        {
            sub = m_pCurLabelString.substr(0, m_iCurLabelCount+1);
            m_iCurLabelCount++;
        }
        
        m_pLabel->setString(sub.c_str());
        if (m_pDelegate) {
            m_pDelegate->typingLabelNodeChanged();
        }
        
        if (m_iCurLabelCount >= m_iTotalLabelCount) {
            unschedule(schedule_selector(ITGTypingLabelNode::update));
            m_bTyping = false;
            
            if (m_pDelegate) {
                m_pDelegate->typtingLabelNodeAnimationEnded();
            }

        }
        m_fCurDelta = 0;
    }
}

bool ITGTypingLabelNode::typingStringEnd()
{
    if (m_bTyping) {
        unschedule(schedule_selector(ITGTypingLabelNode::update));
        m_pLabel->setString(m_pCurLabelString.c_str());
        m_bTyping = false;
        
        if (m_pDelegate) {
            m_pDelegate->typtingLabelNodeAnimationEnded();
        }
        return false;
    } else {
        return true;
    }
}

void ITGTypingLabelNode::setHorizontalAlignment(CCTextAlignment hAlignment)
{
    if (m_pLabel) {
        m_pLabel->setHorizontalAlignment(hAlignment);
    }
}

void ITGTypingLabelNode::setVerticalAlignment(CCVerticalTextAlignment vAlignment)
{
    if (m_pLabel) {
        m_pLabel->setVerticalAlignment(vAlignment);
    }
}

void ITGTypingLabelNode::resetLabelString()
{
    m_pLabel->setString("");
}


