//
//  ITGProgressBar9SpriteNode.cpp
//
//  Created by forestj on 13. 5. 24..
//  Copyright (c) 2013년 Irontree Global. All rights reserved.
//

#include "ITGProgressBar9SpriteNode.h"

USING_NS_CC;
USING_NS_CC_EXT;


ITGProgressBar9SpriteNode::ITGProgressBar9SpriteNode()
{    
    m_eAlignment = kITGProgressBar9SpriteAlignHorizontal;
    m_fPercentage = 0;
    m_pFillSprite = NULL;
    m_pEmptySprite = NULL;
    m_bReverseDirection = false;

}

ITGProgressBar9SpriteNode::~ITGProgressBar9SpriteNode()
{
}



ITGProgressBar9SpriteNode* ITGProgressBar9SpriteNode::create(const CCSize& barSize, cocos2d::extension::CCScale9Sprite *pEmptyBarSprite, cocos2d::extension::CCScale9Sprite *pFillBarSprite)
{
    ITGProgressBar9SpriteNode* pRet = new ITGProgressBar9SpriteNode();
    pRet->setContentSize(barSize);
    
    if (pRet && pRet->initWithSprite(pEmptyBarSprite, pFillBarSprite)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

ITGProgressBar9SpriteNode* ITGProgressBar9SpriteNode::create(const cocos2d::CCSize &barSize, const char *pEmptyBarSpriteName, const char *pFillBarSpriteName, const cocos2d::CCRect &fileSize, const cocos2d::CCRect &inset)
{
    CCScale9Sprite *pEmpty = NULL;
    CCScale9Sprite *pFill = NULL;
    if (pEmptyBarSpriteName) {
        pEmpty = CCScale9Sprite::create(pEmptyBarSpriteName, fileSize, inset);
    }
    
    if (pFillBarSpriteName) {
        pFill = CCScale9Sprite::create(pFillBarSpriteName, fileSize, inset);
    }
    return create(barSize, pEmpty, pFill);
}

bool ITGProgressBar9SpriteNode::init()
{
    
    if (!CCNode::init()) {
        return false;
    }
	return true;
}

#pragma mark public functions

bool ITGProgressBar9SpriteNode::initWithSprite(cocos2d::extension::CCScale9Sprite *pEmptyBarSprite, cocos2d::extension::CCScale9Sprite *pFillBarSprite)
{
    if (!this->init()) {
        return false;
    }
    setAnchorPoint(ccp(0, 0));
    m_eAlignment = kITGProgressBar9SpriteAlignHorizontal;
    m_bReverseDirection = false;
    setSprite(pEmptyBarSprite, pFillBarSprite);
    setPercentage(0.0f);
    
    if (pEmptyBarSprite) {
        this->addChild(pEmptyBarSprite, 1);
    }
    if (pFillBarSprite) {
        this->addChild(pFillBarSprite, 2);
    }
    
    return true;
}

void ITGProgressBar9SpriteNode::setPercentage(float fPercentage)
{
    m_fPercentage = clampf(fPercentage, 0, 100);
    updateProgress(false);
}

void ITGProgressBar9SpriteNode::setPercentage(float fPercentage, bool animate)
{
    m_fPercentage = clampf(fPercentage, 0, 100);
    updateProgress(animate);
}

void ITGProgressBar9SpriteNode::setSprite(cocos2d::extension::CCScale9Sprite *pEmptySprite, cocos2d::extension::CCScale9Sprite *pFillSprite)
{
    if (pEmptySprite) {
        if (m_pEmptySprite != pEmptySprite) {
            CC_SAFE_RETAIN(pEmptySprite);
            CC_SAFE_RELEASE(m_pEmptySprite);
            m_pEmptySprite = pEmptySprite;

            m_pEmptySprite->setPosition(ccp(0, 0));
            m_pEmptySprite->setAnchorPoint(ccp(0, 0));
            CCSize oSize = m_pEmptySprite->getOriginalSize();
            CCSize size = getContentSize();
            m_pEmptySprite->setPreferredSize(CCSizeMake(size.width, oSize.height));
        }
    }
    
    if (pFillSprite) {
        if (m_pFillSprite != pFillSprite) {
            CC_SAFE_RETAIN(pFillSprite);
            CC_SAFE_RELEASE(m_pFillSprite);
            
            m_pFillSprite = pFillSprite;
            
            m_pFillSprite->setPosition(ccp(0, 0));
            m_pFillSprite->setAnchorPoint(ccp(0, 0));
            
            CCSize oSize = m_pFillSprite->getOriginalSize();
            m_pFillSprite->setPreferredSize(CCSizeMake(0, oSize.height));

        }
        
    }
}

void ITGProgressBar9SpriteNode::setAlignment(ITGProgressBar9SpriteAlignment eAlignment)
{
    if (eAlignment != m_eAlignment) {
        m_eAlignment = eAlignment;
    }
}

CCSize ITGProgressBar9SpriteNode::getExpectablePercentageSize(float fExpectPercent)
{
    float fEP = clampf(fExpectPercent, 0, 100);
    CCSize size = getContentSize();;
    CCSize expectSize = m_pFillSprite->getPreferredSize();
    expectSize.width = size.width * fEP/100;
    return expectSize;
}


#pragma mark protected functions

void ITGProgressBar9SpriteNode::updateProgress(bool animate)
{
    if (!m_pFillSprite) {
        return;
    }
    if (m_eAlignment == kITGProgressBar9SpriteAlignHorizontal) {
        CCSize size = getContentSize();
        CCSize cSize = m_pFillSprite->getPreferredSize();
        cSize.width = size.width * m_fPercentage/100;
        
        CCSize originSize = m_pFillSprite->getOriginalSize();
        CCRect capInsets = m_pFillSprite->getCapInsets();
        
        float minWidth = capInsets.origin.x + (originSize.width - (capInsets.origin.x + capInsets.size.width));
//        CCLOG("cSize : %f, minWidth : %f", cSize.width, minWidth);

        if (cSize.width < minWidth) {
            cSize.width = minWidth;
        }

        if (cSize.width == 0 || m_fPercentage == 0) {
            m_pFillSprite->setVisible(false);
        }
        else
        {
            m_pFillSprite->setVisible(true);
        }
        
        
        if (animate) {
            m_pFillSprite->setPreferredSize(cSize);
//            m_pFillSprite->runAction(cocos2d::CCAction *action)
        }
        else
        {
            m_pFillSprite->setPreferredSize(cSize);
        }
    }
    else
    {
        
    }
}
