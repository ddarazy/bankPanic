//
//  ITGModalEnableLayer.cpp
//  Conan
//
//  Created by forestj on 13. 9. 30..
//  Copyright (c) 2013년 __MyCompanyName__. All rights reserved.
//

#include "ITGModalEnableLayer.h"
#include "cocos-ext.h"


USING_NS_CC;
USING_NS_CC_EXT;


ITGModalEnableLayer::ITGModalEnableLayer()
: m_isModal(false)
{
}

ITGModalEnableLayer::~ITGModalEnableLayer()
{
}



bool ITGModalEnableLayer::init()
{
    if (!CCLayer::init()) {
        return false;
    }
	return true;
}

void ITGModalEnableLayer::addModal(cocos2d::CCNode *child, int zOrder)
{
    saveTouchesRecursive(this);
    for (int i=0; i<m_arrSavedTouches.size(); i++) {
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(m_arrSavedTouches[i]);
    }
    
    child->setTag(kModalViewTag);
    addChild(child, zOrder);
    m_isModal = true;
}

void ITGModalEnableLayer::removeModal(cocos2d::CCNode *child, bool cleanup)
{
    child->setTag(-1);
    removeChild(child, cleanup);
    
    touchesRecursive();
    m_isModal = false;
    
}

void ITGModalEnableLayer::removeModal(bool cleanup)
{
    CCNode *pNode = getChildByTag(kModalViewTag);
    if (pNode) {
        pNode->setTag(-1);
        removeChild(pNode, cleanup);
    }
    
    touchesRecursive();
    
    m_isModal = false;
}

void ITGModalEnableLayer::touchesRecursive()
{
    for (int i=0; i<m_arrSavedTouches.size(); i++) {
        CCStandardTouchDelegate *standard = dynamic_cast<CCStandardTouchDelegate*>(m_arrSavedTouches[i]);
        CCTargetedTouchDelegate *targeted = dynamic_cast<CCTargetedTouchDelegate*>(m_arrSavedTouches[i]);
        CCLayer *layer = dynamic_cast<CCLayer*>(m_arrSavedTouches[i]);
        
        
        if (layer) {
            layer->registerWithTouchDispatcher();
        }
        else if(standard)
        {
            CCDirector::sharedDirector()->getTouchDispatcher()->addStandardDelegate(standard, 0);
        }
        else if(targeted)
        {
            
            CCTargetedTouchHandler *handler = (CCTargetedTouchHandler*)CCDirector::sharedDirector()->getTouchDispatcher()->findHandler(targeted);
            if (handler) {
                CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(targeted, 0, handler->isSwallowsTouches());
            }
            else
            {
                CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(targeted, 0, false);
            }
            
        }
        else
        {
            CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(m_arrSavedTouches[i], 0, false);
        }
    }
    
    m_arrSavedTouches.clear();
}

void ITGModalEnableLayer::saveTouchesRecursive(cocos2d::CCNode *pNode)
{
    if (pNode != this) {
        CCTouchDelegate *delegate = dynamic_cast<CCTouchDelegate*>(pNode);
        
        if (delegate) {
            CCTouchHandler *handler = CCDirector::sharedDirector()->getTouchDispatcher()->findHandler(delegate);
            if (handler) {
                m_arrSavedTouches.push_back(delegate);
            }
        }
        
        for (int i=0; i<pNode->getChildrenCount(); i++) {
            CCNode *child = dynamic_cast<CCNode*>(pNode->getChildren()->objectAtIndex(i));
            if (child) {
                saveTouchesRecursive(child);
            }
        }
    }
    else
    {
        for (int i=0; i<pNode->getChildrenCount(); i++) {
            CCNode *child = dynamic_cast<CCNode*>(pNode->getChildren()->objectAtIndex(i));
            if (child) {
                saveTouchesRecursive(child);
            }
        }
    }
}

