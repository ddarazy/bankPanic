//
//  BPScrollableBGLayer.cpp
//  bankPanic
//
//  Created by 전 홍림 on 2014. 6. 18..
//  Copyright (c) 2014년 __MyCompanyName__. All rights reserved.
//

#include "BPScrollableBGLayer.h"


BPScrollableBGLayer::~BPScrollableBGLayer()
{
    CC_SAFE_RELEASE_NULL(m_arrBGImage);
}

BPScrollableBGLayer* BPScrollableBGLayer::createNode(const char *szBGImage, float velocity, int iBGImageCount)
{
    BPScrollableBGLayer *pRet = new BPScrollableBGLayer();
    if (pRet && pRet->init(szBGImage, velocity, iBGImageCount)) {
        pRet->autorelease();
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool BPScrollableBGLayer::init(const char *szBGImage, float velocity, int iBGImageCount)
{
    if (!CCLayer::init()) {
        return false;
    }
    
    //  init velocity -> move per touch move distance
    m_fVelocity = velocity;
    
    //  init bg image data container
    m_arrBGImage = CCArray::create();
    CC_SAFE_RETAIN(m_arrBGImage);
    
    //  init bg image container node
    m_pBGBatchNode = CCSpriteBatchNode::create(szBGImage);
    addChild(m_pBGBatchNode);
    
 
    float offset_x = 0;
    float height = 0;
    for (int i=0; i<iBGImageCount; i++) {
        CCSprite *pBG = CCSprite::create(szBGImage);
        pBG->setAnchorPoint(ccp(0, 0));
        pBG->setPosition(ccp(offset_x, 0));
        m_pBGBatchNode->addChild(pBG);
        m_arrBGImage->addObject(pBG);
        
        CCSize tSize = pBG->getContentSize();
        offset_x += tSize.width;
        height = tSize.height;
    }
    
    setContentSize(CCSizeMake(offset_x, height));
    
	return true;
}

void BPScrollableBGLayer::moveScrolling(float fMoveX)
{
    float posx = m_fVelocity * fMoveX;
    for (int i=0; i<m_arrBGImage->count(); i++) {
        CCSprite *pSpr = (CCSprite*)m_arrBGImage->objectAtIndex(i);
        pSpr->setPositionX(posx);
    }
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    if (fMoveX < 0)
    {
        CCSprite *pFirstSpr = (CCSprite*)m_arrBGImage->objectAtIndex(0);
        float fFirstX = pFirstSpr->getPositionX();
        float fFirstSize = pFirstSpr->getContentSize().width;
        
        if (fFirstX + fFirstSize < 0) {
            CCSprite *pLastSpr = (CCSprite*)m_arrBGImage->lastObject();
            float offset_x = pLastSpr->getPositionX() + pLastSpr->getContentSize().width;
            pFirstSpr->setPositionX(offset_x);
            
            m_arrBGImage->removeObject(pFirstSpr);
            m_arrBGImage->addObject(pFirstSpr);
            
        }
    }
    else
    {
        CCSprite *pLastBG = (CCSprite*)m_arrBGImage->lastObject();
        float fLastX = pLastBG->getPositionX();
        if (fLastX > winSize.width) {
            CCSprite *pFirstBG = (CCSprite*)m_arrBGImage->objectAtIndex(0);
            float offset_x = pFirstBG->getPositionX() - pFirstBG->getContentSize().width;
            pLastBG->setPositionX(offset_x);
            
            m_arrBGImage->removeObject(pLastBG);
            m_arrBGImage->insertObject(pLastBG, 0);
        }

    }
}


